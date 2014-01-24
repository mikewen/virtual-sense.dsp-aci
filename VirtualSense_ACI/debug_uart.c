/*
 * debug_uart.c
 *
 *  Created on: 23/gen/2014
 *      Author: Emanuele Lattanzi
 */

#include <stdio.h>
#include "csl_uart.h"
#include "csl_uartAux.h"
#include "csl_general.h"
#include "csl_sysctrl.h"

CSL_UartObj 		uartObj;
CSL_Status 			status;
CSL_UartHandle    	hUart;
char debugBuffer[256];

CSL_UartSetup uartSetup =
{
	/* Input clock freq in MHz */
    100000000,
	/* Baud rate */
    57600,
	/* Word length of 8 */
    CSL_UART_WORD8,
	/* To generate 1 stop bit */
    0,
	/* Disable the parity */
    CSL_UART_DISABLE_PARITY,
	/* Disable fifo */
	/* Enable trigger 14 fifo */
	CSL_UART_FIFO_DMA1_DISABLE_TRIG14,
	/* Loop Back enable */
    CSL_UART_NO_LOOPBACK,
	/* No auto flow control*/
	CSL_UART_NO_AFE ,
	/* No RTS */
	CSL_UART_NO_RTS ,
};




void init_debug_over_uart(){


    /* Loop counter and error flag */
       status = UART_init(&uartObj,CSL_UART_INST_0,UART_POLLED);
       if(CSL_SOK != status)
       {
           printf("UART_init failed error code %d\n",status);
       }
       else
       {
    	   printf("UART_init Successful\n");
       }

       status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                            CSL_EBSR_PPMODE_1);
       if(CSL_SOK != status)
       {
           printf("SYS_setEBSR failed\n");
       }

       /* Handle created */
       hUart = (CSL_UartHandle)(&uartObj);

       /* Configure UART registers using setup structure */
       status = UART_setup(hUart,&uartSetup);
       if(CSL_SOK != status)
       {
           printf("UART_setup failed error code %d\n",status);
       }
       else
       {
   		printf("UART_setup Successful\n");
       }
}


void printdebug(const char *format, ...){

	va_list arg;
	int done;
	va_start (arg, format);
	done = vsprintf (debugBuffer, format, arg);
    status = UART_fputs(hUart,debugBuffer,0);
    if(CSL_SOK != status)
    {
    	printf("UART_fputs failed error code %d\n",status);
    }
    else
    {
    	printf("\n\nMessage Sent to HyperTerminal :\n");

    }
}

