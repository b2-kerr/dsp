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
#include "codec.h"

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
    /* Start McBSP for the codec data channel */
	MCBSP_start( hMcbspData, MCBSP_XMIT_START | MCBSP_RCV_START | 
					MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 220 );

	MCBSP_write(hMcbspData, 0);
}
