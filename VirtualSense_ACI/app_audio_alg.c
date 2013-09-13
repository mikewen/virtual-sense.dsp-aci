/*
 * $$$MODULE_NAME app_audio_alg.c
 *
 * $$$MODULE_DESC app_audio_alg.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include <string.h>
#include <stdlib.h>
#include "dsplib.h"
#include "i2s_sample.h"
#include "app_globals.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_asrc.h"
#include "app_audio_alg.h"
#include "VC5505_CSL_BIOS_cfg.h"
#include "dbg_sdram.h"
#include "psp_i2s.h"

#include "main_config.h"





extern unsigned char circular_buffer[PROCESS_BUFFER_SIZE];
extern Uint32 bufferInIdx; //logical pointer


static Int16 recInLeftBuf[RXBUFF_SZ_ADCSAMPS];
//static Int16 recOutLeftBuf[RXBUFF_SZ_ADCSAMPS];

#ifdef ENABLE_STEREO_RECORD
static Int16 recInRightBuf[RXBUFF_SZ_ADCSAMPS];
//static Int16 recOutRightBuf[RXBUFF_SZ_ADCSAMPS];
#endif

#ifdef SAMPLE_BY_SAMPLE_PB
static Int16 tempPbOutBuf[MAX_I2S_TXBUFF_SZ];
#endif

/* Perform Record (Rx) audio algorithm processing */
void RecAudioAlgTsk(void)
{
    Uint16 *ptrRxLeft;
    //Int16 codec_input_sample_count;
    Uint16 i;



#ifdef ENABLE_STEREO_RECORD
    Uint16 *ptrRxRight;
#endif

    while (1)
    {
        SEM_pend(&SEM_PingPongRxLeftComplete, SYS_FOREVER);
#ifdef ENABLE_STEREO_RECORD
        SEM_pend(&SEM_PingPongRxRightComplete, SYS_FOREVER);
#endif
#ifdef DEBUG_LOG_PRINT
        //LOG_printf(&trace, "SEM_PingPong obtained");
#endif

        /* Get pointer to ping/pong buffer */
        ptrRxLeft = &ping_pong_i2sRxLeftBuf[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxLeft += I2S_RXBUFF_SZ;
        }
        left_rx_buf_sel ^= 0x1; /* update ping/pong */

#ifdef ENABLE_STEREO_RECORD
        /* Get pointer to right ping/pong buffer */
        ptrRxRight = &ping_pong_i2sRxRightBuf[0];
        if (right_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxRight+= I2S_RXBUFF_SZ;
        }
        right_rx_buf_sel ^= 0x1; /* update ping/pong */
#endif


        //wait if buffer is full
        SEM_pend(&SEM_BufferEmpty, SYS_FOREVER);
        LOG_printf(&trace,  "--- producing %d buffer \n",SEM_count(&SEM_BufferEmpty));
        /* Get data from ping/pong buffers */
        for (i = 0; i < RXBUFF_SZ_ADCSAMPS; i++)
        {
            // NOTE: since we need datapack to be disabled on I2S tx, we need it disabled on I2S rx therefore
            // we get 2 words per DMA transfer so the offset into DMA buffers has to be twice as big
            recInLeftBuf[i] = *ptrRxLeft;
            ptrRxLeft += 2;
            //if (bufferInIdx < PROCESS_BUFFER_SIZE)
            //{
               	   //recInLeftBuf[i] = generate_sinewave_2(1000, 10000);
            circular_buffer[bufferInIdx] =  (recInLeftBuf[i] & 0xFF);
            //circular_buffer[bufferInIdx] = ((circular_buffer[bufferInIdx] & 0xFF) << 8) + ((circular_buffer[bufferInIdx] >> 8) & 0xff);
            bufferInIdx = (bufferInIdx+1) % PROCESS_BUFFER_SIZE;
            circular_buffer[bufferInIdx] =  ((recInLeftBuf[i] >> 8) & 0xFF);
            bufferInIdx = (bufferInIdx+1) % PROCESS_BUFFER_SIZE;
            //SEM_post(&SEM_BufferInReady);
        	//}

            #if defined(SAMPLE_RATE_RX_16kHz) && defined(SAMPLE_RATE_I2S_48kHz)
            // DMA operates at 48KHz but sample rate is
            // set to 16kHz so store every third sample
            ptrRxLeft += 4;
            #endif

#ifdef ENABLE_STEREO_RECORD
            recInRightBuf[i] = *ptrRxRight;
            ptrRxRight += 2;

            #if defined(SAMPLE_RATE_RX_16kHz) && defined(SAMPLE_RATE_I2S_48kHz)
            // DMA operates at 48KHz but sample rate is
            // set to 16kHz so store every third sample
            ptrRxRight += 4;
            #endif
#endif
        }
        SEM_post(&SEM_BufferFull);
        /*                                 */
        /* Insert Record audio algorithm here */
        /*                                    */
        //memcpy(recOutLeftBuf, recInLeftBuf, RXBUFF_SZ_ADCSAMPS); /* dummy */


    }
}

