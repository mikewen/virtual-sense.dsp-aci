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

#include "cslr_sysctrl.h"
#include "main_config.h"

CSL_UartObj 		uartObj;
CSL_Status 			status;
CSL_UartHandle    	hUart;
#pragma DATA_SECTION(uart_debugBuffer, ".uart_debugBuffer");
 static  char uart_debugBuffer[256];
CSL_UartSetup uartSetup;



void init_debug_over_uart(Uint16 clock){
#if DEBUG_UART

	    uartSetup.afeEnable = CSL_UART_NO_AFE;
	    uartSetup.baud = 57600;
	    uartSetup.clkInput = 1000000*clock;
	    uartSetup.fifoControl = CSL_UART_FIFO_DISABLE;
	    uartSetup.loopBackEnable = CSL_UART_NO_LOOPBACK;
	    uartSetup.parity =  CSL_UART_DISABLE_PARITY;
	    uartSetup.rtsEnable = CSL_UART_NO_RTS;
	    uartSetup.stopBits = 0;
        uartSetup.wordLength = CSL_UART_WORD8;
	 /* PP Mode 1 (SPI, GPIO[17:12], UART, and I2S2) */
	    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

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


       /* PP Mode 1 (SPI, GPIO[17:12], UART, and I2S2) */
          CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);
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
#endif
}


void printdebug(const char *format, ...){
#if DEBUG_UART
	va_list arg;
	int done;
	va_start (arg, format);
	done = vsprintf (uart_debugBuffer, format, arg);
    status = UART_fputs(hUart,uart_debugBuffer,0);
#endif
}

