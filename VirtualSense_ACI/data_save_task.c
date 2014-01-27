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
#include <stdlib.h>
#include <stdio.h>
#include "debug_uart.h" // to redirect debug_printf over UART
#include <csl_rtc.h>
#include "app_asrc.h"
#include "VirtualSense_ACIcfg.h"
#include "psp_i2s.h"
#include "lcd_osd.h"

#include "main_config.h"
#include "circular_buffer.h"

#include "ff.h"
#include "make_wav.h"

#define ICR 0x0001


FRESULT rc;
FATFS fatfs;			/* File system object */
FIL wav_file;
Uint32 step = 0;
Uint32 my_step = 0;
Uint16 file_is_open = 0;
Uint16 file_counter = 0;

//extern unsigned char circular_buffer[PROCESS_BUFFER_SIZE];
//extern Uint32 bufferInIdx; //logical pointer
//extern Uint32 bufferOutIdx; //logical pointer

// PRD function. Runs every 10 minutes to start sampling a new file
void CreateNewFile(void){
	debug_debug_printf(  "Timer executes\n");
	SEM_post(&SEM_TimerSave);
}


void DataSaveTask(void)
{
    // display the play audio message
    //print_playaudio();
	//CSL_RtcTime 	 GetTime;
	CSL_RtcDate 	 GetDate;
	CSL_Status    status;
	char file_name[12];
	Uint32 burts_size_bytes = DMA_BUFFER_SZ * 2;
	Uint32 b_size = PROCESS_BUFFER_SIZE;
    rc = f_mount(0, &fatfs);
    debug_printf("Mounting volume\n");
    if(rc) debug_printf("Error mounting volume\n");
    //main loop
    while (1)
    {

    	//wait on semaphore released from a timer function
    	SEM_pend(&SEM_TimerSave, SYS_FOREVER);
    	//status = RTC_getTime(&GetTime);
    	status |= RTC_getDate(&GetDate);

    	if(!status)
    		sprintf(file_name, "%d%d%d%d.wav",GetDate.day,GetDate.month,GetDate.year,file_counter);
    	else
    		sprintf(file_name, "test%d.wav",file_counter);
    	clear_lcd();
    	printstring("Creating   ");
    	printstring(file_name);
    	debug_printf("Creating a new file %s\n",file_name);
    	rc = open_wave_file(&wav_file, file_name, FREQUENCY,SECONDS);
    	if(rc)
    		debug_printf("Error openin a new wav file %d\n",rc);
    	else
    		file_is_open = 1;
    	//clear_lcd();
    	SEM_reset(&SEM_BufferFull,0);
    	//bufferOutIdx = 0;
    	//bufferInIdx = 0;
    	/*while (step < (SECONDS * STEP_PER_SECOND))
    	{
    		// wait on bufferIn ready semaphore
    		SEM_pend(&SEM_BufferFull, SYS_FOREVER);

    		write_data_to_wave(&wav_file, &circular_buffer[bufferOutIdx], burts_size_bytes);
    		bufferOutIdx = ((bufferOutIdx + burts_size_bytes) % b_size);
    		//debug_debug_printf(  "out log %ld\n",bufferOutIdx);
        	//debug_debug_printf(   "buff %d in %d out %d\n",SEM_count(&SEM_BufferFull),bufferInIdx,bufferOutIdx);
        	//printstring(".!");
    		step++;
    	}*/
    	SEM_pend(&SEM_CloseFile, SYS_FOREVER);
    	close_wave_file(&wav_file);
    	file_is_open = 0;
        directory_listing();
        file_counter++;
        step = 0;
        clear_lcd();
        printstring("Done ");
        printstring(file_name);
        debug_printf("File saved %s\n",file_name);
     }
}

void putDataIntoOpenFile(const void *buff, unsigned int number_of_bytes){
	if(file_is_open){
		write_data_to_wave(&wav_file, buff, number_of_bytes);
		my_step++;
	}
	if(my_step == (SECONDS * STEP_PER_SECOND)){
		file_is_open = 0;
		my_step = 0;
		SEM_post(&SEM_CloseFile);
	}

}
