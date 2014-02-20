/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */ 
/*      debug_uart.h                                                            */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Header file for uart debug                                              */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00                                                              */
/*   Author  : Emanuele Lattanzi                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   23th January 2014. Created by Emanuele Lattanzi                       */
/*                                                                           */
/*****************************************************************************/
/*
 *
 */
#include "csl_types.h"


#ifndef DEBUG_UART_MAIN_CONFIG_H
#define DEBUG_UART_MAIN_CONFIG_H
#define debug_printf printdebug

void init_debug_over_uart(Uint16 clock);
void printdebug(const char *format, ...);

#endif

/*****************************************************************************/
/* End of debug_uart.h                                                        */
/*****************************************************************************/
