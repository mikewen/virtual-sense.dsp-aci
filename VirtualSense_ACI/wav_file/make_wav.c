/*
 * make_wav.c
 *
 *  Created on: 02/set/2013
 *      Author: lattanzi
 */

/* make_wav.c
 * Creates a WAV file from an array of ints.
 * Output is monophonic, signed 16-bit samples
 * copyright
 * Fri Jun 18 16:36:23 PDT 2010 Kevin Karplus
 * Creative Commons license Attribution-NonCommercial
 *  http://creativecommons.org/licenses/by-nc/3.0/
 */
#include "VirtualSense_ACIcfg.h"
#include <stdio.h>
#include <assert.h>

#include "make_wav.h"
#include "ff.h"

static unsigned char header[20];
static unsigned long writed = 0;
static unsigned char header_index = 0;

void write_little_endian(unsigned long word, int num_bytes);
void print_header();

FRESULT open_wave_file(FIL *file, char *filename, unsigned long sample_per_second, unsigned long seconds){
	//FIL wav_file;
	UINT bw;
	FRESULT rc;
	//FATFS fatfs;
	unsigned long sample_rate = sample_per_second;
	unsigned int num_channels = 1;
	unsigned int bytes_per_sample = 2;
	unsigned long byte_rate;

	byte_rate = sample_rate*num_channels*bytes_per_sample;

	LOG_printf(&trace, "\nCreate a new file %s.\n", filename);
	LOG_printf(&trace, "\nSampling rate %ld.\n", sample_rate);
	rc = f_open(file, filename, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc) LOG_printf(&trace, "Error creating file %d\n",rc);


	LOG_printf(&trace, "\nWrite headers\n");
	/* write RIFF header */
	rc = f_write(file, "RIFF", 4, &bw);
	if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
	LOG_printf(&trace, "%u bytes written.\n", bw);

	write_little_endian(36 + bytes_per_sample* sample_rate*seconds*num_channels, 4); //TODO size in second
	LOG_printf(&trace, "write the header  subchunk");
	/* write the header  subchunk */
	rc = f_write(file, header, 4, &bw);
	if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
	LOG_printf(&trace, "%u bytes written.\n", bw);
	//print_header();

	header_index = 0;

    rc = f_write(file, "WAVE", 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);

    /* write fmt  subchunk */
    rc = f_write(file, "fmt ", 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);

    LOG_printf(&trace, "writing format\n");
    write_little_endian(16, 4);   /* SubChunk1Size is 16 */
    write_little_endian(1, 2);    /* PCM is format 1 */
    write_little_endian(num_channels, 2);
    write_little_endian(sample_rate, 4);
    write_little_endian(byte_rate, 4);
    write_little_endian(num_channels*bytes_per_sample, 2);  /* block align */
    write_little_endian(8*bytes_per_sample, 2);  /* bits/sample */
    //print_header();

    /* write the header  subchunk */
    rc = f_write(file, header, 20, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);
    header_index = 0;

    //write header

    /* write data subchunk */
    rc = f_write(file, "data", 4, &bw);
    if (rc) printf("Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);

    LOG_printf(&trace, "write bytes_per_sample* num_samples*num_channels\n");
    write_little_endian(bytes_per_sample* sample_rate*seconds*num_channels, 4);

    /* write the header  subchunk */
    rc = f_write(file, header, 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);
    //print_header();
    header_index = 0;

	// now it is possible to write data
    return rc;
}

FRESULT write_data_to_wave(FIL *file, const void *buff, unsigned int number_of_bytes){
	UINT bw;
	FRESULT rc;
	rc = f_write(file, buff, number_of_bytes,&bw);
	if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
	//printf("%u bytes written.\n", bw);
	return rc;
}


FRESULT close_wave_file(FIL *file){
	FRESULT rc;
	LOG_printf(&trace, "\nClose the file.\n");
	rc = f_close(file);
	if (rc) LOG_printf(&trace, "Error closing file %d\n",rc);
	return rc;
}


void print_header(){
	UINT i = 0;
	LOG_printf(&trace, "\n ------------\n");
	for(i = 0; i < 20; i++){
		LOG_printf(&trace, "0x%02x\n",header[i]);
	}
	LOG_printf(&trace, "\n ------------\n");
}

void write_little_endian(unsigned long word, int num_bytes)
{
	while(num_bytes>0)
    {   header[header_index] = word & 0xff;
        num_bytes--;
        word >>= 8;
        header_index++;
    }
}

/* information about the WAV file format from

http://ccrma.stanford.edu/courses/422/projects/WaveFormat/

 */

void write_wav(char * filename, unsigned long num_samples, short int * data, int s_rate)
{
    FIL wav_file;
    UINT bw;
    FRESULT rc;
    unsigned long sample_rate;
    unsigned int num_channels;
    unsigned int bytes_per_sample;
    unsigned long byte_rate;
    unsigned long i,j;    /* counter for samples */



    num_channels = 1;   /* monoaural */
    bytes_per_sample = 2;

    if (s_rate<=0) sample_rate = 44100;
    else sample_rate = (unsigned int) s_rate;

    byte_rate = sample_rate*num_channels*bytes_per_sample;


    LOG_printf(&trace, "\nCreate a new file %s.\n", filename);
    rc = f_open(&wav_file, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (rc) printf("Error creating file %d\n",rc);


    LOG_printf(&trace, "\nWrite headers\n");
 	/* write RIFF header */
    rc = f_write(&wav_file, "RIFF", 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);

    write_little_endian(36 + bytes_per_sample* num_samples*num_channels, 4);
    LOG_printf(&trace, "write the header  subchunk");
    /* write the header  subchunk */
    rc = f_write(&wav_file, header, 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);
    print_header();

    header_index = 0;



    rc = f_write(&wav_file, "WAVE", 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);


    /* write fmt  subchunk */
    rc = f_write(&wav_file, "fmt ", 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);


    LOG_printf(&trace, "writing format\n");
    write_little_endian(16, 4);   /* SubChunk1Size is 16 */
    write_little_endian(1, 2);    /* PCM is format 1 */
    write_little_endian(num_channels, 2);
    write_little_endian(sample_rate, 4);
    write_little_endian(byte_rate, 4);
    write_little_endian(num_channels*bytes_per_sample, 2);  /* block align */
    write_little_endian(8*bytes_per_sample, 2);  /* bits/sample */
    print_header();

    /* write the header  subchunk */
    rc = f_write(&wav_file, header, 20, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);
    header_index = 0;

    //write header

    /* write data subchunk */
    rc = f_write(&wav_file, "data", 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);

    LOG_printf(&trace, "write bytes_per_sample* num_samples*num_channels\n");
    write_little_endian(bytes_per_sample* num_samples*num_channels, 4);

    /* write the header  subchunk */
    rc = f_write(&wav_file, header, 4, &bw);
    if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    LOG_printf(&trace, "%u bytes written.\n", bw);
    print_header();
    header_index = 0;

   //write 10 s data
    for(j =0; j < 10; j++){
    	rc = f_write(&wav_file, data,num_samples*bytes_per_sample,&bw);
    	if (rc) LOG_printf(&trace, "Error writing file %d\n",rc);
    	LOG_printf(&trace, "%u bytes written.\n", bw);
    }
    //write_little_endian(data,num_samples*bytes_per_sample, wav_file);
    /*for (i=0; i< num_samples; i++)
    {   write_little_endian((unsigned int)(data[i]),bytes_per_sample, wav_file);
    }*/

    LOG_printf(&trace, "little_endian wrote %ld bytes \n",writed);

    LOG_printf(&trace, "\nClose the file.\n");
    rc = f_close(&wav_file);
    if (rc) LOG_printf(&trace, "Error closing file %d\n",rc);
}

FRESULT directory_listing(){
	DIR dir;				/* Directory object */
	FILINFO fno;			/* File information object */
	FRESULT rc;

	LOG_printf(&trace, "\nOpen root directory.\n");
	rc = f_opendir(&dir, (const TCHAR *)"");
	if (rc) LOG_printf(&trace, "Error listing directory\n");

	LOG_printf(&trace, "\nDirectory listing...\n");
	for (;;) {
		rc = f_readdir(&dir, &fno);		/* Read a directory item */
		if (rc || !fno.fname[0]) break;	/* Error or end of dir */
		if (fno.fattrib & AM_DIR)
			LOG_printf(&trace, "   <dir>  %s\n", fno.fname);
		else
			LOG_printf(&trace, "%8lu  %s\n", fno.fsize, fno.fname);
	}
	return rc;
}


