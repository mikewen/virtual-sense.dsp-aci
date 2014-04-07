/*
 * $$$MODULE_NAME gpio_control.c
 *
 * $$$MODULE_DESC gpio_control.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** @file gpio_control.c
 *
 *  @brief Code to control GPIO module
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 23-Jun-2011 - Created
 * ============================================================================
 */

#include <std.h>
#include "csl_gpio.h"
#include "gpio_control.h"

CSL_GpioObj gGpioObj;
CSL_GpioObj *hGpio;

#define DBG_GPIO1   ( CSL_GPIO_PIN15 ) //WAS 15
#define DBG_GPIO2   ( CSL_GPIO_PIN16 )

// Debug GPIO's
Int16 dbgGpio1State =0;
Int16 dbgGpio2State =0;

/**
 *  \brief  Function configures GPIO
 *
 *  This function configures the GPIO module.
 *
 *  \param  ioDir  - IODIR1/2 register setting
 *  \param  intEn  - IOINTEN1/2 register setting
 *  \param  intEdg - IOINTEDG1/2 register setting
 *
 *  \return status
 */
Int16 gpioInit(
    Uint32 ioDir, 
    Uint32 intEn, 
    Uint32 intEdg 
)
{
    CSL_Status status;
    CSL_GpioConfig config;

    /* Open GPIO module */
    hGpio = GPIO_open(&gGpioObj, &status);
    if ((hGpio == NULL) || (status != CSL_SOK))
    {
        return GPIOCTRL_OPEN_FAIL;
    }

    /* Reset GPIO module */
    status = GPIO_reset(hGpio);
    if (status != CSL_SOK)
    {
        return GPIOCTRL_RESET_FAIL;
    }

    /* Configure GPIO module */
    config.GPIODIRL = ioDir & 0xFFFF;
    config.GPIODIRH = ioDir >> 16;
    config.GPIOINTENAL = intEn & 0xFFFF;
    config.GPIOINTENAH = intEn >> 16;
    config.GPIOINTTRIGL = intEdg & 0xFFFF;
    config.GPIOINTTRIGH = intEdg >> 16;
    status = GPIO_config(hGpio, &config);
    if (status != CSL_SOK)
    {
        return GPIOCTRL_CFG_FAIL;
    }

    return GPIOCTRL_SOK;
}

Int16 dbgGpio1Write(
    Uint16 gpioOutputValue
)
{
    CSL_Status  status;
    Int16 ret = GPIOCTRL_SOK;

    status = GPIO_write(hGpio, DBG_GPIO1, gpioOutputValue);
    if(status != CSL_SOK)
    {
        ret = GPIOCTRL_WRITE_FAIL;
    }

    return ret;
}

Int16 dbgGpio2Write(
    Uint16 gpioOutputValue
)
{
    CSL_Status  status;
    Int16 ret = GPIOCTRL_SOK;

    status = GPIO_write(hGpio, DBG_GPIO2, gpioOutputValue);
    if(status != CSL_SOK)
    {
        ret = GPIOCTRL_WRITE_FAIL;
    }

    return ret;
}
