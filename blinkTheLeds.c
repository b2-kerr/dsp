#define LED_ON 		1

int currentLed = 0;
int ledCycle = LED_ON;

#include "dsk6416_led.h"

// blink the LEDs based on a periodic interrupt
void blinkLeds(void)
{

	// add a load to the system to more acccurately 
	// simulate a real-world interruption of the audio process
	load(32000);
	
    // turn the current LED on or off according to
    // whether we're in an ON or OFF cycle
    if (ledCycle == LED_ON)
    {
	    DSK6416_LED_on (currentLed++);
    }
    else
    {
	    DSK6416_LED_off(--currentLed);
	}

    // reverse the toggling cycle
	if ((currentLed == 4) || (currentLed == 0))
	{
	    ledCycle ^= LED_ON;
	}
}
