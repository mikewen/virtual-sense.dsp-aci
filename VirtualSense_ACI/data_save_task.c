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
#include "aci/ACI_LCR.h"

#include "aci/hwafft.h"
#include <dsplib.h>


FIL spec_file;

#pragma DATA_SECTION(spec_buffer, ".spec_buffer");
static  char spec_buffer[1024];

#define ICR 0x0001



/* --- Special buffers required for HWAFFT ---*/
#pragma DATA_SECTION(complex_buffer, "cmplxBuf");
Int32 complex_buffer[WND_LEN];

#pragma DATA_SECTION(window_buffer, "windowBuf");
Int32 window_buffer[WND_LEN];

#pragma DATA_SECTION(bitreversed_buffer, "brBuf");
#pragma DATA_ALIGN(bitreversed_buffer, 2*FFT_LENGTH);
Int32 bitreversed_buffer[FFT_LENGTH];

#pragma DATA_SECTION(temporary_buffer,"tmpBuf");
Int32 temporary_buffer[FFT_LENGTH];

#pragma DATA_SECTION(realR, "rfftR");
Int16 realR[FFT_LENGTH];

#pragma DATA_SECTION(imagR, "ifftR");
Int16 imagR[FFT_LENGTH];

#pragma DATA_SECTION(PSD_Result, "PSD");
Int16 PSD_Result[NUM_BINS];

#pragma DATA_SECTION(PSD_Result_sqrt, "PSDsqrt");
Int16 PSD_Result_sqrt[NUM_BINS];


//FRESULT rc;

FIL wav_file;
Uint32 step = 0;
Uint32 my_step = 0;
Uint16 file_is_open = 0;
CSL_WdtObj    wdtObj;
CSL_RtcAlarm  wakeupTime;


FRESULT putDataIntoOpenFile(const void *buff, unsigned int  number_of_bytes);
Uint16 calculateACI(Int16 *dataPointer);

extern FRESULT readNextWakeUpDateTimeFromScheduler(Uint16 i, CSL_RtcAlarm *nextAlarmTime);

void myfprintf(FIL file, const char *format, ...);

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
        Uint16 temperature;
        Uint16 tempC;
        Uint16 tempM;
        Uint16 i = 0;
        f_open(&spec_file, "spectrum.txt", FA_WRITE | FA_CREATE_ALWAYS);
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

		//initialize window buffer
		for(i = 0; i < WND_LEN; i++)
			window_buffer[i] = 0;
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
						// LELE: introducing ACI calculation..... over 512 sample
						// TODO: repeat to cover all 4096 samples
						calculateACI((Int16 *)(bufferPointer+bufferOutIdx));

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

				//SEM_reset(&SEM_BufferFull,0);
				SEM_pend(&SEM_CloseFile, SYS_FOREVER);
				close_wave_file(&wav_file);
				file_is_open = 0;
				in_record = 0;
				//directory_listing();
				numberOfFiles--;
				step = 0;

                debug_printf("    File saved %s\r\n",file_name);

                Uint16 aci1, aci2, aci3, delay;
                ACI(NULL, 10,10,10,10,10); // LINKING TEST LELE
                debug_printf("ACI generate\r\n");

                aci1 = 381;
                aci2 = 0xFFFF;
                aci3 = 65535;

                debug_printf("ACI\r\n");
                debug_printf("%x\n", aci1);
                for(delay = 0; delay < 0xFFFF; delay++)
                	asm(" NOP ");

                debug_printf("%x0A", aci2);
                for(delay = 0; delay < 0xFFFF; delay++)
                 	asm(" NOP ");

                debug_printf("%x\r\n", aci3);
                for(delay = 0; delay < 0xFFFF; delay++)
                 	asm(" NOP ");

                while(1);

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
			WDTIM_stop(hWdt); //LELE: to patch rtconlymode
			asm(" RESET "); // nop
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


Uint16 calculateACI(Int16 *dataPointer){
	 // Perform FFT on windowed buffer
	Int16 *data;
	Int32 *complex_data, *bitrev_data, *temp_data, *fft_data;
	int i = 0;
	int j = 0;
	int f = 0;
	int Peak_Magnitude_Value = 0;
	int Peak_Magnitude_Index = 0;
	Uint16 data_selection;
	/* Initialize relevant pointers */
	data 		 = dataPointer;
	bitrev_data  = bitreversed_buffer;
	temp_data    = temporary_buffer;
	complex_data = complex_buffer;

	//debug_printf("    Calculating ACI\r\n");
	debug_printf("\n\r\n\r.\n\r\n\r");
	/* Convert real data to "pseudo"-complex data (real, 0) */
	/* Int32 complex = Int16 real (MSBs) + Int16 imag (LSBs) */
	for(f = 0; f < 4096/HOP_SIZE; f++){

		for (i = 0; i < HOP_SIZE; i++) {
			/* Copy previous NEW data to current OLD data */
			window_buffer[i] = window_buffer[i + HOP_SIZE];
			/* Update NEW data with current audio in */
			window_buffer[i + HOP_SIZE] = data[i+f*HOP_SIZE];
		}

		for (i = 0; i < FFT_LENGTH; i++) {
			*(complex_data + i) = ( (Int32) ((*(window_buffer + i))*hamming[i]) ) << 16;
		}

		/* Perform bit-reversing */
		hwafft_br(complex_data, bitrev_data, FFT_LENGTH);

		/* Perform FFT */
		if (HWAFFT_SCALE) {
			data_selection = hwafft_512pts(bitrev_data, temp_data, temp_data, bitrev_data, FFT_FLAG, SCALE_FLAG); // hwafft_#pts, where # = 2*HOP_SIZE
		}
		else {
			data_selection = hwafft_512pts(bitrev_data, temp_data, temp_data, bitrev_data, FFT_FLAG, NOSCALE_FLAG);
		}

		/* Return appropriate data pointer */
		if (data_selection == 1) {
			fft_data = temp_data;	// results stored in this scratch vector
		}
		else {
			fft_data = bitrev_data; // results stored in this data vector
		}

		/*myfprintf(spec_file,"\n\n");
		myfprintf(spec_file,"data %x\t", *(fft_data + 64));
		myfprintf(spec_file,"data %x\t", *(fft_data + 64));
		myfprintf(spec_file,"\n\n"); */

		/* Extract real and imaginary parts */
		for (i = 0; i < FFT_LENGTH; i++) {
			*(realR + i) = (Int16)((*(fft_data + i)) >> 16);
			*(imagR + i) = (Int16)((*(fft_data + i)) & 0x0000FFFF);
			//myfprintf(spec_file,"%d\t", *(realR + i));
		}
		//myfprintf(spec_file,"\n\n");


		// Process freq. bins from 0Hz to Nyquist frequency
		// Perform spectral processing here
		//debug_printf(".");
		PSD_Result[0] = ((realR[0]*realR[0]) + (imagR[0]*imagR[0])); // start the search at the first value in the Magnitude plot
		Peak_Magnitude_Value = PSD_Result[0];
		for( j = 1; j < NUM_BINS; j++ )
		{
			PSD_Result[j] = ((realR[j]*realR[j]) + (imagR[j]*imagR[j])); // Convert FFT to magnitude spectrum. Basically Find magnitude of FFT result for each index
		}


		sqrt_16(&PSD_Result[0], &PSD_Result_sqrt[0],NUM_BINS);

		Peak_Magnitude_Value = PSD_Result_sqrt[0];
		for( j = 1; j < NUM_BINS; j++ )
		{
			myfprintf(spec_file,"%d\t", PSD_Result_sqrt[j]);
			if( PSD_Result_sqrt[j] > Peak_Magnitude_Value ) // Peak search on the magnitude of the FFT to find the fundamental frequency
			{
				Peak_Magnitude_Value = PSD_Result_sqrt[j];
				Peak_Magnitude_Index = j;
			}
		}
		myfprintf(spec_file,"\n");
	}
	f_sync(&spec_file);
	//data = data + HOP_SIZE;
	//debug_printf("BufferR[256]= %d \n", BufferR[256]);
	/*debug_printf("realR[128] 	= %d \n", realR[128]);
	debug_printf("PSD_Result[128] 	 = %d \n", PSD_Result[128]);
	debug_printf("Peak_Magnitude_Value = %d \n", Peak_Magnitude_Value);
	debug_printf("Peak_Magnitude_Index = %d \n\n", Peak_Magnitude_Index);*/


}

void myfprintf(FIL file, const char *format, ...){

	va_list arg;
	int done;
	Uint bw = 0;
	FRESULT fatRes;
	va_start (arg, format);
	done = vsprintf (spec_buffer, format, arg);
    fatRes = f_write (&spec_file, &spec_buffer, done, &bw);	/* Write data to a file */
    //f_sync(&spec_file);

}

