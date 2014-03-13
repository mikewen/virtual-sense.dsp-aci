/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */ 
/*      rtc.h                                                            */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Header file for rtc functions                                           */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00                                                              */
/*   Author  : Emanuele Lattanzi                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   27th January 2014. Created by Emanuele Lattanzi                       */
/*                                                                           */
/*****************************************************************************/
/*
 *
 */

#ifndef RTC_CONFIG_H
#define RTC_CONFIG_H

//#include <stdlib.h>
//#include <stdio.h>

unsigned short RTCNeedUpdate();
void initRTC();
void RTC_scheduleAlarmAfterMinutes(unsigned short minutes);
void RTC_shutdownToRTCOnlyMonde();
Int16 RTC_initRtcFromFile();
void RTC_initializaEventEveryMinute();

#endif

/*****************************************************************************/
/* End of rtc.h                                                        */
/*****************************************************************************/
