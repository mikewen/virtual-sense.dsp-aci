/*
 * debug.c
 *
 *  Created on: 23/gen/2014
 *      Author: Emanuele Lattanzi
 */

#include <stdio.h>
#include "psp_common.h"
#include "csl_uart.h"
#include "csl_uartAux.h"
#include "csl_general.h"

#include "cslr_sysctrl.h"
#include "main_config.h"

CSL_UartObj 		uartObj;
CSL_Status 			status;
CSL_UartHandle    	hUart;
#pragma DATA_SECTION(debugBuffer, ".uart_debugBuffer");
 static  char debugBuffer[256];
CSL_UartSetup uartSetup;
Uint8 log_start = 0;

#if DEBUG_LEVEL == 2
#include "ff.h"
FIL log_file;
#endif



void init_debug(Uint16 clock){

#if DEBUG_LEVEL > 0
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

void start_log(){
#if DEBUG_LEVEL == 2
	    FRESULT fatRes;
	    log_start = 1;
	    fatRes = f_open(&log_file, FILE_LOG, FA_WRITE | FA_CREATE_ALWAYS);
#endif
}


void printdebug(const char *format, ...){

	va_list arg;
	int done;
#if DEBUG_LEVEL == 2
	Uint bw = 0;
	FRESULT fatRes;
#endif
	va_start (arg, format);
	done = vsprintf (debugBuffer, format, arg);
#if DEBUG_LEVEL > 0
    status = UART_fputs(hUart,debugBuffer,0);
#if DEBUG_LEVEL == 2
    if(log_start)
    	fatRes = f_write (&log_file, &debugBuffer, done, &bw);	/* Write data to a file */
#endif
#endif
}

