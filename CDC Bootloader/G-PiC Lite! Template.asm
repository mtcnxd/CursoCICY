;******************************************************************************
;   This file is a basic template for assembly code for a PIC18F4550. Copy    *
;   this file into your project directory and modify or add to it as needed.  *
;                                                                             *
;   Refer to the MPASM User's Guide for additional information on the         *
;   features of the assembler.                                                *
;                                                                             *
;   Refer to the PIC18Fx455/x550 Data Sheet for additional                    *
;   information on the architecture and instruction set.                      *
;                                                                             *
;******************************************************************************
;                                                                             *
;    Filename:                                                                *
;    Date:                                                                    *
;    File Version:                                                            *
;                                                                             *
;    Author:                                                                  *
;    Company:                                                                 *
;                                                                             * 
;******************************************************************************
;                                                                             *
;    Files Required: P18F4550.INC                                             *
;                                                                             *
;******************************************************************************

	LIST P=18F4550		;directive to define processor
	#include <P18F4550.INC>	;processor specific variable definitions

;******************************************************************************
;Configuration bits
;Microchip has changed the format for defining the configuration bits, please 
;see the .inc file for futher details on notation.  Below are a few examples.


;   Oscillator Selection:
	 PLLDIV = 5           ;Divide by 5 (20 MHz oscillator input)

;   System Clock Postscaler Selection bits:
     CPUDIV = OSC1_PLL2   ;[Primary Oscillator Src: /1][96 MHz PLL Src: /2]

;   USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1):
     USBDIV = 2           ;USB clock source comes from the 96 MHz PLL divided by 2

;   Oscillator Selection bits:
     FOSC = HSPLL_HS      ;HS oscillator, PLL enabled (HSPLL)

;   Brown-out Reset Enable bits:
     BOR = OFF            ;Brown-out Reset disabled in hardware and software

;   USB Voltage Regulator Enable bit:
     VREGEN = ON          ;USB voltage regulator enabled
;
;   Watchdog Timer Enable bit:
     WDT = OFF            ;WDT disabled (control is placed on the SWDTEN bit)

;   PORTB A/D Enable bit:
     PBADEN = OFF         ;PORTB<4:0> pins are configured as digital I/O on Resets on Reset

;   MCLR Pin Enable bit:
     MCLRE = ON           ;MCLR pin enabled; RE3 input pin disabled

;   Single-Supply ICSP Enable bit:
     LVP = OFF            ;Single-Supply ICSP disabled

;   Background Debugger Enable bit:
     DEBUG = OFF          ;Background debugger disabled, RB6 and RB7 configured as general purpose I/O pins

;   Code Protection bit:
     CP0 = OFF            ;Block 0 (000800-001FFFh) is not code-protected

;   Code Protection bit:
     CP1 = OFF            ;Block 1 (002000-003FFFh) is not code-protected

;   Code Protection bit:
     CP2 = OFF            ;Block 2 (004000-005FFFh) is not code-protected

;   Code Protection bit:
     CP3 = OFF            ;Block 3 (006000-007FFFh) is not code-protected

;   Boot Block Code Protection bit:
     CPB = ON             ;Boot block (000000-0007FFh) is code-protected

;   Data EEPROM Code Protection bit:
     CPD = OFF            ;Data EEPROM is not code-protected

;   Write Protection bit:
     WRT0 = OFF           ;Block 0 (000800-001FFFh) is not write-protected

;   Write Protection bit:
     WRT1 = OFF           ;Block 1 (002000-003FFFh) is not write-protected

;   Write Protection bit:
     WRT2 = OFF           ;Block 2 (004000-005FFFh) is not write-protected

;   Write Protection bit:
     WRT3 = OFF           ;Block 3 (006000-007FFFh) is not write-protected
;
;   Configuration Register Write Protection bit:
     WRTC = OFF           ;Configuration registers (300000-3000FFh) are not write-protected

;   Boot Block Write Protection bit:
     WRTB = OFF           ;Boot block (000000-0007FFh) is not write-protected

;   Data EEPROM Write Protection bit:
     WRTD = OFF           ;Data EEPROM is not write-protected
	

;******************************************************************************
;Variable definitions
; These variables are only needed if low priority interrupts are used. 
; More variables may be needed to store other special function registers used
; in the interrupt routines.

		CBLOCK	0x080
		WREG_TEMP	;variable used for context saving 
		STATUS_TEMP	;variable used for context saving
		BSR_TEMP	;variable used for context saving
		ENDC

		CBLOCK	0x000
		EXAMPLE		;example of a variable in access RAM
		ENDC

;******************************************************************************
;EEPROM data
; Data to be programmed into the Data EEPROM is defined here

		ORG	0xf00000

		DE	"Test Data",0,1,2,3,4,5

;******************************************************************************
;Reset vector
; This code will start executing when a reset occurs.

		ORG	0x0000

		goto	0x2000		;go to start of bootloader main code
		
		ORG 0x2000
		
		goto Main:			;go to start of main code
		

;******************************************************************************
;High priority interrupt vector
; This code will start executing when a high priority interrupt occurs or
; when any interrupt occurs if interrupt priorities are not enabled.

		ORG	0x0008
		
		goto	0x2008
		
		ORG	0x2008
		
		bra	HighInt		;go to high priority interrupt routine

;******************************************************************************
;Low priority interrupt vector and routine
; This code will start executing when a low priority interrupt occurs.
; This code can be removed if low priority interrupts are not used.

		ORG	0x0018
		
		goto	0x2018
		
		ORG 0x2018

		movff	STATUS,STATUS_TEMP	;save STATUS register
		movff	WREG,WREG_TEMP		;save working register
		movff	BSR,BSR_TEMP		;save BSR register

;	*** low priority interrupt code goes here ***


		movff	BSR_TEMP,BSR		;restore BSR register
		movff	WREG_TEMP,WREG		;restore working register
		movff	STATUS_TEMP,STATUS	;restore STATUS register
		retfie

;******************************************************************************
;High priority interrupt routine
; The high priority interrupt code is placed here to avoid conflicting with
; the low priority interrupt vector.

HighInt:

;	*** high priority interrupt code goes here ***


		retfie	FAST

;******************************************************************************
;Start of main program
; The main program code is placed here.

Main:

;	*** main code goes here ***


;******************************************************************************
;End of program

		END
