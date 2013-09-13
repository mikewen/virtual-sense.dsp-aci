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
#include <stdio.h>
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
Uint16 file_counter = 0;
char name[12];
extern unsigned char circular_buffer[PROCESS_BUFFER_SIZE];
extern Uint32 bufferInIdx; //logical pointer
extern Uint32 bufferOutIdx; //logical pointer

// PRD function. Runs every 10 minutes to start sampling a new file
void CreateNewFile(void){
	LOG_printf(&trace, "Timer executes\n");
	SEM_post(&SEM_TimerSave);
}


void DataSaveTask(void)
{
    // display the play audio message
    //print_playaudio();

    LOG_printf(&trace, "\nMount a volume.\n");
    rc = f_mount(0, &fatfs);
    if(rc) LOG_printf(&trace, "Error mounting volume\n");
    //main loop
    while (1)
    {
    	//wait on semaphore released from a timer function
    	SEM_pend(&SEM_TimerSave, SYS_FOREVER);
    	sprintf(name, "test%d.wav",file_counter);
    	clear_lcd();
    	printstring("Creating   ");
    	printstring(name);
    	rc = open_wave_file(&wav_file, name, SAMP_RATE_48KHZ,SECONDS);
    	if(rc) LOG_printf(&trace, "Error openin a new wav file %d\n",rc);
    	//clear_lcd();
    	SEM_reset(&SEM_BufferFull,0);
    	bufferOutIdx = 0;
    	bufferInIdx = 0;
    	while (step < (SECONDS * STEP_PER_SECOND))
    	{
    		// wait on bufferIn ready semaphore
    		SEM_pend(&SEM_BufferFull, SYS_FOREVER);

    		write_data_to_wave(&wav_file, &circular_buffer[bufferOutIdx], (RXBUFF_SZ_ADCSAMPS*2));
    		bufferOutIdx = ((bufferOutIdx + (RXBUFF_SZ_ADCSAMPS *2)) % PROCESS_BUFFER_SIZE);
        	//LOG_printf(&trace,  "consuming %d buffer \n",SEM_count(&SEM_BufferFull));
        	//printstring(".!");
    		step++;
    	}
    	close_wave_file(&wav_file);
        directory_listing();
        file_counter++;
        step = 0;
        clear_lcd();
        printstring("Done ");
        printstring(name);
        LOG_printf(&trace,  "File saved test%d.wav\n",file_counter);
     }
}

