/*
 * $$$MODULE_NAME i2c_display.c
 *
 * $$$MODULE_DESC i2c_display.c
 *
 * Copyright (C) 2014 Emanuele Lattanzi
 *
 /

/**
 *  \file   i2c_display.c
 *
 *  \brief  MIDAS i2c display functions
 *
 *   This file contains the APIs for I2C MIDA LCD Display
 *
 *  (C) Copyright 2014, Emanuele Lattanzi
 *
 *  \author     Emanuele Lattanzi
 *
 *  \version    1.0
 *
 */

#include <stdio.h>
#include "psp_i2c.h"
//#include "dda_i2c.h"
#include "csl_i2c.h"
#include "psp_common.h"

#include "main_config.h"
#include "i2c_display.h"

#define I2C_DISPLAY_ADDRESS          0x3E
#define I2C_READ    1
/** defines the data direction (writing to I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_WRITE   0

#define CSL_I2C_DATA_SIZE        (64)
#define CSL_I2C_OWN_ADDR         (0x2F)
//#define CSL_I2C_SYS_CLK          (12.228)
//#define CSL_I2C_SYS_CLK          (40)
//#define CSL_I2C_SYS_CLK          (60)
//#define CSL_I2C_SYS_CLK          (75)
#define CSL_I2C_SYS_CLK          (40)
#define CSL_I2C_BUS_FREQ         (10)
#define CSL_I2C_EEPROM_ADDR		 (0x50)
#define CSL_I2C_CODEC_ADDR		 (0x18)



extern PSP_Handle    hi2c;
CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;


static char buffer[80];

/*************************************************************************
 Writes a Command to the Midas MCCOG21605B6W-SPTLYI LCD display

 Input:   Command byte.

 Return:

*************************************************************************/
void LCD_Command_Write(Uint8 Cmd)
{
	Uint16 myCMD = (Uint16)Cmd;
	I2C_Write(hi2c, I2C_DISPLAY_ADDRESS, 2, &myCMD);
}

/*************************************************************************
 Shifts the cursor of the Midas MCCOG21605B6W-SPTLYI LCD display

 Display layout: (hex)
 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F

 Input:   Position to move the cursor to

 Return:

*************************************************************************/
void LCD_Shift_Cursor(unsigned char Position)
{
   LCD_Command_Write(Position);
}

void LCD_Init(int ContrastVoltage)
{
#if 0

	/**
	 *  self.write_data(0x00, 0x38) # I don'6 know if this even makes sense. It's from the example!
        self.write_data(0x00, 0x39) # Function Set, 8bit, 2lines, NoDoubleHeight, InstructionTable=1
        self.write_data(0x00, 0x14) # Bias=0 (1/5), AdjIntOsc=4
        self.write_data(0x00, 0x74) # Set Contrast bits 3..0 to 4
        self.write_data(0x00, 0x54) # Icon=off, booster=on, Contrast bits 5+4 = 0
        self.write_data(0x00, 0x6F) # Switch on follower circuit and set ratio to 7
        self.write_data(0x00, 0x38) # Function Set, 8bit, 2lines, NoDoubleHeight, InstructionTable=0
        self.write_data(0x00, 0x0C) # Cursor on
	 */

	hi2c = I2C_Init(0x2f, 390000);  /* 390 kHz, assuming 100 MHz cpu clock */
	debug_printf("i2c created \n");
	debug_printf("write res %d\n",display_Write_Byte(0x38, hi2c));
	debug_printf("write res %d\n",display_Write_Byte(0x39, hi2c));
	debug_printf("write res %d\n",display_Write_Byte(0x14, hi2c));
	debug_printf("write res %d\n",display_Write_Byte(0x74, hi2c));
	debug_printf("write res %d\n",display_Write_Byte(0x54, hi2c));
	debug_printf("write res %d\n",display_Write_Byte(0x6F, hi2c));
	debug_printf("write res %d\n",display_Write_Byte(0x38, hi2c));
	debug_printf("write res %d\n",display_Write_Byte(0x0c, hi2c));
#endif

#if 1
	//Uint16 bToWrite = 0x00;
	Uint16 res = 0;
	Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));
	Uint16 onlyStart = CSL_I2C_START;
	CSL_Status  status;
	//Uint16 vString[] = {'c', 'V', 'i','r','t','u','a','l','S','e','n','s','e'};
	char vString[] = "cVirtualSnesei";//{'c', 'V', 'i','r','t','u','a','l','S','e','n','s','e'};
	status = I2C_init(CSL_I2C0);
		if(status != CSL_SOK)
		{
			debug_printf("I2C Init Failed!!\n");

		}

		/* Setup I2C module */
		i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
		i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
		i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
		i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
		i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
		i2cSetup.ownAddr     = CSL_I2C_OWN_ADDR;
		i2cSetup.sysInputClk = CSL_I2C_SYS_CLK;
		i2cSetup.i2cBusFreq  = CSL_I2C_BUS_FREQ;

		status = I2C_setup(&i2cSetup);
		if(status != CSL_SOK)
		{
			debug_printf("I2C Setup Failed!!\n");


		}
		/* Set the start bit */
		/*CSL_I2C_SETSTART();

		_delay_ms(2);
		CSL_I2C_SETSTOP();
		_delay_ms(2);

		CSL_I2C_SETSTART();*/

		//bToWrite = 0x00;

		Uint16 ustr[sizeof(vString)];
		int t;
		for(t = 0; t < sizeof(vString); t++)
		{
			ustr[t] = (Uint16)vString[t];
		}

		Uint16 b = 0x00;
		status = I2C_write(&b, 1, 0x00, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);
		_delay_ms(10);

		Uint16 bToWrite[] = {0x00, 0x38, 0x39, 0x14, 0x74, 0x54, 0x6F, 0x0F, 0x01};
		status = I2C_write(bToWrite, 9, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);
		_delay_ms(10);

		status = I2C_write(ustr, sizeof(ustr), I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);
		_delay_ms(10);


/*
	    bToWrite = 0x00;
		status = I2C_write(&bToWrite, 1, I2C_DISPLAY_ADDRESS, TRUE, onlyStart, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);
		_delay_ms(10);

		bToWrite = 0x0038;
		status = I2C_write(&bToWrite, 1, 0x00, TRUE, 0x00, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);
		_delay_ms(10);

		bToWrite = 0x0039;
		status = I2C_write(&bToWrite, 1, 0x00, TRUE, 0x00, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);
		_delay_ms(10);




		bToWrite = 0x0014;
		status = I2C_write(&bToWrite, 1, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);

		//midas 5v

		bToWrite = 0x0074;
		status = I2C_write(&bToWrite, 1, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);

		bToWrite = 0x0054;
		status = I2C_write(&bToWrite, 1, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);

		bToWrite = 0x006F;
		status = I2C_write(&bToWrite, 1, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		debug_printf("Write status %d\n",status);




		_delay_ms(10);

		bToWrite = 0x0C;
		status = I2C_write(&bToWrite, 1, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);

		bToWrite = 0x01;
		status = I2C_write(&bToWrite, 1, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);


		bToWrite = 0x06;
		I2C_write(&bToWrite, 1, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		_delay_ms(10);

		CSL_I2C_SETSTOP();*/

		_delay_ms(10);
#endif


}

/*************************************************************************
 Writes a string or char to the Midas MCCOG21605B6W-SPTLYI LCD display

 Input:   String. "Hello World!"

 Return:

*************************************************************************/
void LCD_Write(const char *format, ...){

	va_list aptr;
	int len;

	va_start(aptr, format);
	len = vsprintf(buffer, format, aptr);
	//va_end(aptr);
	//debug_printf("ritorna ret: %d\n", ret);

	Uint16 res = 0;
	Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));
	Uint16 onlyStart = CSL_I2C_START;
	CSL_Status  status;

	status = I2C_init(CSL_I2C0);
	if(status != CSL_SOK)
	{
		debug_printf("I2C Init Failed!!\n");
	}

	/* Setup I2C module */
	i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
	i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
	i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	i2cSetup.ownAddr     = CSL_I2C_OWN_ADDR;
	i2cSetup.sysInputClk = CSL_I2C_SYS_CLK;
	i2cSetup.i2cBusFreq  = CSL_I2C_BUS_FREQ;

	status = I2C_setup(&i2cSetup);
	if(status != CSL_SOK)
	{
		debug_printf("I2C Setup Failed!!\n");
	}


	int t;
	Uint16 ustr[sizeof(buffer)+1];
	ustr[0] = (Uint16)'c';
	for(t = 0; t < len; t++)
	{
		ustr[t+1] = (Uint16)buffer[t];
	}

	Uint16 b = 0x00;
	status = I2C_write(&b, 1, 0x00, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
	debug_printf("Write status %d\n",status);
	_delay_ms(10);

	Uint16 bToWrite[] = {0x00, 0x38, 0x39, 0x14, 0x74, 0x54, 0x6F, 0x0F, 0x01};
	status = I2C_write(bToWrite, 9, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
	debug_printf("Write status %d\n",status);
	_delay_ms(10);

	status = I2C_write(ustr, len+1, I2C_DISPLAY_ADDRESS, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
	debug_printf("Write status %d\n",status);
	_delay_ms(10);
}




#if 0
void LCD_Write_String(const char* dstring)
{
   while(*dstring)   //Is the character pointed at by dstring a zero? If not, write character to LCD
   {
      unsigned char ret;

      ret = i2c_start(DevLCD+I2C_WRITE);   // Set device address and write mode

      if ( ret )
      {
         //Escape 2:
         /* failed to issue start condition, possibly no device found */
         i2c_stop();
         //Stop "while(*dstring)" from continuing looping if no device is there
         break;
      }
      else
      {

         i2c_write(0b01000000);   //Tell the LCD i want to write text/data (Set RS High, 6th bit)
         i2c_write(*dstring++);  //Write the character from dstring to the LCD, then post-inc the dstring is pointing at.
         i2c_stop();
      }
   }
}

int main(void)
{
   i2c_init();      //Initialize I2C
   LCD_Init(0);   //Initialize LCD

   LCD_Write_String("H");

    while(1)
    {
        //TODO:: Please write your application code
    }
}

#endif

#if 1
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
PSP_Result display_Write(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c)
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
        status = I2C_Write(hi2c, I2C_DISPLAY_ADDRESS, writeCount, writeBuff);
    }

    return status;
}

PSP_Result display_Write_Byte(Uint8 byte, PSP_Handle hi2c)
{
    PSP_Result    status;
    Uint16        writeCount;
    Uint16        writeBuff[2];

    status = PSP_E_INVAL_PARAM;

    if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (byte & 0x00FF);
        writeBuff[1] = (byte & 0x00FF);


        /* Write the data */
        debug_printf("i2c w\n");
        status = I2C_Write(hi2c, I2C_DISPLAY_ADDRESS, writeCount, writeBuff);
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
PSP_Result display_Read(Uint16 regAddr, Uint16 *data, PSP_Handle  hi2c)
{
    PSP_Result status  = PSP_E_INVAL_PARAM;
    Uint16 readCount = 1;
    Uint16 readBuff[1];

    regAddr = (regAddr & 0x00FF);

   if(hi2c)
     status = I2C_Read(hi2c,
                    I2C_DISPLAY_ADDRESS,
                    readCount,
                    regAddr,
                    readBuff);

    if(status == PSP_SOK)
     *data = readBuff[1];

    return status;
}

#if 0

Uint8 init_diaplay()
{
	PSP_Result result = PSP_SOK;

	//hi2c = I2C_Init(0x2f, 390000);  /* 390 kHz, assuming 100 MHz cpu clock */
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
                    I2C_DISPLAY_ADDRESS,
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
#endif
#endif
