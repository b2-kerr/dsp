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

#include "c62.h"
#include "dsp_cw.h"

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

short sRxBuffer[RX_BUFFER_SAMPLES] = {0};
#pragma DATA_ALIGN(sRxBuffer, RX_BUFFER_BYTES)

/* These two point to the parts of the total buffer which represent the two channels */
short* gBufRcvL;
short* gBufRcvR;

//#pragma DATA_ALIGN(gBufXmtL, BUFFSIZE*2)


SINE_Obj sineObjL;
SINE_Obj sineObjR;

/*
 *  ======== main ========
 */
void main()
{


    //SINE_init(&sineObjL, 200, 48 * 1000);
	//SINE_init(&sineObjR, 200, 48 * 1000);


}



/*
 *  ======== initHwi =========
 */

void initHwi(void)
{
	//IRQ_enable(IRQ_EVT_EDMAINT);

	C62_enableIER(C62_EINT8);

	IRQ_globalEnable();
}


/* Test the circular buffer stuff */
int dspmain(void){

	LOG_printf(&LOG1,"Starting.....\n");


	int i;

	gBufRcvL = (short*)&sRxBuffer;
	gBufRcvR = (short*)&sRxBuffer + RX_BUFFER_CHANNEL_SAMPLES;

	for ( i=0; i<RX_BUFFER_SAMPLES; i++)
	{
		sRxBuffer[i] = 0;
	}

	CSL_init();
	//TSK_sleep(2000);

	//convolve();

	initMcBSP();
	initEdma();
	initHwi();

	MCBSP_write( hMcbspData, 0 );


	while(1){
		TSK_sleep(2000);
		LOG_printf(&LOG1, "Tick...\n");
	}

	return 0;
}
