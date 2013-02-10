#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "dsk6713_led.h"

/* Length of sine wave table */
#define SINE_TABLE_SIZE   48

/* Codec configuration settings */
DSK6713_AIC23_Config config = { \
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

/* Pre-generated sine wave data, 16-bit signed samples */
Int16 sinetable[SINE_TABLE_SIZE] = {
    0x0000, 0x10b4, 0x2120, 0x30fb, 0x3fff, 0x4dea, 0x5a81, 0x658b,
    0x6ed8, 0x763f, 0x7ba1, 0x7ee5, 0x7ffd, 0x7ee5, 0x7ba1, 0x76ef,
    0x6ed8, 0x658b, 0x5a81, 0x4dea, 0x3fff, 0x30fb, 0x2120, 0x10b4,
    0x0000, 0xef4c, 0xdee0, 0xcf06, 0xc002, 0xb216, 0xa57f, 0x9a75,
    0x9128, 0x89c1, 0x845f, 0x811b, 0x8002, 0x811b, 0x845f, 0x89c1,
    0x9128, 0x9a76, 0xa57f, 0xb216, 0xc002, 0xcf06, 0xdee0, 0xef4c
};

/*
 *  main() - Main code routine, initializes BSL and generates tone
 */

void audio_main()
{
    DSK6713_AIC23_CodecHandle hCodec;
    Int16 msec, sample;

    /* Initialize the board support library, must be called first */
    DSK6713_init();

    DSK6713_LED_on(3);

    /* Start the codec */
    hCodec = DSK6713_AIC23_openCodec(0, &config);

    /* Generate a 1KHz sine wave for 5 seconds */
    for (msec = 0; msec < 1000; msec++)
    {
        for (sample = 0; sample < SINE_TABLE_SIZE; sample++)
        {
            /* Send a sample to the left channel */
            while (!DSK6713_AIC23_write(hCodec, sinetable[sample]));

            /* Send a sample to the right channel */
            while (!DSK6713_AIC23_write(hCodec, sinetable[sample]));
        }
    }

    DSK6713_LED_off(3);

    /* Close the codec */
    DSK6713_AIC23_closeCodec(hCodec);
}
