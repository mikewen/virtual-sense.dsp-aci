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
char name[12];
extern unsigned char circular_buffer[PROCESS_BUFFER_SIZE];
//extern Uint32 bufferInIdx; //logical pointer
//extern Uint32 bufferOutIdx; //logical pointer

void putDataIntoOpenFile(const void *buff, unsigned int number_of_bytes);

// PRD function. Runs every 10 minutes to start sampling a new file
void CreateNewFile(void){
	LOG_printf(&trace, "Timer executes\n");
	SEM_post(&SEM_TimerSave);
}


void DataSaveTask(void)
{
    // display the play audio message
    //print_playaudio();
	//CSL_RtcTime 	 GetTime;
	CSL_RtcDate 	 GetDate;
	CSL_Status    status;
	Uint32 burts_size_bytes = DMA_BUFFER_SZ * 2;
	Uint32 b_size = PROCESS_BUFFER_SIZE;

    LOG_printf(&trace, "\nMount a volume.\n");
    rc = f_mount(0, &fatfs);
    if(rc) LOG_printf(&trace, "Error mounting volume\n");
    //main loop
    while (1)
    {

    	//go to sleep test
    	/**(ioport volatile unsigned int *)ICR = 0x03EF; // Request to disable ports & C55x CPU

    	//add 6 "nop" clearing pipeline
    	asm (" nop");
    	asm (" nop");
    	asm (" nop");
    	asm (" nop");
    	asm (" nop");
    	asm (" nop");

    	asm (" idle"); //idle the CPU */


    	//wait on semaphore released from a timer function
    	SEM_pend(&SEM_TimerSave, SYS_FOREVER);
    	//status = RTC_getTime(&GetTime);
    	status |= RTC_getDate(&GetDate);

    	if(!status)
    		sprintf(name, "%d%d%d%d.wav",GetDate.day,GetDate.month,GetDate.year,file_counter);
    	else
    		sprintf(name, "test%d.wav",file_counter);
    	clear_lcd();
    	printstring("Creating   ");
    	printstring(name);
    	printf("Saving \n");
    	rc = open_wave_file(&wav_file, name, FREQUENCY,SECONDS);
    	if(rc)
    		LOG_printf(&trace, "Error openin a new wav file %d\n",rc);
    	else
    		file_is_open = 1;
    	putDataIntoOpenFile((void *)circular_buffer, 468); // to fill first sector in order to increase performance
    	// wave header is 44 bytes length
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
    		//LOG_printf(&trace, "out log %ld\n",bufferOutIdx);
        	//LOG_printf(&trace,  "buff %d in %d out %d\n",SEM_count(&SEM_BufferFull),bufferInIdx,bufferOutIdx);
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
        printstring(name);
        LOG_printf(&trace,  "File saved test%d.wav\n",file_counter);
        printf("File save done!!!!\n");
     }
}

void putDataIntoOpenFile(const void *buff, unsigned int number_of_bytes){
	if(file_is_open){
		write_data_to_wave(&wav_file, buff, number_of_bytes);
		my_step++;
	}
	if(my_step == ((SECONDS * STEP_PER_SECOND)+1)){
		file_is_open = 0;
		my_step = 0;
		SEM_post(&SEM_CloseFile);
	}

}
