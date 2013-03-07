/*
 *  ======== edma.c ========
 *  This is the code needed to setup the edma.
 */

/**************************************************************\
*  The "EDMA Config" type data structure holds the
*    parameters to be programmed into a EDMA channel.
*  Register Make (RMK) macros build a 32-bit unsigned int;
*    below it is used to build the Options (OPT) register.
*  The OF macros provide the proper typecasting needed for
*    the EDMA Config data structure.
*
* To locate the structure below, use:
*
* Help-->User Manuals-->
* SPRU401 - TMS320C6000 Chip Support Library API Reference Guide
*
* 1. Open the SPRU401 .pdf file.
* 2. Search for "EDMA_OPT_field_symval" and go to the link
* 
* Notice that Table B-23 specifies how to build the 
* OPT RMK structure. If you want to know the options for 
* each field, just look at the table.
* 
* You can locate the other Config fields by searching for:
*
*   "EDMA_SRC_field_symval"
*
* Use this as a template for individual structures:
*
EDMA_Config variableName = {  
    EDMA_OPT_RMK(
        EDMA_OPT_PRI_???,		// Priority
        EDMA_OPT_ESIZE_???,		// Element size
        EDMA_OPT_2DS_???,		// 2 dimensional source
        EDMA_OPT_SUM_???,		// Src update mode
        EDMA_OPT_2DD_???,		// 2 dimensional dest
        EDMA_OPT_DUM_???,		// Dest update mode
        EDMA_OPT_TCINT_???,		// Cause EDMA interrupt
        EDMA_OPT_TCC_OF(???),	// Transfer Complete Code
        EDMA_OPT_TCCM_???,		// Transfer Complete Code Upper Bits (c64x only)
        EDMA_OPT_ATCINT_???,	// Alternate TCC Interrupt (c64x only)
        EDMA_OPT_ATCC_???,		// Alternate Transfer Complete Code (c64x only)
        EDMA_OPT_PDTS_???,		// Peripheral Device Transfer Source (c64x only)
        EDMA_OPT_PDTD_???,		// Peripheral Device Transfer Dest (c64x only)
        EDMA_OPT_LINK_???,		// Enable link parameters
        EDMA_OPT_FS_???			// Use frame sync
    ),
    EDMA_SRC_OF(???),			// src address
    EDMA_CNT_OF(???),			// Count = buffer size 
    EDMA_DST_OF(???),			// dest address
    EDMA_IDX_OF(???),			// frame/element index value
    EDMA_RLD_OF(???)			// reload
};
\**************************************************************/

/*
 *  ======== Include files ========
 */
#include <csl.h>
#include <csl_edma.h>
#include "sine.h"
#include "mcbsp.h"
#include "dsp_cw.h"
#include "convolve.h"
#include "log.h"

/*
 *  ======== Declarations ========
 */

/*
 *  ======== Prototypes ========
 */
void initEdma(void);
void edmaHwi(int tcc);

/*
 *  ======== References ========
 */



extern SINE_Obj sineObjL;
extern SINE_Obj sineObjR;

/*
 *  ======== Global Variables ========
 */

static short* currentAddr = 0;
static int counter = 0;

//short xfrId;

EDMA_Handle	hEdmaXmt;
EDMA_Handle hEdmaReloadXmt;

EDMA_Handle hEdmaRcv;
EDMA_Handle hEdmaReloadRcv[RX_BUFFER_CHANNEL_CHUNKS];
short TCCId[RX_BUFFER_CHANNEL_CHUNKS];


/*
 *
 * SETUP RECEIVE EDMA
 */
EDMA_Config gEdmaConfigRcv = {  
    EDMA_OPT_RMK(
        EDMA_OPT_PRI_LOW,		// Priority
        EDMA_OPT_ESIZE_16BIT,	// Element size
        EDMA_OPT_2DS_NO,		// 2 dimensional source
        EDMA_OPT_SUM_NONE,		// Src update mode
        EDMA_OPT_2DD_NO,		// 2 dimensional dest
        EDMA_OPT_DUM_IDX,		// Dest update mode
        EDMA_OPT_TCINT_YES,		// Cause EDMA interrupt
        EDMA_OPT_TCC_OF(0),		// Transfer Complete Code
//        EDMA_OPT_TCCM_DEFAULT,	// Transfer Complete Code Upper Bits (c64x only)
//        EDMA_OPT_ATCINT_DEFAULT,	// Alternate TCC Interrupt (c64x only)
//        EDMA_OPT_ATCC_DEFAULT,		// Alternate Transfer Complete Code (c64x only)
//        EDMA_OPT_PDTS_DEFAULT,		// Peripheral Device Transfer Source (c64x only)
//        EDMA_OPT_PDTD_DEFAULT,		// Peripheral Device Transfer Dest (c64x only)
        EDMA_OPT_LINK_YES,		// Enable link parameters
        EDMA_OPT_FS_NO			// Use frame sync
    ),
    EDMA_SRC_OF(0),				// src address
    EDMA_CNT_RMK(
    	EDMA_CNT_FRMCNT_OF(RX_BUFFER_CHANNEL_CHUNKS_SAMPLES-1),
    	EDMA_CNT_ELECNT_OF(2)
    ), 
    EDMA_DST_OF(sRxBuffer),		// dest address
    EDMA_IDX_RMK(
    	EDMA_IDX_FRMIDX_OF(-RX_BUFFER_CHANNEL_BYTES + 2),
    	EDMA_IDX_ELEIDX_OF(RX_BUFFER_CHANNEL_BYTES)
    ),
    EDMA_RLD_RMK(
		EDMA_RLD_ELERLD_OF(2),
		EDMA_RLD_LINK_OF(0x0)
	)
};


#if 0

/*
 *
 *
 * SETUP TRANSMIT EDMA
 */
EDMA_Config gEdmaConfigXmt = {  
    EDMA_OPT_RMK(
        EDMA_OPT_PRI_LOW,		// Priority
        EDMA_OPT_ESIZE_16BIT,	// Element size
        EDMA_OPT_2DS_NO,		// 2 dimensional source
        EDMA_OPT_SUM_IDX,		// Src update mode
        EDMA_OPT_2DD_NO,		// 2 dimensional dest
        EDMA_OPT_DUM_NONE,		// Dest update mode
        EDMA_OPT_TCINT_YES,		// Cause EDMA interrupt
        EDMA_OPT_TCC_OF(0),		// Transfer Complete Code
//        EDMA_OPT_TCCM_DEFAULT,	// Transfer Complete Code Upper Bits (c64x only)
//        EDMA_OPT_ATCINT_DEFAULT,	// Alternate TCC Interrupt (c64x only)
//        EDMA_OPT_ATCC_DEFAULT,		// Alternate Transfer Complete Code (c64x only)
//        EDMA_OPT_PDTS_DEFAULT,		// Peripheral Device Transfer Source (c64x only)
//        EDMA_OPT_PDTD_DEFAULT,		// Peripheral Device Transfer Dest (c64x only)
        EDMA_OPT_LINK_YES,		// Enable link parameters
        EDMA_OPT_FS_NO			// Use frame sync
    ),
    EDMA_SRC_OF(gBufXmtL),				// src address
    EDMA_CNT_RMK(
    	EDMA_CNT_FRMCNT_OF(BUFFSIZE-1),
    	EDMA_CNT_ELECNT_OF(2)
    ), 
    EDMA_DST_OF(0),						// dest address
    EDMA_IDX_RMK(
    	EDMA_IDX_FRMIDX_OF(-BUFFSIZE*2 + 2),
    	EDMA_IDX_ELEIDX_OF(BUFFSIZE*2)
    ),
    EDMA_RLD_RMK(
		EDMA_RLD_ELERLD_OF(2),
		EDMA_RLD_LINK_OF(0x0)
	)};


#endif





/*
 *	======== initEdma ========
 */
void initEdma(void)
{
	short gXmtTCC;
	short gRcvTCC;
	int n;
	short* sAddr;

	LOG_printf(&LOG1,"Setting up EDMA....");

	/* Get handle to channel with MCBSP receive event on it */
	hEdmaRcv = EDMA_open(EDMA_CHA_REVT1, EDMA_OPEN_RESET);

	gRcvTCC = EDMA_intAlloc(-1);
	gEdmaConfigRcv.opt |= EDMA_FMK(OPT, TCC, gRcvTCC);

	TCCId[0] = gRcvTCC;

	/* Here the McBSP address is entered */
	gEdmaConfigRcv.src = MCBSP_getRcvAddr( hMcbspData );

	/* Set the destination address */
	gEdmaConfigRcv.dst = EDMA_DST_OF(sRxBuffer);

	/* Create RUNTIME handles based on the above configs */
	EDMA_config(hEdmaRcv, &gEdmaConfigRcv);

	hEdmaReloadRcv[0] = EDMA_allocTable(-1);
	EDMA_config(hEdmaReloadRcv[0],&gEdmaConfigRcv);


	sAddr = sRxBuffer;

	LOG_printf(&LOG1, "First addr: %X", sAddr);

	/* Now sort out all the reloads and linking */
	for(n=1;n<RX_BUFFER_CHANNEL_CHUNKS;n++){

		/* Setup reload for incoming channel */
		hEdmaReloadRcv[n] = EDMA_allocTable(-1);

		/* Jump one channel's worth of data */
		sAddr += RX_BUFFER_CHANNEL_CHUNKS_SAMPLES;

		LOG_printf(&LOG1, "[%d] addr: %X", n, sAddr);

		/* Set the destination address */
		gEdmaConfigRcv.dst = EDMA_DST_OF(sAddr);

		/* Get a new code for this */
		gRcvTCC = EDMA_intAlloc(-1);

		/* Store it */
		TCCId[n] = gRcvTCC;

		gEdmaConfigRcv.opt |= EDMA_FMK(OPT, TCC, gRcvTCC);

		/* Set This In */
		EDMA_config(hEdmaReloadRcv[n], &gEdmaConfigRcv);

		/* Link previous one to this one */
		EDMA_link(hEdmaReloadRcv[n-1],hEdmaReloadRcv[n]);

	}


	/* Link the initial one to the second of the reloads */
	EDMA_link(hEdmaRcv,hEdmaReloadRcv[1]);

	/* Link final one to first one */
	EDMA_link(hEdmaReloadRcv[RX_BUFFER_CHANNEL_CHUNKS-1], hEdmaReloadRcv[0]);


#if 0


	/* Setup reload for outgoing channel */

	hEdmaXmt = EDMA_open(EDMA_CHA_XEVT1, EDMA_OPEN_RESET);
	hEdmaReloadXmt = EDMA_allocTable(-1);

	gXmtTCC = EDMA_intAlloc(-1);
	gEdmaConfigXmt.opt |= EDMA_FMK(OPT, TCC, gXmtTCC);

	gEdmaConfigXmt.dst = MCBSP_getXmtAddr( hMcbspData );

	EDMA_config(hEdmaXmt, &gEdmaConfigXmt);
	EDMA_config(hEdmaReloadXmt, &gEdmaConfigXmt);

	EDMA_link(hEdmaXmt, hEdmaReloadXmt);
	EDMA_link(hEdmaReloadXmt, hEdmaReloadXmt);

#endif



	/* This is the funciton called by the EDMA_intDispatcher function which is the
	 * acutal entry point for the interrupt
	 */
	for (n=0;n<RX_BUFFER_CHANNEL_CHUNKS;n++){
		EDMA_intHook(TCCId[n], edmaHwi);
		EDMA_intClear(TCCId[n]);
		EDMA_intEnable(TCCId[n]);
	}

	EDMA_enableChannel(hEdmaRcv);

	return;
}



int getCount(void){


	return counter;
}

/*
 *	======== edmaHwi ========
 */
void edmaHwi(int tcc)
{


	counter ++;

}


