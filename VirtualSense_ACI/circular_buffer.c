/*
 * $$$MODULE_NAME circular_buffer.c
 *
 * $$$MODULE_DESC circular_buffer.c
 *
*/

#include <std.h>

#include "main_config.h"


// circular buffer for data collection from AIC3204
#pragma DATA_SECTION(circular_buffer, ".circular_buffer");
#pragma DATA_ALIGN(circular_buffer, 2)
unsigned char circular_buffer[PROCESS_BUFFER_SIZE];
// index for bufferIn
Uint16 bufferInIdx = 0; //logical pointers
Uint16 bufferOutIdx = 0;



void circular_buffer_put(Int16 item){
	 circular_buffer[bufferInIdx] =  (item & 0xFF);
	 bufferInIdx = ((bufferInIdx+1) % PROCESS_BUFFER_SIZE);
	 circular_buffer[bufferInIdx] =  ((item >> 8) & 0xFF);
	 bufferInIdx = ((bufferInIdx+1) % PROCESS_BUFFER_SIZE);
}

Int16 circular_buffer_get(){
	Int16 item = 0;
	item = circular_buffer[bufferOutIdx];
	bufferOutIdx = ((bufferOutIdx + 1) % PROCESS_BUFFER_SIZE);
	item += (circular_buffer[bufferOutIdx] << 8);
	bufferOutIdx = ((bufferOutIdx + 1) % PROCESS_BUFFER_SIZE);
	return item;
}
