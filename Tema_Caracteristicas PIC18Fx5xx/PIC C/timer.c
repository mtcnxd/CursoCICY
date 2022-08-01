/////////////////////////////////////////////////////////////////////////
////                            timer.c                              ////
////                                                                 ////
//// Este sencillo ejemplo muestra la manera de manejar los timer    ////
//// como contadores ó para medir el tiempo.                         ////
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

//  Paso 3: Asigne el puerto B para el display:
#define LCD_DATA_PORT getenv("SFR:PORTB")
#include <lcd.c>                    //Se incluye la librería del LCD 2 lineas X 16 Caracteres

#byte    T0CON = 0xFD5
#byte    TMR0L = 0xFD6
#byte    TMR0H = 0xFD7

char linea1[17]=" BIENVENIDOS!!!";  //DECLARACION DE VARIABLES
boolean act_lcd;
int8 dato, segundo;

//***********RECORDAR QUE LOS TIMER INTERRUMPEN CUANDO SU CONTEO PASA DE 0xFFFF A 0x0000**********************

//!#INT_TIMER0                                    // Se podría contar las rayas de un encoder y activar ésta
//!void cuenta()                                  // rutina a manera de bandera cuando el conteo exceda
//!{                                              // la capacidad de los registros del timer de 16 bits
//!
//!   // Escriba su rutina que desee ejecturar cuando timer 0 interrumpa
//!
//!}

#INT_TIMER1                                       // Interrumpe cada 20 uS
void tiempo()
{
   set_timer1(0x8AD0);
   if (segundo == 25) { output_toggle(pin_c1); segundo = 0; act_lcd = true;} else {segundo++;}
}

void main(void) {


   setup_timer_0 (RTCC_DIV_1|RTCC_EXT_H_TO_L);  // Se habilita el timer 0 como contador 
   set_timer0(0);                               // Se inicia el contador a cero


   setup_timer_1 (T1_INTERNAL|T1_DIV_BY_8);     // Se habilita el timer 1 como temporizador
   set_timer1(0x8AD0);                          // Se ajusta el timer1 a 20 mS
                                                // time_int = 4* t1_div_by_8*(65536-set_timer1(val) / val_xtal
                                                // set_timer1(val) = 65536 - (time_int * val_xtal / 4 * t1_div_by_8)

   lcd_init();                      //Se inicializa el LCD
   printf(lcd_putc,"\f CURSO CICY 2012"); 
   printf(lcd_putc,"\n%s",linea1);  // Se escribe en el LCD los carateres correspodientes a la

   dato = 0;
   segundo = 0;
   act_lcd = false;
   TMR0L = 0;

   enable_interrupts(global);                   // Se habilitan interrupciones
   enable_interrupts(int_timer0);
   enable_interrupts(int_timer1);

   
   while (TRUE)
   {
      // Para más detalles del comando lcd_putc consulte los comentarios en C:\Archivos de programa\PICC\Drivers\LCD.c
      // Para màs detalles del comando printf consulte ccs_c_manual.pdf (pag 210) ubicado en la carpeta de bibliografía
      if (act_lcd) { printf(lcd_putc,"\nCuenta %3u veces",TMR0L); act_lcd= false;}  // Se escribe en el LCD los carateres

     }
}
//Paso 3. Compile y grabe en el PIC

