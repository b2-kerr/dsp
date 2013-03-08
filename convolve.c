/*
 * convolve.c
 *
 *  Created on: 6 Mar 2013
 *      Author: Will
 */

#include "dsp2cfg.h"
#include <stdio.h>

#define BUFFER_TYPE_SIZE (sizeof(short))
#define BUFFER_COUNT (32)
#define BUFFER_SIZE_BYTES (BUFFER_COUNT*BUFFER_TYPE_SIZE)



/* The assembly functions */
extern float fira(float *, float *, int, int, int);
extern float conv( short *, float *, int, int, int);
extern float asmTest(int);


#if 0
; usage: w[q] = x; read input sample
; y = fira(w,h,Lh,Nb,q); compute output sample
; q--; if (q==-1) q = Lw-1; update circular index by backshifting
;
; M = filter order
; Lh = M+1 = filter length
; Nb >= 1 + ceil(log2(Lh)) = circular buffer bytes-length exponent
; Lb = 2^(Nb+1) = circular buffer length in bytes
; Lw = Lb/4 = 2^(Nb-1) = circular buffer in 32-bit words
#endif

short w[] ={
		1,2,3,4,5,6,7,8,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1
};
#pragma DATA_ALIGN(w, BUFFER_SIZE_BYTES)


short out[100];

#if 0
float h[FILTER_LENGTH_WORDS] = {
		1,1,1,1,1,1,1,1,1,1,1
};

int convolve(void){

	int n;
	short sTest;
	float ret;
	int nRet;

	LOG_printf(&LOG1,"Entered Convolve...\n");

#if 1
	for(n=00;n<31;n++){

		ret = conv(w,h,
				FILTER_LENGTH_WORDS,
				BLOCK_SIZE_64,
				n);

		out[n] = (short)ret;
		//LOG_printf(&LOG1,"Output [%d]",n);
		//LOG_printf(&LOG1," is: [%d].\n", nRet );
		//TSK_sleep(1000);
	}
#endif



	//sTest = -45;
	//ret = asmTest(sTest);



	LOG_printf(&LOG1, "Finished...\n");

	while(1){

		LOG_printf(&LOG1, "Tick...\n");
		TSK_sleep(1000);
	}

	return 0;

}




int do_convolve(void*incoming,void*outgoing, int samples){

	int n;
	short*out;

	out = (short*)outgoing;

	for(n=0;n<samples;n++){

		*out = (short)conv(incoming,h,FILTER_LENGTH_WORDS,6,n);
		out++;

	}

	return 0;
}
#endif
