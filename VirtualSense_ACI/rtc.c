/*
 * RTC.c
 *
 *  Created on: 16/set/2013
 *      Author: lattanzi
 */


#include "debug_uart.h" // to redirect debug_printf over UART
#include <csl_rtc.h>
#include <csl_intc.h>
#include <csl_general.h>

//#include "VC5505_CSL_BIOS_cfg.h"
#include "VirtualSense_ACIcfg.h"

#define RTC_TIME_PRINT_CYCLE    (0xFFu)
#define RTC_CALL_BACK           (1u)

CSL_RtcTime	     InitTime;
CSL_RtcDate 	 InitDate;
//CSL_RtcTime 	 GetTime;
//CSL_RtcDate 	 GetDate;
CSL_RtcConfig    rtcConfig;
CSL_RtcConfig    rtcGetConfig;
CSL_RtcAlarm     AlarmTime;
CSL_RtcIsrAddr   isrAddr;
CSL_RtcIsrDispatchTable      rtcDispatchTable;
volatile Uint32 rtcTimeCount = RTC_TIME_PRINT_CYCLE;
Uint16    secIntrCnt = 0;

/* Reference the start of the interrupt vector table */
extern void VECSTART(void);
/* Prototype declaration for ISR function */
interrupt void rtc_isr(void);

void rtc_msIntc(void);
void rtc_secIntc(void);
void rtc_minIntc(void);
void rtc_hourIntc(void);
void rtc_dayIntc(void);
void rtc_extEvt(void);
void rtc_alarmEvt(void);

unsigned short RTCNeedUpdate(){
	Uint16 needUpdated = 0;
	needUpdated = CSL_RTC_REGS->RTCNOPWR;
	return needUpdated;
}

void initRTC()
{
	CSL_Status    status;

	/* Set the RTC config structure */
	rtcConfig.rtcyear  = 8;
	rtcConfig.rtcmonth = 8;
	rtcConfig.rtcday   = 8;
	rtcConfig.rtchour  = 8;
	rtcConfig.rtcmin   = 8;
	rtcConfig.rtcsec   = 8;
	rtcConfig.rtcmSec  = 8;

	rtcConfig.rtcyeara  = 8;
	rtcConfig.rtcmontha = 8;
	rtcConfig.rtcdaya   = 8;
	rtcConfig.rtchoura  = 8;
	rtcConfig.rtcmina   = 8;
	rtcConfig.rtcseca   = 8;
	rtcConfig.rtcmSeca  = 10;

	rtcConfig.rtcintcr  = 0x803F;

	/* Set the RTC init structure */
	/* RTC will be initialized with Giulia's born day
	 * 			27-11-2011 14:24:00
	 *
	 */

	InitDate.year  = 11;
    InitDate.month = 11;
    InitDate.day   = 27;

    InitTime.hours = 14;
    InitTime.mins  = 24;
    InitTime.secs  = 00;
    InitTime.mSecs = 00;

	/* Set the RTC alarm time  at one minutes after init state*/
    AlarmTime.year  = 11;
    AlarmTime.month = 11;
    AlarmTime.day   = 27;
    AlarmTime.hours = 14;
    AlarmTime.mins  = 25;
    AlarmTime.secs  = 0;
    AlarmTime.mSecs = 00;

    /* Register the ISR function */
    isrAddr.MilEvtAddr    = rtc_msIntc;
    isrAddr.SecEvtAddr    = rtc_secIntc;
    isrAddr.MinEvtAddr    = rtc_minIntc;
    isrAddr.HourEvtAddr   = rtc_hourIntc;
    isrAddr.DayEvtAddr    = rtc_dayIntc;
    isrAddr.ExtEvtAddr    = rtc_extEvt;
    isrAddr.AlarmEvtAddr  = rtc_alarmEvt;

    status = RTC_setCallback(&rtcDispatchTable, &isrAddr);
	if(status != CSL_SOK)
	{
		debug_printf("RTC_setCallback Failed\n");
		return;
	}
	else
	{
		debug_printf("RTC_setCallback Successful\n");
	}

	/* Configure and enable the RTC interrupts using INTC module */
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	//IRQ_setVecs((Uint32)&VECSTART);
	IRQ_clear(RTC_EVENT);

	IRQ_plug (RTC_EVENT, &rtc_isr);

	IRQ_enable(RTC_EVENT);
	IRQ_globalEnable();

	/* Reset the RTC */

	if(RTCNeedUpdate()){
		RTC_reset();

		/* Configure the RTC module */
		status = RTC_config(&rtcConfig);
		if(status != CSL_SOK)
		{
			debug_printf("RTC_config Failed\n");
			return;
		}
		else
		{
			debug_printf("RTC_config Successful\n");
		}

		/* Set the RTC time */
		status = RTC_setTime(&InitTime);
		if(status != CSL_SOK)
		{
			debug_printf("RTC_setTime Failed\n");
			return;
		}
		else
		{
			debug_printf("RTC_setTime Successful\n");
		}

		/* Set the RTC date */
		status = RTC_setDate(&InitDate);
		if(status != CSL_SOK)
		{
			debug_printf("RTC_setDate Failed\n");
			return;
		}
		else
		{
			debug_printf("RTC_setDate Successful\n");
		}

		/* Set the RTC Alarm time */
		status = RTC_setAlarm(&AlarmTime);
		if(status != CSL_SOK)
		{
			debug_printf("RTC_setAlarm Failed\n");
			return;
		}
		else
		{
			debug_printf("RTC_setAlarm Successful\n");
		}

		/* Set the RTC interrupts */
	}
	/*status = RTC_setPeriodicInterval(CSL_RTC_SEC_PERIODIC_INTERRUPT);
	if(status != CSL_SOK)
	{
		debug_printf("RTC_setPeriodicInterval Failed\n");
		return;
	}
	else
	{
		debug_printf("RTC_setPeriodicInterval Successful\n");
	}
	*/
	/* Enable the RTC SEC interrupts */
	/*status = RTC_eventEnable(CSL_RTC_SECEVENT_INTERRUPT);
	if(status != CSL_SOK)
	{
		debug_printf("RTC_eventEnable for SEC EVENT Failed\n");
		return;
	}
	else
	{
		debug_printf("RTC_eventEnable for SEC EVENT Successful\n");
	}*/

	/* Enable the RTC alarm interrupts */
	/*status = RTC_eventEnable(CSL_RTC_ALARM_INTERRUPT);
	if(status != CSL_SOK)
	{
		debug_printf("RTC_eventEnable for ALARM EVENT Failed\n");
		return;
	}
	else
	{
		debug_printf("RTC_eventEnable for ALARM EVENT Successful\n");
	}*/

	debug_printf("\nStarting the RTC\n\n");
	/* Start the RTC */
	RTC_start();

	/* This loop will display the RTC time for 255 times */
	/*while(rtcTimeCount--)
	{
	 	status = RTC_getTime(&GetTime);
		if(status != CSL_SOK)
		{
			debug_printf("RTC_getTime Failed\n");
			return;
		}

	 	status = RTC_getDate(&GetDate);
		if(status != CSL_SOK)
		{
			debug_printf("RTC_getDate Failed\n");
			return;
		}

		debug_printf("Iteration %d: ",iteration++);

	    debug_printf("Time and Date is : %02d:%02d:%02d:%04d, %02d-%02d-%02d\n",
		GetTime.hours,GetTime.mins,GetTime.secs,GetTime.mSecs,GetDate.day,GetDate.month,GetDate.year);
	} */
}


void RTC_scheduleAlarmAfterMinutes(unsigned short minutes){
	CSL_RtcTime 	 actualTime;
	CSL_RtcDate 	 actualDate;
	CSL_RtcAlarm     nextAlarmTime;


	RTC_getDate(&actualDate);
	RTC_getTime(&actualTime);

	nextAlarmTime.year  = actualDate.year;
	nextAlarmTime.month = actualDate.month;
	nextAlarmTime.day   = actualDate.day;

	nextAlarmTime.hours = actualTime.hours;
	nextAlarmTime.mins  = actualTime.mins+minutes;
	if(actualTime.mins > 59){
		actualTime.mins = 0;
		actualTime.hours = actualTime.hours +1;
	}
	nextAlarmTime.secs  = actualTime.secs;
	nextAlarmTime.mSecs = 00;

	RTC_setAlarm(&nextAlarmTime);
}


void RTC_shutdownToRTCOnlyMonde(){
	unsigned int temp1920,temp1924;

	debug_printf("Set condec into low power mode\n");
	codec_sleep_mode();

	debug_printf("Preparing RTCOnlyMode\n");

	// shutdown led to shutdown extrernal oscillator
	CSL_CPU_REGS->ST1_55 &= ~CSL_CPU_ST1_55_XF_MASK;

    asm("        @#IFR0_L = #0xffff || mmap() "); // clear int flags
              asm("    @#IER0_L = #0x0000 || mmap() "); // set RTC int
              asm("    @#IFR1_L = #0xffff || mmap() "); // clear int flags
              asm("    @#IER1_L = #0x0004 || mmap() "); // set RTC int
              asm(" bit(ST1,#11) = #0 "); // GLOBAL INTERRUPT ENABLE



              // RTC configure


              asm("    *port(#0x1920) = #0x803F "); //clear interrupt flags
              asm("    *port(#0x1900) = #0x0001 "); //RTCINTEN enabled
              asm("    *port(#0x1924) = #0x8020 "); //EXTINTEN enabled
              asm("    *port(#0x1930) = #0x0000 "); //WU_DIR input

              do // waiting until RTC interrupt is enabled in the RTC domain could take 2 RTC clocks for write to propagate
              {
              temp1924 = *(volatile ioport unsigned int *) (0x1924);
              }        while ((temp1924&0x0020)==0);

              temp1920 = *(volatile ioport unsigned int *) (0x1920);
              if ((temp1920&0x0020)!=0)
              {
              asm("    *port(#0x1920) = #0x803F "); //clear interrupt flags
              }
              asm("    *port(#0x1930) = #0x0006 "); //WU_DIR input & LDO & BG shutdown
              asm("    *port(#0x1920) = #0x803F "); //clear interrupt flags

          // power down to RTC only mode
          while (1)
              {
              temp1920 = *(volatile ioport unsigned int *) (0x1920);
              if ((temp1920&0x0020)!=0)
              {
              asm("    *port(#0x1920) = #0x803F "); //clear interrupt flags
              asm("    *port(#0x1930) = #0x0006 "); //WU_DIR input & LDO & BG shutdown
              asm("    *port(#0x1920) = #0x803F "); //clear interrupt flags
              }
              }
	debug_printf("----should never happen ----\n");
}


interrupt void rtc_isr(void)
{

#ifdef RTC_CALL_BACK
    CSL_RTCEventType rtcEventType;

    rtcEventType = RTC_getEventId();

    if (((void (*)(void))(rtcDispatchTable.isr[rtcEventType])))
     {
         ((void (*)(void))(rtcDispatchTable.isr[rtcEventType]))();
     }
#else
    Uint16 statusRegVal;

    statusRegVal = CSL_RTC_REGS->RTCINTFL;

    /* check for alarm interrupt */
    if (CSL_RTC_RTCINTFL_ALARMFL_MASK ==
                (statusRegVal & (Uint16)CSL_RTC_RTCINTFL_ALARMFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_ALARMFL, SET);
    }
    /* check for external event interrupt */
    else if (CSL_RTC_RTCINTFL_EXTFL_MASK ==
                (statusRegVal &(Uint16)CSL_RTC_RTCINTFL_EXTFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_EXTFL, SET);
    }
    /* check for day interrupt */
    else if (CSL_RTC_RTCINTFL_DAYFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_DAYFL_MASK))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_DAYFL, SET);
    }
    /* check for hour interrupt */
    else if (CSL_RTC_RTCINTFL_HOURFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_HOURFL_MASK))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_HOURFL, SET);
    }
    /* check for minute interrupt */
    else if (CSL_RTC_RTCINTFL_MINFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_MINFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MINFL, SET);
    }
    /* check for seconds interrupt */
    else if (CSL_RTC_RTCINTFL_SECFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_SECFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_SECFL, SET);
    }
    /* check for milliseconds interrupt */
    else if (CSL_RTC_RTCINTFL_MSFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_MSFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MSFL, SET);
    }
#endif
}

void rtc_msIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MSFL, SET);
}

void rtc_secIntc(void)
{
	CSL_RtcTime 	 GetTime;
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_SECFL, SET);
	secIntrCnt++;
	RTC_getTime(&GetTime);
	debug_printf("\nRTC actual time %d:%d:%d\n\n",GetTime.hours, GetTime.mins,GetTime.secs);
}

void rtc_minIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MINFL, SET);
}

void rtc_hourIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_HOURFL, SET);
}

void rtc_dayIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_DAYFL, SET);
}

void rtc_extEvt(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_EXTFL, SET);
}

void rtc_alarmEvt(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_ALARMFL, SET);
    debug_printf("\nRTC Alarm Interrupt\n\n");
    SEM_post(&SEM_TimerSave);
}




