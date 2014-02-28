/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */ 
/*      main_config.h                                                            */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Header file for main configuration.                                     */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00                                                              */
/*   Author  : Emanuele Lattanzi                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   12th September 2013. Created by Emanuele Lattanzi                       */
/*                                                                           */
/*****************************************************************************/
/*
 *
 */

#ifndef MAIN_CONFIG_H
#define MAIN_CONFIG_H
/*****************************************************************************

			 192 kHz: P=1, R=1, J=7, D=1680 (0x690)  => .1680 => J.D = 7.1680
                      PLL_CLK = (12e6 * 1 * 7.1680)/1 = 86016000
                      NADC=2, MADC=7, AOSR=32
                      ADC_FS = PLL_CLK/(2 * 7 * 32) = 192000
              96 kHz: P=1, R=1, J=7, D=1680 (0x690)  => .1680 => J.D = 7.1680
                                  PLL_CLK = (12e6 * 1 * 7.1680)/1 = 86016000
                      NADC=2, MADC=7, AOSR=64
                      ADC_FS = PLL_CLK/(2 * 7 * 64) = 96000
              48 kHz: P=1, R=1, J=7, D=1680 (0x690)  => .1680 => J.D = 7.1680
                                  PLL_CLK = (12e6 * 1 * 7.168)/1 = 86016000
                      NADC=2, MADC=7, AOSR=128
                      ADC_FS = PLL_CLK/(2 * 7 * 128) = 48000
              24 kHz: P=2, R=1, J=7, D=1680 (0x690)  => .1680 => J.D = 7.1680
                                  PLL_CLK = (12e6 * 1 * 7.1680)/1 = 86016000
                      NADC=2, MADC=7, AOSR=128
                      ADC_FS = PLL_CLK/(2 * 7 * 128) = 24000
              16 kHz: P=3, R=1, J=7, D=1680 (0x690)  => .1680 => J.D = 7.1680
                                  PLL_CLK = (12e6 * 1 * 7.1680)/1 = 86016000
                      NADC=2, MADC=7, AOSR=128
                      ADC_FS = PLL_CLK/(2 * 7 * 128) = 16000

*****************************************************************************/
/*
#define S_RATE_192KHZ										   (192000L)
#define S_RATE_96KHZ										 	(96000L)
#define S_RATE_48KHZ										 	(48000L)
#define S_RATE_24KHZ										 	(24000L)
#define S_RATE_16KHZ										 	(16000L)

#define IMPEDANCE_10K											  (0x10)
#define IMPEDANCE_20K											  (0x20)
#define IMPEDANCE_40K											  (0x30)
*/


//#define FREQUENCY 								           (S_RATE_48KHZ)
//#define GAIN														 (35)
//#define IMPEDANCE										  (IMPEDANCE_40K)

#define FFT_LENGHT                                            	   (1024)

//#define SECONDS                                                      (10)
#define PROCESS_BUFFER_SIZE                              		   (512L)

//number of DMA_BUFFER to cover one second
//#define STEP_PER_SECOND                         (FREQUENCY/DMA_BUFFER_SZ)

#define DMA_BUFFER_SZ      					   				       (256L) // number of sample in fat sector
#define DMA_TARNSFER_SZ            						(2*DMA_BUFFER_SZ)

#define RTC_FILE_CONFIG											"time.rtc"
#define FILE_SHEDULER											"scheduler.bin"
#define FILE_COUNTER											"pc.bin"

#define FW_VER "0.1"

#define MODE_ALWAYS_ON	1
#define MODE_EVERY_MINUT 2
#define MODE_CALENDAR 3

#define DEBUG_UART 0

#endif

/* Operation mode:
 * 0 error
 * 1 interrupt evry minute
 * 2
 */
extern Uint8 mode;
/* Frequency:
 * 0 error
 * 1 16KHZ
 * 2 24KHZ
 * 3 48KHZ
 * 4 96KHZ
 * 5 192KHz
 */
extern Uint32 frequency;
extern Uint32 step_per_second;
/* Gain:
 * 1 byte from 0 to 50
 */
extern Uint8 gain;
/* Impdence:
 * 0x10 10K
 * 0x20 20K
 * 0x30 30K
 */
extern Uint8 impedence;
/* Wav lenght:
 * 0 no file
 * 2 byte for lenght
 */
extern Uint16 seconds;

extern Uint16 file_counter;
/*****************************************************************************/
/* End of main_config.h                                                        */
/*****************************************************************************/
