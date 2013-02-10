/*
 * main.c
 */

#include <stdio.h>



#include <csl.h>
#include <csl_cache.h>
#include <csl_edma.h>
#include <csl_timer.h>
#include <csl_irq.h>


#include <dsk6713.h>
#include "dsk6713_led.h"

#include "dspbios_test1cfg.h"

#include <c62.h>


#define FCPU         225000000        /* CPU clock frequency                  */
#define SRATE        8000             /* data sample rate (simulated w/timer  */
#define TPRD         (FCPU/(4)) /* timer period                         */

extern	cregister volatile unsigned int	CSR;	// control status register
extern	cregister volatile unsigned int	ICR;	// interrupt clear register
extern	cregister volatile unsigned int	IER;	// interrupt enable reg.

volatile unsigned int jingo;

TIMER_Handle hTimer;   /* Handle for the timer device                 */

void audio_main();

//volatile unsigned int toggle;

void main(void) {
	
	  /* initialize the CSL library */
	  CSL_init();

	  DSK6713_init();



	  /* Let's open up a timer device, we'll use this to simulate input events */
	   /* at a given sample rate.                                                */
	   hTimer = TIMER_open(TIMER_DEV1, TIMER_OPEN_RESET);

	   /* Configure up the timer. */
	   TIMER_configArgs(hTimer,
	     TIMER_CTL_OF(0x00000200),
	     TIMER_PRD_OF((TPRD)), /* timer period  */
	     TIMER_CNT_OF(0)
	   );

	   DSK6713_LED_init();
	   DSK6713_LED_off(0);
	   DSK6713_LED_off(1);
	   DSK6713_LED_off(2);
	   DSK6713_LED_off(3);

	   jingo=0;

	//   ICR  = 0x10;								// clear INT4 (precaution)
	  // 	IER |= 0x10;								// enable INT4 as CPU interrupt
	   //DSK6713_LED_on(0);
	  // C62_enableIER(C62_EINT4);

//	HWI_enable();


	   /* Finally, enable the timer which will drive everything. */
	   TIMER_start(hTimer);

}

void dowork (void){

	unsigned int* leds = (unsigned int*)0x90080000;

	if (jingo){
		//DSK6713_LED_on(1);
		*leds = 0;
	}
		else
		{	//DSK6713_LED_off(2);

			*leds = 1;
		}

	jingo = ++jingo & 1;

	return;
	//LOG_printf(&LOG0, "Bingo\n");
}


interrupt void edma_int (void){}

void task1(void){

	while(1){
		  audio_main();
		  TSK_sleep(1000);
	}

}

interrupt void bingo (void){

	dowork();

	//jingo = 67;

	C62_enableIER(C62_EINT4);
}


