/*
 *  ======== codec.c ========
 */

/*
 *	======== Include ========
 */
#include <csl.h>
#include <csl_mcbsp.h>	
#include <dsk6713.h>
#include <dsk6713_aic23.h>

/*
 *  ======== Prototypes ========
 */
void initCodec(MCBSP_Handle hMcbsp);

/*
 *	======== Declarations ========
 */

DSK6713_AIC23_CodecHandle hCodec;

/* Codec configuration settings */
DSK6713_AIC23_Config AICconfig = { \
    0x0017,  /* 0 DSK6713_AIC23_LEFTINVOL  Left line input channel volume */ \
    0x0017,  /* 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume */\
    0x00d8,  /* 2 DSK6713_AIC23_LEFTHPVOL  Left channel headphone volume */  \
    0x00d8,  /* 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume */ \
    0x0011,  /* 4 DSK6713_AIC23_ANAPATH    Analog audio path control */      \
    0x0000,  /* 5 DSK6713_AIC23_DIGPATH    Digital audio path control */     \
    0x0000,  /* 6 DSK6713_AIC23_POWERDOWN  Power down control */             \
    0x0043,  /* 7 DSK6713_AIC23_DIGIF      Digital audio interface format */ \
    0x0081,  /* 8 DSK6713_AIC23_SAMPLERATE Sample rate control */            \
    0x0001   /* 9 DSK6713_AIC23_DIGACT     Digital interface activation */   \
};


/*
 *
 *
 *  ======== initCodec ========
 */
void initCodec(MCBSP_Handle hMcbsp)
{
    int i = 0;
    int j;
    int resetReg = 15;
    int resetVal = 0;
	
	// Codec configuration settings
	short codecConfig[10] = {
    0x0017,  /* 0 DSK6416_AIC23_LEFTINVOL  Left line input channel volume */ 
    0x0017,  /* 1 DSK6416_AIC23_RIGHTINVOL Right line input channel volume */
    0x01f9,  /* 2 DSK6416_AIC23_LEFTHPVOL  Left channel headphone volume */  
    0x01f9,  /* 3 DSK6416_AIC23_RIGHTHPVOL Right channel headphone volume */ 
    0x0011,  /* 4 DSK6416_AIC23_ANAPATH    Analog audio path control */      
    0x0000,  /* 5 DSK6416_AIC23_DIGPATH    Digital audio path control */     
    0x0000,  /* 6 DSK6416_AIC23_POWERDOWN  Power down control */             
    0x0043,  /* 7 DSK6416_AIC23_DIGIF      Digital audio interface format */ 
    0x0081,  /* 8 DSK6416_AIC23_SAMPLERATE Sample rate control */            
    0x0001   /* 9 DSK6416_AIC23_DIGACT     Digital interface activation */   
	};

	/* Wait for XRDY signal before writing data to DXR */
	while (!MCBSP_xrdy(hMcbsp));

	/* Write value to Reset the Codec */
	MCBSP_write(hMcbsp, (resetReg << 9) | resetVal );
	
    /* Loop to write to each Control Register on the Codec */
    for (i = 0; i < 10; i++) {
		/* Mask off upper 22 bits - these are used for register address */
		codecConfig[i] &= 0x1ff;

		/* Wait for XRDY signal before writing data to DXR */
		while (!MCBSP_xrdy(hMcbsp));		

		/* Write 16 bit data value to DXR */
		MCBSP_write(hMcbsp, (i << 9) | codecConfig[i]);
		
		/* Add delay for Codec values to settle */
		for(j = 0; j < 1000; j++);			
	}
}


int codecInitNew(void){

	Uint32 temp;

	/* Initialize the board support library, must be called first */
	DSK6713_init();

	/* Start the codec */
	hCodec = DSK6713_AIC23_openCodec(0, &AICconfig);

	DSK6713_AIC23_setFreq(hCodec,DSK6713_AIC23_FREQ_32KHZ);

	DSK6713_AIC23_read(hCodec,&temp);

	return 0;
}
