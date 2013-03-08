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
#define RX_BUFFER_CHANNEL_SAMPLES (2048)

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


/* Block sizes (in bytes) for the circular buffering */
enum{
	BLOCK_SIZE_2 = 0,
	BLOCK_SIZE_4,
	BLOCK_SIZE_8,
	BLOCK_SIZE_16,
	BLOCK_SIZE_32,
	BLOCK_SIZE_64,
	BLOCK_SIZE_128,
	BLOCK_SIZE_256,
	BLOCK_SIZE_512,
	BLOCK_SIZE_1024,
	BLOCK_SIZE_2048,
	BLOCK_SIZE_4096
};

#define FILTER_LENGTH_WORDS 11
#define FILTER_MIDDLE ((FILTER_LENGTH_WORDS-1)/2)

extern float H[FILTER_LENGTH_WORDS];

typedef struct MsgObj {
    QUE_Elem elem; /* first field for QUE */
    short* addr; /* message value */
    } MsgObj, *Msg;

typedef struct {
	short TCCID;
	short* LAddr;
	short* RAddr;
}chunksData_s;

/* Location of the data */
extern chunksData_s RXchunksData[16];
extern chunksData_s TXchunksData[16];

/* The buffers are defined elsewhere */
extern short  sRxBuffer[RX_BUFFER_SAMPLES];
extern short  sTxBuffer[RX_BUFFER_SAMPLES];

/* Pointers representing the channel data within the larger buffers */
extern short*  sTX_L;
extern short*  sTX_R;
extern short*  sRX_L;
extern short*  sRX_R;

#endif /* APPLICATION_WIDE_H_ */
