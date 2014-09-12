/*
 * $$$MODULE_NAME app_asrc.c
 *
 * $$$MODULE_DESC app_asrc.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include <string.h>
#include "csl_audioClass.h"
#include "dda_dma.h"
#include "psp_i2s.h"
#include "asrc_dly_fix.h"
#include "app_globals.h"
#include "utils.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_asrc.h"
#include "VirtualSense_ACIcfg.h"

/* Number of supported sampling rates */
#define AASRC_NUM_SAMP_RATES        ( 7 )

/* Indices of supported sampling rates */
#define AASRC_SAMP_RATE_8KHZ_IDX    ( 0 )
#define AASRC_SAMP_RATE_16KHZ_IDX   ( 1 )
#define AASRC_SAMP_RATE_22_05KHZ    ( 2 )
#define AASRC_SAMP_RATE_24KHZ       ( 3 )
#define AASRC_SAMP_RATE_32KHZ       ( 4 )
#define AASRC_SAMP_RATE_44_1KHZ     ( 5 )
#define AASRC_SAMP_RATE_48KHZ_IDX   ( 6 )

/* Provides translation of sampling rate in Hz to sampling rate index */
static const Int32 gsAasrcSampRateHzToIdx[AASRC_NUM_SAMP_RATES] = 
{
    SAMP_RATE_8KHZ, 
    SAMP_RATE_16KHZ, 
    SAMP_RATE_22_05KHZ, 
    SAMP_RATE_24KHZ, 
    SAMP_RATE_32KHZ, 
    SAMP_RATE_44_1KHZ, 
    SAMP_RATE_48KHZ
};

/* Nominal phase increment table
Rows: {8, 16, 22.05, 24, 32, 44.1, 48}
Cols: {8, 16, 22.05, 24, 32, 44.1, 48}
Entries are row(i)/col(j) in S32Q16
*/
static const Int32 gsAasrcNomPhsIncr[AASRC_NUM_SAMP_RATES][AASRC_NUM_SAMP_RATES] = 
{
    0x00010000, 0x00008000, 0x00005CE1, 0x00005555, 0x00004000, 0x00002E70, 0x00002AAA,
    0x00020000, 0x00010000, 0x0000B9C2, 0x0000AAAA, 0x00008000, 0x00005CE1, 0x00005555,
    0x0002C199, 0x000160CC, 0x00010000, 0x0000EB33, 0x0000B066, 0x00008000, 0x00007599,
    0x00030000, 0x00018000, 0x000116A3, 0x00010000, 0x0000C000, 0x00008B51, 0x00008000,
    0x00040000, 0x00020000, 0x00017384, 0x00015555, 0x00010000, 0x0000B9C2, 0x0000AAAA,
    0x00058333, 0x0002C199, 0x00020000, 0x0001D666, 0x000160CC, 0x00010000, 0x0000EB33,
    0x00060000, 0x00030000, 0x00022D47, 0x00020000, 0x00018000, 0x000116A3, 0x00010000
};

Uint32 gDacSampRateHz;      /* current DAC sampling rate */
Uint32 gUsbPbSampRateHz;    /* current USB playback sampling rate */

ASRC_Obj pbAsrc;                    /* playback ASRC object */
ASRC_Handle hPbAsrc = &pbAsrc;      /* playback ASRC handle */
#pragma DATA_SECTION(pbAsrcInFifo, ".pbAsrcInFifo");
#pragma DATA_ALIGN(pbAsrcInFifo, 2);
Int16 pbAsrcInFifo[ASRC_NUM_CH_STEREO*PB_ASRC_IN_FIFO_SZ]; /* playback ASRC input FIFO */
#pragma DATA_SECTION(pbAsrcHbCircBuf, ".pbAsrcHbCircBuf");
#pragma DATA_ALIGN(pbAsrcHbCircBuf, 2);
Int16 pbAsrcHbCircBuf[ASRC_NUM_CH_STEREO*ASRC_DEF_NUM_TAPS_DECBY2]; /* circular buffer for decimation by 2 */

Uint16 gPbAsrcNomOutTransSz;    /* playback ASRC nominal output transacion size */

/* Playback ASRC output FIFO */
Int16 pbAsrcOutFifo[ASRC_OUT_FIFO_NUM_BLK][ASRC_OUT_FIFO_BLK_SIZE];
/* Playback ASRC number of output samples in each output FIFO block */
Uint16 pbAsrcOutFifoBlkNumSamps[ASRC_OUT_FIFO_NUM_BLK];
/* Playback ASRC output sample count for current output block */
Uint16 pbAsrcOutFifoOutBlkSampCnt;
/* Playback ASRC index of output FIFO input block */
Uint16 pbAsrcOutFifoInBlk;
/* Playback ASRC index of output FIFO output block */
Uint16 pbAsrcOutFifoOutBlk;
Uint16 pbAsrcOutFifoInError = 0;
Uint16 pbAsrcOutFifoOutError = 0;

#ifdef ENABLE_REC_ASRC
Uint32 gAdcSampRateHz;      /* current ADC sampling rate */
Uint32 gUsbRecSampRateHz;   /* current USB record sampling rate */

ASRC_Obj recAsrc;                   /* record ASRC object */
ASRC_Handle hRecAsrc = &recAsrc;    /* record ASRC handle */
#pragma DATA_SECTION(recAsrcInFifo, ".recAsrcInFifo");
#pragma DATA_ALIGN(recAsrcInFifo, 2);
Int16 recAsrcInFifo[ASRC_NUM_CH_MONO*REC_ASRC_IN_FIFO_SZ]; /* record ASRC input FIFO */
#pragma DATA_SECTION(recAsrcHbCircBuf, ".recAsrcHbCircBuf");
#pragma DATA_ALIGN(recAsrcHbCircBuf, 2);
Int16 recAsrcHbCircBuf[ASRC_NUM_CH_MONO*ASRC_DEF_NUM_TAPS_DECBY2]; /* circular buffer for decimation by 2 */

Uint16 gRecAsrcNomOutTransSz;   /* record ASRC nominal output transacion size */

volatile Uint16 gRecFrameCnt;
volatile Uint16 gRecNumFrames;

#endif /* ENABLE_REC_ASRC */

/* Pitch calculation variables */
Uint16 dmaChanNum;              /* DMA channel number for Left Channel Tx DMA (0-15) */
Uint16 dmaBaseAddr;             /* 16 LSBs Left Channel Tx DMA source base address */
Uint16 dmaFrameLenSamps;        /* DMA frame length in samples */
volatile ioport Uint16 *dmaStartAddrReg; /* I/O space address of Left Channel Tx DMA Source Start Address register */
volatile Int16 dmaCXferCnt;     /* current count of completed DMA transfers between SOF's for Left Channel Tx DMA */
Uint16 numCDmaXfer;             /* number of completed DMA transfers between SOF's for Left Channel Tx DMA */
Uint16 dmaCurAddr;              /* 16 LSBs Left Channel Tx DMA source address */
Bool firstSof;                  /* indicates whether to initiate DMA sample count computation */
Uint16 dmaCurCnt;               /* DMA transfer count in frame for current SOF */
Uint16 dmaPrevCnt;              /* DMA transfer count in frame for previous SOF */
volatile Uint16 i2sRxSampCnt;   /* Rx sample count */

/* Initializes ASRC - called at power-up and whenever sample rate changes */
void initAsrc(
    ASRC_Handle hAsrc, 
    Uint16 numCh,               /* number of channels: 0 => mono, 1 => stereo */
    Int16 *inFifo,              /* input FIFO */
    Uint16 inFifoSz,            /* input FIFO size (one channel) */
    Uint32 inSampRateHz,        /* input sampling rate (Hz) */
    Uint32 outSampRateHz,       /* output sampling rate (Hz) */
    Uint32 transRateHz,         /* transaction rate (Hz) */
    Int16 *asrcHbCircBuf,       /* ASRC halfband circular buffer */
    Uint16 *pNomOutTransSz      /* address of nominal output trasaction size */
)
{
    Uint16 inSampRateIdx;
    Uint16 outSampRateIdx;
    Int32 nomPhaseIncr;
    Uint16 nomInTransSz;
    ASRC_Prms asrcPrms;
    Uint16 nomOutTransSz;

    /* Get nominal phase increment */
    /* Look up index for input sampling frequency */
    getValIdx(inSampRateHz, (Int32 *)gsAasrcSampRateHzToIdx, 
        AASRC_NUM_SAMP_RATES, &inSampRateIdx);
    /* Look up index for output sampling frequency */
    getValIdx(outSampRateHz, (Int32 *)gsAasrcSampRateHzToIdx, 
        AASRC_NUM_SAMP_RATES, &outSampRateIdx);
    nomPhaseIncr = gsAasrcNomPhsIncr[inSampRateIdx][outSampRateIdx];

    /* Compute nominal input transaction size */
    nomInTransSz = inSampRateHz/transRateHz; // FL: check this
    /* Compute nominal output transaction size */
    nomOutTransSz = outSampRateHz/transRateHz; // FL: check this

    /* Initialize ASRC object */
    asrcPrms.inNumCh = numCh;
    asrcPrms.inFifo = inFifo;
    asrcPrms.inFifoSz = inFifoSz;
    asrcPrms.nomPhaseIncr = nomPhaseIncr;
    asrcPrms.nomTransSz = nomInTransSz;
    asrcPrms.inFmt = ASRC_IN_FMT_INTERLEAVED;
    asrcPrms.k0 = (Uint32)ASRC_DEF_K0;
    asrcPrms.k1 = (Uint32)ASRC_DEF_K1;
    asrcPrms.numTapsPerPhase = ASRC_DEF_NUM_TAPS_PER_PHASE;
    asrcPrms.numPhases = ASRC_DEF_NUM_PHASES;
    asrcPrms.coefs16b = (Int16 *)asrcDefCoefs16b;

    /* Initialize halfband filter */
    asrcPrms.enable_decby2 = (inSampRateHz < 2*outSampRateHz) ? 0 : 1;
    asrcPrms.cirBuf = asrcHbCircBuf;
    asrcPrms.numTaps_decby2 = ASRC_DEF_NUM_TAPS_DECBY2;
    asrcPrms.coefs16b_decby2_mod = (Int16*)asrcDefCoefs16b_decby2_mod;

    ASRC_init(hAsrc, &asrcPrms);

    *pNomOutTransSz = nomOutTransSz;
}

/* Resets ASRC output FIFO */
void resetAsrcOutFifo(
    Int16 asrcOutFifo[][ASRC_OUT_FIFO_BLK_SIZE], /* output FIFO */
    Uint16 asrcOutFifoBlkNumSamps[],        /* # samples in each output FIFO block */
    Uint16 *pAsrcOutFifoInBlk,              /* output FIFO input block */
    Uint16 *pAsrcOutFifoOutBlk,             /* output FIFO output block */
    Uint16 *pAsrcOutFifoOutBlkSampCnt,      /* output FIFO output block sample count */
    Uint16 numCh,                           /* # channels */
    Uint16 nomOutTransSz                    /* nominal output transaction size */
)
{
    /* Clear output FIFO */
    memset(asrcOutFifo, 0, ASRC_OUT_FIFO_NUM_BLK * ASRC_OUT_FIFO_BLK_SIZE);

    /* Set # samples in each output FIFO block to nominal output transaction size */
    memset(asrcOutFifoBlkNumSamps, numCh*nomOutTransSz, ASRC_OUT_FIFO_NUM_BLK/2);

    /* Initialize input block number */
    //*pAsrcOutFifoInBlk = ASRC_OUT_FIFO_NUM_BLK/2;
    *pAsrcOutFifoInBlk = 2;

    /* Initialize output block number */
    *pAsrcOutFifoOutBlk = 0;

    /* Clear output block sample count */
    *pAsrcOutFifoOutBlkSampCnt = 0;
}

/* Combines multiple ASRC output frames into single output frame */
Int16 combineAsrcOutput(
    Int16 asrcOutFifo[][ASRC_OUT_FIFO_BLK_SIZE], /* output FIFO */
    Uint16 asrcOutFifoBlkNumSamps[],    /* # samples in each output FIFO block */
    Uint16 asrcOutFifoInBlk,            /* output FIFO input block */
    Uint16 *pAsrcOutFifoOutBlk,         /* output FIFO output block */
    Uint16 *pAsrcOutFifoOutBlkSampCnt,  /* output FIFO output block sample count */
    Uint16 numCh,                       /* # channels */
    Uint16 outFmt,                      /* output stream format: 0 => split buffer, 1 => interleaved */
    Int16 *outFrame,                    /* output frame */
    Int16 *outFrameR,                   /* right ch. output frame for stereo, output format unpacked */
    Uint16 outFrameNumSamps             /* # samples in output frame (one channel) */
)
{
    Uint16 curBlk;
    Uint16 outBlkSampCnt;
    Uint16 sampCnt;
    Uint16 sampCntThresh;
    Uint16 endBlk;
    Int16 *pOutFrameL;
    Int16 *pOutFrameR;
    Uint16 outSampInc;
    Uint16 addSampCnt;
    Int16 *pAsrcOutSamp;
    Uint16 curBlkNumSamps;
    Uint16 i;
    Int16 status = AASRC_SOK;


    outBlkSampCnt = *pAsrcOutFifoOutBlkSampCnt;

    /* Check if required number of output samples available in ASRC Output FIFO */
    curBlk = *pAsrcOutFifoOutBlk;
    sampCnt = asrcOutFifoBlkNumSamps[curBlk] - outBlkSampCnt;
    sampCntThresh = numCh * outFrameNumSamps;
    curBlk = (curBlk + 1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
    while ((curBlk != asrcOutFifoInBlk) && (sampCnt < sampCntThresh))
    {
        sampCnt += asrcOutFifoBlkNumSamps[curBlk];
        curBlk = (curBlk + 1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
    }

    if (sampCnt >= sampCntThresh) /* samples available */
    {
        endBlk = curBlk;
        curBlk = *pAsrcOutFifoOutBlk;
        pOutFrameL = &outFrame[0];
        addSampCnt = outFrameNumSamps;

        if ((outFmt != CMBASRC_OUTFMT_UNPK) && (numCh == 2)) /* stereo, interleaved or split buffer output */
        {
            if (outFmt == CMBASRC_OUTFMT_INT)
            {
                pOutFrameR = pOutFrameL+1;
                outSampInc = 2;
            }
            else if (outFmt == CMBASRC_OUTFMT_SBUF)
            {
                pOutFrameR = pOutFrameL+outFrameNumSamps;
                outSampInc = 1;
            }
            while (curBlk != endBlk)
            {
                curBlkNumSamps = (asrcOutFifoBlkNumSamps[curBlk]-outBlkSampCnt)>>1; /* # samples for mono */
                pAsrcOutSamp = &asrcOutFifo[curBlk][outBlkSampCnt];
                if (addSampCnt >= curBlkNumSamps) /* consume entire block */
                {
                    for (i = 0; i < curBlkNumSamps; i++)
                    {
                        /* Left ch. */
                        *pOutFrameL = *pAsrcOutSamp;
                        pAsrcOutSamp++;
                        pOutFrameL += outSampInc;

                        /* Right ch. */
                        *pOutFrameR = *pAsrcOutSamp;
                        pAsrcOutSamp++;
                        pOutFrameR += outSampInc;
                    }
                    addSampCnt -= curBlkNumSamps;
                    outBlkSampCnt = 0;
                }
                else /* consume partial block */
                {
                    for (i = 0; i < addSampCnt; i++)
                    {
                        /* Left ch. */
                        *pOutFrameL = *pAsrcOutSamp;
                        pAsrcOutSamp++;
                        pOutFrameL += outSampInc;

                        /* Right ch. */
                        *pOutFrameR = *pAsrcOutSamp;
                        pAsrcOutSamp++;
                        pOutFrameR += outSampInc;
                    }
                    //asrcOutputFifoBlkNumSamps[curBlk] = 2*(curBlkNumSamps - addSampCnt);
                    outBlkSampCnt = 2*addSampCnt;
                }

                curBlk = (curBlk + 1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
            }

            *pAsrcOutFifoOutBlk = (outBlkSampCnt == 0) ? endBlk : (endBlk-1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
            *pAsrcOutFifoOutBlkSampCnt = outBlkSampCnt;
        }
        else if ((outFmt == CMBASRC_OUTFMT_UNPK) && (numCh == 2)) /* stereo, unpacked output */
        {
            pOutFrameR = outFrameR;

            while (curBlk != endBlk)
            {
                curBlkNumSamps = (asrcOutFifoBlkNumSamps[curBlk]-outBlkSampCnt)>>1; /* # samples for mono */
                pAsrcOutSamp = &asrcOutFifo[curBlk][outBlkSampCnt];
                if (addSampCnt >= curBlkNumSamps) /* consume entire block */
                {
                    for (i = 0; i < curBlkNumSamps; i++)
                    {
                        /* Left ch. */
                        *pOutFrameL = *pAsrcOutSamp;
                        pOutFrameL++;
                        *pOutFrameL = 0;
                        pOutFrameL++;
                        pAsrcOutSamp++;

                        /* Right ch. */
                        *pOutFrameR = *pAsrcOutSamp;
                        pOutFrameR++;
                        *pOutFrameR = 0;
                        pOutFrameR++;
                        pAsrcOutSamp++;
                    }
                    addSampCnt -= curBlkNumSamps;
                    outBlkSampCnt = 0;
                }
                else /* consume partial block */
                {
                    for (i = 0; i < addSampCnt; i++)
                    {
                        /* Left ch. */
                        *pOutFrameL = *pAsrcOutSamp;
                        pOutFrameL++;
                        *pOutFrameL = 0;
                        pOutFrameL++;
                        pAsrcOutSamp++;

                        /* Right ch. */
                        *pOutFrameR = *pAsrcOutSamp;
                        pOutFrameR++;
                        *pOutFrameR = 0;
                        pOutFrameR++;
                        pAsrcOutSamp++;
                    }
                    //asrcOutputFifoBlkNumSamps[curBlk] = 2*(curBlkNumSamps - addSampCnt);
                    outBlkSampCnt = 2*addSampCnt;
                }

                curBlk = (curBlk + 1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
            }

            *pAsrcOutFifoOutBlk = (outBlkSampCnt == 0) ? endBlk : (endBlk-1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
            *pAsrcOutFifoOutBlkSampCnt = outBlkSampCnt;
        }
        else if ((outFmt != CMBASRC_OUTFMT_UNPK) && (numCh == 1)) /* mono, packed output */
        {
            while (curBlk != endBlk)
            {
                curBlkNumSamps = asrcOutFifoBlkNumSamps[curBlk]-outBlkSampCnt;
                pAsrcOutSamp = &asrcOutFifo[curBlk][outBlkSampCnt];
                if (addSampCnt >= curBlkNumSamps) /* consume entire block */
                {
                    for (i = 0; i < curBlkNumSamps; i++)
                    {
                        *pOutFrameL = *pAsrcOutSamp;
                        pAsrcOutSamp++;
                        pOutFrameL++;
                    }
                    addSampCnt -= curBlkNumSamps;
                    outBlkSampCnt = 0;
                }
                else /* consume partial block */
                {
                    for (i = 0; i < addSampCnt; i++)
                    {
                        *pOutFrameL = *pAsrcOutSamp;
                        pAsrcOutSamp++;
                        pOutFrameL++;
                    }
                    //asrcOutputFifoBlkNumSamps[curBlk] = 2*(curBlkNumSamps - addSampCnt);
                    outBlkSampCnt = addSampCnt;
                }

                curBlk = (curBlk + 1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
            }

            *pAsrcOutFifoOutBlk = (outBlkSampCnt == 0) ? endBlk : (endBlk-1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
            *pAsrcOutFifoOutBlkSampCnt = outBlkSampCnt;
        }
        else if ((outFmt == CMBASRC_OUTFMT_UNPK) && (numCh == 1)) /* mono, unpacked output */
        {
            while (curBlk != endBlk)
            {
                curBlkNumSamps = asrcOutFifoBlkNumSamps[curBlk]-outBlkSampCnt;
                pAsrcOutSamp = &asrcOutFifo[curBlk][outBlkSampCnt];
                if (addSampCnt >= curBlkNumSamps) /* consume entire block */
                {
                    for (i = 0; i < curBlkNumSamps; i++)
                    {
                        *pOutFrameL = *pAsrcOutSamp;
                        pOutFrameL++;
                        *pOutFrameL = 0;
                        pOutFrameL++;
                        pAsrcOutSamp++;
                    }
                    addSampCnt -= curBlkNumSamps;
                    outBlkSampCnt = 0;
                }
                else /* consume partial block */
                {
                    for (i = 0; i < addSampCnt; i++)
                    {
                        *pOutFrameL = *pAsrcOutSamp;
                        pOutFrameL++;
                        *pOutFrameL = 0;
                        pOutFrameL++;
                        pAsrcOutSamp++;
                    }
                    //asrcOutputFifoBlkNumSamps[curBlk] = 2*(curBlkNumSamps - addSampCnt);
                    outBlkSampCnt = addSampCnt;
                }

                curBlk = (curBlk + 1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
            }

            *pAsrcOutFifoOutBlk = (outBlkSampCnt == 0) ? endBlk : (endBlk-1) & (ASRC_OUT_FIFO_NUM_BLK - 1);
            *pAsrcOutFifoOutBlkSampCnt = outBlkSampCnt;
        }
        else
        {
            return AASRC_INV_PRM;
        }
    }
    else /* not enough samples available */
    {
        status = AASRC_OUT_FIFO_UND;
    }

    return status;
}

/* Initializes pitch calculation */
Int16 initPitchCalc(
    DMA_ChanHandle hDmaTxLeft
)
{
    Uint16 dmaFrameLenWords;
    Uint32 pingPongBaseAddr, pingPongEndAddr;
    Uint16 dmaEngNum, dmaEngChanNum;
    CSL_DmaDrvRegsOvly dmaRegs;

    /* Compute DMA frame length in 16-bit words */
    /* Divide by 2: 2x for bytes */
    dmaFrameLenWords = (Uint16)(hDmaTxLeft->dataLen>>1);

    /* Check DMA ping/pong buffer contained in 32K data page */
    pingPongBaseAddr = (Uint32)hDmaTxLeft->srcAddr;
    pingPongEndAddr = pingPongBaseAddr + dmaFrameLenWords - 1;
    if ((pingPongBaseAddr & 0xF8000) == (pingPongEndAddr & 0xF8000))
    {
        /* Compute DMA frame length in samples */
        /* Divide by 4: 2x for unpacked, 2x for ping/pong, 
        i.e. assumes UNPACKED I2S mode and DMA HW PING/PONG */
        dmaFrameLenSamps = dmaFrameLenWords>>2;

        /* Get 15 LSBs of DMA ping/pong buffer */
        dmaBaseAddr = (Uint16)((Uint32)hDmaTxLeft->srcAddr & 0x7FFF);

        dmaEngNum = hDmaTxLeft->chanNum/DMA_ENGINE_COUNT;
        switch(dmaEngNum)
        {
        case DMA_ENGINE0:
            dmaRegs = CSL_DMA_REGS0;
            break;
        case DMA_ENGINE1:
            dmaRegs = CSL_DMA_REGS1;
            break;
        case DMA_ENGINE2:
            dmaRegs = CSL_DMA_REGS2;
            break;
        case DMA_ENGINE3:
            dmaRegs = CSL_DMA_REGS3;
            break;
        }

        dmaEngChanNum = hDmaTxLeft->chanNum - dmaEngNum*DMA_ENGINE_COUNT;
        switch (dmaEngChanNum)
        {
        case DMA_CHAN0:
            dmaStartAddrReg = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH0SADR0;
            break;
        case DMA_CHAN1:
            dmaStartAddrReg = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH1SADR0;
            break;
        case DMA_CHAN2:
            dmaStartAddrReg = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH2SADR0;
            break;
        case DMA_CHAN3:
            dmaStartAddrReg = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH3SADR0;
            break;
        }

        dmaChanNum = hDmaTxLeft->chanNum; /* init. DMA channel number, 0-15 */
        dmaCXferCnt = 0; /* init. DMA completed transfer count */
        firstSof = TRUE;
    }
    else
    {
        return AASRC_PCALC_INV_PPBUF; /* error -- ping/pong buffer spans 64K word boundary */
    }

    return AASRC_SOK;
}

/* Latches current DMA transfer state */
void latchDmaXferState(void)
{
    Uint16 ifrValue;

    /* Sample DMA source address */
    dmaCurAddr = *dmaStartAddrReg;
    /* Latch and clear DMA completed transfer count */
    numCDmaXfer = dmaCXferCnt;
    dmaCXferCnt = 0;

    ifrValue = CSL_DMAEVTINT_REGS->DMAIFR;
    if (((ifrValue>>dmaChanNum)&0x1) == 0x1)
    {
        /* Resample DMA source address */
        dmaCurAddr = *dmaStartAddrReg;
        /* Adjust DMA completed transfer count */
        numCDmaXfer += 1;
        /* Ajust DMA completed transfer count so current 
        completed transfer not counted more than once */
        dmaCXferCnt = -1;
    }
}

/* Compute DMA transfer count since last SOF */
Uint16 computeDmaXferCnt(void)
{
    Uint16 sampCnt;

    if (firstSof == FALSE)
    {           
        dmaPrevCnt = dmaCurCnt;

        /* Compute current DMA transer count in frame */
        dmaCurCnt = (dmaCurAddr>>1)-dmaBaseAddr; /* current address /2 since byte address */
        dmaCurCnt = dmaCurCnt>>1; /* count /2 since unpacked */
        if (dmaCurCnt >= dmaFrameLenSamps)
        {
            dmaCurCnt -= dmaFrameLenSamps;
        }

        /* Compute sample count since last SOF */
        sampCnt = numCDmaXfer*dmaFrameLenSamps - dmaPrevCnt + dmaCurCnt;
    }
    else
    {
        /* Compute current DMA transer count in frame */
        dmaCurCnt = (dmaCurAddr>>1)-dmaBaseAddr; /* current address /2 since byte address */
        dmaCurCnt = dmaCurCnt>>1; /* count /2 since unpacked */
        if (dmaCurCnt >= dmaFrameLenSamps)
        {
            dmaCurCnt -= dmaFrameLenSamps;
        }

        sampCnt = AASRC_PCALC_FIRST_SOF;

        firstSof = FALSE;
    }

    return sampCnt;
}

/* Calculates pitch */
void calcPitch(
    Uint16 dmaSampCnt
)
{
    if (usb_play_mode == TRUE)
    {
        /* Update phase */
        ASRC_wrtNumOutSamps(hPbAsrc, dmaSampCnt);
    }

    if (set_record_interface >= 2)
    {
        /* Compute record number of input samples for frame */
#if defined(SAMPLE_RATE_TX_48kHz) && defined(SAMPLE_RATE_RX_16kHz)  /* 48 kHz playback, but 16 kHz record => count every 3rd sample for record */
        i2sRxSampCnt += dmaSampCnt;                     /* add residual sample count from previous SOF to sample count from current SOF */
        gRecNumSamps = i2sRxSampCnt/3;
        i2sRxSampCnt = i2sRxSampCnt - 3*gRecNumSamps;   /* update residual sample count */
#else
        gRecNumSamps = dmaSampCnt;
#endif
#ifdef ENABLE_STEREO_RECORD
        gRecNumSamps <<= 1;
#endif

        /* Compute record number of output samples for frame */
#ifndef ENABLE_REC_ASRC
        gRecOutTransSz = gRecNumSamps;

#else
        if (gRecFrameCnt == 0)
        {
            gRecOutTransSz = gRecAsrcNomOutTransSz;
            ASRC_wrtNumOutSamps(hRecAsrc, gRecOutTransSz);
        }
        else if (gRecFrameCnt == (gRecNumFrames-1))
        {
            gRecOutTransSz = gRecAsrcNomOutTransSz+1;
            ASRC_wrtNumOutSamps(hRecAsrc, gRecOutTransSz);
        }

        gRecFrameCnt++;
        if (gRecFrameCnt >= gRecNumFrames)
        {
            gRecFrameCnt = 0;
        }
#endif
    }
}
