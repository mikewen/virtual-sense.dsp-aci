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
#include "app_asrc.h"
#include "VC5505_CSL_BIOS_cfg.h"
#include "psp_i2s.h"
#include "lcd_osd.h"

#include "main_config.h"
#include "circular_buffer.h"

#include "ff.h"
#include "make_wav.h"


FRESULT rc;
FATFS fatfs;			/* File system object */
FIL wav_file;
Uint16 step = 0;

extern unsigned char circular_buffer[PROCESS_BUFFER_SIZE];
extern Uint32 bufferInIdx; //logical pointer
extern Uint32 bufferOutIdx; //logical pointer





void DataSaveTask(void)
{
    // display the play audio message
    print_playaudio();

    LOG_printf(&trace, "\nMount a volume.\n");
    rc = f_mount(0, &fatfs);
    if(rc) LOG_printf(&trace, "Error mounting volume\n");


    rc = open_wave_file(&wav_file, "test1.wav", SAMP_RATE_48KHZ,SECONDS);
    if(rc) LOG_printf(&trace, "Error openin a new wav file %d\n",rc);
    clear_lcd();
    SEM_reset(&SEM_BufferFull,0);
    bufferOutIdx = 0;
    bufferInIdx = 0;
    while (1)
    {
        // wait on bufferIn ready semaphore
        SEM_pend(&SEM_BufferFull, SYS_FOREVER);
        //SEM_pend(&SEM_BufferInReady, SYS_FOREVER);
        if(step < (SECONDS * STEP_PER_SECOND)){
        	write_data_to_wave(&wav_file, &circular_buffer[bufferOutIdx], (RXBUFF_SZ_ADCSAMPS*2));
        	bufferOutIdx = ((bufferOutIdx + (RXBUFF_SZ_ADCSAMPS *2)) % PROCESS_BUFFER_SIZE);
        	//LOG_printf(&trace,  "consuming %d buffer \n",SEM_count(&SEM_BufferFull));
        	//printstring(".!");
        	
        }
        step++;
        if(step == (SECONDS * STEP_PER_SECOND)){
        	 close_wave_file(&wav_file);
        	 directory_listing();
             printstring("Program Terminated");
             LOG_printf(&trace,  "\n***Program has Terminated***\n" );
        }

    }


}

