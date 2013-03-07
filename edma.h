/*
 *  ======== edma.h ========
 *  This file contains reference for all functions
 *  and globals contained in edma.c
 */

extern void initEdma(void);
extern void edmaHwi(int tcc);
extern void edmaHwiTX(int tcc);
extern EDMA_Handle hEdma;
int getRXCount(void);
int getTXCount(void);
