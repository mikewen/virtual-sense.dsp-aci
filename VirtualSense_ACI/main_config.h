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

#define S_RATE_192KHZ										   (192000L)
#define S_RATE_96KHZ										 	(96000L)
#define S_RATE_48KHZ										 	(48000L)
#define S_RATE_24KHZ										 	(24000L)
#define S_RATE_16KHZ										 	(16000L)

#define IMPEDANCE_10K											  (0x10)
#define IMPEDANCE_20K											  (0x20)
#define IMPEDANCE_40K											  (0x30)



#define FREQUENCY 								           (S_RATE_48KHZ)
#define GAIN														 (48)
#define IMPEDANCE										  (IMPEDANCE_10K)

#define FFT_LENGHT                                            	   (1024)

#define SECONDS                                                       (5)
#define PROCESS_BUFFER_SIZE                              		   (512L)

//number of DMA_BUFFER to cover one second
#define STEP_PER_SECOND                      (S_RATE_48KHZ/DMA_BUFFER_SZ)

#define DMA_BUFFER_SZ      					   				       (256L) // number of sample in fat sector
#define DMA_TARNSFER_SZ            						(2*DMA_BUFFER_SZ)

#endif

/*****************************************************************************/
/* End of main_config.h                                                        */
/*****************************************************************************/
