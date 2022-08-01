////////////////////////////////////////////////////////////////////////
//18F4550Trainer_V1.0 Bootloader - www.microingenia.com     16/02/2010//
//                                                                    //
//Bootloader based on CCS one. Power your Trainer with PIN_C0 to low  //
//and use MiEBootloader Software to burn your code!                   //
//                                                                    //
//Released under the cc by-nc-sa 3.0 License.                         //
//http://creativecommons.org/licenses/by-nc-sa/3.0/                   //
////////////////////////////////////////////////////////////////////////
#include <18F2550.h>
//#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,CPUDIV1,VREGEN,MCLR,NOBROWNOUT,PLL5,PUT,WRTB
  #fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,CPUDIV1,VREGEN,MCLR,PLL2,NOPBADEN
#use delay(clock=48000000)

#define  ON    output_high
#define  OFF   output_low

#define  LEDR  PIN_C2
#define  BTLDR PIN_A2

// Bootloader
// PIN_C0 - GND -> Btld Mode
// PIN_C0 - VCC -> Code Mode
