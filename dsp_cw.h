/*
 * application_wide.h
 *
 *  Created on: 13 Feb 2013
 *      Author: Will
 */

#ifndef APPLICATION_WIDE_H_
#define APPLICATION_WIDE_H_

/* Incoming buffer is 4 bytes unsigned integers. This is becuase
 * it's easier to deal with at the assembly level */

#define BUFFER_CHUNK

#define BUFFSIZE 20
#define BUFFSIZE_BYTES (sizeof(unsigned int)*BUFFSIZE)

#include "dsp2cfg.h"

extern short  gBufRcvL[BUFFSIZE];
extern short  gBufRcvR[BUFFSIZE];

extern short gBufXmtL[BUFFSIZE];
extern short gBufXmtR[BUFFSIZE];

#endif /* APPLICATION_WIDE_H_ */
