;
;  ======== _load ========
;  This function simulates a load on the DSP by executing N * loopCount
;  instructions, where loopCount is the input parameter to load().
;
;      void load(int loopCount)
;
;  The loop is using 8 instructions. One instruction for sub, nop and
;  b, plus nop 5. The extra nop added after sub is to make the number
;  of instructions in the loop a power of 2.
;

N       .set    1000 			; 1000 cycles at 1.0ns/cycle = 1 microsecond
		.def 	_load
_load:

        mv a4, b0               ; use b0 as loop counter
        mvk N,b1
        mpy b1,b0,b0
        nop
        shru b0,3,b0            ; (loop counter)= (# loops)/8 

loop:
   [b0] sub b0,1,b0
        nop
   [b0] b loop
        nop 5

        b b3
        nop 5                   ; return
        
        .end
        