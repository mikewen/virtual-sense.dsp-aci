/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */ 
/* 	i2c_display.h                                                        */
/*                                                                           */
/*  \brief  MIDAS i2c display functions
 *
 *   This file contains the APIs for I2C MIDA LCD Display
 *
 *  (C) Copyright 2014, Emanuele Lattanzi
 *
 *  \author     Emanuele Lattanzi
 *
 *  \version    1.0
 *              */
/*                                                                           */
/*****************************************************************************/
/*
 *
 */

#ifndef I2C_DISPLAY_H
#define I2C_DISPLAY_H

void LCD_Init(int ContrastVoltage);
void LCD_Write(const char *format, ...);
PSP_Result display_Write_Byte(Uint8 byte, PSP_Handle hi2c);

static inline void  _delay_ms(Uint16 ms){
	Uint16 looper = 0;
	/* Give some delay */
	for(looper = 0; looper < 0xFF; looper++){;}
}
static inline void  _delay_us(Uint16 us){
	Uint16 looper = 0;
	/* Give some delay */
	for(looper = 0; looper < 0x0F; looper++){;}
}

#endif

/*****************************************************************************/
/* End of i2c_display.h                                                       */
/*****************************************************************************/
