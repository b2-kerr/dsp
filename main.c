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

#pragma DATA_ALIGN(sRxBuffer, RX_BUFFER_BYTES)
#pragma DATA_ALIGN(sTxBuffer, RX_BUFFER_BYTES)


/* These two point to the parts of the total buffer which represent the two channels */
short* sTX_L;
short* sTX_R;

short* sRX_L;
short* sRX_R;


//#pragma DATA_ALIGN(gBufXmtL, BUFFSIZE*2)


SINE_Obj sineObjL;
SINE_Obj sineObjR;

/*
 *  ======== main ========
 */
void main()
{

	LOG_printf(&LOG1,"Main starting.....\n");

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

	//printf("Hello there....");
	LOG_printf(&LOG1,"DSP MAIN Starting.....\n");

	int i;

	sRX_L = (short*)&sRxBuffer;
	sRX_R = (short*)&sRxBuffer + RX_BUFFER_CHANNEL_SAMPLES;

	for ( i=0; i<RX_BUFFER_SAMPLES; i++)
	{
		sRxBuffer[i] = 0;
	}

	CSL_init();

	//TSK_sleep(2000);

	//convolve();

#if 1
	LOG_printf(&LOG1, "Init McBSP...\n");
	initMcBSP();
	LOG_printf(&LOG1, "Init EDMA...\n");
	initEdma();
	LOG_printf(&LOG1, "Init HWI...\n");
	initHwi();
	LOG_printf(&LOG1, "Start McBSP...\n");
	MCBSP_write( hMcbspData, 0 );
#endif

	while(1){
		TSK_sleep(2000);
		LOG_printf(&LOG1, "Count is at RX:TX [%d:%d]",getRXCount(), getTXCount());
		//LOG_printf(&LOG1, "Tick...\n");
		//usleep(1000000);
		TSK_yield();
	}

	return 0;
}
