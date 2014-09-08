 /*
 * $$$MODULE_NAME i2s_sample.c
 *
 * $$$MODULE_DESC i2s_sample.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   i2s_sample.c
 *
 *  \brief  I2S application program
 *
 *  I2S sample test file which defines the interfaces for testing the
 *  uasability of the I2S driver.
 *
 *  (C) Copyright 2004, Texas Instruments, Inc
 *
 *  \author        PR Mistral
 *  \version    1.0   Initial implementation
 *  \version    1.1   Modified for review comments
 */

#include <string.h>
//#include "psp_i2s.h"
#include "dda_i2s.h"
#include "psp_dma.h"
#include "dda_dma.h"
#include "csl_intc.h"    /* SampleBySample */
#include "app_globals.h"
#include "asrc_dly_fix.h" 
#include "app_asrc.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_usbac_descs.h"
#include "i2s_sample.h"
#include "gpio_control.h"
#include "VirtualSense_ACIcfg.h"

#include <clk.h>

#include "dbg_sdram.h"

#include "main_config.h"
#include "circular_buffer.h"

//extern Uint16 bufferInIdx; //logical pointer

/* I2S handles */
PSP_Handle       i2sHandleTx;
PSP_Handle       i2sHandleRx;

Uint16    fsError1 = 0;    /**< FSYNC gobal parameter                 */
Uint16    ouError1 = 0;    /**< under/over run global parameter    */
Int16 sample;
static Int16 artificialValue = 0;
//Uint16 outwrap = 0;

extern Uint32 bufferInIdx;// = 0; //logical pointers
extern Uint16 in_record;
extern Int32 bufferInside;
extern void putDataIntoOpenFile(const void *buff, unsigned int number_of_bytes);
extern Int16 circular_buffer[PROCESS_BUFFER_SIZE];

DMA_ChanHandle   hDmaTxLeft;
DMA_ChanHandle   hDmaTxRight;
DMA_ChanHandle   hDmaRxLeft;
DMA_ChanHandle   hDmaRxRight;

#ifdef ENABLE_RECORD
/* Codec input ping/pong buffer (Left ch.) */
#pragma DATA_SECTION(my_i2sRxLeftBuf, ".my_i2sRxLeftBuf");
#pragma DATA_ALIGN(my_i2sRxLeftBuf, 2);
Uint16 my_i2sRxLeftBuf[2*DMA_TARNSFER_SZ]; /* 2x for ping/pong */
Uint16 my_samples[DMA_BUFFER_SZ];
Int16 left_rx_buf_sel = 0x0;

/* Codec input ping/pong buffer (Right ch.) */
//#pragma DATA_SECTION(my_i2sRxRightBuf, ".my_i2sTxLeftBuf");
//#pragma DATA_ALIGN(my_i2sRxRightBuf, 2);
//Uint16 my_i2sRxRightBuf[2*DMA_TARNSFER_SZ]; /* 2x for ping/pong */ //LELE IS NOT USED
Int16 right_rx_buf_sel = 0x0;

// codec input buffer
Uint16 codec_input_buffer[CODEC_INPUT_BUFFER_SIZE];
//codec input buffer input index
Uint16 codec_input_buffer_input_index;
//codec input buffer output index
Uint16 codec_input_buffer_output_index;
Uint32 codec_input_buffer_overflow = 0;
Uint32 codec_input_buffer_underflow = 0;

#endif  // ENABLE_RECORD

//extern unsigned char cicular_buffer[PROCESS_BUFFER_SIZE];
//extern Uint32 bufferInIdx; //logical pointer

static Int16 recInLeftBuf = 0;

/* Codec output ping/pong buffer (Left ch.) */
/* NOTE: Left & Right interleaved channels for sample-by-sample playback */
#pragma DATA_SECTION(my_i2sTxLeftBuf, ".my_i2sTxLeftBuf");
#pragma DATA_ALIGN(my_i2sTxLeftBuf, 2);
Int16 my_i2sTxLeftBuf[2*DMA_TARNSFER_SZ]; /* 2x for ping/pong */

/* Codec output ping/pong buffer (Right ch.) */
#pragma DATA_ALIGN(my_i2sTxRightBuf, 2);
Int16 my_i2sTxRightBuf[2*DMA_TARNSFER_SZ]; /* 2x for ping/pong */

/* Output ping/pong buffer selection variable */
Int16 tx_buf_sel;
/* Run-time size of Tx ping/pong buffer */
Uint16 i2sTxBuffSz;

/* Pointer to current output ping/pong buffer */
Int16 *codec_output_buffer;
/* Current output buffer sample count */
Uint16 codec_output_buf_samp_cnt;
/* Indicates whether codec output buffer available */
Bool codec_output_buffer_avail;
Uint16 codec_output_buffer_out_error;

/* Zeros I2S buffers for Playback */
static void zeroI2sBuf(
    Bool enableStereo, 
    Uint16 pingPongBufSz, 
    Int16 *pingPongI2sLeftBuf, 
    Int16 *pingPongI2sRightBuf
);

void i2sTxRxInit(void);
void i2sTxRxStart(void);

Int16 i2sPlayAudio(PSP_Handle        i2sHandle,
                   Uint32            *i2sNextTxLeftBuf,
                   Uint32            *i2sNextTxRightBuf
                   );

Int16 i2sReceiveData(PSP_Handle       i2sHandle,
                     Uint32           *i2sNextRxLeftBuf,
                     Uint32           *i2sNextRxRightBuf
                     );
Int16 i2sStopTransfer(PSP_Handle    i2sHandle);
void DDC_I2S_write(DDC_I2SHandle hI2s, Uint16 *writeBuff, Uint16 buffLen);

/* Initializes I2S and associated DMA channels for Playback and Record */
Int16 i2sInit(
    I2sInitPrms *pI2sInitPrms
)
{
    PSP_I2SOpMode opMode;
    PSP_I2SConfig i2sTxConfig;
    PSP_DMAConfig dmaTxConfig;
    PSP_I2SConfig i2sRxConfig;
    PSP_DMAConfig dmaRxConfig;

    if (pI2sInitPrms->enablePlayback == TRUE)
    {
        /* Initialize I2S instance for Playback */
        i2sTxConfig.dataformat  = PSP_I2S_DATAFORMAT_LJUST;
        i2sTxConfig.i2sMode     = PSP_I2S_SLAVE;
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_16;
        i2sTxConfig.signext     = PSP_I2S_SIGNEXT_ENABLE;
        i2sTxConfig.datapack    = PSP_I2S_DATAPACK_DISABLE;
        i2sTxConfig.datadelay   = PSP_I2S_DATADELAY_ONEBIT;
        i2sTxConfig.clk_pol     = PSP_I2S_FALLING_EDGE;
        i2sTxConfig.fsync_pol   = PSP_I2S_FSPOL_LOW;
        i2sTxConfig.loopBack    = PSP_I2S_LOOPBACK_DISABLE;
        i2sTxConfig.datatype    = PSP_I2S_MONO_DISABLE;
        i2sTxConfig.fsdiv       = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sTxConfig.clkdiv      = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        if (pI2sInitPrms->sampleBySamplePb == TRUE)
        {
            opMode = PSP_I2S_INTERRUPT; /* change from PSP_DMA_INTERRUPT to PSP_I2S_INTERRUPT SampleBySample */
        }
        else
        {
            opMode = PSP_DMA_INTERRUPT;
        }

        i2sHandleTx = I2S_INIT(pI2sInitPrms->i2sPb, PSP_I2S_TRANSMIT, 
            PSP_I2S_CHAN_STEREO, opMode, &i2sTxConfig, NULL);
        if (i2sHandleTx == NULL)
        {
            return I2SSAMPLE_I2SINIT_PB_FAIL;
        }

        if (pI2sInitPrms->sampleBySamplePb == FALSE)
        {
            /* Initialize DMA channels for Playback */
            dmaTxConfig.pingPongMode    = TRUE;
            dmaTxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaTxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaTxConfig.chanDir         = PSP_DMA_WRITE;
            dmaTxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaTxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
            dmaTxConfig.dataLen         = (2*2*DMA_TARNSFER_SZ); /* bytes */
            dmaTxConfig.srcAddr         = (Uint32)pI2sInitPrms->pingPongI2sTxLeftBuf;
            dmaTxConfig.callback        = I2S_DmaTxLChCallBack;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXLT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXLT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXLT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaTxLeft = I2S_DMA_INIT(i2sHandleTx, &dmaTxConfig);
            if (hDmaTxLeft == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }

            if (pI2sInitPrms->enableStereoPb)
            {
                /* Request and configure a DMA channel for right channel data */
                dmaTxConfig.srcAddr   = (Uint32)pI2sInitPrms->pingPongI2sTxRightBuf;
                dmaTxConfig.callback  = I2S_DmaTxRChCallBack;

                switch (pI2sInitPrms->i2sPb)
                {
                case PSP_I2S_0: 
                    dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXRT0;
                    break;
                case PSP_I2S_1:
                    dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXRT0;
                    break;
                case PSP_I2S_2:
                    dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXRT0;
                    break;
                case PSP_I2S_3:
                    dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXRT0;
                    break;
                default:
                    return I2SSAMPLE_INV_PRMS;
                }

                /* Request and configure a DMA channel for right data */
                hDmaTxRight  =  I2S_DMA_INIT(i2sHandleTx, &dmaTxConfig);
                if (hDmaTxRight == NULL)
                {
                    return I2SSAMPLE_DMAINIT_PB_FAIL;
                }
            }

            /* Zero buffers */
            zeroI2sBuf(pI2sInitPrms->enableStereoPb, 
                2*MAX_I2S_TXBUFF_SZ, 
                pI2sInitPrms->pingPongI2sTxLeftBuf, 
                pI2sInitPrms->pingPongI2sTxRightBuf);
        }
        else
        {
            /* Zero buffers */
            zeroI2sBuf(FALSE, 
                2*MAX_I2S_TXBUFF_SZ, 
                pI2sInitPrms->pingPongI2sTxLeftBuf, 
                NULL);
        }
    }

    if (pI2sInitPrms->enableRecord == TRUE)
    {
        /* Initialize I2S instance for Record */
        i2sRxConfig.dataformat   = PSP_I2S_DATAFORMAT_LJUST;
        i2sRxConfig.i2sMode      = PSP_I2S_SLAVE;
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_16;
        i2sRxConfig.signext      = PSP_I2S_SIGNEXT_ENABLE;
        i2sRxConfig.datapack     = PSP_I2S_DATAPACK_DISABLE;
        i2sRxConfig.datadelay    = PSP_I2S_DATADELAY_ONEBIT;
        i2sRxConfig.clk_pol      = PSP_I2S_FALLING_EDGE;
        i2sRxConfig.fsync_pol    = PSP_I2S_FSPOL_LOW;
        i2sRxConfig.loopBack     = PSP_I2S_LOOPBACK_DISABLE;
        i2sRxConfig.datatype     = PSP_I2S_MONO_DISABLE;
        i2sRxConfig.fsdiv        = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sRxConfig.clkdiv       = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        i2sHandleRx = I2S_INIT(pI2sInitPrms->i2sRec, PSP_I2S_RECEIVE, 
            PSP_I2S_CHAN_STEREO, PSP_DMA_INTERRUPT, &i2sRxConfig, NULL);
        if (i2sHandleRx == NULL)
        {
            return I2SSAMPLE_I2SINIT_REC_FAIL;
        }

        /* Initialize DMA channels for Record */
        dmaRxConfig.pingPongMode    = TRUE;
        dmaRxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
        dmaRxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
        dmaRxConfig.chanDir         = PSP_DMA_READ;
        dmaRxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
        dmaRxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
        dmaRxConfig.dataLen         = 2*(2*DMA_TARNSFER_SZ); /* bytes */
        dmaRxConfig.destAddr        = (Uint32)pI2sInitPrms->pingPongI2sRxLeftBuf;
        dmaRxConfig.callback        = I2S_DmaRxLChCallBack;

        switch (pI2sInitPrms->i2sRec)
        {
        case PSP_I2S_0:
            dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_RX;
            dmaRxConfig.srcAddr     =  (Uint32)I2S0_I2SRXLT0;
            break;
        case PSP_I2S_1:
            dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_RX;
            dmaRxConfig.srcAddr     =  (Uint32)I2S1_I2SRXLT0;
            break;
        case PSP_I2S_2:
            dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_RX;
            dmaRxConfig.srcAddr     =  (Uint32)I2S2_I2SRXLT0;
            break;
        case PSP_I2S_3:
            dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_RX;
            dmaRxConfig.srcAddr     =  (Uint32)I2S3_I2SRXLT0;
            break;
        default:
            return I2SSAMPLE_INV_PRMS;
        }

        /* Request and configure a DMA channel for left channel data */
        hDmaRxLeft = I2S_DMA_INIT(i2sHandleRx, &dmaRxConfig);
        if (hDmaRxLeft == NULL)
        {
            return I2SSAMPLE_DMAINIT_REC_FAIL;
        }

        if (pI2sInitPrms->enableStereoRec)
        {
            /* Request and configure a DMA channel for right channel data */
            switch (pI2sInitPrms->i2sRec)
            {
            case PSP_I2S_0: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S0_I2SRXRT0;
                break;
            case PSP_I2S_1: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S1_I2SRXRT0;
                break;
            case PSP_I2S_2: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S2_I2SRXRT0;
                break;
            case PSP_I2S_3: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S3_I2SRXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            dmaRxConfig.destAddr = (Uint32)pI2sInitPrms->pingPongI2sRxRightBuf;
            dmaRxConfig.callback = I2S_DmaRxRChCallBack;

            /* Request and configure a DMA channel for right data */
            hDmaRxRight = I2S_DMA_INIT(i2sHandleRx, &dmaRxConfig);
            if (hDmaRxRight == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }
                }

        /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoRec, 
            2*I2S_RXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sRxLeftBuf, 
            pI2sInitPrms->pingPongI2sRxRightBuf);

    }

    return I2SSAMPLE_SOK;
}

/* Zeros I2S buffers */
void zeroI2sBuf(
    Bool enableStereo, 
    Uint16 pingPongBufSz, 
    Int16 *pingPongI2sLeftBuf, 
    Int16 *pingPongI2sRightBuf
)
{
    /* Clear left channel */
    memset(pingPongI2sLeftBuf, 0, pingPongBufSz);

    /* Clear right channel */
    if (enableStereo == TRUE)
    {
        memset(pingPongI2sRightBuf, 0, pingPongBufSz);
    }
}

/* Function to play an audio on I2S */
Int16 i2sPlayAudio(PSP_Handle        i2sHandle,
                   Uint32            *i2sNextTxLeftBuf,
                   Uint32            *i2sNextTxRightBuf
                  )
{
    Int16 status = PSP_E_INVAL_PARAM;

    if((i2sHandle != NULL)        &&
       (i2sNextTxLeftBuf != NULL) &&
       (i2sNextTxRightBuf != NULL))
    {
        status = I2S_TransmitData(i2sHandle, i2sNextTxLeftBuf, i2sNextTxRightBuf,
                                  hDmaTxLeft, hDmaTxRight);
    }

    return status;
}

/* Stops the I2S data transfer */
Int16 i2sStopTransfer(PSP_Handle    i2sHandle)
{
    Int16 status;

    if(i2sHandle != NULL)
    {
        /* Release the DMA channels */
        status   =  I2S_DMA_Deinit(hDmaTxLeft);
        status  |=  I2S_DMA_Deinit(hDmaTxRight);

        #ifdef ENABLE_RECORD
        status   =  I2S_DMA_Deinit(hDmaRxLeft);
       status  |=  I2S_DMA_Deinit(hDmaRxRight);
        #endif // ENABLE_RECORD

        /* Deinitialize the I2S instance */
        status  |=  I2S_DeInit(i2sHandle);
    }

    return status;
}

/*
 * interrupt routine to test output to I2S on a SampleBySample basis
 ***********************************************************************/
void i2s_txIsr(void) /* dispatcher used */
//interrupt void i2s_txIsr(void) /* no dispatcher used */
{ /* SampleBySample */
#ifdef SAMPLE_BY_SAMPLE_PB
    Int16     status;
#if defined(SAMPLE_RATE_TX_48kHz) && defined(SAMPLE_RATE_RX_16kHz)  /* 48 kHz playback, but 16 kHz record => count every 3rd sample for record */
    static Uint16 sample_cnt = 0;
#endif

    /* Get sample from output FIFO, place in I2S output */
    *(ioport volatile unsigned *)I2S_I2STXLT1 = codec_output_buffer[codec_output_buf_samp_cnt];     /* left */
    *(ioport volatile unsigned *)I2S_I2STXRT1 = codec_output_buffer[codec_output_buf_samp_cnt+1];   /* right */

    if (usb_play_mode == TRUE)
    {
        /* Accumulate phase */
        status = ASRC_accPhase(hPbAsrc);
        if (status != ASRC_SOK)
        {
            gPbAsrcError++;
#ifdef DEBUG_LOG_PRINT
            LOG_printf(&trace, "ERROR: ASRC_accPhase() failed: %d\n", status);
#endif
        }
    }

    codec_output_buf_samp_cnt += 2;

    /* Check if swapping ping/pong buffers */
    if (codec_output_buf_samp_cnt >= i2sTxBuffSz)
    {
        if (codec_output_buffer_avail == FALSE) /* check output FIFO underflow */
        {
            codec_output_buffer_out_error++;
#ifdef DEBUG_LOG_PRINT
            LOG_printf(&trace, "ERROR: codec output FIFO UNDERFLOW");
#endif
        }

        tx_buf_sel ^= 0x1;
        codec_output_buffer = ping_pong_i2sTxLeftBuf + tx_buf_sel*i2sTxBuffSz;
        codec_output_buf_samp_cnt = 0;
        codec_output_buffer_avail = FALSE;

        SEM_post(&SEM_PingPongTxLeftComplete);
    }

#if defined(SAMPLE_RATE_TX_48kHz) && defined(SAMPLE_RATE_RX_16kHz)  /* 48 kHz playback, but 16 kHz record => count every 3rd sample for record */
    if ((set_record_interface >= 2) && (sample_cnt >= 2))
    {
        i2sRxSampCnt++;
    }

    if (++sample_cnt >= 3)
    {
        sample_cnt = 0;
    }
#else
    if (set_record_interface >= 2) /* (48 kHz playback and 48 kHz record) or (16 kHz playback and 16 kHz record) */
    {
        i2sRxSampCnt++;
    }
#endif

#endif /* SAMPLE_BY_SAMPLE_PB */
}

/*
 * interrupt routine to test input from I2S on a SampleBySample basis
 ***********************************************************************/
void i2s_rxIsr(void) /* dispatcher used */
//interrupt void i2s_rxIsr(void) /* no dispatcher used */
{ /* SampleBySample */
}

/* Resets codec output buffer */
void reset_codec_output_buffer(void)
{
    //memset(my_i2sTxLeftBuf, 0, 2*I2SS_RXBUFF_SZ); /* 2x for ping/pong */
    //memset(my_i2sTxRightBuf, 0, 2*I2SS_RXBUFF_SZ); /* 2x for ping/pong */
    //codec_output_buffer = &my_i2sTxLeftBuf[0];
    tx_buf_sel = 0x0;
    codec_output_buf_samp_cnt = 0;
    codec_output_buffer_avail = TRUE;
    codec_output_buffer_out_error = 0;
}

/* SampleBySample
 * copy of CSL I2S_transEnable function to work with DDC_I2SObj type handle
 ***********************************************************************/
void DDC_I2S_transEnable(DDC_I2SHandle    hI2s, Uint16 enableBit)
{
    ioport    CSL_I2sDrvRegs      *localregs;

    localregs =     hI2s->regs;
    //localregs->SCRL = 0x2A00;

    if(enableBit == TRUE)
    {
        /*  Enable the transmit and receive bit */
        CSL_FINST(localregs->SCRL, I2S_I2SSCTRL_ENABLE, SET);
    }
    else
    {
        /*  Disable the transmit and receive bit */
        CSL_FINST(localregs->SCRL, I2S_I2SSCTRL_ENABLE, CLEAR);
    }
}

/* SampleBySample
 * copy of CSL I2S_write function to work with DDC_I2SObj type handle
 ***********************************************************************/
void DDC_I2S_write(DDC_I2SHandle    hI2s,
                Uint16 *writeBuff, Uint16 buffLen)
{
    ioport    CSL_I2sDrvRegs      *localregs;
    Uint16    i2sIrStatus;

    if((NULL == writeBuff) || (0 == buffLen))
    {
        return;
    }

    localregs = hI2s->regs;

        while(buffLen > 0)
        {
            /* Copy data from local buffer to transmit register  */
            localregs->TRW0M = *writeBuff++;
            if(hI2s->chanType == PSP_I2S_CHAN_STEREO) //I2S_CHAN_STEREO)
            {
                localregs->TRW1M = *writeBuff++;
                buffLen -= 1;
            }
            buffLen -= 1;
        }
        // check for errors
        i2sIrStatus = localregs->IRL;

        if(i2sIrStatus & CSL_I2S_I2SINTFL_FERRFL_MASK)
        {
            fsError1++;
        }

        if(i2sIrStatus & CSL_I2S_I2SINTFL_OUERRFL_MASK)
        {
            ouError1++;
        }
}

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
#if 1 //LELE 26-09-13
void I2S_DmaTxLChCallBack(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
)
{

    //dmaCXferCnt++;



    if ((dataCallback != NULL) && (dmaStatus == PSP_DMA_TRANSFER_COMPLETE))

    {
        SEM_post(&SEM_PingPongTxLeftComplete);
    }
    else
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "Left TX DMA Failed");
#endif
    }
}
#endif

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxRChCallBack(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
)
{
    if ((dataCallback != NULL) && (dmaStatus == PSP_DMA_TRANSFER_COMPLETE))
    {
        SEM_post(&SEM_PingPongTxRightComplete);
    }
    else
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "Right TX DMA Failed");
#endif
    }
}

/**
 *   \brief Call back function for DMA left channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxLChCallBack(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
)
{
        Uint16 *ptrRxLeft;
        Uint16 i;



#ifdef ENABLE_RECORD
    if ((dataCallback != NULL) && (dmaStatus == PSP_DMA_TRANSFER_COMPLETE))
    {
        /* Get pointer to ping/pong buffer */
        ptrRxLeft = &my_i2sRxLeftBuf[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
                /* this buffer has data to be processed */
            ptrRxLeft += DMA_TARNSFER_SZ;
        }
        left_rx_buf_sel ^= 0x1; /* update ping/pong */
        // copy data to the

        for (i = 0; i < DMA_BUFFER_SZ; i++)
        {
            // NOTE: since we need datapack to be disabled on I2S tx, we need it disabled on I2S rx therefore
            // we get 2 words per DMA transfer so the offset into DMA buffers has to be twice as big


            if(in_record && (bufferInside < PROCESS_BUFFER_SIZE/*/2*/)){

            	recInLeftBuf = *ptrRxLeft;
				artificialValue++;//generate_sinewave_2(16000,15000);//40; // to compensate filter b 20dB attenuation (TODO add only if 192khz sampling rate)
				//recInLeftBuf = artificialValue++;//generate_sinewave_2(8000,16000);//40; // to compensate filter b 20dB attenuation (TODO add only if 192khz sampling rate)
				if(frequency == 192000)
					recInLeftBuf *= digital_gain; // to compensate filter b 20dB attenuation (TODO add only if 192khz sampling rate)
				ptrRxLeft += 2;

            	circular_buffer[bufferInIdx] = recInLeftBuf; //LELE test for byte
            	bufferInIdx = ((bufferInIdx+1) % PROCESS_BUFFER_SIZE);
            	/*
            	circular_buffer[bufferInIdx] =  (recInLeftBuf & 0xFF);
            	bufferInIdx = ((bufferInIdx+1) % PROCESS_BUFFER_SIZE);
            	circular_buffer[bufferInIdx] =  ((recInLeftBuf >> 8) & 0xFF);
            	bufferInIdx = ((bufferInIdx+1) % PROCESS_BUFFER_SIZE); */
            	bufferInside++;
            }
        }
       	//if(bufferInside >= PROCESS_BUFFER_SIZE/*/2*/)
        //putDataIntoOpenFile((void *)circular_buffer, PROCESS_BUFFER_SIZE);
    }
    else
    {
    	printf("Left RX DMA Failed");
    }
    //artificialValue++;

#endif // ENABLE_RECORD
}

/**
 *   \brief Call back function for DMA right channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxRChCallBack(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
)
{
#ifdef ENABLE_STEREO_RECORD
    if ((dataCallback != NULL) && (dmaStatus == PSP_DMA_TRANSFER_COMPLETE))
    {
        SEM_post(&SEM_PingPongRxRightComplete);
    }
    else
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "Right RX DMA Failed");
#endif
    }

#endif // ENABLE_STEREO_RECORD
}
