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
#include <csl_rtc.h>
#include <csl_intc.h>
#include "csl_wdt.h"
#include "app_asrc.h"
#include "VirtualSense_ACIcfg.h"
#include "psp_i2s.h"
#include "lcd_osd.h"
//#include "gpio_control.h"

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
CSL_RtcAlarm  wakeupTime;


FRESULT putDataIntoOpenFile(const void *buff, unsigned int  number_of_bytes);

extern FRESULT readNextWakeUpDateTimeFromScheduler(Uint16 i, CSL_RtcAlarm *nextAlarmTime);

extern Int16 circular_buffer[PROCESS_BUFFER_SIZE];
extern Uint32 bufferInIdx; //logical pointer
extern Uint32 bufferOutIdx; //logical pointer
extern Int32 bufferInside; //number of item in buffer
extern Uint16 in_record; //logical pointer
extern Uint16 numberOfFiles;
extern Uint16 ID;
extern Uint8 stopWriting;



// PRD function. Runs every 10 minutes to start sampling a new file
void CreateNewFile(void){
	debug_printf(  "Timer executes\r\n");

}


void DataSaveTask(void)
{
    // display the play audio message
    //print_playaudio();
        CSL_RtcTime      GetTime;
        CSL_RtcDate      GetDate;
        CSL_RtcAlarm	 nowTime;
        FRESULT rc;
        FRESULT write_result;

        WDTIM_Config     hwConfig,getConfig;
        CSL_Status               status;
        CSL_WdtHandle    hWdt = NULL;
        char file_name[128];
        Uint32 b_size = PROCESS_BUFFER_SIZE;
        unsigned int writingSamples = 0;
        Uint32 remainingSamples = 0;
        Int16 * bufferPointer = circular_buffer;
        Uint16 programCounter;


        /* Open the WDTIM module */
		hWdt = (CSL_WdtObj *)WDTIM_open(WDT_INST_0, &wdtObj, &status);
		if(NULL == hWdt)
		{
				debug_printf("   WDTIM: Open for the watchdog Failed\r\n");

		}
		else
		{
				//debug_printf("   WDTIM: Open for the watchdog Passed\r\n");
		}

		hwConfig.counter  = 0xFFFF;
		hwConfig.prescale = 0x7FFF;

		/* Configure the watch dog timer */
		status = WDTIM_config(hWdt, &hwConfig);
		if(CSL_SOK != status)
		{
				debug_printf("   WDTIM: Config for the watchdog Failed\r\n");

		}
		else
		{
				//debug_printf("   WDTIM: Config for the watchdog Passed\r\n");
		}

		/* Start the watch dog timer */
		status = WDTIM_start(hWdt);
		if(CSL_SOK != status)
		{
				debug_printf("   WDTIM: Start for the watchdog Failed\r\n");

		}
		else
		{
				//debug_printf("   WDTIM: Start for the watchdog Passed\r\n");
		}
    //main loop

		programCounter =  readProgramCounter();
		//debug_printf("readProgramCounter\r\n");
		debug_printf(" Program counter is %d\r\n",programCounter);
		debug_printf("   Starting task\r\n");
    while (1)
    {
        //wait on semaphore released from a timer function
        //wdt_Refresh();
        while((seconds > 0) && (numberOfFiles > 0) && !stopWriting) {//if second==0 don't save nothings

				RTC_getDate(&GetDate);
				RTC_getTime(&GetTime);
				nowTime.day = GetDate.day;
				nowTime.month = GetDate.month;
				nowTime.year = GetDate.year;
				nowTime.hours = GetTime.hours;
				nowTime.mins = GetTime.mins;

				sprintf(file_name, "%d__%d_%d_%d__%d-%d-%d.wav",ID, GetDate.day,GetDate.month,GetDate.year, GetTime.hours, GetTime.mins, GetTime.secs);
				debug_printf("    Creating a new file %s\r\n",file_name);

				//rc = open_wave_file(&wav_file, file_name, FREQUENCY, SECONDS);
				rc = open_wave_file(&wav_file, file_name, frequency, seconds);
				if(rc)
						debug_printf("    Error opening a new wav file %d\r\n",rc);
				else{
					bufferOutIdx = 0;
					bufferInIdx = 0;
					bufferInside = 0;
					file_is_open = 1;
					in_record = 1;
				}
				putDataIntoOpenFile((void *)bufferPointer, (unsigned int)16340); // to fill first sector in order to increase performance
				// try filling cluster to avoid contiguous sample saving problems
				while(file_is_open){ // should be controlled by the file size????
						while(bufferInside <= 4096);//spin-lock to wait buffer samples

						writingSamples = bufferInside; // number of sample need to be rounded to end of the linear buffer
						// round to 256 multiple
						/*writingSamples = writingSamples/256;
						writingSamples = writingSamples*256; */
						remainingSamples = b_size - bufferOutIdx; // bufferOutIdx is the next readable sample
						writingSamples = 4096; // to force two sector
						//debug_printf("writing samples is %ld --- remaining samples is %ld\r\n", writingSamples, remainingSamples);
						if(remainingSamples < writingSamples)
							writingSamples = remainingSamples;
						//writingSamples = writingSamples < remainingSamples?writingSamples:remainingSamples;

						//debug_printf("writing samples %d from index %ld\r\n",writingSamples, bufferOutIdx );
						write_result = putDataIntoOpenFile(((void *)(bufferPointer+bufferOutIdx)), (writingSamples*2));
						if(!write_result)
							WDTIM_service(hWdt);

						//debug_printf("b inside %ld\r\n",  bufferInside);
						//readingIndex = bufferOutIdx+writingSamples;
						/*if(readingIndex >= b_size){
								debug_printf("---- index exceed array %ld  %ld\r\n",bufferOutIdx, readingIndex);
						}*/
						bufferOutIdx = ((bufferOutIdx + writingSamples) % b_size);
						bufferInside-=writingSamples ; // sample number
						/*if(bufferOutIdx == 0)
							debug_printf("out of buffer new out index is %ld\r\n", bufferOutIdx); */

				}

				// wave header is 44 bytes length
				//clear_lcd();
				//SEM_reset(&SEM_BufferFull,0);
				SEM_pend(&SEM_CloseFile, SYS_FOREVER);
				close_wave_file(&wav_file);
				file_is_open = 0;
				in_record = 0;
				//directory_listing();
				numberOfFiles--;
				step = 0;
                //clear_lcd();
                debug_printf("    File saved %s\r\n",file_name);
        }
        // read next wake-up datetime

        //rc = increaseProgramCounter(programCounter);
        //debug_printf("program counter increased return %d\r\n",rc);
        debug_printf("   Task completed!!!!\r\n");
        debug_printf("   Looking for next wake-up datetime in scheduler file.... %d\r\n",rc);
        rc = readNextWakeUpDateTimeFromScheduler(programCounter, &wakeupTime);
        while(!isAfter(wakeupTime,nowTime)){
        	debug_printf("   Looking for next wake-up datetime in scheduler file.... \r\n");
        	rc = increaseProgramCounter(programCounter);
        	programCounter++;
        	rc = readNextWakeUpDateTimeFromScheduler(programCounter, &wakeupTime);
        }
        increaseProgramCounter(programCounter);
        debug_printf("   Going to sleep until: %d/%d/%d %d:%d:%d \r\n",
					wakeupTime.day, wakeupTime.month, wakeupTime.year,
					wakeupTime.hours, wakeupTime.mins, wakeupTime.secs);
        status = RTC_setAlarm(&wakeupTime);
        if(status != CSL_SOK)
		{
			debug_printf("    RTC: setAlarm Failed CPU remains active\r\n");
		} else {
			RTC_shutdownToRTCOnlyMonde();
		}

    }
}

FRESULT putDataIntoOpenFile(const void *buff, unsigned int number_of_bytes){
	FRESULT res = 0;
	if(file_is_open){
		res = write_data_to_wave(&wav_file, buff, number_of_bytes);
		my_step+=(number_of_bytes/512);
        //wdt_Refresh();
	}
	//if(my_step == ((SECONDS * STEP_PER_SECOND)+1)){
	if(my_step >= ((seconds * step_per_second)+1)){
		file_is_open = 0;
		in_record = 0;
		my_step = 0;
		SEM_post(&SEM_CloseFile);
	}
	return  res;

}
