/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */ 
/* 	circular_buffer.h                                                        */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Header file for circular buffer.                                        */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00	                                                         */ 
/*   Author  : Emanuele Lattanzi                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   13th September 2013. Created by Emanuele Lattanzi                       */
/*                                                                           */
/*****************************************************************************/
/*
 *
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

void circular_buffer_put(Int16 item);
Int16 circular_buffer_get();

#endif

/*****************************************************************************/
/* End of circular_buffer.h                                                        */
/*****************************************************************************/
