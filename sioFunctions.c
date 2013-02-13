#include <std.h>
#include <sio.h>
#include "audioappcfg.h"

#define BUFFSIZE 512
#define BUFALIGN 1

/* inStream and outStream are SIO handles created in main */
SIO_Handle inStream, outStream;

extern Int ISRAM;

/*
 * ======== createStreams ========
 */


void createStreams()
{
    SIO_Attrs attrs;
    
    attrs = SIO_ATTRS;
    attrs.align = BUFALIGN;
    attrs.model = SIO_ISSUERECLAIM;
	attrs.segid = ISRAM;

    /* open the I/O streams */
    inStream = SIO_create("/dioCodec", SIO_INPUT, BUFFSIZE*4, &attrs);

    outStream = SIO_create("/dioCodec", SIO_OUTPUT, BUFFSIZE*4, &attrs);
}

void primeStreams()
{
	Ptr rcvPing, rcvPong, xmtPing, xmtPong;

    /* Allocate buffers for the SIO buffer exchanges */
    rcvPing = (Ptr)MEM_calloc(0, BUFFSIZE*4, BUFALIGN);
    rcvPong = (Ptr)MEM_calloc(0, BUFFSIZE*4, BUFALIGN);
    xmtPing = (Ptr)MEM_calloc(0, BUFFSIZE*4, BUFALIGN);
    xmtPong = (Ptr)MEM_calloc(0, BUFFSIZE*4, BUFALIGN);

    
    /* Issue the first & second empty buffers to the input stream */
    SIO_issue(inStream, rcvPing, BUFFSIZE*4, NULL);
    SIO_issue(inStream, rcvPong, BUFFSIZE*4, NULL);

    /* Issue the first & second empty buffers to the output stream */
    SIO_issue(outStream, xmtPing, BUFFSIZE*4, NULL);
    SIO_issue(outStream, xmtPong, BUFFSIZE*4, NULL);

}


void splitBuff(short *inBuff, short inBuffLen, short *outBuffL, short *outBuffR)
{

	int i;

	for (i=0;i < inBuffLen; i++) {
		outBuffL[i] = inBuff[2*i];
		outBuffR[i] = inBuff[2*i+1];
	}
}


void joinBuff(short *inBuffL, short *inBuffR, short inBuffLen, short *outBuff)
{

	int i;

	for (i=0;i < inBuffLen; i++) {
		outBuff[2*i] = inBuffL[i];
		outBuff[2*i+1]=inBuffR[i];
	}
}
