MEMORY
{  
  CROM	  (RX) : origin = 0xfe0000, length = 0x000600  /*  1536B */  
}
SECTIONS
{
    .text   : > SARAM
    .switch : > DARAM
    .cinit  : > DARAM
    .pinit  : > DARAM
    .printf : > SARAM
    .const  : > SARAM
    .data   : > SARAM
    .cio    : > DARAM

    .bss    : > SARAM
    .my_i2sRxLeftBuf    : block(0x10000)    {} > DARAM
    .pbAsrcInFifo       : > DARAM
    .recAsrcInFifo      : > DARAM
    .pbAsrcHbCircBuf    : > DARAM
    .recAsrcHbCircBuf   : > DARAM
    .data_buf			: > DARAM
    .scratch_buf		: > DARAM
    .data_br_buf 		: > DARAM
    .uart_debugBuffer	: > DARAM
    .circular_buffer	: > SARAM
    .spec_buffer		: > DARAM



     cmplxBuf  : > DARAM
	 BufR      : > DARAM
	 PSD	   : > DARAM
	 tmpBuf	   : > DARAM
	 brBuf	   : > DARAM
	 rfftR     : > DARAM
	 ifftR     : > DARAM
    .charrom    : >  CROM
}

/* C5535 HWAFFT function addresses */
/* C5535 HWAFFT ROM table addresses */
_hwafft_br       = 0x00fefe9c;
_hwafft_8pts     = 0x00fefeb0;
_hwafft_16pts    = 0x00feff9f;
_hwafft_32pts    = 0x00ff00f5;
_hwafft_64pts    = 0x00ff03fe;
_hwafft_128pts   = 0x00ff0593;
_hwafft_256pts   = 0x00ff07a4;
_hwafft_512pts   = 0x00ff09a2;
_hwafft_1024pts  = 0x00ff0c1c;

