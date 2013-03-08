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
#include <mbx.h>

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

/*
 *
 * SETUP RECEIVE EDMA
 */
EDMA_Config gEdmaConfigRcv = {  
    EDMA_OPT_RMK(
        EDMA_OPT_PRI_HIGH,		// Priority
        EDMA_OPT_ESIZE_16BIT,	// Element size
        EDMA_OPT_2DS_NO,		// 2 dimensional source
        EDMA_OPT_SUM_NONE,		// Src update mode
        EDMA_OPT_2DD_NO,		// 2 dimensional dest
        EDMA_OPT_DUM_IDX,		// Dest update mode
        EDMA_OPT_TCINT_YES,		// Cause EDMA interrupt
        EDMA_OPT_TCC_OF(0),		// Transfer Complete Code
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

	short gRcvTCC;
	int n = 0;
	short* sAddr;
	EDMA_Config tempConfig;
	Uint32 temp32;

	LOG_printf(&LOG1,"Setting up RX EDMA....");

	sAddr = sTxBuffer;

	/* Pre-allocate a bunch of tables */
	if(EDMA_allocTableEx(RX_BUFFER_CHANNEL_CHUNKS,hEdmaReloadXmt)!=RX_BUFFER_CHANNEL_CHUNKS){
		LOG_printf(&LOG1,"ERROR: Cannot allocate Reload PRAM.\n");
		return -1;
	}

	/* Here the McBSP address is entered */
	gEdmaConfigXmt.dst = MCBSP_getXmtAddr( hMcbspData );

	/* Sort out all the reloads and linking */
	for(n=0;n<RX_BUFFER_CHANNEL_CHUNKS;n++){

		/* Setup reload for incoming channel */
		//hEdmaReloadXmt[n] = EDMA_allocTable(n+5);

		/* Set the destination address */
		gEdmaConfigXmt.src = EDMA_SRC_OF(sAddr);

		/* Get a new Transfer Complete code for this */
		temp32 = EDMA_intAlloc(-1);

		gRcvTCC = temp32;

		TXchunksData[gRcvTCC].TCCID = gRcvTCC;
		TXchunksData[gRcvTCC].LAddr = sAddr;
		TXchunksData[gRcvTCC].RAddr = sAddr+RX_BUFFER_CHANNEL_SAMPLES;

		/* Now set the transfer complete code */
		gEdmaConfigXmt.opt &= 0xFFF0FFFF;
		temp32 &= 0xF;
		gEdmaConfigXmt.opt |= temp32<<16;

		LOG_printf(&LOG1, "Chunk [%d] is \n",n);
		LOG_printf(&LOG1, "TCC[%d] addr[%X]\n", gRcvTCC, (unsigned int)sAddr);

		/* Set This In */
		EDMA_config(hEdmaReloadXmt[n], &gEdmaConfigXmt);

		if(n>0){
			/* Link previous one to this one */
			EDMA_link(hEdmaReloadXmt[n-1],hEdmaReloadXmt[n]);
		}

		//EDMA_intClear(gRcvTCC);
		EDMA_intEnable(gRcvTCC);

		EDMA_intHook(gRcvTCC, edmaHwiTX);

		/* Jump one channel's worth of data */
		sAddr += RX_BUFFER_CHANNEL_CHUNKS_SAMPLES;

	}

	/* Link final one to first one */
	EDMA_link(hEdmaReloadXmt[RX_BUFFER_CHANNEL_CHUNKS-1], hEdmaReloadXmt[0]);

	/* Get handle to channel with MCBSP receive event on it */
	hEdmaXmt = EDMA_open(EDMA_CHA_XEVT1, EDMA_OPEN_ENABLE);

	/* Now set this new channel to have the same config as the reload 0 */
	EDMA_getConfig(hEdmaReloadXmt[0],&tempConfig);
	EDMA_config(hEdmaXmt,&tempConfig);


	for(n=0;n<RX_BUFFER_CHANNEL_CHUNKS;n++){

		EDMA_getConfig(hEdmaReloadXmt[n],&tempConfig);
		LOG_printf(&LOG1, "TX Reload [%d] at Addr [%X].\n",n,tempConfig.rld);

	}

	EDMA_getConfig(hEdmaXmt,&tempConfig);
	LOG_printf(&LOG1, "TX Reload Init at Addr [%X].\n",tempConfig.rld);

	EDMA_enableChannel(hEdmaXmt);

	return 0;
}


int edmaSetupRX(void){

	short gRcvTCC;
	int n = 0;
	short* sAddr;
	EDMA_Config tempConfig;
	Uint32 temp32;

	LOG_printf(&LOG1,"Setting up RX EDMA....");

	sAddr = sRxBuffer;


	/* Pre-allocate a bunch of tables */
	if(EDMA_allocTableEx(RX_BUFFER_CHANNEL_CHUNKS,hEdmaReloadRcv)!=RX_BUFFER_CHANNEL_CHUNKS){
		LOG_printf(&LOG1,"ERROR: Cannot allocate Reload PRAM.\n");
		return -1;
	}

	/* Here the McBSP address is entered */
	gEdmaConfigRcv.src = MCBSP_getRcvAddr( hMcbspData );



	/* Sort out all the reloads and linking */
	for(n=0;n<RX_BUFFER_CHANNEL_CHUNKS;n++){

		/* Setup reload for incoming channel */
		//hEdmaReloadRcv[n] = EDMA_allocTable(n+5);

		/* Set the destination address */
		gEdmaConfigRcv.dst = EDMA_DST_OF(sAddr);

		/* Get a new Transfer Complete code for this */
		temp32 = EDMA_intAlloc(-1);

		gRcvTCC = temp32;

		RXchunksData[gRcvTCC].TCCID = gRcvTCC;
		RXchunksData[gRcvTCC].LAddr = sAddr;
		RXchunksData[gRcvTCC].RAddr = sAddr+RX_BUFFER_CHANNEL_SAMPLES;

		/* Now set the transfer complete code */
		gEdmaConfigRcv.opt &= 0xFFF0FFFF;
		temp32 &= 0xF;
		gEdmaConfigRcv.opt |= temp32<<16;

		//gEdmaConfigRcv.opt |= EDMA_FMK(OPT, TCC, gRcvTCC);

		LOG_printf(&LOG1, "Chunk [%d] is \n",n);
		LOG_printf(&LOG1, "TCC[%d] addr[%X]\n", gRcvTCC, (unsigned int)sAddr);

		/* Set This In */
		EDMA_config(hEdmaReloadRcv[n], &gEdmaConfigRcv);

		if(n>0){
			/* Link previous one to this one */
			EDMA_link(hEdmaReloadRcv[n-1],hEdmaReloadRcv[n]);
		}

		//EDMA_intClear(gRcvTCC);
		EDMA_intEnable(gRcvTCC);

		EDMA_intHook(gRcvTCC, edmaHwi);

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


	for(n=0;n<RX_BUFFER_CHANNEL_CHUNKS;n++){

		EDMA_getConfig(hEdmaReloadRcv[n],&tempConfig);
		LOG_printf(&LOG1, "Reload [%d] at Addr [%X].\n",n,tempConfig.rld);

	}

	EDMA_getConfig(hEdmaRcv,&tempConfig);
	LOG_printf(&LOG1, "Reload Init at Addr [%X].\n",tempConfig.rld);



	return 0;
}




/*
 *	======== initEdma ========
 */
void initEdma(void)
{

	EDMA_resetAll();
	EDMA_intResetAll();
	//EDMA_clearPram(0);

	/* Setup receive stream */
	edmaSetupRX();

	/* Setup transmit stream */
	edmaSetupTX();


	EDMA_enableChannel(hEdmaRcv);
	EDMA_enableChannel(hEdmaXmt);


	LOG_printf(&LOG1, "Receive channel says: %d.\n",EDMA_getChannel(hEdmaRcv));


}



int getRXCount(void){

	return counterRX;
}

int getTXCount(void){

	return counterTX;
}






void swi_go(void){

	static int in=0,out=0;
	int n;
	static short* outmemL,*inmemL;
	static short* outmemR,*inmemR;

	if(FALSE==MBX_pend(&MBX0,&inmemL,0)){
		in++;
		return;
	}

	if(FALSE==MBX_pend(&MBX1,&outmemL,0)){
		out++;
		return;
	}

	inmemR = inmemL + RX_BUFFER_CHANNEL_SAMPLES;
	outmemR = outmemL + RX_BUFFER_CHANNEL_SAMPLES;

	for (n=0;n<RX_BUFFER_CHANNEL_CHUNKS_SAMPLES;n++){
		*outmemL = *inmemL;
		*outmemR = *inmemR;

		outmemL++;inmemL++;
		outmemR++;inmemR++;
	}


}


/*
 *	======== edmaHwi ========
 */
void edmaHwi(int tcc)
{

	static int a[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	static int error=0;

	a[tcc]++;

	counterRX ++;

	/* Buffer of data is ready, post this in */

	if(FALSE==MBX_post(&MBX0,&RXchunksData[tcc].LAddr,0)){
		error++;
	}

	/* Schedule SWI */
	//SWI_andn(&SWI0,0x1);

	//currentAddr = RXchunksData[tcc].LAddr;

	/* Disable events? */

	return;
}


/*
 *	======== edmaHwi ========
 */
void edmaHwiTX(int tcc)
{

	int n;
	short*inmem;
	short*outmemL;
	short*outmemR;

	static int error=0;

	static int a[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	a[tcc]++;

	counterTX++;

	//inmem = sRxBuffer;

	/* Put this buffer in the outgoing mbox */
	/* TODO: Test for failure. */
	if(FALSE==MBX_post(&MBX1,&TXchunksData[tcc].LAddr,0)){
				error++;
	}

#if 0
	if(MBX_pend(&MBX0,&inmem,0) == FALSE ){
		/* NO Buffer available is BAD */
		return;
	}
#endif

	/* Post SWI saying there's a free output buffer */
	//SWI_andn(&SWI0,0x2);

#if 0
	//inmem = currentAddr;
	outmemL = TXchunksData[tcc].LAddr;
	outmemR = TXchunksData[tcc].RAddr;

	for (n=0;n<RX_BUFFER_CHANNEL_CHUNKS_SAMPLES;n++){
		*outmemL = *inmem;
		*outmemR = *(inmem+RX_BUFFER_CHANNEL_SAMPLES);
		outmemL++;outmemR++;inmem++;
	}
#endif


	return;
}


