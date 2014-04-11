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
Int16 circular_buffer[PROCESS_BUFFER_SIZE];
// index for bufferIn
Uint32 bufferInIdx = 0; //logical pointers
Uint32 bufferOutIdx = 0;
Uint32 b_size = PROCESS_BUFFER_SIZE;
Int32 bufferInside = 0;
Uint16 in_record = 0;



void circular_buffer_put(Int16 item){
	 circular_buffer[bufferInIdx] =  item;
	 /*circular_buffer[bufferInIdx] =  (item & 0xFF);
	 bufferInIdx = ((bufferInIdx+1) % b_size);
	 circular_buffer[bufferInIdx] =  ((item >> 8) & 0xFF);
	 bufferInIdx = ((bufferInIdx+1) % b_size);*/
	 /*circular_buffer[bufferInIdx] = item ;
	 bufferInIdx = ((bufferInIdx+1) % b_size);*/
	 bufferInside++;
}

Int16 circular_buffer_get(){

	Int16 item = 0;
	item = circular_buffer[bufferOutIdx];
	/*bufferOutIdx = ((bufferOutIdx + 1) % b_size);
	item += (circular_buffer[bufferOutIdx] << 8);
	bufferOutIdx = ((bufferOutIdx + 1) % b_size);*/
	/*item = circular_buffer[bufferOutIdx];
	bufferOutIdx = ((bufferOutIdx + 1) % b_size); */
	bufferInside--;
	return item;
}

void init_buffer(){
	Uint32 i = 0;
	for(i = 0; i < PROCESS_BUFFER_SIZE; i++)
		circular_buffer[i] = i;
}
