/////////////////////////////////////////////////////////////////////////
////                            pwm.c                                ////
////                                                                 ////
//// Este sencillo ejemplo muestra la manera de manejar los mòdulos  ////
//// del pwm del PIC                                                 ////
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

// Por favor, complete los 4 pasos siguientes ...
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
#BYTE ADCON0  = 0x0FC2              // Registro de control del ADC

int16 valor_pwm;
boolean inc;
int1 rojo;

void main(void) {

    //***************  CONFIGURACIÓN DEL PWM   **************   
   setup_ccp1(CCP_PWM);             // Configure CCP1 as a PWM
   setup_ccp2(CCP_PWM);             // Configure CCP2 as a PWM
   setup_timer_2(T2_DIV_BY_4, 127, 1);    // The cycle time will be (1/clock)*4*t2div*(period+1)
                                    // In this program clock=48000000 and period=127
                                    // (1/48000000)*4*4*128 =  42.66 uS or 23.437 Khz

   valor_pwm = 0;                   // Determina el valor de salida que se carga el modulo pwm
   rojo = 0;                        // Determina si se activa pwm1 ó pwm2
   inc = true;                      // Determina si el valor de salida se incrementa ó decrementa
   
   while (TRUE)
   {
      if (valor_pwm==0){inc=true; rojo++;}
      if (valor_pwm==512) {inc=false;}

      if (inc) {valor_pwm++;} else {valor_pwm--;}                              // Se incrementa/decrementa el valor de salida
      if (rojo) {set_pwm1_duty(valor_pwm);} else  {set_pwm2_duty(valor_pwm);}  // Se ajusta el pwm1 ó pwm2 al valor indicado

      delay_ms(4);               // Se espera un lapso de tiempo para ver la salida
      
     }
}
//Paso 3. Compile y grabe en el PIC

