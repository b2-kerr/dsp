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

/*
 *  ======== Declarations ========
 */
//#define BUFFSIZE 2000

/*
 *  ======== Prototypes ========
 */

void initHwi(void);


/*
 *  ======== Global Variables ========
 */
short gBufXmtL[BUFFSIZE];
short gBufXmtR[BUFFSIZE];
short gBufRcvL[BUFFSIZE];
short gBufRcvR[BUFFSIZE];

SINE_Obj sineObjL;
SINE_Obj sineObjR;

/*
 *  ======== main ========
 */
void main()
{
	int i;

	CSL_init();

    SINE_init(&sineObjL, 200, 48 * 1000);
	SINE_init(&sineObjR, 200, 48 * 1000);

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

  //  while (1) {						// Loop Forever
  //  }
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


