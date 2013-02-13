            .title  "Flash bootup utility for 6713 dsk"
            .option D,T
            .length 102
            .width  140

;EMIF Register Addresses
EMIF_GCTL       .equ  0x01800000  ;EMIF global control
EMIF_CE1        .equ  0x01800004  ;address of EMIF CE1 control reg.
EMIF_CE0        .equ  0x01800008  ;EMIF CE0control
EMIF_CE2        .equ  0x01800010  ;EMIF CE2control
EMIF_CE3        .equ  0x01800014  ;EMIF CE3control
EMIF_SDRAMCTL   .equ  0x01800018  ;EMIF SDRAM control
EMIF_SDRAMTIM   .equ  0x0180001c  ;EMIF SDRAM timer
EMIF_SDRAMEXT   .equ  0x01800020  ;EMIF SDRAM extension

; EMIF Register Values for 6713 DSK
EMIF_GCTL_V     .equ  0x00000078  ;
EMIF_CE0_V      .equ  0xffffbf33  ;EMIF CE0 SDRAM
EMIF_CE1_V      .equ  0x02208802  ;EMIF CE1 Flash 8-bit
EMIF_CE2_V      .equ  0x22a28a22  ;EMIF CE2 Daughtercard 32-bit async
EMIF_CE3_V      .equ  0x22a28a22  ;EMIF CE3 Daughtercard 32-bit async
EMIF_SDRAMCTL_V .equ  0x47115000  ;EMIF SDRAM control
EMIF_SDRAMTIM_V .equ  0x00000578  ;SDRAM timing (refresh)
EMIF_SDRAMEXT_V .equ  0x000a8529  ;SDRAM extended control

COPY_TABLE      .equ  0x90000400

            .sect ".boot_load"
            .global _boot

_boot:      
;************************************************************************
;* DEBUG LOOP -  COMMENT OUT B FOR NORMAL OPERATION
;************************************************************************

            zero B1
_myloop:  ; [!B1] B _myloop  
            nop  5
_myloopend: nop

;************************************************************************
;* CONFIGURE EMIF
;************************************************************************

        ;****************************************************************
        ; *EMIF_GCTL = EMIF_GCTL_V;
        ;****************************************************************

            mvkl  EMIF_GCTL,A4    
      ||    mvkl  EMIF_GCTL_V,B4

            mvkh  EMIF_GCTL,A4
      ||    mvkh  EMIF_GCTL_V,B4

            stw   B4,*A4

        ;****************************************************************
        ; *EMIF_CE0 = EMIF_CE0_V
        ;****************************************************************

            mvkl  EMIF_CE0,A4       
      ||    mvkl  EMIF_CE0_V,B4     

            mvkh  EMIF_CE0,A4
      ||    mvkh  EMIF_CE0_V,B4

            stw   B4,*A4

        ;****************************************************************
        ; *EMIF_CE1 = EMIF_CE1_V (setup for 8-bit async)
        ;****************************************************************

            mvkl  EMIF_CE1,A4       
      ||    mvkl  EMIF_CE1_V,B4

            mvkh  EMIF_CE1,A4
      ||    mvkh  EMIF_CE1_V,B4

            stw   B4,*A4
        
        ;****************************************************************
        ; *EMIF_CE2 = EMIF_CE2_V (setup for 32-bit async)
        ;****************************************************************

            mvkl  EMIF_CE2,A4       
      ||    mvkl  EMIF_CE2_V,B4

            mvkh  EMIF_CE2,A4
      ||    mvkh  EMIF_CE2_V,B4

            stw   B4,*A4

        ;****************************************************************
        ; *EMIF_CE3 = EMIF_CE3_V (setup for 32-bit async)
        ;****************************************************************

      ||    mvkl  EMIF_CE3,A4    
      ||    mvkl  EMIF_CE3_V,B4     ;

            mvkh  EMIF_CE3,A4
      ||    mvkh  EMIF_CE3_V,B4

            stw   B4,*A4

        ;****************************************************************
        ; *EMIF_SDRAMCTL = EMIF_SDRAMCTL_V
        ;****************************************************************
      ||    mvkl  EMIF_SDRAMCTL,A4      
      ||    mvkl  EMIF_SDRAMCTL_V,B4    ;

            mvkh  EMIF_SDRAMCTL,A4
      ||    mvkh  EMIF_SDRAMCTL_V,B4

            stw   B4,*A4

        ;****************************************************************
        ; *EMIF_SDRAMTIM = EMIF_SDRAMTIM_V
        ;****************************************************************
      ||    mvkl  EMIF_SDRAMTIM,A4      
      ||    mvkl  EMIF_SDRAMTIM_V,B4    ;

            mvkh  EMIF_SDRAMTIM,A4
      ||    mvkh  EMIF_SDRAMTIM_V,B4

            stw   B4,*A4

        ;****************************************************************
        ; *EMIF_SDRAMEXT = EMIF_SDRAMEXT_V
        ;****************************************************************
      ||    mvkl  EMIF_SDRAMEXT,A4      
      ||    mvkl  EMIF_SDRAMEXT_V,B4    ;

            mvkh  EMIF_SDRAMEXT,A4
      ||    mvkh  EMIF_SDRAMEXT_V,B4

            stw   B4,*A4


;****************************************************************************
; copy sections
;****************************************************************************
        mvkl  COPY_TABLE, a3 ; load table pointer
        mvkh  COPY_TABLE, a3

        ldw   *a3++, b1     ; Load entry point

copy_section_top:
        ldw   *a3++, b0     ; byte count 
        ldw   *a3++, a4     ; ram start address
        nop   3

 [!b0]  b copy_done         ; have we copied all sections?
        nop   5

copy_loop:
        ldb   *a3++,b5
        sub   b0,1,b0       ; decrement counter
 [ b0]  b     copy_loop     ; setup branch if not done
 [!b0]  b     copy_section_top
        zero  a1
 [!b0]  and   3,a3,a1
        stb   b5,*a4++
 [!b0]  and   -4,a3,a5      ; round address up to next multiple of 4
 [ a1]  add   4,a5,a3       ; round address up to next multiple of 4

;****************************************************************************
; jump to entry point
;****************************************************************************
copy_done:
        b    .S2 b1
        nop   5
