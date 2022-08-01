/////////////////////////////////////////////////////////////////////////
////                            serie.c                              ////
////                                                                 ////
//// Este sencillo ejemplo muestra la comunicación entre dos micro   ////
//// controladores usando el puerto serie (RS-232)                   ////
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
#include <18F2550.h>
#device ADC=10                      //Descomente ésta opción en caso de usar el ADC a 10bits

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
#include "punteros.h"

//  Paso 3: Asigne el puerto B para el display:
#define LCD_DATA_PORT getenv("SFR:PORTB")
#include <lcd.c>                    //Se incluye la librería del LCD 2 lineas X 16 Caracteres

#include <stdlib.h>

#BYTE PORTA  = 0x0F80               // Registro del puerto A
#BYTE ADCON0  = 0x0FC2              // Registro de control del ADC
#BYTE INTCON  = 0x0FF2              // Registro de control del TIMER


#use rs232(UART1, baud=9600, DISABLE_INTS, errors)

char linea1[17]=" BIENVENIDOS!!!";  //DECLARACION DE VARIABLES
union {
  char *car;
  long *car_nulo;
}serie;

boolean Mostrar=FALSE;

#INT_RDA
void recibir(void) {                //Recibe lo que se envia por puerto RS232

      *serie.car_nulo = getc();      //*car_nulo, automaticamente coloca el nulo en la sig posicion.
      serie.car++;                   //apuntar a la siguiente posicion.
      Mostrar=TRUE;                 //Encender la Bandera para mostrar el contenido de s

}

void main(void) {

   char c[50];
   char L1=0;
   int16 dato;

    //***************  CONFIGURACIÓN DEL ADC   **************   
   setup_adc_ports(AN0);            // Se selecciona AN0 como entrada analógica y las demás como digitales
   setup_adc( VSS_VDD );            // Se indica el rango de voltaje que tendrá la entrada análoga
   set_adc_channel( 0 );            // Indica de que pin se hará la conversion
                                    // Si hace un cambio de pin en el programa pricipal, no olvidar
                                    // esperar 10uS para que la entrada se estabilice con delay_us(10);
   setup_adc( ADC_CLOCK_div_16 );   // Indica la frecuencia que se usará el reloj del ADC ADC_CLOCK_DIV_16

    //***************  CONFIGURACIÓN DEL TIMER0   **************   
   setup_timer_0 (T0_INTERNAL|T0_DIV_256);  // Se habilita el timer 0 como TIMER aprox 0.7 seg
   set_timer0(0);                           // Se inicia el contador a cero
                                            // time_int = 4* t1_div_by_128*(65536-set_timer0(val) / val_xtal

   lcd_init();                      //Se inicializa el LCD
   printf(lcd_putc,"\fCURSO CICY  2012"); // El código hexadecimal de '\f' es 0x0C
   printf(lcd_putc,"\n%s",linea1);  // Se escribe en el LCD los carateres correspondientes a la
   
   serie.car=c;                    // Se muestra el dato recibido

    
    //***  Se habilitan interrupciones utilizadas en el puerto serie   ***
   disable_interrupts(INT_TBE);
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   while (TRUE)
   {
      if (Mostrar) {
         Mostrar = FALSE;
         printf(LCD_PUTC,"%s",c);            //Para evitar el parpadeo del LCD.
         serie.car=c;
         *serie.car_nulo=0; 
         }

      if (bit_test(INTCON,2)) {
         read_adc(ADC_START_ONLY);           // Se inicla la conversión del ADC
         while (bit_test(ADCON0,1));         // Se espera a que el ADC concluya la conversión
         dato = read_adc(ADC_READ_ONLY);     // El ADC concluye la conversión y se almacena el resultado en una variable de 16 bits
         if (PORTAbits.B1) 
            printf("\fEl ADC es: %Lu\n Gabriel Pool",dato);   // Se envia la conversión del ADC por el puerto serie
         else
            printf("\fEl ADC es: %Lu",dato);   // Se envia la conversión del ADC por el puerto serie
         bit_clear(INTCON,2);
         //printf("HOLA\r");
      }
}
}
//Paso 3. Compile y grabe en el PIC

