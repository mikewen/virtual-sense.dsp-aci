/*
 * $$$MODULE_NAME csl_usb_iso_fullspeed_example.c
 *
 * $$$MODULE_DESC csl_usb_iso_fullspeed_example.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** @file csl_usb_iso_fullspeed_example.c
 *
 *  @brief USB Audio Class functional layer full speed mode example source file
 *
 *  This example tests the operation of VC5505 usb in full speed mode.
 *  NOTE: For Testing Audio class module a macro CSL_AC_TEST needs to be defined
 *  This includes some code in csl_usbAux.h file which is essential for Audio class
 *  operation and not required for MUSB stand alone testing.
 *  define this macro in pre defined symbols in project biuld options
 *  (Defined in the current usb audio class example pjt).
 *  Semaphores and mail boxes are used in the Audio class example code as the USB operation
 *  is not possible with out OS calls. DSP BIOS version 5.32.03 is used for this purpose.
 *  Definig Start transfer and complete transfer call back functions is must
 *  and Audio class module does not work if they are not implemeted properly. A call back
 *  is sent to this functions from MUSB module.
 *
 *  NOTE: Message boxes and semaphores are reused from MSC module.
 *  Name MSC is not replaced with Auidio class at some places for quick reusability
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505 AND
 * C5515. MAKE SURE THAT PROPER CHIP VERSION MACRO CHIP_5505/CHIP_5515 IS
 * DEFINED IN THE FILE c55xx_csl\inc\csl_general.h.
 *
 *  Path: \(CSLPATH)\example\usb\example5
 */

/* ============================================================================
 * Revision History
 * ================
 * 20-Dec-2008 Created
 * ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <csl_mmcsd.h>
#include "csl_sysctrl.h"
#include <csl_rtc.h>

#include "rtc.h"
#include "csl_types.h"
#include "csl_error.h"
#include "csl_intc.h"
#include "csl_gpio.h"
#include "csl_usb.h"
#include "csl_audioClass.h"

#include "soc.h"
//#include "psp_i2s.h"
#include "dda_dma.h"
#include "i2s_sample.h"
#include "gpio_control.h"
#include "pll_control.h"
#include "app_globals.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_usbac_descs.h"
#include "codec_aic3254.h"
#include "user_interface.h"
#include "app_asrc.h"
#include "sample_rate.h"
#include "i2c_display.h"
#include "i2c_thsensor.h"

#ifdef C5535_EZDSP_DEMO
#include "lcd_osd.h"
#include "dsplib.h"
#include "soc.h"
#include "cslr.h"
#include "cslr_sysctrl.h"

#include "wdt.h"
#include "main_config.h"
#include "circular_buffer.h"

#include "ff.h"
#include "make_wav.h"

#undef ENABLE_REC_ASRC
#undef ENABLE_ASRC

extern CSL_Status pll_sample_freq(Uint16 freq);
extern PSP_Result set_sampling_frequency_gain_impedence(unsigned long SamplingFrequency,
		unsigned int ADCgain, unsigned int impedance);
CSL_Status  CSL_i2cPowerTest(void);
void init_all_peripheral(void);

FRESULT updateTimeFromFile();
FRESULT initConfigFromSchedulerFile(Uint16 index); // read configuration from scheduler file at index line
Uint16 readProgramCounter();					  // read from file the program counter
FRESULT increaseProgramCounter(Uint16 pc);		  // increase program counter
FRESULT readNextWakeUpDateTimeFromScheduler(Uint16 i, CSL_RtcAlarm *nextAlarmTime);

void calculate_FFT(unsigned char *input, int size);

FATFS fatfs;			/* File system object */
FIL file_config;
FIL rtc_time_file;


CSL_RtcTime      RTCGetTime;
CSL_RtcDate      RTCGetDate;
CSL_RtcAlarm	 stopWritingTime;

Uint8 mode = MODE_ALWAYS_ON;
Uint32 frequency = 3;
Uint32 step_per_second = 187;//frequency/DMA_BUFFER_SZ;
Uint8 gain = 40;
Uint8 impedance = 3;
Uint16 seconds = 5;
Uint16 recTimeMinutes;
Uint16 numberOfFiles = 0;
Uint16 ID = 0;
Uint8 digital_gain = 1;



// Demo switch flag: 0 - power display, 1 - spectrum analyzer
Uint16 DemoSwitchFlag = 1;

// buffer for perform FFT
//#pragma DATA_ALIGN(bufferFFT, 4)
//DATA bufferFFT[FFT_LENGHT];
// scarch buffer for FFT
//DATA bufferScrach[FFT_LENGHT];
// display buffer for spectrum display
int display_buffer[128];

extern Uint16 my_i2sRxLeftBuf[2*DMA_BUFFER_SZ]; /* 2x for ping/pong */
extern Uint16 my_i2sRxRightBuf[2*DMA_BUFFER_SZ]; /* 2x for ping/pong */
extern Int16 my_i2sTxLeftBuf[2*DMA_BUFFER_SZ]; /* 2x for ping/pong */
extern Int16 my_i2sTxRightBuf[2*DMA_BUFFER_SZ]; /* 2x for ping/pong */

#endif



#include "VirtualSense_ACIcfg.h"

 /* Debug: enable run-time storage of data to SDRAM */
//#define STORE_PARAMETERS_TO_SDRAM

// Clock gating for unused peripherals
void ClockGating(void);

/* Initializes application */
void CSL_acTest(void);

/* Resets C5515 */
void C5515_reset(void);

extern void initRTC(void);
/**
 *  \brief  CSL Audio Class main function
 *
 *  \param  None
 *
 *  \return None
 */
void main(void)
{
    CSL_Status status;
    CSL_Status mmcStatus;
    Uint32 gpioIoDir;

    /* Clock gate all peripherals */
    // Disable all peripheral
    CSL_SYSCTRL_REGS->PCGCR1 = 0x7FFF;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x007F;

    // turn on led to turn on oscillator
    CSL_CPU_REGS->ST1_55 |= CSL_CPU_ST1_55_XF_MASK;

    mmcStatus = MMC_init();
	if (mmcStatus != CSL_SOK)
	{
		debug_printf("API: MMC_init Failed!\r\n");

	}

    #if 0
    /* SP0 Mode 2 (GP[5:0]) -- GPIO02/GPIO04 for debug  */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE2);


    /* SP1 Mode 2 (GP[11:6]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE2); /* need GPIO10 for AIC3204 reset */
#endif

    SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                                   CSL_EBSR_SP0MODE_0);
    SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                                    CSL_EBSR_SP1MODE_0);

    /* PP Mode 1 (SPI, GPIO[17:12], UART, and I2S2) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

	/* Initialize GPIO module */

	/* GPIO02 and GPIO04 for debug */
	/* GPIO10 for AIC3204 reset */
	gpioIoDir = ((((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN16)| // 16 is SD1_ENABLE
		        (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN17)); // 17 is OSCILLATOR ENABLE

	gpioInit(gpioIoDir, 0x00000000, 0x00000000);

    /* Reset C5515 -- ungates all peripherals */
    C5515_reset();

    /* Initialize DSP PLL */
    status = pll_sample_freq(40);
    if (status != CSL_SOK)
    {
        exit(EXIT_FAILURE);
    }


    /* Clear pending timer interrupts */
    CSL_SYSCTRL_REGS->TIAFR = 0x7;
    /* Initialize GPIO module */

    /* Enable the USB LDO */
    //*(volatile ioport unsigned int *)(0x7004) |= 0x0001;

    init_debug(40);

    // set the GPIO pin 10 - 11 to output, set SYS_GPIO_DIR0 (0x1C06) bit 10 and 11 to 1
    //LELE *(volatile ioport unsigned int *)(0x1C06) |= 0x600;
    //mount sdcard: must be High capacity(>4GB), standard capacity have a problem
    init_all_peripheral();

}
void CSL_acTest(void){

}
/**
 *  \brief  Audio Class intialization function
 *
 *  \param  None
 *
 *  \return None
 */
void init_all_peripheral(void)
{
    I2sInitPrms i2sInitPrms;
    PSP_Result result;
    Int16 status;
    FRESULT rc;
    FRESULT rc_fat;
	//UINT bw;

	FIL null_file;
	Uint16 current_pc = 0;



	//FIL rtc_time_file;

	// turn off led to turn on oscillator
	// LELE: XF now is step-up enable need to be always active
	// CSL_CPU_REGS->ST1_55 &=~CSL_CPU_ST1_55_XF_MASK;

    debug_printf("Start Configuration....\r\n");
	// for debug LELE
	//init_buffer();

   	debug_printf("Starting device....\r\n");

   	debug_printf("Init RTC....\r\n");

   	debug_printf("Init RTC now....\r\n");

   	debug_printf("Init RTC now..now..\r\n");


	//Initialize RTC
    initRTC();




    dbgGpio1Write(1); // ENABLE SD_1
    dbgGpio2Write(1); // ENABLE OSCILLATOR

    //Initialize and start Watch dog
	//wdt_Init();
	/*wdt_test();
	debug_printf("fine wdg....\r\n"); */
	       //while(1);

    //mount sdcard: must be High capacity(>4GB), standard capacity have a problem
    rc = f_mount(0, &fatfs);
    if(rc){

    	debug_printf("Error mounting volume\r\n");

    }
    else{

    	debug_printf("Mounting volume\r\n");

    }

    rc_fat = f_open(&null_file, "null.void", FA_READ);

	debug_printf(" try to open null.void\r\n");

	if(rc_fat){
		debug_printf("null.void doesn't exist\r\n");
	}


	 rc_fat = f_open(&null_file, "null2.void", FA_READ);

	debug_printf(" try to open null2.void\r\n");
	if(rc_fat){
		debug_printf("null2.void doesn't exist\r\n");

	}


	// LELE Calling this function does not run. Need to explicitely
	// do it here !!!!
	//RTC initilization from file
		/*if( RTC_initRtcFromFile() )
				debug_printf("RTC: time.rtc doesn't exists\r\n");
		else{
				debug_printf("RTC: initialized from time.rtc file\r\n");
				//to enable delete: change _FS_MINIMIZE to 0 in ffconf.h
				//f_unlink (RTC_FILE_CONFIG);
				//debug_printf("time.rtc file deleted\r\n");
		}*/


	LCD_Write("VirtualSenseDSP");
	_delay_ms(1000);

	start_log();
	debug_printf("\r\n");
	debug_printf("Firmware version:");
	debug_printf(FW_VER);
	debug_printf("\r\n");
	debug_printf("\r\n");
	debug_printf("VirtualSenseDSP:\r\n");
	debug_printf(" Copyright Emanuele Lattanzi 2014\r\n");
	debug_printf(" Department of Basic Sciences and Foundations \r\n");
	debug_printf(" University of Urbino - Urbino Italy \r\n");
	debug_printf(" Contact: emanuele.lattanzi@uniurb.it \r\n");
	debug_printf("\r\n");

	debug_printf("Start configuration\r\n");
	rc = updateTimeFromFile();

	debug_printf(" Check sensors\r\n");
	debug_printf("  Temperature: %dmC\n", THS_ReadTemp());
	debug_printf("  Humidity:    %d%%\n", THS_ReadHumid());

	current_pc =  readProgramCounter();
	//debug_printf("readProgramCounter\r\n");
	debug_printf(" Program counter is %d\r\n",current_pc);
	rc = initConfigFromSchedulerFile(current_pc);

		// Initialize audio module
	debug_printf(" codec parameters:\r\n");
	debug_printf("  Freq is %ld step per second: %ld\r\n", frequency, step_per_second);
	debug_printf("  Gain is %d \r\n", gain);
	debug_printf("  Impedence is 0x%x \r\n", impedance);
	debug_printf("  File sized in Seconds is %d \r\n", seconds);
	debug_printf("  Minute to record is %d \r\n", recTimeMinutes);

	// turn on led
	//CSL_CPU_REGS->ST1_55 |= CSL_CPU_ST1_55_XF_MASK;

        //Initialize I2S
    i2sTxBuffSz = 2*DMA_BUFFER_SZ;
    /* Reset codec output buffer */
    //reset_codec_output_buffer();
    //debug_printf("reset codec output buffer\r\n");

    /* Initialize DMA hardware and driver */
    DMA_init(); // To enable MMCSD DMA
    debug_printf(" DMA INIT\r\n");

    DMA_HwInit();
    debug_printf(" DMA HW INIT\r\n");
    DMA_DrvInit();
    debug_printf(" DMA DrvInit\r\n");

    /* Initialize I2S and DMA channels for Playback and Record */
    /* playback */
    i2sInitPrms.enablePlayback = TRUE;
    i2sInitPrms.enableStereoPb = TRUE;
    i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)my_i2sTxLeftBuf;  /* note: only Tx Left used for stereo, sample-by-sample Pb */
    i2sInitPrms.sampleBySamplePb = FALSE;
    i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)my_i2sTxRightBuf;
    i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID;
    /* record */
    i2sInitPrms.enableRecord = TRUE;
    i2sInitPrms.enableStereoRec = FALSE;
    i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)my_i2sRxLeftBuf;
    i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID;
    status = i2sInit(&i2sInitPrms);
    if (status != I2SSAMPLE_SOK)
    {
        debug_printf(" ERROR: Unable to initialize I2S\r\n");
        exit(EXIT_FAILURE);
    }


#if 1 // to remove sampling
    /* Start left Rx DMA */
    DMA_StartTransfer(hDmaRxLeft);
    debug_printf(" DMA Start Transfer\r\n");
    /* Set HWAI ICR */
    *(volatile ioport Uint16 *)0x0001 = 0xFC0E | (1<<9);
    asm("   idle");

    /* Clock gate usused peripherals */

    //ClockGating();

    //

    // init lcd
    //LCD_Init(0);
    //int a = 2;
    //LCD_Write("Sense %d\n 4", a);


    //debug_printf("ClokGating\r\n");
    DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx, TRUE); /* enable I2S transmit and receive */

    result = set_sampling_frequency_gain_impedence(frequency, gain, impedance);
    if (result != 0)
    {
        debug_printf(" ERROR: Unable to configure audio codec\r\n");
        exit(EXIT_FAILURE);
    }
    Set_Mute_State(TRUE);


    // init lcd
    //LCD_Init(0);

    debug_printf("Initialization completed\r\n");
    debug_printf("\r\n");
#endif
}



FRESULT updateTimeFromFile(){
	FRESULT fatRes;

	Int16 status;

	Uint16 field = 0;
	UINT bw;

	fatRes = f_open(&rtc_time_file, RTC_FILE_CONFIG, FA_READ);
	debug_printf(" try to open---%s \r\n", RTC_FILE_CONFIG);
	if(!fatRes){
		// update rtc time
		// first 2 bites are day
		fatRes = f_read(&rtc_time_file,  &field, 2, &bw);
		//debug_printf(" Day is %d \r\n", field);
		RTCGetDate.day = field;

		fatRes = f_read(&rtc_time_file,  &field, 2, &bw);
		//debug_printf(" Month is %d \r\n", field);
		RTCGetDate.month = field;

		fatRes = f_read(&rtc_time_file,  &field, 2, &bw);
		//debug_printf(" Year is %d \r\n", field);
		RTCGetDate.year = field;

		fatRes = f_read(&rtc_time_file,  &field, 2, &bw);
		//debug_printf(" Hour is %d \r\n", field);
		RTCGetTime.hours = field;

		fatRes = f_read(&rtc_time_file,  &field, 2, &bw);
		//debug_printf(" Min is %d \r\n", field);
		RTCGetTime.mins = field;

		debug_printf(" Setting Iternal RTC date time to %d-%d-%d_%d:%d\r\n",RTCGetDate.day,RTCGetDate.month,RTCGetDate.year, RTCGetTime.hours, RTCGetTime.mins);
		/* Set the RTC time */
		status = RTC_setTime(&RTCGetTime);
		if(status != CSL_SOK)
		{
				debug_printf(" RTC_setTime Failed\r\n");
				return;
		}
		else
		{
				//debug_printf(" RTC_setTime Successful\r\n");
		}

		/* Set the RTC date */
		status = RTC_setDate(&RTCGetDate);
		if(status != CSL_SOK)
		{
				debug_printf(" RTC_setDate Failed\r\n");
				return;
		}
		else
		{
				//debug_printf(" RTC_setDate Successful\r\n");
		}
	}else {
			debug_printf(" RTC: %s doesn't exists\r\n", RTC_FILE_CONFIG);
	}
	    // END INIT RTC
}

// read configuration from scheduler file at index line
/**
 *
 *   PC   ID
 *  [2B] [2B]
 *
 *  MODE == 1
 *  	   {		start time       }  {        stop time         }
 *  MODE  DD   MM   YY   hh   mm   ss   DD   MM   YY   hh   mm   ss  FIL_S FREQ  GAIN  IMP
 *  [1B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B]  [1B]  [1B]  [1B] // 34BYTES
 *
 *
 * MODE == 2
 *         {		start time       }  {     DON'T CARE       }
 *  MODE  DD   MM   YY   hh   mm   ss   XX   XX   XX   XX   XX  len  FIL_S FREQ  GAIN  IMP
 *  [1B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B] [2B]  [1B]  [1B]  [1B] // 34BYTES
 *
 */

FRESULT initConfigFromSchedulerFile(Uint16 index){
	FRESULT fatRes;

	Int16 status;
	CSL_RtcTime      startTime;
	CSL_RtcTime      stopTime;
	CSL_RtcDate      startDate;
	CSL_RtcDate      stopDate;

	CSL_RtcTime      nowTime;
	CSL_RtcDate      nowDate;
	CSL_RtcAlarm	 datetime;
	//CSL_RtcAlarm	 nextDatetime;
	CSL_RtcAlarm	 nowDatetime;

	Uint16 field = 0;
	UINT bw;
	Uint16 lineIndex = index;
	Uint16 linesToSkip= index;

	char line[34];

	// read current date and time
	RTC_getDate(&nowDate);
	RTC_getTime(&nowTime);
	nowDatetime.day 	= nowDate.day;
	nowDatetime.month 	= nowDate.month;
	nowDatetime.year 	= nowDate.year;
	nowDatetime.hours	= nowTime.hours;
	nowDatetime.mins 	= nowTime.mins;
	nowDatetime.secs 	= nowTime.secs;

	debug_printf(" Current date time is: %d/%d/%d %d:%d:%d \r\n",
			nowDatetime.day, nowDatetime.month, nowDatetime.year,
			nowDatetime.hours, nowDatetime.mins, nowDatetime.secs);
	debug_printf("\r\n");

	LCD_Write("Date-time:      %d/%d/%d %d:%d:%d",
			nowDatetime.day, nowDatetime.month, nowDatetime.year,
			nowDatetime.hours, nowDatetime.mins, nowDatetime.secs);
	int d,e;
	for (d=0; d<0xFFF; d++)
		for (e=0; e<0xFFF; e++)
			asm(" NOP ");

	//read config from file
	//debug_printf("Read scheduler file\r\n");
	fatRes = f_open(&file_config, FILE_SHEDULER, FA_READ);
	if(!fatRes) {
		// skip PC
		fatRes = f_read(&file_config,  &field, 2, &bw);
		//debug_printf(" PC is %d \r\n", field);
		// read ID
		fatRes = f_read(&file_config,  &ID, 2, &bw);
		//debug_printf(" ID is %d \r\n", ID);

		// NOW SKIP INDEX-1 lines
		//index-=1;
		SKIP:
		debug_printf(" Skipping %d lines\r\n", linesToSkip);
		while(linesToSkip>0){
			//debug_printf(" Skipping %d lines\r\n", index);
			fatRes = f_read(&file_config,  line, 30, &bw);
			linesToSkip--;
			lineIndex++;
		}

		// read MODE
		fatRes = f_read(&file_config,  &mode, 1, &bw);
		//debug_printf(" Mode is %d \r\n", mode);
		if(mode == MODE_ALWAYS_ON) {
			// START DATETIME
			// DAY
			debug_printf(" MODE_ALWAYS_ON\r\n");
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startDate.day = field;
			//debug_printf(" Start day is %d \r\n", startDate.day);
			// MONTH
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startDate.month = field;
			//debug_printf(" Start month is %d \r\n", startDate.month);
			// YEAR
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startDate.year = field;
			//debug_printf(" Start year is %d \r\n", startDate.year);
			// HOURS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startTime.hours = field;
			//debug_printf(" Start hours is %d \r\n", startTime.hours);
			// MINUTES
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startTime.mins = field;
			//debug_printf(" Start mins is %d \r\n", startTime.mins);
			// SECONDS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startTime.secs = field;
			//debug_printf(" Start secs is %d \r\n", startTime.secs);

			// IF START DATETIME IS AFTER NOW GO TO SLEEP UNTIL START DATETIME
			datetime.day 	= startDate.day;
			datetime.month 	= startDate.month;
			datetime.year 	= startDate.year;
			datetime.hours	= startTime.hours;
			datetime.mins 	= startTime.mins;
			datetime.secs 	= startTime.secs;

			debug_printf("  START date time: %d/%d/%d %d:%d:%d \r\n",
					datetime.day, datetime.month, datetime.year,
					datetime.hours, datetime.mins,datetime.secs);

			if(isAfter(datetime, nowDatetime)){
				status = RTC_setAlarm(&datetime);
				if(status != CSL_SOK)
				{
					debug_printf("  RTC: setAlarm Failed\r\n");
				} else {
					debug_printf("  Next task start date is in the future\r\n");
					debug_printf("  Going to sleep until: %d/%d/%d %d:%d:%d \r\n",
							datetime.day, datetime.month, datetime.year,
							datetime.hours, datetime.mins,datetime.secs);

					LCD_Write("LPMode wakeup:  %d/%d/%d %d:%d:%d",
							  datetime.day, datetime.month, datetime.year,
							  datetime.hours, datetime.mins,datetime.secs);
				}
				set_sampling_frequency_gain_impedence(frequency, gain, impedance);
				RTC_shutdownToRTCOnlyMonde();
			}

			// STOP DATETIME
			// DAY
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopDate.day = field;
			//debug_printf(" Stop day is %d \r\n", stopDate.day);
			// MONTH
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopDate.month = field;
			//debug_printf(" Stop month is %d \r\n", stopDate.month);
			// YEAR
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopDate.year = field;
			//debug_printf(" Stop year is %d \r\n", stopDate.year);
			// HOURS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopTime.hours = field;
			//debug_printf(" Stop hours is %d \r\n", stopTime.hours);
			// MINUTES
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopTime.mins = field;
			//debug_printf(" Stop mins is %d \r\n", stopTime.mins);
			// SECONDS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopTime.secs = field;
			//debug_printf(" Stop secs is %d \r\n", stopTime.secs);

			// file size seconds
			fatRes = f_read(&file_config,  &field, 2, &bw);
			seconds = field;
			recTimeMinutes = 15000; // max number of minutes.... the writing routine is terminated by an interrupt....
			//debug_printf(" File size in seconds is %d \r\n", seconds);

			//frequency
			fatRes = f_read(&file_config,  &field, 1, &bw);
			if(field == 1)
				frequency = 16000; // S_RATE_16KHZ
			else if(field == 2)
				frequency = 24000; // S_RATE_24KHZ
			else if(field == 3)
				frequency = 48000; // S_RATE_48KHZ
			else if(field == 4)
				frequency = 96000; // S_RATE_96KHZ
			else if(field == 5)
				frequency = 192000; // S_RATE_192KHz
			//debug_printf(" Frequency is is %d \r\n", frequency);
			step_per_second = frequency/DMA_BUFFER_SZ;
			//gain
			fatRes = f_read(&file_config,  &field, 1, &bw);
			gain = field;
			//impedance
			fatRes = f_read(&file_config,  &field, 1, &bw);
			if(field == 1)
				impedance = 0x10; // IMPEDANCE_10K
			else if(field == 2)
				impedance = 0x20; // IMPEDANCE_20K
			else if(field == 3)
				impedance = 0x30; // IMPEDANCE_40K
			//debug_printf(" Impedance is %X \r\n", impedance);

			// IF STOP DATETIME IS NOT IN THE FUTURE  GOTO SKIP LINES PROCEDURE
			// schedule interrupt to stop writing at the end
			stopWritingTime.day 	= stopDate.day;
			stopWritingTime.month 	= stopDate.month;
			stopWritingTime.year	= stopDate.year;
			stopWritingTime.hours	= stopTime.hours;
			stopWritingTime.mins	= stopTime.mins;
			stopWritingTime.secs	= stopTime.secs;

			LCD_Write("M-ALWAYSON stop:%d/%d/%d %d:%d:%d",
					  stopWritingTime.day, stopWritingTime.month, stopWritingTime.year,
					  stopWritingTime.hours, stopWritingTime.mins,stopWritingTime.secs);

			debug_printf("  STOP date time: %d/%d/%d %d:%d:%d \r\n",
					stopWritingTime.day, stopWritingTime.month, stopWritingTime.year,
					stopWritingTime.hours, stopWritingTime.mins,stopWritingTime.secs);

			if(!isAfter(stopWritingTime, nowDatetime)){
				debug_printf("  Stop date time is elapsed!!!\r\n");
				debug_printf("  Need to skip a line .... scheduler or program counter are out of date???\r\n");
				if(stopWritingTime.year == 1){ // to sleep when at the end of scheduler file
					set_sampling_frequency_gain_impedence(frequency, gain, impedance);
					RTC_shutdownToRTCOnlyMonde();
				}
				increaseProgramCounter(lineIndex);
				linesToSkip = 1;
				goto SKIP;
			}

			status = RTC_setAlarm(&stopWritingTime);

			if(status != CSL_SOK)
			{
				debug_printf("  RTC: setAlarm Failed\r\n");
			} else {
				debug_printf("  Actual task will end at: %d/%d/%d %d:%d:%d \r\n",
						stopWritingTime.day, stopWritingTime.month, stopWritingTime.year,
						stopWritingTime.hours, stopWritingTime.mins,stopWritingTime.secs);
			}


		}else if (mode == MODE_CALENDAR){
			// START DATETIME
			// DAY
			debug_printf(" MODE_CALENDAR\r\n");
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startDate.day = field;
			//debug_printf(" Start day is %d \r\n", startDate.day);
			// MONTH
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startDate.month = field;
			//debug_printf(" Start month is %d \r\n", startDate.month);
			// YEAR
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startDate.year = field;
			//debug_printf(" Start year is %d \r\n", startDate.year);
			// HOURS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startTime.hours = field;
			//debug_printf(" Start hours is %d \r\n", startTime.hours);
			// MINUTES
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startTime.mins = field;
			//debug_printf(" Start mins is %d \r\n", startTime.mins);
			// SECONDS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			startTime.secs = field;
			//debug_printf(" Start secs is %d \r\n", startTime.secs);

			// IF START DATETIME IS AFTER NOW GO TO SLEEP UNTIL START DATETIME
			datetime.day 	= startDate.day;
			datetime.month 	= startDate.month;
			datetime.year 	= startDate.year;
			datetime.hours	= startTime.hours;
			datetime.mins 	= startTime.mins;
			datetime.secs 	= startTime.secs;

			debug_printf("  START date time: %d/%d/%d %d:%d:%d \r\n",
								datetime.day, datetime.month, datetime.year,
								datetime.hours, datetime.mins, datetime.secs);

			if(isAfter(datetime, nowDatetime)){
				status = RTC_setAlarm(&datetime);
				if(status != CSL_SOK)
				{
					debug_printf("  RTC: setAlarm Failed\r\n");
				} else {
					debug_printf("  Next task start date is in the future\r\n");
					debug_printf("  Going to sleep until: %d/%d/%d %d:%d:%d \r\n",
									datetime.day, datetime.month, datetime.year,
									datetime.hours, datetime.mins,datetime.secs);

					LCD_Write("LPMode wakeup:  %d/%d/%d %d:%d:%d",
							  datetime.day, datetime.month, datetime.year,
							  datetime.hours, datetime.mins,datetime.secs);
				}
				set_sampling_frequency_gain_impedence(frequency, gain, impedance);
				RTC_shutdownToRTCOnlyMonde();
			}
			// STOP DATETIME
			// DAY
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopDate.day = field;
			//debug_printf(" Stop day is %d \r\n", stopDate.day);
			// MONTH
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopDate.month = field;
			//debug_printf(" Stop month is %d \r\n", stopDate.month);
			// YEAR
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopDate.year = field;
			//debug_printf(" Stop year is %d \r\n", stopDate.year);
			// HOURS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopTime.hours = field;
			//debug_printf(" Stop hours is %d \r\n", stopTime.hours);
			// MINUTES
			fatRes = f_read(&file_config,  &field, 2, &bw);
			stopTime.mins = field;
			stopTime.secs = 0;
			//debug_printf(" Stop mins is %d \r\n", stopTime.mins);



			// REC TIME IN MINUTES
			fatRes = f_read(&file_config,  &field, 2, &bw);
			recTimeMinutes = field;
			//debug_printf(" RecTimeMinutes is %d \r\n", recTimeMinutes);
			// file size seconds
			fatRes = f_read(&file_config,  &field, 2, &bw);
			seconds = field;
			//debug_printf(" File size in seconds is %d \r\n", seconds);

			//frequency
			fatRes = f_read(&file_config,  &field, 1, &bw);
			if(field == 1)
				frequency = 16000; // S_RATE_16KHZ
			else if(field == 2)
				frequency = 24000; // S_RATE_24KHZ
			else if(field == 3)
				frequency = 48000; // S_RATE_48KHZ
			else if(field == 4)
				frequency = 96000; // S_RATE_96KHZ
			else if(field == 5)
				frequency = 192000; // S_RATE_192KHz
			//debug_printf(" Frequency is %d \r\n", frequency);
			step_per_second = frequency/DMA_BUFFER_SZ;
			//gain
			fatRes = f_read(&file_config,  &field, 1, &bw);
			gain = field;
			//impedance
			fatRes = f_read(&file_config,  &field, 1, &bw);
			if(field == 1)
				impedance = 0x10; // IMPEDANCE_10K
			else if(field == 2)
				impedance = 0x20; // IMPEDANCE_20K
			else if(field == 3)
				impedance = 0x30; // IMPEDANCE_40K
			//debug_printf(" Impedance is %X \r\n", impedance);

			// IF STOP DATETIME IS NOT IN THE FUTURE  GOTO SKIP LINES PROCEDURE
						// schedule interrupt to stop writing at the end
			stopWritingTime.day 	= stopDate.day;
			stopWritingTime.month 	= stopDate.month;
			stopWritingTime.year	= stopDate.year;
			stopWritingTime.hours	= stopTime.hours;
			stopWritingTime.mins	= stopTime.mins;
			stopWritingTime.secs	= stopTime.secs;

			LCD_Write("M-CALENDAR stop:%d/%d/%d %d:%d:%d",
					  stopWritingTime.day, stopWritingTime.month, stopWritingTime.year,
					  stopWritingTime.hours, stopWritingTime.mins,stopWritingTime.secs);

			debug_printf("  STOP date time: %d/%d/%d %d:%d:%d \r\n",
					stopWritingTime.day, stopWritingTime.month, stopWritingTime.year,
					stopWritingTime.hours, stopWritingTime.mins,stopWritingTime.secs);

			if(!isAfter(stopWritingTime, nowDatetime)){
				debug_printf("  Stop date time is elapsed!!!\r\n");
				debug_printf("  Need to skip a line .... scheduler or program counter are out of date???\r\n");
				if(stopWritingTime.year == 1) {// to sleep when at the end of scheduler file
					set_sampling_frequency_gain_impedence(frequency, gain, impedance);
					RTC_shutdownToRTCOnlyMonde();
				}
				increaseProgramCounter(lineIndex);
				lineIndex++;
				goto SKIP;
			}

		}
		else
			debug_printf(" Mode not valid\r\n");
	}
	else{
		debug_printf(" Read config file error: default initialization value\r\n"); //error: file don't exist
		mode = MODE_ALWAYS_ON;
		frequency = 48000; // S_RATE_48KHZ
		impedance = 0x20; // IMPEDANCE_20K
		gain = 10;
		seconds = 60;
	}
	numberOfFiles = (Uint16)((((long unsigned int )recTimeMinutes) * 60)/seconds);
	if(frequency > 96000)
		digital_gain = 40;
	debug_printf(" Number of file to write...%d\r\n",numberOfFiles); //error: file don't exist
	fatRes = f_close (&file_config);
	return fatRes;
}

// read from file the program counter
/*Uint16 readProgramCounter(){
	Uint16 line = 0;
	Uint bw = 0;
	FRESULT fatRes;
	FIL fileProgramCounter;
	//debug_printf(" Opening program counter file %s\r\n", FILE_PROGRAM_COUNTER);
	fatRes = f_open(&fileProgramCounter, FILE_PROGRAM_COUNTER, FA_READ);
	if(!fatRes) {
		fatRes = f_read(&fileProgramCounter,  &line, 2, &bw);
		debug_printf(" Program counter is %d return code %d\r\n", line, fatRes);
		fatRes = f_close (&fileProgramCounter);
	}else{
		debug_printf(" Program counter file not found \r\n");
	}
	return line;
}*/

// increase program counter
/*FRESULT increaseProgramCounter(Uint16 pc){
	Uint16 newPc = pc+1;
	Uint bw = 0;
	FRESULT fatRes;


	fatRes = f_open(&fileProgramCounter, FILE_PROGRAM_COUNTER, FA_WRITE | FA_CREATE_ALWAYS);
	if(!fatRes) {
		fatRes = f_write (&fileProgramCounter, &newPc, 2, &bw);
		debug_printf("   Program counter write %d return code %d\r\n", newPc, fatRes);
		fatRes = f_close (&fileProgramCounter);
	}else {
		debug_printf("   Program counter error writing %d\r\n", fatRes);
	}
	return fatRes;
}*/

FRESULT readNextWakeUpDateTimeFromScheduler(Uint16 i, CSL_RtcAlarm *nextAlarmTime){
	FRESULT fatRes;

	Int16 status;
	Uint16 field = 0;
	UINT bw;
	Uint16 index = i+1;
	char line[34];


	//read config from file
	//debug_printf("Read scheduler file\r\n");
	fatRes = f_open(&file_config, FILE_SHEDULER, FA_READ);
	if(!fatRes) {
		// skip PC
		fatRes = f_read(&file_config,  &field, 2, &bw);
		//debug_printf(" PC is %d \r\n", field);
		// read ID
		fatRes = f_read(&file_config,  &ID, 2, &bw);
		//debug_printf(" ID is %d \r\n", ID);

		// NOW SKIP INDEX-1 lines
		//index-=1;
		debug_printf("   Skipping %d lines\r\n", index);
		while(index>0){
			//debug_printf(" Skipping %d lines\r\n", index);
			fatRes = f_read(&file_config,  line, 30, &bw);
			index--;
		}

		// read MODE
		fatRes = f_read(&file_config,  &mode, 1, &bw);
		//debug_printf(" Mode is %d \r\n", mode);
		if(mode == MODE_ALWAYS_ON || MODE_CALENDAR) {
			// START DATETIME
			// DAY
			fatRes = f_read(&file_config,  &field, 2, &bw);
			nextAlarmTime->day = field;
			//debug_printf(" wake-up day is %d \r\n", nextAlarmTime->day);
			// MONTH
			fatRes = f_read(&file_config,  &field, 2, &bw);
			nextAlarmTime->month = field;
			//debug_printf(" wake-up month is %d \r\n", nextAlarmTime->month);
			// YEAR
			fatRes = f_read(&file_config,  &field, 2, &bw);
			nextAlarmTime->year = field;
			//debug_printf(" wake-up year is %d \r\n", nextAlarmTime->year);
			// HOURS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			nextAlarmTime->hours = field;
			//debug_printf(" wake-up hours is %d \r\n", nextAlarmTime->hours);
			// MINUTES
			fatRes = f_read(&file_config,  &field, 2, &bw);
			nextAlarmTime->mins = field;
			//debug_printf(" wake-up mins is %d \r\n", nextAlarmTime->mins);
			// SECONDS
			fatRes = f_read(&file_config,  &field, 2, &bw);
			nextAlarmTime->secs = field;
			//debug_printf(" wake-up secs is %d \r\n", nextAlarmTime->secs);
		}
		else
			debug_printf(" Mode not valid while looking for next wake-up datetime\r\n");
	}
	else{
		debug_printf(" Read config file error: default initialization value\r\n"); //error: file don't exist
	}
	fatRes = f_close (&file_config);
	return fatRes;
}




/* Resets C5515 */
void C5515_reset(void)
{
    volatile int i;

    // disable all interrupts (IER0 and IER1)
    *(volatile ioport unsigned int *)(0x0000) = 0x0000;
    *(volatile ioport unsigned int *)(0x0045) = 0x0000;

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;

    // enable all peripherials
    *(volatile ioport unsigned int *)(0x1c02) = 0;//0x3FA5;
    // 0011 1111 1010 0101
    *(volatile ioport unsigned int *)(0x1c03) = 0;

    // reset peripherals
    *(volatile ioport unsigned int *)(0x1c04) = 0x0020;
    *(volatile ioport unsigned int *)(0x1c05) = 0x00BF;
    // some delay
    for (i=0; i<0xFFF; i++);

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;
}

// Clock gating for unused peripherals
void ClockGating(void)
{
    Uint16 pcgcr_value, clkstop_value;
    
    // set PCGCR1
    pcgcr_value = 0; 
    // clock gating SPI
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_SPICG, DISABLED);
    // clock gating SD/MMC
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD0CG, DISABLED); //LELE to use SD on MMC0
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD1CG, DISABLED);
    // clock stop request for UART
#if DEBUG_LEVEL < 1
    clkstop_value = CSL_FMKT(SYS_CLKSTOP_URTCLKSTPREQ, REQ);
    // write to CLKSTOP
    CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
    // wait for acknowledge
    while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_URTCLKSTPACK)==0);
    // clock gating UART
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_UARTCG, DISABLED);
#endif
    // clock stop request for EMIF
    clkstop_value = CSL_FMKT(SYS_CLKSTOP_EMFCLKSTPREQ, REQ);
    // write to CLKSTOP
    CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
    // wait for acknowledge
    while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_EMFCLKSTPACK)==0);
    // clock gating EMIF
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_EMIFCG, DISABLED);
    // clock gating unused I2S (I2S 0, 1, 3)
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S0CG, DISABLED);
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S1CG, DISABLED);
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S2CG, DISABLED);
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S3CG, DISABLED);
    // clock gating DMA0
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR1_DMA0CG, DISABLED);
    // clock gating Timer 1
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR1CG, DISABLED);
    // clock gating Timer 2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR2CG, DISABLED);
    // write to PCGCR1
    CSL_FSET(CSL_SYSCTRL_REGS->PCGCR1, 15, 0, pcgcr_value);
    
    // set PCGCR2
    pcgcr_value = 0; 
    // clock gating LCD
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_LCDCG, DISABLED);
    // clock gating SAR
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR2_SARCG, DISABLED);
    // clock gating DMA1
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA1CG, DISABLED);
    // clock gating DMA2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA2CG, DISABLED);
    // clock gating DMA3
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA3CG, DISABLED);
    // clock analog registers
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR2_ANAREGCG, DISABLED);
    // write to PCGCR2
    CSL_FSET(CSL_SYSCTRL_REGS->PCGCR2, 15, 0, pcgcr_value);
    
    // disable the CLKOUT. It is on reset
    // set bit 2 of ST3_55 to 1
    //LELE 18-09-2013
   /* asm("    bit(ST3, #ST3_CLKOFF) = #1");
    
    // turn off the XF
    // set bit 13 of ST1_55 to 0
    asm("    bit(ST1, #ST1_XF) = #0"); */

#ifdef C5535_EZDSP
    // turn off the DS3-6
    // set the GPIO pin 14 - 15 to output, set SYS_GPIO_DIR0 (0x1C06) bit 14 and 15 to 1 
   /* *(volatile ioport unsigned int *)(0x1C06) |= 0xC000;
    // set the GPIO pin 16 - 17 to output, set SYS_GPIO_DIR1 (0x1C07) bit 0 and 1 to 1 
    *(volatile ioport unsigned int *)(0x1C07) |= 0x0003;
    
    // set the GPIO 14 - 15 to 0, set SYS_GPIO_DATAOUT0 (0x1C0A) bit 14 and 15 to 0
    //*(volatile ioport unsigned int *)(0x1C0A) &= 0x3FFF;
    *(volatile ioport unsigned int *)(0x1C0A) |= 0xC000;
    // set the GPIO 16 - 17 to 0, set SYS_GPIO_DATAOUT1 (0x1C0B) bit 0 and 1 to 0
    //*(volatile ioport unsigned int *)(0x1C0B) &= 0xFFFC;
    *(volatile ioport unsigned int *)(0x1C0B) |= 0x0003;
*/
    //LELE: to reboot FFTHWA
    *(ioport volatile unsigned *)0x0001 = 0x000E;
    asm(" idle"); // must add at least one blank before idle in " ".
#endif

    return;
}

#if 0
void userIdle(void)
{
    // set CPUI bit in ICR
    *(volatile ioport Uint16 *)(0x0001) = 0x000F;
    // execute idle instruction to make CPU idle
    asm("    idle");        
}
#endif
