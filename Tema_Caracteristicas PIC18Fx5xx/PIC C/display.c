/////////////////////////////////////////////////////////////////////////
////                            display.c                            ////
////                                                                 ////
//// Este sencillo ejemplo muestra la manera de manejar el display   ////
//// de dos lineas por 16 caracteres                                 ////
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
#BYTE PORTA  = 0x0F80               // Registro del puerto A

char linea1[17]=" BIENVENIDOS!!!";  //DECLARACION DE VARIABLES
boolean act_lcd;
int8 dato;


void main(void) {


   lcd_init();                      //Se inicializa el LCD
   printf(lcd_putc,"\f CURSO CICY 2012"); 
   printf(lcd_putc,"\n%s",linea1);  // Se escribe en el LCD los carateres correspodientes a la

   dato = 0;
   
   while (TRUE)
   {
      if (!bit_test(PORTA,5)) {dato++; delay_ms(50); while (!bit_test(PORTA,5)); act_lcd = 1;}
      // Para más detalles del comando lcd_putc consulte los comentarios en C:\Archivos de programa\PICC\Drivers\LCD.c
      // Para màs detalles del comando printf consulte ccs_c_manual.pdf (pag 210) ubicado en la carpeta del tema 6
      if (act_lcd) { printf(lcd_putc,"\nCuenta %3u veces",dato); act_lcd= false;}  // Se escribe en el LCD los carateres

     }
}
//Paso 3. Compile y grabe en el PIC

