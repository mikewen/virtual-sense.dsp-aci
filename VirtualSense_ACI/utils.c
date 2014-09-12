/*
 * $$$MODULE_NAME utils.c
 *
 * $$$MODULE_DESC utils.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include "utils.h"

/* Gets index of value in array */
Int16 getValIdx(
    Int32 val, 
    Int32 *valArray, 
    Uint16 numVal, 
    Uint16 *pIdx
)
{
    Uint16 idx;
    Bool found;
    Int16 status;

    idx = 0;
    found = FALSE;
    do {
        if (val != valArray[idx])
        {
            idx++;
        }
        else
        {
            found = TRUE;
        }
    } while ((idx < numVal) && (found == FALSE));

    if (found == TRUE)
    {
        *pIdx = idx;
        status = UTL_SOK;
    }
    else
    {
        status = UTL_GETVALIDX_VALNF;
    }

    return status;
}
