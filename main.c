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

short gBufRcvL[BUFFSIZE];
//#pragma DATA_ALIGN(gBufXmtL, BUFFSIZE*2)

short gBufRcvR[BUFFSIZE];
//#pragma DATA_ALIGN(gBufXmtL, BUFFSIZE*2)

short gBufXmtL[BUFFSIZE];
short gBufXmtR[BUFFSIZE];

SINE_Obj sineObjL;
SINE_Obj sineObjR;

/*
 *  ======== main ========
 */
void main()
{
	int i;

	CSL_init();

    //SINE_init(&sineObjL, 200, 48 * 1000);
	//SINE_init(&sineObjR, 200, 48 * 1000);

	initMcBSP();
	initEdma();
	initHwi();

	MCBSP_write( hMcbspData, 0 );

	for ( i=0; i<BUFFSIZE; i++)
	{
		gBufXmtL[i] = 0;
		gBufXmtR[i] = 0;
		gBufRcvL[i] = 0;
		gBufRcvR[i] = 0;
	}

}


/*
 *  ======== initHwi =========
 */

void initHwi(void)
{
	//IRQ_enable(IRQ_EVT_EDMAINT);

	//C62_enableIER(C62_EINT8);

	//IRQ_globalEnable();
}


/* Test the circular buffer stuff */
int dspmain(void){

	LOG_printf(&LOG1,"Starting.....\n");

	TSK_sleep(2000);

	convolve();

	return 0;
}
