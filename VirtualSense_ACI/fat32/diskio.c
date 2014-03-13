#include "debug_uart.h" // to redirect printf over UART
#include <csl_general.h>
#include <csl_mmcsd.h>
#include <csl_types.h>
#include <csl_intc.h>
#include <csl_rtc.h>
#include "diskio.h"             /* FatFs lower layer API */

#include <csl_pll.h>
#include "csl_sysctrl.h"


#include "gpio_control.h"

#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)
#define CSL_PLL_CLOCKIN    (32768u)

#define CSL_SD_CLOCK_MAX_KHZ      (20000u)
#define CSL_MMC_CLOCK_MAX_KHZ     (5000u)


static DWORD totalSectors;

Uint16 readed_buffer[256];
//BYTE buffer[512];
Uint16 writer_buffer[256];

//static BYTE Buffer[BUFSIZE];

/* CSL MMCSD Data structures */
CSL_MMCControllerObj    pMmcsdContObj;
CSL_MmcsdHandle                 mmcsdHandle;
CSL_MMCCardObj                  mmcCardObj;
CSL_MMCCardIdObj                cardIdObj;
CSL_MMCCardObj					mmcCardObj;

CSL_MMCCardCsdObj 		cardCsdObj;

/* CSL DMA data structures */
CSL_DMA_Handle        dmaWrHandle;
CSL_DMA_Handle        dmaRdHandle;
CSL_DMA_ChannelObj    dmaWrChanObj;
CSL_DMA_ChannelObj    dmaRdChanObj;
CSL_DMA_Handle        dmaHandle;

/* CSL RTC data structures */
CSL_RtcTime      GetTime;
CSL_RtcDate      GetDate;



CSL_Status configSdCard (CSL_MMCSDOpMode    opMode);
Uint16 computeClkRate(Uint32    memMaxClk);


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/


DSTATUS disk_initialize (BYTE pdrv)
{
        // Put your code here

        CSL_Status    status = CSL_SOK;
        status = configSdCard(CSL_MMCSD_OPMODE_DMA);
        //status = configSdCard(CSL_MMCSD_OPMODE_POLLED);


        if(status != CSL_SOK)
        {
                printf("SD card initialization Failed\n");
                printf("\nMMCSD-ATAFS DMA MODE TEST FAILED!!\n");
        }
        return status;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
        BYTE pdrv               /* Physical drive nmuber (0..) */
)
{
        DSTATUS stat = RES_OK;
        /*int result;

        switch (pdrv) {
        case ATA :
                result = ATA_disk_status();

                // translate the reslut code here

                return stat;

        case MMC :
                result = MMC_disk_status();

                // translate the reslut code here

                return stat;

        case USB :
                result = USB_disk_status();

                // translate the reslut code here

                return stat;
        }
        return STA_NOINIT;*/
        return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
        BYTE pdrv,              /* Physical drive nmuber (0..) */
        BYTE *buff,             /* Data buffer to store read data */
        DWORD sector,   /* Sector address (LBA) */
        BYTE count              /* Number of sectors to read (1..128) */
)
{
        DRESULT res = RES_OK;

        CSL_Status              status;
        Uint32          cardAddr = (Uint32)sector;
    Uint16          noOfBytes = (Uint16)512;
    BYTE buffer[512];

    Uint16 i,j;
        //read the entire block (512) byte
        //printf("Reading addres %d and sector %d\n",cardAddr, count);
        for(j=0; j < count; j++){
                status = MMC_read(mmcsdHandle, cardAddr, noOfBytes, readed_buffer);
                if(status !=  CSL_SOK){
                        res = RES_ERROR;
                        break;
                }
                //printf("status %d\n",status);
            for(i=0; i < 256; i++){
                        buffer[i*2+1] = 0xFF & (readed_buffer[i] >> 8);
                        buffer[i*2] = 0xFF & readed_buffer[i];
                        //printf("i = %d -- 0x%02x%02x\n",i,buffer[i*2+1],buffer[i*2]);
                }
                //printf("\n");
                memcpy(buff, &buffer[0], noOfBytes);
                buff+=noOfBytes;
        }
        // Put your code here

        return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
        BYTE pdrv,                      /* Physical drive nmuber (0..) */
        const BYTE *buff,       /* Data to be written */
        DWORD sector,           /* Sector address (LBA) */
        BYTE count                      /* Number of sectors to write (1..128) */
)
{
        DRESULT res = RES_OK;
        CSL_Status              status;
        Uint32          cardAddr = (Uint32)sector;
        Uint16          noOfBytes = (Uint16)512;
        Uint16                  i = 0;
        Uint16                  j = 0;
        Uint32  				h = 0;
        Uint32					index2 = 0;


        //debug_printf("Writing %d bytes at address %d\n", noOfBytes, cardAddr);
        for(j=0; j < count; j++){
        		//debug_printf("writing sector %d starting from add %d\n",count, cardAddr);
                for(i=0; i < 256; i++){
                        writer_buffer[i] = ((buff[j*512+i*2+1] << 8)|(buff[j*512+i*2]));
                        //debug_printf("%x %x\n", buff[j*512+i*2+1], buff[j*512+i*2]);
                }
                /*for(h=0;h<100000;h++){ */
                	dbgGpio2Write(1);
                	status = MMC_write(mmcsdHandle, cardAddr, noOfBytes, writer_buffer);
                	dbgGpio2Write(0);
                	if(status !=  CSL_SOK)
                		res = RES_ERROR;
                	/*cardAddr++;
                	for(index2 =0; index2<100000;index2++){
                		asm(" nop "); // is a wait loop
                	}*/
                /*}*/
                //printf("status = %d\n",status);
                cardAddr++;
        }

        return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
        BYTE pdrv,              /* Physical drive nmuber (0..) */
        BYTE cmd,               /* Control code */
        void *buff              /* Buffer to send/receive control data */
)
{
        DRESULT res;

        if (disk_status(pdrv) & STA_NOINIT) return RES_NOTRDY;  /* Check if card is in the socket */

        res = RES_ERROR;
        switch (cmd) {
                case CTRL_SYNC :                /* Make sure that no pending write process */
                        res = RES_OK;
                        break;

                case GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
                        *(DWORD*)buff = totalSectors;
                        //printf("request sectors number %ld\n",totalSectors);
                        res = RES_OK;
                        break;

                case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
                        *(DWORD*)buff = 512;
                        res = RES_OK;
                        break;

                default:
                        res = RES_PARERR;
        }

        return res;
}
#endif



/**
 *  \brief  Function to initialize and configure SD card
 *
 *  \param  opMode   - Operating Mode of MMCSD; POLL/DMA
 *
 *  \return Test result
 */
CSL_Status configSdCard (CSL_MMCSDOpMode    opModes)
{
	Uint32             sectCount;
		Uint32             cardAddr;
		Uint32             index;
		Uint32             index2;
		CSL_Status	       mmcStatus;
		CSL_Status	       status;
		Uint16		       count;
		Uint16		       actCard;
		Uint16             rca;
		Uint16             clockDiv;
		CSL_CardType       cardType;
		CSL_MMCSDOpMode    opMode;
		Uint32 gpioIoDir;

		cardType  = CSL_CARD_NONE;
		sectCount = 0;
		opMode    = CSL_MMCSD_OPMODE_DMA;

		/* Initialize MMCSD module */

		dbgGpio1Write(0); //RED

	    dbgGpio2Write(0); // WHITE


	    if(CSL_SOK != mmcStatus)
	    {
	        printf("SYS_setEBSR failed\n");
	        return (mmcStatus);
	    }

		/* Initialize Dma */
	    mmcStatus = DMA_init();
	    if (mmcStatus != CSL_SOK)
	    {
	        printf("API: DMA_init Failed!\n");
	        return(mmcStatus);
	    }

		/* Open Dma channel for MMCSD write */
		dmaWrHandle = DMA_open(CSL_DMA_CHAN0, &dmaWrChanObj, &mmcStatus);
	    if((dmaWrHandle == NULL) || (mmcStatus != CSL_SOK))
	    {
	        printf("API: DMA_open for MMCSD Write Failed!\n");
	        return(mmcStatus);
	    }

		/* Open Dma channel for MMCSD read */
		dmaRdHandle = DMA_open(CSL_DMA_CHAN1, &dmaRdChanObj, &mmcStatus);
	    if((dmaRdHandle == NULL) || (mmcStatus != CSL_SOK))
	    {
	        printf("API: DMA_open for MMCSD Read Failed!\n");
	        return(mmcStatus);
	    }

		/* Open the MMCSD module */
	#ifdef C5515_EZDSP
		mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD1_INST,	opMode, &mmcStatus);
	#else
		mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST,	opMode, &mmcStatus);
	#endif
		if((mmcStatus != CSL_SOK) || (mmcsdHandle == NULL))
		{
			printf("API: MMC_open Failed\n");
	        return(mmcStatus);
		}

		/* Set the DMA handles */
		if(opMode == CSL_MMCSD_OPMODE_DMA)
		{
			/* Set the DMA handle for MMC read */
			mmcStatus = MMC_setDmaHandle(mmcsdHandle, dmaWrHandle, dmaRdHandle);
			if(mmcStatus != CSL_SOK)
			{
				printf("API: MMC_setDmaHandle for MMCSD Failed\n");
		        return(mmcStatus);
			}
		}

		/* Send CMD0 to the card */
		mmcStatus = MMC_sendGoIdle(mmcsdHandle);
		if(mmcStatus != CSL_SOK)
		{
			printf("API: MMC_sendGoIdle Failed\n");
			return(mmcStatus);
		}

		/* Check for the card */
	    mmcStatus = MMC_selectCard(mmcsdHandle, &mmcCardObj);
		if((mmcStatus == CSL_ESYS_BADHANDLE) ||
		   (mmcStatus == CSL_ESYS_INVPARAMS))
		{
			printf("API: MMC_selectCard Failed\n");
			return(mmcStatus);
		}

		/* Verify whether valid memory card is detected or not */
		if(mmcCardObj.cardType == CSL_MMC_CARD)
		{
			printf("MMC Card Detected!\n\n");
			cardType = CSL_MMC_CARD;
			cardAddr = (sectCount)*(CSL_MMCSD_BLOCK_LENGTH);

			/* Send the MMC card identification Data */
			mmcStatus = MMC_sendAllCID(mmcsdHandle, &cardIdObj);
			if(mmcStatus != CSL_SOK)
			{
				printf("API: MMC_sendAllCID Failed\n");
				return(mmcStatus);
			}

			/* Set the MMC Relative Card Address */
			mmcStatus = MMC_setRca(mmcsdHandle, &mmcCardObj, 0x0001);
			if(mmcStatus != CSL_SOK)
			{
				printf("API: MMC_setRca Failed\n");
				return(mmcStatus);
			}

			/* Read the MMC Card Specific Data */
			mmcStatus = MMC_getCardCsd(mmcsdHandle, &cardCsdObj);
			if(mmcStatus != CSL_SOK)
			{
				printf("API: MMC_getCardCsd Failed\n");
				return(mmcStatus);
			}

			/* Get the clock divider value for the current CPU frequency */
			clockDiv = computeClkRate(CSL_MMC_CLOCK_MAX_KHZ);
		}
		else if(mmcCardObj.cardType == CSL_SD_CARD)
		{
			printf("SD Card Detected!\n");
			cardType = CSL_SD_CARD;

			/* Check if the card is high capacity card */
			if(mmcsdHandle->cardObj->sdHcDetected == TRUE)
			{
				printf("SD card is High Capacity Card\n");
				printf("Memory Access Uses Block Addressing\n\n");

				/* For the SDHC card Block addressing will be used.
				   Sector address will be same as sector number */
				cardAddr = sectCount;
			}
			else
			{
				printf("SD card is Standard Capacity Card\n");
				printf("Memory Access Uses Byte Addressing\n\n");

				/* For the SD card Byte addressing will be used.
				   Sector address will be product of  sector number and sector size */
				cardAddr = (sectCount)*(CSL_MMCSD_BLOCK_LENGTH);
			}

			/* Set the init clock */
		    mmcStatus = MMC_sendOpCond(mmcsdHandle, 70);
			if(mmcStatus != CSL_SOK)
			{
				printf("API: MMC_sendOpCond Failed\n");
				return(mmcStatus);
			}

			/* Send the SD card identification Data */
			mmcStatus = SD_sendAllCID(mmcsdHandle, &cardIdObj);
			if(mmcStatus != CSL_SOK)
			{
				printf("API: SD_sendAllCID Failed\n");
				return(mmcStatus);
			}

			/* Set the SD Relative Card Address */
			mmcStatus = SD_sendRca(mmcsdHandle, &mmcCardObj, &rca);
			if(mmcStatus != CSL_SOK)
			{
				printf("API: SD_sendRca Failed\n");
				return(mmcStatus);
			}

			/* Read the SD Card Specific Data */
			mmcStatus = SD_getCardCsd(mmcsdHandle, &cardCsdObj);
			if(mmcStatus != CSL_SOK)
			{
				printf("API: SD_getCardCsd Failed\n");
				return(mmcStatus);
			}

			/* Get the clock divider value for the current CPU frequency */
			clockDiv = computeClkRate(CSL_SD_CLOCK_MAX_KHZ);
		}
		else
		{
			printf("NO Card Detected!\n");
			printf("Insert MMC/SD Card!\n");
			return(CSL_ESYS_FAIL);
		}

		/* Set the card type in internal data structures */
		mmcStatus = MMC_setCardType(&mmcCardObj, cardType);
		if(mmcStatus != CSL_SOK)
		{
			printf("API: MMC_setCardType Failed\n");
			return(mmcStatus);
		}

		/* Set the card pointer in internal data structures */
		mmcStatus = MMC_setCardPtr(mmcsdHandle, &mmcCardObj);
		if(mmcStatus != CSL_SOK)
		{
			printf("API: MMC_setCardPtr Failed\n");
			return(mmcStatus);
		}

		/* Get the number of cards */
		mmcStatus = MMC_getNumberOfCards(mmcsdHandle, &actCard);
		if(mmcStatus != CSL_SOK)
		{
			printf("API: MMC_getNumberOfCards Failed\n");
			return(mmcStatus);
		}

		/* Set clock for read-write access */
		mmcStatus = MMC_sendOpCond(mmcsdHandle, clockDiv);
		if(mmcStatus != CSL_SOK)
		{
			printf("API: MMC_sendOpCond Failed\n");
			return(mmcStatus);
		}

		/* Set Endian mode for read and write operations */
	  	mmcStatus = MMC_setEndianMode(mmcsdHandle, CSL_MMCSD_ENDIAN_LITTLE,
	  	                              CSL_MMCSD_ENDIAN_LITTLE);
		if(mmcStatus != CSL_SOK)
		{
			printf("API: MMC_setEndianMode Failed\n");
			return(mmcStatus);
		}

		/* Set block length for the memory card
		 * For high capacity cards setting the block length will have
		 * no effect
		 */
		mmcStatus = MMC_setBlockLength(mmcsdHandle, CSL_MMCSD_BLOCK_LENGTH);
		if(mmcStatus != CSL_SOK)
		{
			printf("API: MMC_setBlockLength Failed\n");
			return(mmcStatus);
		}
		return(CSL_SOK);
}

/**
 *  \brief    Function to calculate the memory clock rate
 *
 * This function computes the memory clock rate value depending on the
 * CPU frequency. This value is used as clock divider value for
 * calling the API MMC_sendOpCond(). Value of the clock rate computed
 * by this function will change depending on the system clock value
 * and MMC maximum clock value passed as parameter to this function.
 * Minimum clock rate value returned by this function is 0 and
 * maximum clock rate value returned by this function is 255.
 * Clock derived using the clock rate returned by this API will be
 * the nearest value to 'memMaxClk'.
 *
 *  \param    memMaxClk  - Maximum memory clock rate
 *
 *  \return   MMC clock rate value
 */
Uint16 computeClkRate(Uint32    memMaxClk)
{
	Uint32    sysClock;
	Uint32    remainder;
	Uint16    clkRate;

	sysClock  = 0;
	remainder = 0;
	clkRate   = 0;

	/* Get the clock value at which CPU is running */
	sysClock = getSysClk();

	if (sysClock > memMaxClk)
	{
		if (memMaxClk != 0)
		{
			clkRate   = sysClock / memMaxClk;
			remainder = sysClock % memMaxClk;

            /*
             * If the remainder is not equal to 0, increment clock rate to make
             * sure that memory clock value is less than the value of
             * 'memMaxClk'.
             */
			if (remainder != 0)
			{
				clkRate++;
			}

            /*
             * memory clock divider '(2 * (CLKRT + 1)' will always
             * be an even number. Increment the clock rate in case of
             * clock rate is not an even number.
             */
			if (clkRate%2 != 0)
			{
				clkRate++;
			}

			/*
			 * AT this point 'clkRate' holds the value of (2 * (CLKRT + 1).
			 * Get the value of CLKRT.
			 */
			clkRate = clkRate/2;
			clkRate = clkRate - 1;

			/*
			 * If the clock rate is more than the maximum allowed clock rate
			 * set the value of clock rate to maximum value.
			 * This case will become true only when the value of
			 * 'memMaxClk' is less than the minimum possible
			 * memory clock that can be generated at a particular CPU clock.
			 *
			 */
			if (clkRate > CSL_MMC_MAX_CLOCK_RATE)
			{
				clkRate = CSL_MMC_MAX_CLOCK_RATE;
			}
		}
		else
		{
			clkRate = CSL_MMC_MAX_CLOCK_RATE;
		}
	}

	return 1;//LELE to speedup MMC(clkRate);
}




DWORD get_fattime ()
{
        CSL_Status    status;
        status = RTC_getTime(&GetTime);
        status |= RTC_getDate(&GetDate);
        if(!status){
                    //printf("Time and Date is : %02d:%02d:%02d:%04d, %02d-%02d-%02d\n",
                        //GetTime.hours,GetTime.mins,GetTime.secs,GetTime.mSecs,GetDate.day,GetDate.month,GetDate.year);
                        return    ((DWORD)(2000 + GetDate.year - 1980) << 25)   /* Year = 2012 */
                                                | ((DWORD)GetDate.month << 21)                          /* Month = 1 */
                                                | ((DWORD)GetDate.day << 16)                            /* Day_m = 1*/
                                                | ((DWORD)(GetTime.hours) << 11)                        /* Hour = 0 */
                                                | ((DWORD)GetTime.mins << 5)                            /* Min = 0 */
                                                | ((DWORD)GetTime.secs >> 1);                           /* Sec = 0 */
        }else {

                return    ((DWORD)(2012 - 1980) << 25)  /* Year = 2012 */
                                | ((DWORD)1 << 21)                              /* Month = 1 */
                                | ((DWORD)1 << 16)                              /* Day_m = 1*/
                                | ((DWORD)0 << 11)                              /* Hour = 0 */
                                | ((DWORD)0 << 5)                               /* Min = 0 */
                                | ((DWORD)0 >> 1);                              /* Sec = 0 */
        }
}
