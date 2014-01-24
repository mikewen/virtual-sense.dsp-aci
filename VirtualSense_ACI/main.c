/*
 * $$$MODULE_NAME csl_usb_iso_fullspeed_example.c
 *
 * $$$MODULE_DESC csl_usb_iso_fullspeed_example.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** @file csl_usb_iso_fullspeed_example.c
 *
 *  @brief USB Audio Class functional layer full speed mode example source file
 *
 *  This example tests the operation of VC5505 usb in full speed mode.
 *  NOTE: For Testing Audio class module a macro CSL_AC_TEST needs to be defined
 *  This includes some code in csl_usbAux.h file which is essential for Audio class
 *  operation and not required for MUSB stand alone testing.
 *  define this macro in pre defined symbols in project biuld options
 *  (Defined in the current usb audio class example pjt).
 *  Semaphores and mail boxes are used in the Audio class example code as the USB operation
 *  is not possible with out OS calls. DSP BIOS version 5.32.03 is used for this purpose.
 *  Definig Start transfer and complete transfer call back functions is must
 *  and Audio class module does not work if they are not implemeted properly. A call back
 *  is sent to this functions from MUSB module.
 *
 *  NOTE: Message boxes and semaphores are reused from MSC module.
 *  Name MSC is not replaced with Auidio class at some places for quick reusability
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505 AND
 * C5515. MAKE SURE THAT PROPER CHIP VERSION MACRO CHIP_5505/CHIP_5515 IS
 * DEFINED IN THE FILE c55xx_csl\inc\csl_general.h.
 *
 *  Path: \(CSLPATH)\example\usb\example5
 */

/* ============================================================================
 * Revision History
 * ================
 * 20-Dec-2008 Created
 * ============================================================================
 */

#include <stdlib.h>
#include "csl_types.h"
#include "csl_error.h"
#include "csl_intc.h"
#include "csl_gpio.h"
#include "csl_usb.h"
#include "csl_audioClass.h"
#include "soc.h"
//#include "psp_i2s.h"
#include "dda_dma.h"
#include "i2s_sample.h"
#include "gpio_control.h"
#include "pll_control.h"
#include "app_globals.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_usbac_descs.h"
#include "codec_aic3254.h"
#include "user_interface.h"
#include "app_asrc.h"
#include "sample_rate.h"

#ifdef C5535_EZDSP_DEMO
#include "lcd_osd.h"
#include "dsplib.h"
#include "soc.h"
#include "cslr.h"
#include "cslr_sysctrl.h"

#include "main_config.h"
#include "circular_buffer.h"

#include "ff.h"
#include "make_wav.h"

#undef ENABLE_REC_ASRC
#undef ENABLE_ASRC

CSL_Status  CSL_i2cPowerTest(void);
void calculate_FFT(unsigned char *input, int size);
// Demo switch flag: 0 - power display, 1 - spectrum analyzer
Uint16 DemoSwitchFlag = 1;

// buffer for perform FFT
//#pragma DATA_ALIGN(bufferFFT, 4)
//DATA bufferFFT[FFT_LENGHT];
// scarch buffer for FFT
//DATA bufferScrach[FFT_LENGHT];
// display buffer for spectrum display
int display_buffer[128];

extern Uint16 my_i2sRxLeftBuf[2*DMA_BUFFER_SZ]; /* 2x for ping/pong */
extern Uint16 my_i2sRxRightBuf[2*DMA_BUFFER_SZ]; /* 2x for ping/pong */
extern Int16 my_i2sTxLeftBuf[2*DMA_BUFFER_SZ]; /* 2x for ping/pong */
extern Int16 my_i2sTxRightBuf[2*DMA_BUFFER_SZ]; /* 2x for ping/pong */

#endif



#include "VirtualSense_ACIcfg.h"

 /* Debug: enable run-time storage of data to SDRAM */
//#define STORE_PARAMETERS_TO_SDRAM

// Clock gating for unused peripherals
void ClockGating(void);

/* Initializes application */
void CSL_acTest(void);

/* Resets C5515 */
void C5515_reset(void);

extern void initRTC(void);
/**
 *  \brief  CSL Audio Class main function
 *
 *  \param  None
 *
 *  \return None
 */
void main(void)
{
    CSL_Status status;
    Uint32 gpioIoDir;
    Uint32 j = 0;

    // Disable all tracing
    //TRC_disable(TRC_GBLTARG);
    // Disable trace log
    //LOG_disable(&trace);

    /* Clock gate all peripherals */
    CSL_SYSCTRL_REGS->PCGCR1 = 0x7FFF;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x007F;

    /* SP0 Mode 2 (GP[5:0]) -- GPIO02/GPIO04 for debug  */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE2);


    /* SP1 Mode 2 (GP[11:6]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE2); /* need GPIO10 for AIC3204 reset */

    /* PP Mode 1 (SPI, GPIO[17:12], UART, and I2S2) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

    /* Reset C5515 -- ungates all peripherals */
    C5515_reset();

    /* Initialize DSP PLL */
    status = pll_sample();
    if (status != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize PLL");
#endif
        exit(EXIT_FAILURE);
    }

    /* Clear pending timer interrupts */
    CSL_SYSCTRL_REGS->TIAFR = 0x7;

    /* Initialize GPIO module */

    /* GPIO02 and GPIO04 for debug */
    /* GPIO10 for AIC3204 reset */
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN2) | 
        (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN4) |
        (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10);

    status = gpioInit(gpioIoDir, 0x00000000, 0x00000000);
    if (status != GPIOCTRL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize GPIO");
#endif
        exit(EXIT_FAILURE);
    }

     /* Enable the USB LDO */
    //*(volatile ioport unsigned int *)(0x7004) |= 0x0001;
}

/**
 *  \brief  Audio Class intialization function
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_acTest(void)
{
    I2sInitPrms i2sInitPrms;
    CSL_UsbConfig usbConfig;
    PSP_Result result;
    Int16 status;

#ifdef DEBUG_LOG_PRINT
    LOG_printf(&trace, "USB ISO FULL SPEED MODE");
#endif

    /* Initialize audio module */
    result = set_sampling_frequency_gain_impedence(FREQUENCY, GAIN, IMPEDANCE);
    Set_Mute_State(TRUE);
    if (result != 0)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to configure audio codec");
#endif
        exit(EXIT_FAILURE);
    }
    else
    {
#ifdef C5535_EZDSP_DEMO
        /* Initialize the OLED display */
        oled_init();
#endif
       initRTC();
        
        i2sTxBuffSz = 2*DMA_BUFFER_SZ;
        /* Reset codec output buffer */
        reset_codec_output_buffer();

        /* Initialize DMA hardware and driver */
        DMA_init(); // To enable MMCSD DMA
        DMA_HwInit();
        DMA_DrvInit();

        /* Initialize I2S and DMA channels for Playback and Record */
        /* playback */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
        i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)my_i2sTxLeftBuf;  /* note: only Tx Left used for stereo, sample-by-sample Pb */
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)my_i2sTxRightBuf;
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID;
        /* record */
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = FALSE;
        i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)my_i2sRxLeftBuf;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID;
        status = i2sInit(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
#ifdef DEBUG_LOG_PRINT
            LOG_printf(&trace, "ERROR: Unable to initialize I2S");
#endif
            exit(EXIT_FAILURE);
        }


        /* Start left Rx DMA */
        DMA_StartTransfer(hDmaRxLeft);

        /* Set HWAI ICR */
        *(volatile ioport Uint16 *)0x0001 = 0xFC0E | (1<<9);
        asm("   idle");

        /* Clock gate usused peripherals */

        ClockGating();

        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx, TRUE); /* enable I2S transmit and receive */

    }

#ifdef DEBUG_LOG_PRINT
    LOG_printf(&trace, "Initialization complete");
#endif
}

/* Resets C5515 */
void C5515_reset(void)
{
    volatile int i;

    // disable all interrupts (IER0 and IER1)
    *(volatile ioport unsigned int *)(0x0000) = 0x0000;
    *(volatile ioport unsigned int *)(0x0045) = 0x0000;

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;

    // enable all peripherials
    *(volatile ioport unsigned int *)(0x1c02) = 0;
    *(volatile ioport unsigned int *)(0x1c03) = 0;

    // reset peripherals
    *(volatile ioport unsigned int *)(0x1c04) = 0x0020;
    *(volatile ioport unsigned int *)(0x1c05) = 0x00BF;
    // some delay
    for (i=0; i<0xFFF; i++);

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;
}

// Clock gating for unused peripherals
void ClockGating(void)
{
    Uint16 pcgcr_value, clkstop_value;
    
    // set PCGCR1
    pcgcr_value = 0; 
    // clock gating SPI
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_SPICG, DISABLED);
    // clock gating SD/MMC
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD0CG, DISABLED);
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD1CG, DISABLED);
    // clock stop request for UART
    clkstop_value = CSL_FMKT(SYS_CLKSTOP_URTCLKSTPREQ, REQ);
    // write to CLKSTOP
    CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
    // wait for acknowledge
    while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_URTCLKSTPACK)==0);
    // clock gating UART
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_UARTCG, DISABLED);
    // clock stop request for EMIF
    //clkstop_value = CSL_FMKT(SYS_CLKSTOP_EMFCLKSTPREQ, REQ);
    // write to CLKSTOP
    //CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
    // wait for acknowledge
    //while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_EMFCLKSTPACK)==0);
    // clock gating EMIF
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR1_EMIFCG, DISABLED);
    // clock gating unused I2S (I2S 0, 1, 3)
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S0CG, DISABLED);
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S1CG, DISABLED);
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S2CG, DISABLED);
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S3CG, DISABLED);
    // clock gating DMA0
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR1_DMA0CG, DISABLED);
    // clock gating Timer 1
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR1CG, DISABLED);
    // clock gating Timer 2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR2CG, DISABLED);
    // write to PCGCR1
    CSL_FSET(CSL_SYSCTRL_REGS->PCGCR1, 15, 0, pcgcr_value);
    
    // set PCGCR2
    pcgcr_value = 0; 
    // clock gating LCD
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_LCDCG, DISABLED);
    // clock gating SAR
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR2_SARCG, DISABLED);
    // clock gating DMA1
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA1CG, DISABLED);
    // clock gating DMA2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA2CG, DISABLED);
    // clock gating DMA3
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA3CG, DISABLED);
    // clock analog registers
    //pcgcr_value |= CSL_FMKT(SYS_PCGCR2_ANAREGCG, DISABLED);
    // write to PCGCR2
    CSL_FSET(CSL_SYSCTRL_REGS->PCGCR2, 15, 0, pcgcr_value);
    
    // disable the CLKOUT. It is on reset
    // set bit 2 of ST3_55 to 1
    //LELE 18-09-2013
   /* asm("    bit(ST3, #ST3_CLKOFF) = #1");
    
    // turn off the XF
    // set bit 13 of ST1_55 to 0
    asm("    bit(ST1, #ST1_XF) = #0"); */

#ifdef C5535_EZDSP
    // turn off the DS3-6
    // set the GPIO pin 14 - 15 to output, set SYS_GPIO_DIR0 (0x1C06) bit 14 and 15 to 1 
    *(volatile ioport unsigned int *)(0x1C06) |= 0xC000;
    // set the GPIO pin 16 - 17 to output, set SYS_GPIO_DIR1 (0x1C07) bit 0 and 1 to 1 
    *(volatile ioport unsigned int *)(0x1C07) |= 0x0003;
    
    // set the GPIO 14 - 15 to 0, set SYS_GPIO_DATAOUT0 (0x1C0A) bit 14 and 15 to 0
    //*(volatile ioport unsigned int *)(0x1C0A) &= 0x3FFF;
    *(volatile ioport unsigned int *)(0x1C0A) |= 0xC000;
    // set the GPIO 16 - 17 to 0, set SYS_GPIO_DATAOUT1 (0x1C0B) bit 0 and 1 to 0
    //*(volatile ioport unsigned int *)(0x1C0B) &= 0xFFFC;
    *(volatile ioport unsigned int *)(0x1C0B) |= 0x0003;

    //LELE: to reboot FFTHWA
    *(ioport volatile unsigned *)0x0001 = 0x000E;
    asm(" idle"); // must add at least one blank before idle in " ".
#endif

    return;
}

#if 1
void userIdle(void)
{
    // set CPUI bit in ICR
    *(volatile ioport Uint16 *)(0x0001) = 0x000F;
    // execute idle instruction to make CPU idle
    asm("    idle");        
}
#endif
