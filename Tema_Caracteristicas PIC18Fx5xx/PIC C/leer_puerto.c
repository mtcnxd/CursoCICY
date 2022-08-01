/////////////////////////////////////////////////////////////////////////
////                            Puertos.c                            ////
////                                                                 ////
//// Este sencillo ejemplo muestra la manera de leer y sacar datos   ////
//// digitales en los puertos del PIC                                ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Realizado con el compilador CCS PCWH 4.104                      ////
//// Este programa fue desarrollado para estimular el uso del        ////
//// puerto USB y desarrollar tarjetas de adquisición económicas     ////
////                                                                 ////
////                  C U R S O   2 0 1 2   C I C Y                  ////
////                                                                 ////
//// Impartido por Centro de Investigación Científica de Yucatán AC  ////
//// Depto de Instrumentación ( www.cicy.mx) por los Ings.           ////
////                                                                 ////
//// Lugo Jimenez Julio  jalugo@cicy.mx                              ////
//// Molina Amaro Juan   jmolina@cicy.mx                              ////
//// Pool Balam Gabriel  gabriel.pool@cicy.mx                        ////
//// Torres Sanchez Gamaliel  gtorres@cicy.mx                        ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////

// Por favor, complete los 3 pasos siguientes ...
// Paso 1:
// Cambie la siguiente instrucción de acuerdo al PIC que utilice PIC18F2455/2550/4455/4550
#include <18F2455.h>

// No modificar los fusibles por compatibilidad del bootloader
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL2,CPUDIV1,VREGEN,MCLR,NOPBADEN
// Paso 2: Ajuste el fusible del PLL de acuerdo al Xtal que utilice
// No olvide que PLL1 = Para un Xtal de 4Mhz
//               PLL2 = Para un Xtal de 8Mhz
//               PLL3 = Para un Xtal de 12Mhz
//               PLL4 = Para un Xtal de 16Mhz
//               PLL5 = Para un Xtal de 20Mhz , etc.

#use delay(clock=48000000)
#include "microchip_bootloader.h"  //Si NO está usando el bootloader en el PIC es IMPORTANTE que comente
                                    //ésta opción, de lo contrario el programa NO FUNCIONA.
int8 dato;

void main(void) {

dato = 0;

   while (TRUE)
   {
      dato = input_a ();
      output_b(dato);

      if (dato == 0x1f) {output_high(pin_c1);output_high(pin_c2);} else {output_low(pin_c1);output_low(pin_c2);}
      if (!input(pin_a1)){output_high(pin_c1);} else {output_low(pin_c1);}
      if (!input(pin_a3)){output_high(pin_c2);} else {output_low(pin_c2);}
      
     }
}

//Paso 3: Compile y grabe en el PIC
