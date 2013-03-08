/*
 *  ======== mcbsp.c ========
 */

/**************************************************************\
*  The "MCBSP Config" type data structure holds the
*    parameters to be programmed into a McBSP.
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
* 2. Search for "MCBSP_SPCR_field_symval" and go to the link
* 
* Use this as a template for individual structures:
*
MCBSP_Config template = {
   MCBSP_SPCR_RMK(
		MCBSP_SPCR_FREE_,
        MCBSP_SPCR_SOFT_,
        MCBSP_SPCR_FRST_,
        MCBSP_SPCR_GRST_,
        MCBSP_SPCR_XINTM_,
        MCBSP_SPCR_XSYNCERR_,
        MCBSP_SPCR_XRST_,
        MCBSP_SPCR_DLB_,
        MCBSP_SPCR_RJUST_,
        MCBSP_SPCR_CLKSTP_,
        MCBSP_SPCR_DXENA_,
        MCBSP_SPCR_RINTM_,
        MCBSP_SPCR_RSYNCERR_,
        MCBSP_SPCR_RRST_
	),
	MCBSP_RCR_RMK(
        MCBSP_RCR_RPHASE_,
        MCBSP_RCR_RFRLEN2_,
        MCBSP_RCR_RWDLEN2_,
        MCBSP_RCR_RCOMPAND_,
        MCBSP_RCR_RFIG_,
        MCBSP_RCR_RDATDLY_,
        MCBSP_RCR_RFRLEN1_OF(),
        MCBSP_RCR_RWDLEN1_,
        MCBSP_RCR_RWDREVRS_
	),
	MCBSP_XCR_RMK(
        MCBSP_XCR_XPHASE_,
        MCBSP_XCR_XFRLEN2_,
        MCBSP_XCR_XWDLEN2_,
        MCBSP_XCR_XCOMPAND_,
        MCBSP_XCR_XFIG_,
        MCBSP_XCR_XDATDLY_,
        MCBSP_XCR_XFRLEN1_OF(),
        MCBSP_XCR_XWDLEN1_,
        MCBSP_XCR_XWDREVRS_
	),
    MCBSP_SRGR_DEFAULT,
	MCBSP_MCR_DEFAULT,
	MCBSP_RCERE0_DEFAULT,
	MCBSP_RCERE1_DEFAULT,
	MCBSP_RCERE2_DEFAULT,
	MCBSP_RCERE3_DEFAULT,
	MCBSP_XCERE0_DEFAULT,
	MCBSP_XCERE1_DEFAULT,
	MCBSP_XCERE2_DEFAULT,
	MCBSP_XCERE3_DEFAULT,
	MCBSP_PCR_RMK(
        MCBSP_PCR_XIOEN_,
        MCBSP_PCR_RIOEN_,
        MCBSP_PCR_FSXM_,
        MCBSP_PCR_FSRM_,
        MCBSP_PCR_CLKXM_,
        MCBSP_PCR_CLKRM_,
        MCBSP_PCR_CLKSSTAT_,
        MCBSP_PCR_DXSTAT_,
        MCBSP_PCR_FSXP_,
        MCBSP_PCR_FSRP_,
        MCBSP_PCR_CLKXP_,
        MCBSP_PCR_CLKRP_
	)
};
\**************************************************************/

/*
 *  ======== Include files ========
 */

#include <csl.h>
#include <csl_mcbsp.h>
#include <std.h>
#include <swi.h>
#include <log.h>
#include <c6x.h>
#include <csl.h>
#include <csl_edma.h>
#include <csl_irq.h>
#include <csl_mcbsp.h>
#include "codec.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "aic23.h"

/*
 *  ======== Declarations ========
 */

/*
 *  ======== Prototypes ========
 */
void initMcBSP(void);

/*
 *  ======== Global Variables ========
 */
MCBSP_Handle hMcbsp1;                 // McBSP1 (codec data) handle
DSK6713_AIC23_CodecHandle dskHandle;



/* McBSP codec data channel configuration */
static MCBSP_Config mcbspCfg1 = {
        MCBSP_FMKS(SPCR, FREE, NO)              |
        MCBSP_FMKS(SPCR, SOFT, NO)              |
        MCBSP_FMKS(SPCR, FRST, YES)             |
        MCBSP_FMKS(SPCR, GRST, YES)             |
        MCBSP_FMKS(SPCR, XINTM, XRDY)           |
        MCBSP_FMKS(SPCR, XSYNCERR, NO)          |
        MCBSP_FMKS(SPCR, XRST, YES)             |
        MCBSP_FMKS(SPCR, DLB, OFF)              |
        MCBSP_FMKS(SPCR, RJUST, RZF)            |
        MCBSP_FMKS(SPCR, CLKSTP, DISABLE)       |
        MCBSP_FMKS(SPCR, DXENA, OFF)            |
        MCBSP_FMKS(SPCR, RINTM, RRDY)           |
        MCBSP_FMKS(SPCR, RSYNCERR, NO)          |
        MCBSP_FMKS(SPCR, RRST, YES),

        MCBSP_FMKS(RCR, RPHASE, SINGLE)         |
        MCBSP_FMKS(RCR, RFRLEN2, DEFAULT)       |
        MCBSP_FMKS(RCR, RWDLEN2, DEFAULT)       |
        MCBSP_FMKS(RCR, RCOMPAND, MSB)          |
        MCBSP_FMKS(RCR, RFIG, NO)               |
        MCBSP_FMKS(RCR, RDATDLY, 0BIT)          |
        MCBSP_FMKS(RCR, RFRLEN1, OF(1))         |
        MCBSP_FMKS(RCR, RWDLEN1, 16BIT)         |
        MCBSP_FMKS(RCR, RWDREVRS, DISABLE),

        MCBSP_FMKS(XCR, XPHASE, SINGLE)         |
        MCBSP_FMKS(XCR, XFRLEN2, DEFAULT)       |
        MCBSP_FMKS(XCR, XWDLEN2, DEFAULT)       |
        MCBSP_FMKS(XCR, XCOMPAND, MSB)          |
        MCBSP_FMKS(XCR, XFIG, NO)               |
        MCBSP_FMKS(XCR, XDATDLY, 0BIT)          |
        MCBSP_FMKS(XCR, XFRLEN1, OF(1))         |
        MCBSP_FMKS(XCR, XWDLEN1, 16BIT)         |
        MCBSP_FMKS(XCR, XWDREVRS, DISABLE),

        MCBSP_FMKS(SRGR, GSYNC, DEFAULT)        |
        MCBSP_FMKS(SRGR, CLKSP, DEFAULT)        |
        MCBSP_FMKS(SRGR, CLKSM, DEFAULT)        |
        MCBSP_FMKS(SRGR, FSGM, DEFAULT)         |
        MCBSP_FMKS(SRGR, FPER, DEFAULT)         |
        MCBSP_FMKS(SRGR, FWID, DEFAULT)         |
        MCBSP_FMKS(SRGR, CLKGDV, DEFAULT),

        MCBSP_MCR_DEFAULT,
        MCBSP_RCER_DEFAULT,
        MCBSP_XCER_DEFAULT,

        MCBSP_FMKS(PCR, XIOEN, SP)              |
        MCBSP_FMKS(PCR, RIOEN, SP)              |
        MCBSP_FMKS(PCR, FSXM, EXTERNAL)         |
        MCBSP_FMKS(PCR, FSRM, EXTERNAL)         |
        MCBSP_FMKS(PCR, CLKXM, INPUT)           |
        MCBSP_FMKS(PCR, CLKRM, INPUT)           |
        MCBSP_FMKS(PCR, CLKSSTAT, DEFAULT)      |
        MCBSP_FMKS(PCR, DXSTAT, DEFAULT)        |
        MCBSP_FMKS(PCR, FSXP, ACTIVEHIGH)       |
        MCBSP_FMKS(PCR, FSRP, ACTIVEHIGH)       |
        MCBSP_FMKS(PCR, CLKXP, FALLING)         |
        MCBSP_FMKS(PCR, CLKRP, RISING)
};

/* Codec configuration settings */
DSK6713_AIC23_Config config = DSK6713_AIC23_DEFAULTCONFIG;

		#if 0
{ \
    0x0017,  /* 0 DSK6713_AIC23_LEFTINVOL  Left line input channel volume */ \
    0x0017,  /* 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume */\
    0x01f9,  /* 2 DSK6713_AIC23_LEFTHPVOL  Left channel headphone volume */  \
    0x01f9,  /* 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume */ \
    0x0011,  /* 4 DSK6713_AIC23_ANAPATH    Analog audio path control */      \
    0x0000,  /* 5 DSK6713_AIC23_DIGPATH    Digital audio path control */     \
    0x0000,  /* 6 DSK6713_AIC23_POWERDOWN  Power down control */             \
    0x0043,  /* 7 DSK6713_AIC23_DIGIF      Digital audio interface format */ \
    0x0001,  /* 8 DSK6713_AIC23_SAMPLERATE Sample rate control */            \
    0x0001   /* 9 DSK6713_AIC23_DIGACT     Digital interface activation */   \
};
#endif


MCBSP_Config mcbspCfgControl = {
   MCBSP_SPCR_RMK(
		MCBSP_SPCR_FREE_NO,
        MCBSP_SPCR_SOFT_NO,
        MCBSP_SPCR_FRST_YES,
        MCBSP_SPCR_GRST_YES,
        MCBSP_SPCR_XINTM_XRDY,
        MCBSP_SPCR_XSYNCERR_NO,
        MCBSP_SPCR_XRST_YES,
        MCBSP_SPCR_DLB_OFF,
        MCBSP_SPCR_RJUST_RZF,
        MCBSP_SPCR_CLKSTP_NODELAY,
        MCBSP_SPCR_DXENA_OFF,
        MCBSP_SPCR_RINTM_RRDY,
        MCBSP_SPCR_RSYNCERR_NO,
        MCBSP_SPCR_RRST_YES
	),
	MCBSP_RCR_DEFAULT,
	MCBSP_XCR_RMK(
        MCBSP_XCR_XPHASE_SINGLE,
        MCBSP_XCR_XFRLEN2_OF(0),
        MCBSP_XCR_XWDLEN2_8BIT,
        MCBSP_XCR_XCOMPAND_MSB,
        MCBSP_XCR_XFIG_NO,
        MCBSP_XCR_XDATDLY_0BIT,
        MCBSP_XCR_XFRLEN1_OF(0),
        MCBSP_XCR_XWDLEN1_16BIT,
        MCBSP_XCR_XWDREVRS_DISABLE
	),
    MCBSP_SRGR_RMK(    
        MCBSP_SRGR_GSYNC_FREE,
        MCBSP_SRGR_CLKSP_RISING,
        MCBSP_SRGR_CLKSM_INTERNAL,
        MCBSP_SRGR_FSGM_DXR2XSR,
        MCBSP_SRGR_FPER_OF(0),
        MCBSP_SRGR_FWID_OF(19),
        MCBSP_SRGR_CLKGDV_OF(99)
	),
	MCBSP_MCR_DEFAULT,
	//C67x Needs RCER
	MCBSP_RCER_DEFAULT,		// C67x ONLY, C67x ONLY
	
	//C64x Needs RCER0-3
//	MCBSP_RCERE0_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_RCERE1_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_RCERE2_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_RCERE3_DEFAULT,		// C64x ONLY, C64x ONLY

	//C67x Needs XCER
	MCBSP_XCER_DEFAULT,		// C67x ONLY, C67x ONLY

	//C64x Needs RCER0-3
//	MCBSP_XCERE0_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_XCERE1_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_XCERE2_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_XCERE3_DEFAULT,		// C64x ONLY, C64x ONLY
	MCBSP_PCR_RMK(
        MCBSP_PCR_XIOEN_SP,
        MCBSP_PCR_RIOEN_SP,
        MCBSP_PCR_FSXM_INTERNAL,
        MCBSP_PCR_FSRM_EXTERNAL,
        MCBSP_PCR_CLKXM_OUTPUT,
        MCBSP_PCR_CLKRM_INPUT,
        MCBSP_PCR_CLKSSTAT_DEFAULT,
        MCBSP_PCR_DXSTAT_DEFAULT,
        MCBSP_PCR_FSXP_ACTIVELOW,
        MCBSP_PCR_FSRP_DEFAULT,
        MCBSP_PCR_CLKXP_FALLING,
        MCBSP_PCR_CLKRP_DEFAULT
	)
};

MCBSP_Config mcbspCfgData = {
   MCBSP_SPCR_RMK(
		MCBSP_SPCR_FREE_NO,
        MCBSP_SPCR_SOFT_NO,
        MCBSP_SPCR_FRST_YES,
        MCBSP_SPCR_GRST_YES,
        MCBSP_SPCR_XINTM_XRDY,
        MCBSP_SPCR_XSYNCERR_NO,
        MCBSP_SPCR_XRST_YES,
        MCBSP_SPCR_DLB_OFF,
        MCBSP_SPCR_RJUST_RZF,
        MCBSP_SPCR_CLKSTP_DISABLE,
        MCBSP_SPCR_DXENA_OFF,
        MCBSP_SPCR_RINTM_RRDY,
        MCBSP_SPCR_RSYNCERR_NO,
        MCBSP_SPCR_RRST_YES
	),
	MCBSP_RCR_RMK(
        MCBSP_RCR_RPHASE_SINGLE,
        MCBSP_RCR_RFRLEN2_DEFAULT,
        MCBSP_RCR_RWDLEN2_DEFAULT,
        MCBSP_RCR_RCOMPAND_MSB,
        MCBSP_RCR_RFIG_NO,
        MCBSP_RCR_RDATDLY_0BIT,
        MCBSP_RCR_RFRLEN1_OF(1),
        MCBSP_RCR_RWDLEN1_16BIT,
        MCBSP_RCR_RWDREVRS_DISABLE
	),
	MCBSP_XCR_RMK(
        MCBSP_XCR_XPHASE_SINGLE,
        MCBSP_XCR_XFRLEN2_DEFAULT,
        MCBSP_XCR_XWDLEN2_DEFAULT,
        MCBSP_XCR_XCOMPAND_MSB,
        MCBSP_XCR_XFIG_NO,
        MCBSP_XCR_XDATDLY_0BIT,
        MCBSP_XCR_XFRLEN1_OF(1),
        MCBSP_XCR_XWDLEN1_16BIT,
        MCBSP_XCR_XWDREVRS_DISABLE
	),
    MCBSP_SRGR_DEFAULT,
	MCBSP_MCR_DEFAULT,
	//C67x Needs RCER
	MCBSP_RCER_DEFAULT,		// C67x ONLY, C67x ONLY
	
	//C64x Needs RCER0-3
//	MCBSP_RCERE0_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_RCERE1_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_RCERE2_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_RCERE3_DEFAULT,		// C64x ONLY, C64x ONLY

	//C67x Needs XCER
	MCBSP_XCER_DEFAULT,		// C67x ONLY, C67x ONLY

	//C64x Needs RCER0-3
//	MCBSP_XCERE0_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_XCERE1_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_XCERE2_DEFAULT,		// C64x ONLY, C64x ONLY
//	MCBSP_XCERE3_DEFAULT,		// C64x ONLY, C64x ONLY
	MCBSP_PCR_RMK(
        MCBSP_PCR_XIOEN_SP,
        MCBSP_PCR_RIOEN_SP,
        MCBSP_PCR_FSXM_EXTERNAL,
        MCBSP_PCR_FSRM_EXTERNAL,
        MCBSP_PCR_CLKXM_INPUT,
        MCBSP_PCR_CLKRM_INPUT,
        MCBSP_PCR_CLKSSTAT_DEFAULT,
        MCBSP_PCR_DXSTAT_DEFAULT,
        MCBSP_PCR_FSXP_ACTIVEHIGH,
        MCBSP_PCR_FSRP_ACTIVEHIGH,
        MCBSP_PCR_CLKXP_FALLING,
        MCBSP_PCR_CLKRP_RISING
	)
};

/* McBSP Handles */

MCBSP_Handle hMcbspControl, hMcbspData;

/*
 *	======== initMcBSP ========
 */
void initMcBSP(void) {

	/* Open McBSP for codec control */
	hMcbspControl = MCBSP_open( MCBSP_DEV0 , MCBSP_OPEN_RESET );        

	/* Open McBSP for codec data */
	hMcbspData = MCBSP_open( MCBSP_DEV1 , MCBSP_OPEN_RESET );

	/* Configure McBSP for codec control */
    MCBSP_config( hMcbspControl, &mcbspCfgControl );

    /* Configure McBSP for codec data */
	MCBSP_config( hMcbspData, &mcbspCfgData );

	/* Start McBSP for the codec control channel */
	MCBSP_start( hMcbspControl, MCBSP_XMIT_START | MCBSP_SRGR_START |
					MCBSP_SRGR_FRAMESYNC, 100 );

	/* Call the provided codec initialiation routine */
	initCodec( hMcbspControl );

	/* Clear any garbage from the codec data port */
    if( MCBSP_rrdy( hMcbspData ) )
		MCBSP_read( hMcbspData );



}


void startMcBSP(void){

	  /* Start McBSP for the codec data channel */
		MCBSP_start( hMcbspData, MCBSP_XMIT_START | MCBSP_RCV_START |
						MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 220 );

		MCBSP_write(hMcbspData, 0);

		MCBSP_write(hMcbspData, 0);

}
