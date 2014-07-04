#include "csl_wdt.h"
#include "main_config.h"
//#include "csl_general.h"
//#include <stdio.h>

/* Macros to return the test result */
#define CSL_TEST_FAILED    (1u)
#define CSL_TEST_PASSED    (0)


CSL_WdtHandle    hWdt;// = NULL;
CSL_WdtObj    wdtObjj;

Int16 wdt_Init(void)
{
	// Watchdog timer Object structure


	CSL_Status		 status;

	WDTIM_Config	 hwConfig, getConfig;
	//Uint32           counter;
	//Uint32			 time;
	Uint16 			 delay;

	/* Open the WDTIM module */
	//hWdt = (CSL_WdtObj *)WDTIM_open(WDT_INST_0, &wdtObj, &status);
	hWdt = WDTIM_open(WDT_INST_0, &wdtObjj, &status);
	if(NULL == hWdt)
	{
		debug_printf("WDTIM: Open for the watchdog Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		debug_printf("WDTIM: Open for the watchdog Passed\n");
	}

	hwConfig.counter  = 0xFFFF;
	hwConfig.prescale = 0xFFFF;

	/* Configure the watch dog timer */
	status = WDTIM_config(hWdt, &hwConfig);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Config for the watchdog Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		debug_printf("WDTIM: Config for the watchdog Passed\n");
	}

	/* Read the configured values */
	status = WDTIM_getConfig(hWdt, &getConfig);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Get Config for the watchdog Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		debug_printf("WDTIM: Get Config for the watchdog Passed\n");
	}

	/* Verify the configurations */
	if((hwConfig.counter  != getConfig.counter) ||
	   (hwConfig.prescale != getConfig.prescale))
	{
		debug_printf("WDTIM: Get and Set Configuration Mis-Matched \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		debug_printf("WDTIM: Get and Set Configuration Matched \n");
	}


	// Start the watch dog timer
	status = WDTIM_start(hWdt);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Start for the watchdog Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		debug_printf("WDTIM: Start for the watchdog Passed\n");
	}

	for (delay = 0; delay < 10; delay++);

	return 0;
}



Int16 wdt_test(void)
{
	// Watchdog timer Object structure

	CSL_Status		 status;

	WDTIM_Config	 hwConfig, getConfig;
	//Uint32           counter;
	//Uint32			 time;
	//Uint16 			 delay;

	Uint32           counter;
	Uint32			 time;
	Uint16 			 delay;

/*
	// Start the watch dog timer
		status = WDTIM_start(hWdt);
		if(CSL_SOK != status)
		{
			debug_printf("WDTIM: Start for the watchdog Failed\n");
			return (CSL_TEST_FAILED);
		}
		else
		{
			debug_printf("WDTIM: Start for the watchdog Passed\n");
		}

		for (delay = 0; delay < 10; delay++);

		// Stop the watch dog timer
		status = WDTIM_stop(hWdt);
		if(CSL_SOK != status)
		{
			debug_printf("WDTIM: Stop for the watchdog Failed\n");
			return (CSL_TEST_FAILED);
		}
		else
		{
			debug_printf("WDTIM: Stop for the watchdog Passed\n");
		}

		// Get the timer count
		status = WDTIM_getCnt(hWdt, &time);
		if(CSL_SOK != status)
		{
			debug_printf("WDTIM: Get Count for the watchdog Failed\n");
			return (CSL_TEST_FAILED);
		}
		else
		{
			debug_printf("WDTIM: Get Count for the watchdog is %ld:\n", time);
		}

		// Start the watch dog timer
		status = WDTIM_start(hWdt);
		if(CSL_SOK != status)
		{
			debug_printf("WDTIM: Start for the watchdog Failed\n");
			return (CSL_TEST_FAILED);
		}
		else
		{
			debug_printf("WDTIM: Start for the watchdog Passed\n");
		}
*/
		counter = 0;
//		for(counter=0; counter<0x105; counter++)
		while(1)
		{
			if(counter < 0x100)
			{
				WDTIM_service(hWdt);
				debug_printf("\nWDT Service - %ld\n",counter);

				// Get the timer count
				status = WDTIM_getCnt(hWdt, &time);
				if(CSL_SOK != status)
				{
					debug_printf("WDTIM: Get Count for the watchdog Failed\n");
					return (CSL_TEST_FAILED);
				}
				else
				{
					debug_printf("Watchdog Count is: %ld\n", time);
				}
			}
			else
			{
				debug_printf("\nWDT Out of Service - %ld\n",counter);

				// Get the timer count
				status = WDTIM_getCnt(hWdt, &time);
				if(CSL_SOK != status)
				{
					debug_printf("WDTIM: Get Count for the watchdog Failed\n");
					return (CSL_TEST_FAILED);
				}
				else
				{
					debug_printf("Watchdog Count is: %ld\n", time);
				}
			}

			counter++;
		}


	return 0;
}

Int16 wdt_Start(void)
{

	CSL_Status		 status;

		WDTIM_Config	 hwConfig, getConfig;
		//Uint32           counter;
		//Uint32			 time;
		//Uint16 			 delay;

		Uint32           counter;
		Uint32			 time;
		Uint16 			 delay;

	// Start the watch dog timer
		status = WDTIM_start(hWdt);
		if(CSL_SOK != status)
		{
			debug_printf("WDTIM: Start for the watchdog Failed\n");
			return (CSL_TEST_FAILED);
		}
		else
		{
			debug_printf("WDTIM: Start for the watchdog Passed\n");
		}

		for (delay = 0; delay < 10; delay++);

		// Stop the watch dog timer
		status = WDTIM_stop(hWdt);
		if(CSL_SOK != status)
		{
			debug_printf("WDTIM: Stop for the watchdog Failed\n");
			return (CSL_TEST_FAILED);
		}
		else
		{
			debug_printf("WDTIM: Stop for the watchdog Passed\n");
		}

		// Get the timer count
		status = WDTIM_getCnt(hWdt, &time);
		if(CSL_SOK != status)
		{
			debug_printf("WDTIM: Get Count for the watchdog Failed\n");
			return (CSL_TEST_FAILED);
		}
		else
		{
			debug_printf("WDTIM: Get Count for the watchdog is %ld:\n", time);
		}

		// Start the watch dog timer
		status = WDTIM_start(hWdt);
		if(CSL_SOK != status)
		{
			debug_printf("WDTIM: Start for the watchdog Failed\n");
			return (CSL_TEST_FAILED);
		}
		else
		{
			debug_printf("WDTIM: Start for the watchdog Passed\n");
		}



	/*
	CSL_Status status;
	Uint16 delay;

	// Start the watch dog timer
	status = WDTIM_start(hWdt);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Start for the watchdog Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		debug_printf("WDTIM: Start for the watchdog Passed\n");
	}

	for (delay = 0; delay < 100; delay++);

	return 0;
	*/
}

Int16 wdt_Stop(void)
{
	CSL_Status status;

	/* Stop the watch dog timer */
	status = WDTIM_stop(hWdt);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Stop for the watchdog Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		debug_printf("WDTIM: Stop for the watchdog Passed\n");
	}

	return 0;
}

Int32 wdt_GetCount(void)
{
	CSL_Status status;
	Uint32 time;

	/* Get the timer count */
	status = WDTIM_getCnt(hWdt, &time);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Get Count for the watchdog Failed\n");
		//return (CSL_TEST_FAILED);
		return -1;
	}
	else
	{
		debug_printf("WDTIM: Get Count for the watchdog is: %ld\n", time);
	}

	return time;
}

void wdt_Refresh(void)
{
	CSL_Status status;

	status = WDTIM_service(hWdt);
	if(CSL_SOK != status)
	{
		debug_printf("WDTIM: Service for the watchdog Failed\n");
		//return (CSL_TEST_FAILED);
		//return -1;
	}
	//debug_printf("WDTIM: service\n");
}
