/*
 *  ======== main.c ========
 *  This file contains all the functions for Lab2 except
 *  SINE_init() and SINE_blockFill().
 */

/*
 *  ======== Include files ========
 */


#include <std.h>
#include <csl.h>
#include <csl_edma.h>
#include <csl_irq.h>
#include "sine.h"
#include "edma.h"
#include "mcbsp.h"
#include "dsk6713.h"
#include "dsk6713_led.h"
#include "filters.h"

#include "c62.h"
#include "dsp_cw.h"
#include <stdio.h>
#include "convolve.h"


/*
 *  ======== Declarations ========
 */

extern float conv( short *, float *, int, int, int);

/*
 *  ======== Prototypes ========
 */

void initHwi(void);
void test(void);


/*
 *  ======== Global Variables ========
 */

short sRxBuffer[RX_BUFFER_SAMPLES];
short sTxBuffer[RX_BUFFER_SAMPLES];
short sProcess[RX_BUFFER_CHANNEL_CHUNKS_BYTES];

#pragma DATA_ALIGN(sRxBuffer, RX_BUFFER_CHANNEL_BYTES)
#pragma DATA_ALIGN(sTxBuffer, RX_BUFFER_CHANNEL_BYTES)


chunksData_s RXchunksData[16];
chunksData_s TXchunksData[16];

#if 1
float H[FILTER_LENGTH_WORDS] = {
		0.090909,
		0.090909,
		0.090909,
		0.090909,
		0.090909,
		0.090909,
		0.090909,
		0.090909,
		0.090909,
		0.090909,
		0.090909
};


#else

float H[FILTER_LENGTH_WORDS] = {
		0,0,0,0,0,1,0,0,0,0,0
};

#endif



SINE_Obj sineObjL;
SINE_Obj sineObjR;

/*
 *  ======== main ========
 */
void main()
{

	int i;

	CSL_init();
	DSK6713_init();

	LOG_printf(&LOG1,"Main starting.....\n");

	for ( i=0; i<RX_BUFFER_SAMPLES; i++)
	{
		sRxBuffer[i] = 0;
		sTxBuffer[i] = 0;
	}

#if 1
	initMcBSP();

	LOG_printf(&LOG1, "Init EDMA...\n");
	initEdma();

#endif



}



/*
 *  ======== initHwi =========
 */

void initHwi(void)
{
	/* Int8 is the EDMA interrrupt to the CPU */
	C62_enableIER(C62_EINT8);
	//C62_enableIER(C62_EINT4);

	/* Turns on Global Interrupts */
	IRQ_globalEnable();
}


void audiotsk (void){

	short*inputL,*outputL;
	short*inputR,*outputR;
	int n;
	float ret;
	const float*currentFilter;
	int currentFilterLength;

	while(1){

	/* Wait for input buffer */
	MBX_pend(&MBX0,&inputL,SYS_FOREVER);

	/* Wait for output buffer */
	MBX_pend(&MBX1,&outputL,SYS_FOREVER);

	inputR = inputL + RX_BUFFER_CHANNEL_SAMPLES;
	outputR = outputL + RX_BUFFER_CHANNEL_SAMPLES;

	currentFilter = &flt_BandPass[0];
	currentFilterLength = 101;

	for (n=0;n<RX_BUFFER_CHANNEL_CHUNKS_SAMPLES;n++){


#if 1
		//C62_disableIER(C62_EINT8);

		ret = conv(
				inputL,
				(float*)currentFilter,
				currentFilterLength,
				BLOCK_SIZE_4096,
				n);

		//C62_enableIER(C62_EINT8);


		*outputL = (short)(ret*1.2);


		ret = conv(
				inputR,
				(float*)currentFilter,
				currentFilterLength,
				BLOCK_SIZE_4096,
				n);

		*outputR = (short)(ret*1.2);


#else
		*outputL = *inputR;
		*outputR = *inputR;

		inputR++;
		inputL++;


#endif

		outputL++;
		outputR++;



	}

	}


}


/* Test the circular buffer stuff */
int dspmain(void){

	LOG_printf(&LOG1,"DSP MAIN Starting.....\n");

	int i;
	int led=0;
	int ledd=1;


	DSK6713_LED_init();

#if 1
	initHwi();

	startMcBSP();
#endif


	while(1){

		LOG_printf(&LOG1, "Count is at RX:TX [%d:%d]",getRXCount(), getTXCount());

		DSK6713_LED_off(led);

		led+=ledd;

		if(led==3)
			ledd*=-1;

		if(led==0)
			ledd*=-1;

		DSK6713_LED_on(led);

		TSK_sleep(2000);
	}

	return 0;
}
