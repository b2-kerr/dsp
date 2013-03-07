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
#define CHIP_6713 1

#include <csl.h>
#include <csl_edma.h>
#include "sine.h"
#include "mcbsp.h"
#include "dsp_cw.h"
#include "convolve.h"
#include "log.h"
#include "edma.h"
#include "que.h"
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

static int counterTX = 0;
static int counterRX = 0;

static short* currentAddr;



//short xfrId;

EDMA_Handle	hEdmaXmt;
EDMA_Handle hEdmaReloadXmt[RX_BUFFER_CHANNEL_CHUNKS];

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
    EDMA_SRC_OF(sTxBuffer),				// src address
    EDMA_CNT_RMK(
    	EDMA_CNT_FRMCNT_OF(RX_BUFFER_CHANNEL_CHUNKS_SAMPLES-1),
    	EDMA_CNT_ELECNT_OF(2)
    ), 
    EDMA_DST_OF(0),						// dest address
    EDMA_IDX_RMK(
    	EDMA_IDX_FRMIDX_OF(-RX_BUFFER_CHANNEL_BYTES + 2),
    	EDMA_IDX_ELEIDX_OF(RX_BUFFER_CHANNEL_BYTES)
    ),
    EDMA_RLD_RMK(
		EDMA_RLD_ELERLD_OF(2),
		EDMA_RLD_LINK_OF(0x0)
	)};



int edmaSetupTX(void){

	short txTCC;
	int n;
	short* sAddr;

	LOG_printf(&LOG1,"Setting up TX EDMA....");

		/* Get handle to channel with MCBSP receive event on it */
		hEdmaXmt = EDMA_open(EDMA_CHA_XEVT1, EDMA_OPEN_RESET);

		/* Get a tcc code */
		txTCC = EDMA_intAlloc(-1);
		gEdmaConfigXmt.opt |= EDMA_FMK(OPT, TCC, txTCC);

		TXchunksData[txTCC].TCCID = txTCC;

		/* Here the McBSP address is entered */
		gEdmaConfigXmt.dst = MCBSP_getXmtAddr( hMcbspData );

		/* Set the destination address */
		gEdmaConfigXmt.src = EDMA_SRC_OF(sTxBuffer);

		/* Create RUNTIME handles based on the above configs */
		EDMA_config(hEdmaXmt, &gEdmaConfigXmt);

		hEdmaReloadXmt[0] = EDMA_allocTable(-1);
		EDMA_config(hEdmaReloadXmt[0],&gEdmaConfigXmt);

		sAddr = sTxBuffer;

		TXchunksData[txTCC].LAddr = sAddr;
		TXchunksData[txTCC].RAddr = sAddr+RX_BUFFER_CHANNEL_SAMPLES;

		/* Now sort out all the reloads and linking */
		for(n=1;n<RX_BUFFER_CHANNEL_CHUNKS;n++){

			/* Setup reload for incoming channel */
			hEdmaReloadXmt[n] = EDMA_allocTable(-1);

			/* Jump one channel's worth of data */
			sAddr += RX_BUFFER_CHANNEL_CHUNKS_SAMPLES;

			//LOG_printf(&LOG1, "[%d] addr: %X", n, sAddr);

			/* Set the destination address */
			gEdmaConfigXmt.src = EDMA_SRC_OF(sAddr);

			/* Get a new code for this */
			txTCC = EDMA_intAlloc(-1);

			TXchunksData[txTCC].TCCID = txTCC;
			TXchunksData[txTCC].LAddr = sAddr;
			TXchunksData[txTCC].RAddr = sAddr+RX_BUFFER_CHANNEL_SAMPLES;

			gEdmaConfigXmt.opt |= EDMA_FMK(OPT, TCC, txTCC);

			/* Set This In */
			EDMA_config(hEdmaReloadXmt[n], &gEdmaConfigXmt);

			/* Link previous one to this one */
			EDMA_link(hEdmaReloadXmt[n-1],hEdmaReloadXmt[n]);

			EDMA_intHook(txTCC, edmaHwiTX);
			EDMA_intClear(txTCC);
			EDMA_intEnable(txTCC);

		}

		/* Link the initial one to the second of the reloads */
		EDMA_link(hEdmaXmt,hEdmaReloadXmt[1]);

		/* Link final one to first one */
		EDMA_link(hEdmaReloadXmt[RX_BUFFER_CHANNEL_CHUNKS-1], hEdmaReloadXmt[0]);

		EDMA_enableChannel(hEdmaXmt);

		return 0;
	}



int edmaSetupRX(void){

	short gRcvTCC;
	int n = 0;
	short* sAddr;
	EDMA_Config tempConfig;

	LOG_printf(&LOG1,"Setting up EDMA....");

	sAddr = sRxBuffer;

	/* Pre-allocate a bunch of tables */
	EDMA_allocTableEx(RX_BUFFER_CHANNEL_CHUNKS,hEdmaReloadRcv);

	/* Here the McBSP address is entered */
	gEdmaConfigRcv.src = MCBSP_getRcvAddr( hMcbspData );

	/* Sort out all the reloads and linking */
	for(n=0;n<RX_BUFFER_CHANNEL_CHUNKS;n++){

		/* Setup reload for incoming channel */
		//hEdmaReloadRcv[n] = EDMA_allocTable(-1);

		/* Set the destination address */
		gEdmaConfigRcv.dst = EDMA_DST_OF(sAddr);

		/* Get a new Transfer Complete code for this */
		gRcvTCC = EDMA_intAlloc(-1);

		RXchunksData[gRcvTCC].TCCID = gRcvTCC;
		RXchunksData[gRcvTCC].LAddr = sAddr;
		RXchunksData[gRcvTCC].RAddr = sAddr+RX_BUFFER_CHANNEL_SAMPLES;

		/* Now set the transfer complete code */
		gEdmaConfigRcv.opt |= EDMA_FMK(OPT, TCC, gRcvTCC);

		LOG_printf(&LOG1, "Chunk [%d] is \n",n);
		LOG_printf(&LOG1, "TCC[%d] addr[%X]\n", gRcvTCC, (unsigned int)sAddr);

		/* Set This In */
		EDMA_config(hEdmaReloadRcv[n], &gEdmaConfigRcv);

		if(n>0){
			/* Link previous one to this one */
			EDMA_link(hEdmaReloadRcv[n-1],hEdmaReloadRcv[n]);
		}

		EDMA_intHook(gRcvTCC, edmaHwi);
		EDMA_intClear(gRcvTCC);
		EDMA_intEnable(gRcvTCC);

		/* Jump one channel's worth of data */
		sAddr += RX_BUFFER_CHANNEL_CHUNKS_SAMPLES;

	}


	/* Link final one to first one */
	EDMA_link(hEdmaReloadRcv[RX_BUFFER_CHANNEL_CHUNKS-1], hEdmaReloadRcv[0]);

	/* Get handle to channel with MCBSP receive event on it */
	hEdmaRcv = EDMA_open(EDMA_CHA_REVT1, EDMA_OPEN_RESET);

	/* Now set this new channel to have the same config as the reload 0 */
	EDMA_getConfig(hEdmaReloadRcv[0],&tempConfig);
	EDMA_config(hEdmaRcv,&tempConfig);

#if 0
	gRcvTCC = EDMA_intAlloc(-1);
	gEdmaConfigRcv.opt |= EDMA_FMK(OPT, TCC, gRcvTCC);

	EDMA_intHook(gRcvTCC, edmaHwi);
	EDMA_intClear(gRcvTCC);
	EDMA_intEnable(gRcvTCC);

	RXchunksData[gRcvTCC].TCCID = gRcvTCC;



	/* Set the destination address */
	gEdmaConfigRcv.dst = EDMA_DST_OF(sRxBuffer);

	/* Create RUNTIME handles based on the above configs */
	EDMA_config(hEdmaRcv, &gEdmaConfigRcv);

	hEdmaReloadRcv[0] = EDMA_allocTable(-1);
	EDMA_config(hEdmaReloadRcv[0],&gEdmaConfigRcv);


	RXchunksData[gRcvTCC].LAddr = sAddr;
	RXchunksData[gRcvTCC].RAddr = sAddr+RX_BUFFER_CHANNEL_SAMPLES;

	LOG_printf(&LOG1, "Chunk [%d] is \n",n);
	LOG_printf(&LOG1, "TCC[%d] addr[%X]\n", gRcvTCC, (unsigned int)sAddr);

	/* Link the initial one to the second of the reloads */
	EDMA_link(hEdmaRcv,hEdmaReloadRcv[1]);


#endif

	EDMA_enableChannel(hEdmaRcv);

	return 0;
}




/*
 *	======== initEdma ========
 */
void initEdma(void)
{

	//QUE_new(&Q1);

	/* Setup receive stream */
	edmaSetupRX();

	/* Setup transmit stream */
	//edmaSetupTX();

}



int getRXCount(void){

	return counterRX;
}

int getTXCount(void){

	return counterTX;
}


/*
 *	======== edmaHwi ========
 */
void edmaHwi(int tcc)
{
	int n;
	int found = 0;
	short*addr;

	static int a[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	a[tcc]++;

	counterRX ++;

	addr = RXchunksData[tcc].LAddr;

	currentAddr = addr;

	for(n=0;n<RX_BUFFER_CHANNEL_CHUNKS_SAMPLES;n++){

		*addr = 0;
		addr++;

	}

	return;
}



/*
 *	======== edmaHwi ========
 */
void edmaHwiTX(int tcc)
{
	int n;
	int found = 0;
	short*addr;
	short*addr2;

	static int a[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	a[tcc]++;

	counterTX++;



	return;
}


