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

#include "c62.h"
#include "dsp_cw.h"
#include <stdio.h>
#include "convolve.h"


/*
 *  ======== Declarations ========
 */
//#define BUFFSIZE 2000

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

chunksData_s RXchunksData[16];
chunksData_s TXchunksData[16];

//#pragma DATA_ALIGN(sRxBuffer, RX_BUFFER_BYTES)
//#pragma DATA_ALIGN(sTxBuffer, RX_BUFFER_BYTES)


SINE_Obj sineObjL;
SINE_Obj sineObjR;

/*
 *  ======== main ========
 */
void main()
{


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


/* Test the circular buffer stuff */
int dspmain(void){

	LOG_printf(&LOG1,"DSP MAIN Starting.....\n");

	int i;

	CSL_init();
	DSK6713_init();

	LOG_printf(&LOG1,"Main starting.....\n");

	for ( i=0; i<RX_BUFFER_SAMPLES; i++)
	{
		sRxBuffer[i] = 0;
		sTxBuffer[i] = 0;
	}

	initMcBSP();

	LOG_printf(&LOG1, "Init EDMA...\n");
	initEdma();

	LOG_printf(&LOG1, "Init HWI...\n");
	initHwi();

	startMcBSP();

	while(1){

		LOG_printf(&LOG1, "Count is at RX:TX [%d:%d]",getRXCount(), getTXCount());

		TSK_sleep(2000);
	}

	return 0;
}
