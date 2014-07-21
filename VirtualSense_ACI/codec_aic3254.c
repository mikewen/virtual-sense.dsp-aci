/*
 * $$$MODULE_NAME codec_aic3254.c
 *
 * $$$MODULE_DESC codec_aic3254.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   codec_aic3254.c
 *
 *  \brief  codec configuration function
 *
 *   This file contains the APIs for codec(AIC3254) read and write using I2C
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author     PR Mistral
 *
 *  \version    1.0
 *
 */

#include "psp_i2s.h"
#include "psp_i2c.h"
#include "dda_i2c.h"
#include "psp_common.h"
#include "app_globals.h"
#include "codec_aic3254.h"

#include "main_config.h"

#define I2C_OWN_ADDR            (0x2F)
#define I2C_BUS_FREQ            (10000u)
#define I2C_CODEC_ADDR          (0x18)

PSP_Handle    hi2c = NULL;


PSP_Result codec_sleep_mode(){
	PSP_Result result = PSP_SOK;

	if (hi2c)
	{
		result = AIC3254_Write(0, 1, hi2c); // select page 1 write 1 to page register to select page 1
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	    // disable analog power
	    result = AIC3254_Write(1, 0x08, hi2c);
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	    // disable digital power
	    result = AIC3254_Write(2, 0x04, hi2c);
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	}else {
		debug_printf("ERROR codec handler null\n");
	}
}
PSP_Result set_sampling_frequency_gain_impedence(unsigned long SamplingFrequency, unsigned int ADCgain, unsigned int impedance)
{
	PSP_Result result = PSP_SOK;
    volatile Uint16 looper;


	unsigned int PLLPR 			= 0x91;  // Default to 48000 Hz
	unsigned int AOSR  			=  128;  // default to 48000 Hz
	unsigned int BCLK_DIVIDER	= 0x84;  // default to clock I2S bus 48000 Hz
										 // (6144000/BCLK_DIVIDER) should be  == to 32*fs
	unsigned int PRB			= 0x01;   // processing block 1 default
	unsigned int gain;

	if ( ADCgain >= 48)
	{
		gain = 95;      //  Limit gain to 47.5 dB
	    ADCgain = 48;   // For display using printf()
	}
	else
	{
		gain = (ADCgain << 1); // Convert 1dB steps to 0.5dB steps
	}

	switch (SamplingFrequency)
	{
		case 192000:
		    PLLPR = 0x91; // 1001 0001b. PLL on. P = 1, R = 1.
		    AOSR  = 32;
		    BCLK_DIVIDER	= 0x81;
		    PRB = 0x07; // 13
		break;

		case 96000:
		    PLLPR = 0x91; // 1001 0001b. PLL on. P = 1, R = 1.
		    AOSR  = 64;
		    BCLK_DIVIDER	= 0x82;
		    PRB = 0x07; // 7
		break;

		case 48000:
	     	PLLPR = 0x91; // 1001 0001b. PLL on. P = 1, R = 1.
	     	BCLK_DIVIDER	= 0x84;
	    break;

	    case 24000:
	     	PLLPR = 0xA1; // 1010 0001b. PLL on. P = 2, R = 1.
	     	BCLK_DIVIDER	= 0x88;
	    break;

	    case 16000:
	    	PLLPR = 0xB1; // 1011 0001b. PLL on. P = 3, R = 1.
	    	BCLK_DIVIDER	= 0x8c;
	    break;
	}

	/* Reset AIC3204 */
	/* NOTE: Assumes EBSR and GPIO are set correctly before function is called */
	CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, 0x0000); /* reset active low */
	for(looper=0; looper<10; looper++ )
	    asm("    nop");
	CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, 0x0400);

	hi2c = I2C_Init(0x2f, 390000);  /* 390 kHz, assuming 100 MHz cpu clock */
	if (hi2c)
	{
		result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	    result = AIC3254_Write(1, 1, hi2c); // reset codec
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	    /* Select the PLL input and CODEC_CLKIN */
	    /* PLL input is assumed as 12MHz */
	    result = AIC3254_Write(4, 0x03, hi2c);
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	    // Power up the PLL and set P = 1 & R = 1
	    result = AIC3254_Write(5, PLLPR, hi2c);
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	    // Set J value to 7
	    result = AIC3254_Write(6, 0x07, hi2c);
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	    // Set D value(MSB) = 0x06
	    result = AIC3254_Write(7, 0x06, hi2c);
	    if (result != PSP_SOK)
	    {
	        return result;
	    }

	     // Set D value(LSB) = 0x90
	     result = AIC3254_Write(8, 0x90, hi2c); // 0x690 => .1680 => D = 7.1680
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // Set NDAC to 2
	     result = AIC3254_Write(11,0x82, hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // Set MDAC to 7
	     result = AIC3254_Write(12,0x87, hi2c);

	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     /* Set DAC OSR MSB value to 0 */
	     result = AIC3254_Write(13, 0x0, hi2c );
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // Set DAC OSR LSB value
	     result = AIC3254_Write(14, AOSR, hi2c );
	     if (result != PSP_SOK)
	     {
	         return result;
	     }
	     // Set BCLK is ADC_MOD_CLK --> (PLL_CLK/(MADC*NADC)) = 6144000
	     //11: BDIV_CLKIN = ADC_MOD_CLK
	     result = AIC3254_Write(29,0x03, hi2c); // 6144000 => 6.144 Mhz
	     if (result != PSP_SOK)
	     {
	         return result;
	     }
	     // Set BCLK divider value to 2 to transfer 32 bit word at 96Khz
	     // 32 * 96000 = 3.072 Mhz
	     // BCLK=ADC_MOD_CLK/N =(6144000/2) = 3.072 Mhz = 32*fs
	     // For 32 bit clocks per frame in Master mode ONLY
	     result = AIC3254_Write(30,BCLK_DIVIDER, hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // Set NADC to 2
	     result = AIC3254_Write(18,0x82, hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // Set MADC to 7
	     result = AIC3254_Write(19,0x87, hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // Set ADC OSR LSB value
	     result = AIC3254_Write(20, AOSR, hi2c );
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // Enable processing block
	     result = AIC3254_Write(61, PRB, hi2c );
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(27,0xd, hi2c); // BCLK and WCLK is set as op to AIC3254(Master)
	     if (result != PSP_SOK)
	     {
	         return result;
	     }


	     result = AIC3254_Write(0,1,hi2c);// select page 1
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	                // power up Mic Bias using LDO-IN
	     result = AIC3254_Write(51,0x48,hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(0x1,0x8,hi2c);// Disable crude AVDD generation from DVDD
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     //Enable Master Analog Power Control
	     result = AIC3254_Write(0x2,0x1,hi2c);// Enable Analog Blocks and internal LDO
	     if (result != PSP_SOK)
	     {
	         return result;
	     }
	     // Set the input power-up time to 3.1ms (for ADC)
	     //w 30 47 32
	     result = AIC3254_Write(0x47,0x32,hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     //# Set the REF charging time to 40ms
	     //w 30 7b 01
	     result = AIC3254_Write(0x7b,0x01,hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     //01: IN1L is routed to Left MICPGA with 10k resistance
	     //10: IN1L is routed to Left MICPGA with 20k resistance
	     //11: IN1L is routed to Left MICPGA with 40k resistance

	     //Route Common Mode to LEFT_P with impedance

	     // IN2_L  we use
	     // IN2_R
	     result = AIC3254_Write(52,(impedance>>2),hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // differential configuration
	     // need to pass impedance
	     // CM configuration need to pass impedance << 2
	     // or impedance  >> 4
	     //Route Common Mode to LEFT_M with impedance
	     result = AIC3254_Write(54,(impedance>>2)/*impedance*/,hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     //Route IN2R to RIGHT_P with impedance
	     result = AIC3254_Write(55,0x00,hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     // differential configuration
		 // need to pass impedance
		 // CM configuration need to pass impedance << 2
		 // or impedance  >> 4
		 //Route Common Mode to LEFT_M with impedance
	     //Route Common Mode to RIGHT_M with impedance
	     result = AIC3254_Write(57,0x00,hi2c);
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     //Unmute Left MICPGA
	     result = AIC3254_Write(59,gain,hi2c); // Gain = 30 dB
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     //Unmute Right MICPGA
	     result = AIC3254_Write(60,0x00,hi2c); // Gain = 30 dB
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(64,0x2,hi2c); // left vol=right vol
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(63,0xd4, hi2c); // power up left,right data paths and set channel
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(64,0xc,hi2c); // left vol=right vol; muted
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(0,1,hi2c);// select page 1
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(0x10,0,hi2c);// unmute HPL , 0dB gain
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(0x11,0,hi2c);// unmute HPR , 0dB gain
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(0x9,0x30,hi2c);// power up HPL,HPR
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(0x0,0x0,hi2c);// select page 0
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(0x40,0x2,hi2c);// unmute DAC with right vol=left vol
	     if (result != PSP_SOK)
	     {
	         return result;
	     }

	     result = AIC3254_Write(65,/*20 48*/ 0,hi2c);// set DAC gain to 0dB
	     if (result != PSP_SOK)
	     {
	         return result;
	     }
         //Powerup left and right ADC
         //LELE Powerup  only left ADC 0x80 -- was 0xc0
	     result = AIC3254_Write(81,0xc0,hi2c);
	     if (result != PSP_SOK)
	     {
	    	 return result;
	     }
	     //Unmute left and right ADC
	     //LELE Unmute only left ADC 0x08-- was 0x00
	     result = AIC3254_Write(82,0x00,hi2c);
	     if (result != PSP_SOK)
	     {
	    	 return result;
	     }
	     result = AIC3254_Write(0x0,0x1,hi2c);// select page 1
	     if (result != PSP_SOK)
	     {
	    	 return result;
	     }

	     result = AIC3254_Write(0x10,0,hi2c);// unmute HPL , 0dB gain
	     if (result != PSP_SOK)
	     {
	    	 return result;
	     }
	     result = AIC3254_Write(0x11,0,hi2c);// unmute HPR , 0dB gain
	     if (result != PSP_SOK)
	     {
	    	 return result;
	     }

	     // write 1 to page register to select page 1 - prepare for next headset volume change
	     result = AIC3254_Write(0, 1, hi2c);
	     if (result != PSP_SOK)
	     {
	    	 return FALSE;
	     }

#if 1 // debug
	     result = AIC3254_Write(0x0,0x0,hi2c);// select page 0
	     if (result != PSP_SOK)
	     {
	    	 return result;
	     }
	     // route ADC_FS to WCLK (I2S FS)
	     result = AIC3254_Write(33, 0x10, hi2c);
	     if (result != PSP_SOK)
	     {
	    	 return result;
	     }
#endif

        return result;
  }
else
 {
      return PSP_E_DRIVER_INIT;
 }

}


/*
 * Mute control for codec output
 * TRUE = Mute codec output
 * FALSE = UnMute codec output
 ***********************************************************************/
Bool Set_Mute_State(Bool flag)
{
    PSP_Result    result = PSP_SOK;
    Bool retval;

    retval = TRUE;

    // write 0 to page register to select page 0
    result = AIC3254_Write(0, 0, hi2c);
    if (result != PSP_SOK) 
    {
        retval = FALSE;
    }
    else
    {
        if (flag == TRUE)
        {
            //mute output
            result = AIC3254_Write(64,0xd,hi2c);
            if (result != PSP_SOK) 
            {
                retval = FALSE;
            }
        }
        else
        {
            //unmute output
            result = AIC3254_Write(64,0x1,hi2c);
            if (result != PSP_SOK) 
            {
                retval = FALSE;
            }
        }
    }
#if 1
    // write 1 to page register to select page 1 - prepare for next headset volume change
    result = AIC3254_Write(0, 1, hi2c);
    if (result != PSP_SOK) 
    {
        retval = FALSE;
    }
#endif
    return retval;
}

#define HEADPHONE_DRIVER_GAIN_MUTE_ENABLE  0x40    // bit6 =1 mute headphone driver
#define VOLUME_STEP_SIZE                   256
#define VOLUME_TABLE_MAX_GAIN_INDEX        29      // headphone gain setting = 29 -> 29 dB gain
#define VOLUME_TABLE_MAX_ATTNEUATION_INDEX 35      // headphone gain setting = 0x3A -> -6dB gain
#define USB_MAX_ATTENUATION_VALUE          -32768
#define VOLUME_TABLE_MUTE_HEADPHONE_INDEX  36      // headphone gain setting = 0x7B set gain to -5dB with headphone driver muted

// table has both gain and attenuation settings for headphone output of the codec.
// 0 : no gain/no attenuation, gain : 1 - 29, attenuation : 0x3F - 0x3A, muted: 0x7B
const Uint16 volume_table[] =  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
    0x3F,0x3E,0x3D,0x3C,0x3B,0x3A,(0x3B | HEADPHONE_DRIVER_GAIN_MUTE_ENABLE)
};

/*
 * Change gain setting of headphone output of codec
 * volume = gain setting received from USB
 * channel = 0:left channel, =1 right channel
 ***********************************************************************/
Bool Adjust_Volume(Int16 volume, Uint16 channel)
{
    PSP_Result    result = PSP_SOK;
    Uint16        gain;

    // goto max attentuation
    if (volume == USB_MAX_ATTENUATION_VALUE)
    {
        // the max attenuation for the headpphone  is -6dB so we will mute the headphone driver
        // and set the codec gain to the lowest value(-5dB) that allows the headphone driver
        // to be muted. any volume change other than the max attenuation will turn off the
        // headphone driver mute
        gain = VOLUME_TABLE_MUTE_HEADPHONE_INDEX;
    }
    else if (volume >= 0)
    {
        // determine gain index
        gain = volume/VOLUME_STEP_SIZE;

        // check range
        if(gain > VOLUME_TABLE_MAX_GAIN_INDEX)
        {
            // set to max gain
            gain = VOLUME_TABLE_MAX_GAIN_INDEX;
        }
    }
    else
    {
        // determine attenuation index
        gain = (-volume)/VOLUME_STEP_SIZE;
        if (gain !=0)
        {
            //index from start of attentuation values in table
            gain += VOLUME_TABLE_MAX_GAIN_INDEX;
            if (gain > VOLUME_TABLE_MAX_ATTNEUATION_INDEX)
            {
                // set to max attenuation
                gain = VOLUME_TABLE_MAX_ATTNEUATION_INDEX;
            }
        }

    }

    if (channel == 0)
    {
        //adjust volume setting of left headphone
        result = AIC3254_Write(0x10,volume_table[gain],hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
    }
    else
    {
        //adjust volume setting of right headphone
        result = AIC3254_Write(0x11,volume_table[gain],hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
    }
    return TRUE;
}

/**
 *  \brief Codec write function
 *
 *  Function to write a byte of data to a codec register.
 *
 *  \param regAddr  [IN]  Address of the register to write the data
 *  \param regData  [IN]  Data to write into the register
 *
 *  \return PSP_SOK - if successful, else suitable error code
 */
PSP_Result AIC3254_Write(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c)
{
    PSP_Result    status;
    Uint16        writeCount;
    Uint16        writeBuff[2];

    status = PSP_E_INVAL_PARAM;

    //if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = (regData & 0x00FF);

        /* Write the data */
        status = I2C_Write(hi2c, I2C_CODEC_ADDR, writeCount, writeBuff);
    }

    return status;
}

/**
 *  \brief Codec read function
 *
 *  Function to read a byte of data from a codec register.
 *
 *  \param regAddr  [IN]  Address of the register to read the data
 *  \param data     [IN]  Pointer to the data read from codec register
 *
 *  \return PSP_SOK - if successful, else suitable error code
 */
PSP_Result AIC3254_Read(Uint16 regAddr, Uint16 *data, PSP_Handle  hi2c)
{
    PSP_Result status  = PSP_E_INVAL_PARAM;
    Uint16 readCount = 1;
    Uint16 readBuff[1];

    regAddr = (regAddr & 0x00FF);

   if(hi2c)
     status = I2C_Read(hi2c,
                    I2C_CODEC_ADDR,
                    readCount,
                    regAddr,
                    readBuff);

    if(status == PSP_SOK)
     *data = readBuff[1];

    return status;
}


Uint16   codec_Ioctl(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c)
{
    return PSP_SOK;
}
