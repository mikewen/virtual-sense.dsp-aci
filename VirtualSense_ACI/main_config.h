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

#define FFT_LENGHT                                                   1024
#define SECONDS                                                         5
#define PROCESS_BUFFER_SIZE                              		    2048L //BYTE size of the fat sector
#define SAMP_RATE                                                 192000L
#define STEP_PER_SECOND                                              187L //number of DMA_BUFFER to cover one second

//#define DMA_BUFFER_MS                                                (10)
#define DMA_BUFFER_SZ          					   				   (1024L) // number of sample in fat sector
#define DMA_TARNSFER_SZ            						(2*DMA_BUFFER_SZ)

#endif

/*****************************************************************************/
/* End of main_config.h                                                        */
/*****************************************************************************/
