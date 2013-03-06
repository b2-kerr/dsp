/*
 *  ======== sine.h ========
 *  This file contains prototypes for all functions
 *  contained in sine.c
 */
#ifndef SINE_Obj
typedef struct {
  float freqTone;
  float freqSampRate;
  float a;
  float b;
  float y0;
  float y1;
  float y2;
  float count;
  float aInitVal;
  float bInitVal;
  float y0InitVal;
  float y1InitVal;
  float y2InitVal;
  float countInitVal;
} SINE_Obj;
#endif

void copyData(short *inbuf, short *outbuf ,int length);
void SINE_init(SINE_Obj *sineObj, float freqTone, float freqSampRate);
void SINE_blockFill(SINE_Obj *myObj, short *buf, int len);
void SINE_addPacked(SINE_Obj *myObj, short *inbuf, int length);
void SINE_add(SINE_Obj *myObj, short *inbuf, int length);


