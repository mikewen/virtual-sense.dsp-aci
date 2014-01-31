/*
 * make_wav.h
 *
 *  Created on: 02/set/2013
 *      Author: lattanzi
 */

#ifndef MAKE_WAV_H_
#define MAKE_WAV_H_

#include "ff.h"


void write_wav(const TCHAR * filename, unsigned long num_samples, short int * data, int s_rate);
    /* open a file named filename, write signed 16-bit values as a
        monoaural WAV file at the specified sampling rate
        and close the file
    */

FRESULT close_wave_file(FIL *file);
FRESULT write_data_to_wave(FIL *file, const void *buff, unsigned int number_of_bytes);
FRESULT open_wave_file(FIL *file, const TCHAR *filename, unsigned long sample_per_second, unsigned long seconds);
FRESULT directory_listing();

#endif /* MAKE_WAV_H_ */
