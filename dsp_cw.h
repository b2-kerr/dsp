/*
 * application_wide.h
 *
 *  Created on: 13 Feb 2013
 *      Author: Will
 */

#ifndef APPLICATION_WIDE_H_
#define APPLICATION_WIDE_H_

#include "dsp2cfg.h"


/* This is STEREO, so the TOTAL buffer is double the CHANNEL buffer */

/* How many 2-byte samples in each channel buffer? */
#define RX_BUFFER_CHANNEL_SAMPLES (64)

/* The main buffer is split into two buffers: one for each channel */
#define RX_BUFFER_SAMPLES (RX_BUFFER_CHANNEL_SAMPLES*2)

/* Samples are 2bytes each */
#define RX_BUFFER_BYTES (RX_BUFFER_SAMPLES*2)
#define RX_BUFFER_CHANNEL_BYTES (RX_BUFFER_CHANNEL_SAMPLES*2)

/* How many chunks is each channel buffer split into? */
#define RX_BUFFER_CHANNEL_CHUNKS (4)
#define RX_BUFFER_CHANNEL_CHUNKS_BYTES (RX_BUFFER_CHANNEL_BYTES/RX_BUFFER_CHANNEL_CHUNKS)
#define RX_BUFFER_CHANNEL_CHUNKS_SAMPLES (RX_BUFFER_CHANNEL_SAMPLES/RX_BUFFER_CHANNEL_CHUNKS)

#if RX_BUFFER_CHANNEL_CHUNKS_BYTES < 2
	#error "Increase Channel Buffer Size"
#endif

/* The buffers are defined elsewhere */
extern short  sRxBuffer[RX_BUFFER_SAMPLES];
extern short*  gBufRcvL;
extern short*  gBufRcvR;

//extern short gBufXmtL[BUFFSIZE];
//extern short gBufXmtR[BUFFSIZE];

#endif /* APPLICATION_WIDE_H_ */
