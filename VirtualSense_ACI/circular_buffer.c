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

#pragma DATA_SECTION(circular_buffer2, ".circular_buffer2");
#pragma DATA_ALIGN(circular_buffer2, 2)
unsigned char circular_buffer2[PROCESS_BUFFER_SIZE];

unsigned char * buffer = circular_buffer; // at start points to the first circular buffer
// index for bufferIn
Uint32 bufferInIdx = 0; //logical pointers
Uint32 bufferOutIdx = 0;
Uint32 b_size = PROCESS_BUFFER_SIZE;
Int32 bufferInside = 0;

Uint32 buffer2InIdx = 0; //logical pointers
Uint32 buffer2OutIdx = 0;
Int32 buffer2Inside = 0;

Uint16 in_record = 0;



void circular_buffer_put(Int16 item){
	buffer[bufferInIdx] =  (item & 0xFF);
	bufferInIdx = ((bufferInIdx+1) % b_size);
	if(bufferInIdx == 0) // switch buffer
		buffer = buffer==circular_buffer?circular_buffer2:circular_buffer;
	buffer[bufferInIdx] =  ((item >> 8) & 0xFF);
	bufferInIdx = ((bufferInIdx+1) % b_size);
	if(bufferInIdx == 0) // switch buffer
		buffer = buffer==circular_buffer?circular_buffer2:circular_buffer;
	bufferInside++;
}

Int16 circular_buffer_get(){

	Int16 item = 0;
	item = circular_buffer[bufferOutIdx];
	bufferOutIdx = ((bufferOutIdx + 1) % b_size);
	item += (circular_buffer[bufferOutIdx] << 8);
	bufferOutIdx = ((bufferOutIdx + 1) % b_size);
	/*item = circular_buffer[bufferOutIdx];
	bufferOutIdx = ((bufferOutIdx + 1) % b_size); */
	bufferInside--;
	return item;
}
