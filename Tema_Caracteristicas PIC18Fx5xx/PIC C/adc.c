/////////////////////////////////////////////////////////////////////////
////                            adc.c                                ////
////                                                                 ////
//// Este sencillo ejemplo muestra la manera de leer datos           ////
//// analógicos usando al adc del PIC                                ////
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
//#device ADC=10                      //Descomente ésta opción en caso de usar el ADC a 10bits
                                    // ... y cambie la variable donde almacena la lectura del ADC a 16 bits
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

int8 dato;

void main(void) {

    //***************  CONFIGURACIÓN DEL ADC   **************   
   setup_adc_ports(AN0);            // Se selecciona AN0 como entrada analógica y las demás como digitales
   setup_adc( VSS_VDD );            // Se indica el rango de voltaje que tendrá la entrada análoga
   set_adc_channel( 0 );            // Indica de que pin se hará la conversion
                                    // Si hace un cambio de pin en el programa pricipal, no olvidar
                                    // esperar 10uS para que la entrada se estabilice con delay_us(10);
   setup_adc( ADC_CLOCK_div_16 );   // Indica la frecuencia que se usará el reloj del ADC ADC_CLOCK_DIV_16

   dato = 0;

   while (TRUE)
   {
            read_adc(ADC_START_ONLY);           // Se inicla la conversión del ADC
            while (bit_test(ADCON0,1));         // Se espera a que el ADC concluya la conversión
            dato = read_adc(ADC_READ_ONLY);     // El ADC concluye la conversión y se almacena el resultado en una variable de 16 bits
            output_b(dato);                     // Se muestra el dato en binario en el puerto b
     }
}

//Paso 3. Compile y grabe en el PIC
