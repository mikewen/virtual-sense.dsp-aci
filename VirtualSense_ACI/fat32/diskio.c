#include <stdio.h>
#include <csl_general.h>
#include <csl_mmcsd.h>
#include <csl_types.h>
#include <csl_intc.h>
#include <csl_rtc.h>
#include "diskio.h"             /* FatFs lower layer API */

//#include <csl_mmcsd.h>
//#include "chk_mmc.h"
//#include "csl_sysctrl.h"
//#include "sdcard.h"           /* Example: MMC/SDC contorl */

#define CSL_SD_CLOCK_MAX_KHZ      (25000u)


#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)
#define CSL_PLL_CLOCKIN    (32768u)



static DWORD totalSectors;

Uint16 readed_buffer[256];
BYTE buffer[512];
Uint16 writer_buffer[256];

//static BYTE Buffer[BUFSIZE];

/* CSL MMCSD Data structures */
CSL_MMCControllerObj    pMmcsdContObj;
CSL_MmcsdHandle                 mmcsdHandle;
CSL_MMCCardObj                  mmcCardObj;
CSL_MMCCardIdObj                sdCardIdObj;
CSL_MMCCardCsdObj               sdCardCsdObj;

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
Uint16 computeClkRate(void);
Uint32 getSysClk(void);


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


    
        //printf("Writing %d bytes at address %d\n",noOfBytes,cardAddr);
        for(j=0; j < count; j++){
                //printf("writing sector %d starting from add %d\n",count,cardAddr);
                 for(i=0; i < 256; i++){
                        writer_buffer[i] = ((buff[j*512+i*2+1] << 8)|(buff[j*512+i*2]));
                        //printf("i = %d -- 0x%04x\n",i,writer_buffer[i]);

                }
                status = MMC_write(mmcsdHandle, cardAddr, noOfBytes, writer_buffer);
                if(status !=  CSL_SOK)
                        res = RES_ERROR;
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
CSL_Status configSdCard (CSL_MMCSDOpMode    opMode)
{
        CSL_Status     status;
        CSL_MMCConfig  mm_config;
        Uint16             actCard;
        Uint16         clockDiv;
        Uint16         rca;

        /* Get the clock divider value for the current CPU frequency */
        clockDiv = computeClkRate();
        clockDiv = 1;//computeClkRate(); //LELE: setting MMCSD bus speed to 25MHz

        /* Initialize MMCSD CSL module */
        status = MMC_init();



        /* Open MMCSD CSL module */
#ifdef C5515_EZDSP
        mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD1_INST,
                                                   opMode, &status);
#else
        mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST,
                                                   opMode, &status);
#endif
        if(mmcsdHandle == NULL)
        {
                printf("MMC_open Failed\n");
                return (status);
        }


        /* Configure the DMA in case of operating mode is set to DMA */
        if(opMode == CSL_MMCSD_OPMODE_DMA)
        {
                /* Initialize Dma */
                /*status = DMA_init();
                if (status != CSL_SOK)
                {
                        printf("DMA_init Failed!\n");
                        return(status);
                }*/

                /* Open Dma channel for MMCSD write */
                dmaWrHandle = DMA_open(CSL_DMA_CHAN0, &dmaWrChanObj, &status);
                if((dmaWrHandle == NULL) || (status != CSL_SOK))
                {
                        printf("DMA_open for MMCSD Write Failed!\n");
                        return(status);
                }

                /* Open Dma channel for MMCSD read */
                dmaRdHandle = DMA_open(CSL_DMA_CHAN1, &dmaRdChanObj, &status);
                if((dmaRdHandle == NULL) || (status != CSL_SOK))
                {
                        printf("DMA_open for MMCSD Read Failed!\n");
                        return(status);
                }

                /* Set the DMA handle for MMC read */
                status = MMC_setDmaHandle(mmcsdHandle, dmaWrHandle, dmaRdHandle);
                if(status != CSL_SOK)
                {
                        printf("API: MMC_setDmaHandle for MMCSD Failed\n");
                        return(status);
                }
        }

        /* Reset the SD card */
        status = MMC_sendGoIdle(mmcsdHandle);
        if(status != CSL_SOK)
        {
                printf("MMC_sendGoIdle Failed\n");
                return (status);
        }

        /* Check for the card */
    status = MMC_selectCard(mmcsdHandle, &mmcCardObj);
        if((status == CSL_ESYS_BADHANDLE) ||
           (status == CSL_ESYS_INVPARAMS))
        {
                printf("MMC_selectCard Failed\n");
                return (status);
        }

        /* Verify whether the SD card is detected or not */
        if(mmcCardObj.cardType == CSL_SD_CARD)
        {
                printf("SD Card detected\n");

                /* Check if the card is high capacity card */
                if(mmcsdHandle->cardObj->sdHcDetected == TRUE)
                {
                        printf("SD card is High Capacity Card\n");
                        printf("Memory Access will use Block Addressing\n");
                }
                else
                {
                        printf("SD card is Standard Capacity Card\n");
                        printf("Memory Access will use Byte Addressing\n");
                }
        }
        else
        {
                if(mmcCardObj.cardType == CSL_CARD_NONE)
                {
                        printf("No Card detected\n");
                }
                else
                {
                        printf("SD Card not detected\n");
                }
                printf("Please Insert SD Card\n");
                return(CSL_ESYS_FAIL);
        }

        /* Set the init clock */
    status = MMC_sendOpCond(mmcsdHandle, 70);
        if(status != CSL_SOK)
        {
                printf("MMC_sendOpCond Failed\n");
                return (status);
        }

        /* Send the card identification Data */
        status = SD_sendAllCID(mmcsdHandle, &sdCardIdObj);
        if(status != CSL_SOK)
        {
                printf("SD_sendAllCID Failed\n");
                return (status);
        }

        /* Set the Relative Card Address */
        status = SD_sendRca(mmcsdHandle, &mmcCardObj, &rca);
        if(status != CSL_SOK)
        {
                printf("SD_sendRca Failed\n");
                return (status);
        }

        /* Read the SD Card Specific Data */
        status = SD_getCardCsd(mmcsdHandle, &sdCardCsdObj);
        if(status != CSL_SOK)
        {
                printf("SD_getCardCsd Failed\n");
                return (status);
        }

        /* Set the card type in internal data structures */
        status = MMC_setCardType(&mmcCardObj, CSL_SD_CARD);
        if(status != CSL_SOK)
        {
                printf("MMC_setCardType Failed\n");
                return (status);
        }

        /* Set the card pointer in internal data structures */
        status = MMC_setCardPtr(mmcsdHandle, &mmcCardObj);
        if(status != CSL_SOK)
        {
                printf("MMC_setCardPtr Failed\n");
                return (status);
        }

        /* Get the number of cards */
        status = MMC_getNumberOfCards(mmcsdHandle, &actCard);
        if(status != CSL_SOK)
        {
                printf("MMC_getNumberOfCards Failed\n");
                return (status);
        }


        /* Set bus width - Optional */
        status = SD_setBusWidth(mmcsdHandle, 1);
        if(status != CSL_SOK)
        {
                printf("API: SD_setBusWidth Failed\n");
                return(status);
        }

        /* Disable SD card pull-up resistors - Optional */
        status = SD_configurePullup(mmcsdHandle, 0);
        if(status != CSL_SOK)
        {
                printf("API: SD_configurePullup Failed\n");
                return(status);
        }



        /* Set clock for read-write access */
    status = MMC_sendOpCond(mmcsdHandle, clockDiv);
        if(status != CSL_SOK)
        {
                printf("MMC_sendOpCond Failed\n");
                return (status);
        }

        /* Set Endian mode for read and write operations */
        status = MMC_setEndianMode(mmcsdHandle, CSL_MMCSD_ENDIAN_LITTLE,
                                   CSL_MMCSD_ENDIAN_LITTLE);
        if(status != CSL_SOK)
        {
                printf("MMC_setEndianMode Failed\n");
                return(status);
        }

        /* Set block length for the memory card
         * For high capacity cards setting the block length will have
         * no effect
         */
        status = MMC_setBlockLength(mmcsdHandle, CSL_MMCSD_BLOCK_LENGTH);
        if(status != CSL_SOK)
        {
                printf("MMC_setBlockLength Failed\n");
                return(status);
        }

        // LELE to enable 4 bit bus
        /*      status = MMC_getConfig(mmcsdHandle, &mm_config);
                if(status != CSL_SOK){
                        printf("get config  Failed!\n");
                        return(status);
                }

                mm_config.mmcctl |= 0x4;
                status = MMC_config(mmcsdHandle, &mm_config);
                if(status != CSL_SOK){
                        printf("set  config  Failed!\n");
                        return(status);
                }
                // LELE end to enable 4 bit bus

        */
        return (status);
}

/**
 *  \brief    Function to calculate the memory clock rate
 *
 * This function computes the memory clock rate value depending on the
 * CPU frequency. This value is used as clock divider value for
 * calling the API MMC_sendOpCond(). Value of the clock rate computed
 * by this function will change depending on the system clock value
 * and MMC maximum clock value defined by macro 'CSL_SD_CLOCK_MAX_KHZ'.
 * Minimum clock rate value returned by this function is 0 and
 * maximum clock rate value returned by this function is 255.
 * Clock derived using the clock rate returned by this API will be
 * the nearest value to 'CSL_SD_CLOCK_MAX_KHZ'.
 *
 *  \param    none
 *
 *  \return   MMC clock rate value
 */
Uint16 computeClkRate(void)
{
        Uint32    sysClock;
        Uint32    remainder;
        Uint32    memMaxClk;
        Uint16    clkRate;

        sysClock  = 0;
        remainder = 0;
        memMaxClk = CSL_SD_CLOCK_MAX_KHZ;
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
             * 'CSL_SD_CLOCK_MAX_KHZ'.
             */
                        if (remainder != 0)
                        {
                                clkRate++;
                        }

            /*
             * memory clock divider '(2 * (CLKRT + 1)' will always
             * be an even number. Increment the clock rate in case of
             * clock rate is not an even number
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
                         * 'CSL_SD_CLOCK_MAX_KHZ' is less than the minimum possible
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

        return (clkRate);
}

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    none
 *
 *  \return   System clock value in KHz
 */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514))

Uint32 getSysClk(void)
{
        Bool      pllRDBypass;
        Bool      pllOutDiv;
        Uint32    sysClk;
        Uint16    pllVP;
        Uint16    pllVS;
        Uint16    pllRD;
        Uint16    pllVO;

        pllVP = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_VP);
        pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_VS);

        pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDRATIO);
        pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OD);

        pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDBYPASS);
        pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIVEN);

        sysClk = CSL_PLL_CLOCKIN;

        if (0 == pllRDBypass)
        {
                sysClk = sysClk/(pllRD + 4);
        }

        sysClk = (sysClk * ((pllVP << 2) + pllVS + 4));

        if (1 == pllOutDiv)
        {
                sysClk = sysClk/(pllVO + 1);
        }

        /* Return the value of system clock in KHz */
        return(sysClk/1000);
}

#else

Uint32 getSysClk(void)
{
        Bool      pllRDBypass;
        Bool      pllOutDiv;
        Bool      pllOutDiv2;
        Uint32    sysClk;
        Uint16    pllVP;
        Uint16    pllVS;
        Uint16    pllRD;
        Uint16    pllVO;
        Uint16    pllDivider;
        Uint32    pllMultiplier;

        pllVP = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_MH);
        pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_ML);

        pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDRATIO);
        pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_ODRATIO);

        pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDBYPASS);
        pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIVEN);
        pllOutDiv2  = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIV2BYPASS);

        pllDivider = ((pllOutDiv2) | (pllOutDiv << 1) | (pllRDBypass << 2));

        pllMultiplier = ((Uint32)CSL_PLL_CLOCKIN * ((pllVP << 2) + pllVS + 4));

        switch(pllDivider)
        {
                case CSL_PLL_DIV_000:
                case CSL_PLL_DIV_001:
                        sysClk = pllMultiplier / (pllRD + 4);
                break;

                case CSL_PLL_DIV_002:
                        sysClk = pllMultiplier / ((pllRD + 4) * (pllVO + 4) * 2);
                break;

                case CSL_PLL_DIV_003:
                        sysClk = pllMultiplier / ((pllRD + 4) * 2);
                break;

                case CSL_PLL_DIV_004:
                case CSL_PLL_DIV_005:
                        sysClk = pllMultiplier;
                break;

                case CSL_PLL_DIV_006:
                        sysClk = pllMultiplier / ((pllVO + 4) * 2);
                break;

                case CSL_PLL_DIV_007:
                        sysClk = pllMultiplier / 2;
                break;
        }

        /* Return the value of system clock in KHz */
        return(sysClk/1000);
}

#endif


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
                                                | ((DWORD)(GetTime.hours + 1) << 11)                            /* Hour = 0 */
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
