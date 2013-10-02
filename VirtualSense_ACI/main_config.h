/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */ 
/* 	main_config.h                                                            */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Header file for main configuration.                                     */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00	                                                         */ 
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

#define FFT_LENGHT						  	  1024
#define SECONDS 				  				 5

#define SAMP_RATE					   		 48000L
#define STEP_PER_SECOND 				 	   100L //SAMP_RATE_48KHZ/PROCESS_BUFFER_SIZE

#define DMA_BUFFER_MS					   	    (10)
#define DMA_BUFFER_SZ 	       (DMA_BUFFER_MS * 48L)
#define DMA_TARNSFER_SZ            (2*DMA_BUFFER_SZ)

#define PROCESS_BUFFER_SIZE         DMA_TARNSFER_SZ //BYTE size

#endif

/*****************************************************************************/
/* End of main_config.h                                                        */
/*****************************************************************************/
