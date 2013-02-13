//  ======== sine.c ========
//  The coefficient A and the three initial values
//  generate a 200Hz tone (sine wave) when running
//  at a sample rate of 48KHz.
//
//  Even though the calculations are done in floating
//  point, this function returns a short value since
//  this is what's needed by a 16-bit codec (DAC).

//  ======== Includes ========
#include "sine.h"
#include <std.h>
#include <math.h>

//  ======== Definitions ========
#define PI 3.1415927

//  ======== Prototypes ========
void SINE_init(SINE_Obj *sineObj, float freqTone, float freqSampRate);
void SINE_blockFill(SINE_Obj *sineObj, short *buf, int len);
void SINE_addPacked(SINE_Obj *sineObj, short *inbuf, int length);
void SINE_add(SINE_Obj *sineObj, short *inbuf, int length);
static short sineGen(SINE_Obj *sineObj);
static float degreesToRadiansF(float d);
void copyData(short *inbuf, short *outbuf ,int length );

//	======== Globals ========

//	======== SINE_init ========
//  Initializes the sine wave generation algorithm 	
void SINE_init(SINE_Obj *sineObj, float freqTone, float freqSampRate)
{
	float rad = 0;

	if(freqTone == NULL)
		sineObj->freqTone = 200;
	else
		sineObj->freqTone = freqTone;
	
	if(freqSampRate == NULL)
		sineObj->freqSampRate = 48 * 1000;
	else
		sineObj->freqSampRate = freqSampRate;

	rad = sineObj->freqTone / sineObj->freqSampRate;
	rad = rad * 360.0;
	rad = degreesToRadiansF(rad);

	sineObj->a  = 2 * cosf(rad);
	sineObj->b  = -1;
	sineObj->y0 = 0;
	sineObj->y1 = sinf(rad);
	sineObj->y2 = 0;
	sineObj->count = sineObj->freqTone * sineObj->freqSampRate;

	sineObj->aInitVal  = sineObj->a;
	sineObj->bInitVal  = sineObj->b;
	sineObj->y0InitVal = sineObj->y0;
	sineObj->y1InitVal = sineObj->y1;
	sineObj->y2InitVal = sineObj->y2;
	sineObj->countInitVal = sineObj->count;
}

//	======== SINE_blockFill ========
//  Generate a block of sine data using sineGen 	
void SINE_blockFill(SINE_Obj *sineObj, short *buf, int len) 
{ 
	int i = 0;

    for (i = 0;i < len; i++) { 
		buf[i]  = sineGen(sineObj);
    }
}

//	======== SINE_addPacked ========
//  add the sine wave to the indicated buffer of packed
//  left/right data
//  divide the sine wave signal by 8 and add it
void SINE_addPacked(SINE_Obj *sineObj, short *inbuf, int length)
{
	int i = 0;
	static short temp;

	for (i = 0; i < length; i+=2) {
		temp = sineGen(sineObj);	
		inbuf[i] = (inbuf[i]) + (temp>>4);
		inbuf[i+1] = (inbuf[i+1]) + (temp>>4);
	}
}

//  ======== SINE_add ========
//  add the sine wave to the indicated buffer
void SINE_add(SINE_Obj *sineObj, short *inbuf, int length)
{
	int i = 0;
	short temp;

	for (i = 0; i < length; i++) {
		temp = sineGen(sineObj);	
		inbuf[i] = (inbuf[i]) + (temp>>4);
	}
}

//	======== sineGen ========
//  Generate a single sine wave value 	
static short sineGen(SINE_Obj *sineObj) 
{ 	
	float result;
	
	if (sineObj->count > 0) {
		sineObj->count = sineObj->count - 1;
	}
	else {
		sineObj->a  = sineObj->aInitVal;
		sineObj->b  = sineObj->bInitVal;
		sineObj->y0 = sineObj->y0InitVal;
		sineObj->y1 = sineObj->y1InitVal;
		sineObj->y2 = sineObj->y2InitVal;
		sineObj->count = sineObj->countInitVal;
	}

	sineObj->y0 = (sineObj->a * sineObj->y1) + (sineObj->b * sineObj->y2);  
	sineObj->y2 = sineObj->y1;
	sineObj->y1 = sineObj->y0;
	
	//  To scale full 16-bit range we would multiply y[0] 
	//  by 32768 using a number slightly less than this 
	//  (such as 28000) helps to prevent overflow.
	result = sineObj->y0 * 28000;

	//  We recast the result to a short value upon returning it
    //  since the D/A converter is programmed to accept 16-bit
    //  signed values.
	return((short)result);
}

//	======== degreesToRadiansF ========
//  Converts a floating point number from degrees to radians 	
static float degreesToRadiansF(float d)
{
  return(d * PI / 180);
}

//	======== copyData ========
//  copy data from one buffer to the other.
void copyData(short *inbuf, short *outbuf ,int length )
{
	int i = 0;
	
    for (i = 0; i < length; i++) {
    	outbuf[i]  = inbuf[i]*0.5;
    }
}
