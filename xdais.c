/*
 *  ======== xdais.c ========
 *  This is the code needed to setup the xdais algorithms.
 */

/*
 *  ======== Include files ========
 */






/*
 *  ======== Declarations ========
 */
#define BUFFSIZE 512

/*
 *  ======== Prototypes ========
 */
void initAlgs(void);

/*
 *  ======== References ========
 */
extern Int ISRAM;        /* name of the internal heap for the C64x */
extern Int IRAM;         /* name of the internal heap for the C67x */
extern Int SDRAM;        /* name of the external heap */

/*
 *  ======== Global Variables ========
 */
			/* FIR handles */


/*
 *	======== initAlgs ========
 */
void initAlgs(void)
{
	FIR_Params firParams;
	
						/* set up heaps */				 

    firParams = FIR_PARAMS;                     /* default parameters */
    firParams.coeffPtr   = (short *)coeffs;     /* filter coefficients */
    firParams.filterLen  = 345;                 /* filter size or number of coefficients */
    firParams.frameLen   = BUFFSIZE;            /* frame size in samples */

    // create algorithm instance for left channel 
	

	// create algorithm instance for right channel 
    
}
