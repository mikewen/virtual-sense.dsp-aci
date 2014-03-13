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
#include <csl_intc.h>
#include "csl_wdt.h"
#include "app_asrc.h"
#include "VirtualSense_ACIcfg.h"
#include "psp_i2s.h"
#include "lcd_osd.h"
#include "gpio_control.h"

#include "main_config.h"
#include "circular_buffer.h"
#include "rtc.h"

#include "wdt.h"

#include "ff.h"
#include "make_wav.h"

#define ICR 0x0001


//FRESULT rc;

FIL wav_file;
Uint32 step = 0;
Uint32 my_step = 0;
Uint16 file_is_open = 0;
CSL_WdtObj    wdtObj;

FRESULT putDataIntoOpenFile(const void *buff, unsigned int number_of_bytes);
extern unsigned char circular_buffer[PROCESS_BUFFER_SIZE];
extern unsigned char circular_buffer2[PROCESS_BUFFER_SIZE];
unsigned char * used_buffer = circular_buffer;
//extern Uint32 bufferInIdx; //logical pointer
extern Uint32 bufferOutIdx; //logical pointer
extern Uint32 buffer2OutIdx;
extern Int32 bufferInside; //number of item in buffer
extern Uint16 in_record; //logical pointer

// PRD function. Runs every 10 minutes to start sampling a new file
void CreateNewFile(void){
	debug_printf(  "Timer executes\n");

}


void DataSaveTask(void)
{
    // display the play audio message
    //print_playaudio();
	CSL_RtcTime 	 GetTime;
	CSL_RtcDate 	 GetDate;
	FRESULT rc;
	FRESULT write_result;

	char file_name[128];
	Uint32 b_size = PROCESS_BUFFER_SIZE;

	CSL_Status		 status;
	CSL_WdtHandle    hWdt = NULL;
	WDTIM_Config	 hwConfig,getConfig;
	Uint32           counter;
	Uint32			 time;
	Uint16 			 delay;



	/* Open the WDTIM module */
	hWdt = (CSL_WdtObj *)WDTIM_open(WDT_INST_0, &wdtObj, &status);
	if(NULL == hWdt)
	{
		debug_printf("WDTIM: Open for the watchdog Failed\n");

	}
	else
	{
		debug_printf("WDTIM: Open for the watchdog Passed\n");
	}

	hwConfig.counter  = 0xFFFF;
	hwConfig.prescale = 0x7FFF;

	/* Configure the watch dog timer */
	status = WDTIM_config(hWdt, &hwConfig);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Config for the watchdog Failed\n");

	}
	else
	{
		debug_printf("WDTIM: Config for the watchdog Passed\n");
	}

	/* Start the watch dog timer */
	status = WDTIM_start(hWdt);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Start for the watchdog Failed\n");

	}
	else
	{
		debug_printf("WDTIM: Start for the watchdog Passed\n");
	}

	for (delay = 0; delay < 10; delay++);

	/* Get the timer count */
	status = WDTIM_getCnt(hWdt, &time);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Get Count for the watchdog Failed\n");

	}
	else
	{
		debug_printf("WDTIM: Get Count for the watchdog is %ld:\n", time);
	}

	//main loop
    while (1)
    {
    	if(seconds > 0) {//if second==0 don't save nothings

			RTC_getDate(&GetDate);
			RTC_getTime(&GetTime);
			sprintf(file_name, "%d_%d_%d__%d-%d-%d.wav",GetDate.day,GetDate.month,GetDate.year, GetTime.hours, GetTime.mins, GetTime.secs);
			debug_printf("Creating a new file %s\n",file_name);

			//rc = open_wave_file(&wav_file, file_name, FREQUENCY, SECONDS);
			rc = open_wave_file(&wav_file, file_name, frequency, seconds);
			if(rc){
				debug_printf("Error opening a new wav file %d\n",rc);
				break;
			}
			else{
				file_is_open = 1;
				in_record = 1;
			}
			putDataIntoOpenFile((void *)circular_buffer, 468); // to fill first sector in order to increase performance
			while(file_is_open){ // should be controlled by the file size????
				while(bufferInside <= 255);//spin-lock to wait buffer samples

				write_result = putDataIntoOpenFile((void *)(used_buffer+bufferOutIdx), 512);
				bufferOutIdx = ((bufferOutIdx + 512)% b_size);
				if(bufferOutIdx == 0) { // switch buffer
					used_buffer = used_buffer==circular_buffer?circular_buffer2:circular_buffer;
					if(!write_result)
						WDTIM_service(hWdt);
				}
				bufferInside-=256; // sample number

			}
			// wave header is 44 bytes length
			//clear_lcd();
			//SEM_reset(&SEM_BufferFull,0);
			SEM_pend(&SEM_CloseFile, SYS_FOREVER);
			close_wave_file(&wav_file);
			file_is_open = 0;
			in_record = 0;
			//directory_listing();
			file_counter++;
			step = 0;
	        //clear_lcd();
	        debug_printf("File saved %s\n",file_name);
    	}
        // Put DSP into RTC only mode
        if(mode != MODE_ALWAYS_ON) {
        	RTC_shutdownToRTCOnlyMonde();
        } else {
        	//SEM_post(&SEM_TimerSave);
        }
     }
}

FRESULT putDataIntoOpenFile(const void *buff, unsigned int number_of_bytes){
	FRESULT res;
	if(file_is_open){
		res = write_data_to_wave(&wav_file, buff, number_of_bytes);
		my_step++;
        //wdt_Refresh();
	}
	//if(my_step == ((SECONDS * STEP_PER_SECOND)+1)){
	if(my_step == ((seconds * step_per_second)+1)){
		file_is_open = 0;
		in_record = 0;
		my_step = 0;
		SEM_post(&SEM_CloseFile);
	}
	return res;
}
