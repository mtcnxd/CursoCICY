//************************APLICACIÓN USB-COM VIRTUAL***********************

//// Realizado con el compilador CCS PCWH 4.099 con Xtal de 8.0 MHz  ////
////                                                                 ////
//// Por: ING. JULIO ALEJANDRO LUGO JIMENEZ    jalugo@cicy.mx        ////
////      ING. GAMALIEL TORRES SÁNCHEZ         gtorres@cicy.mx       ////
////      ING. GABRIEL JESÚS POOL BALAM        gabriel.pool@cicy.mx  ////
////                                                                 ////
////     CENTRO DE INVESTIGACIÓN CIENTÍFICA DE YUCATÁN, A. C.        ////
////        MERIDA, YUCATAN A 4 DE JUNIO DE 2010                  ////
////                                                                 ////
//// ESTE PROGRAMA FUE REALIZADO PARA USARSE CON LA TARJETA DE       ////
//// DESARROLLO PROPIA BASADA EN UN MICROCONTROLADOR PIC18F2455.     ////
//// LA TARJETA CUENTA CON EL SIGUIENTE HARDWARE:                    ////
//// 1. CRISTAL DE 8 MHz.                                            ////
//// 2. POTENCIÓMETRO DE 10 K OHMS CONECTADO EN EL PUERTO A0         ////
//// 3. 5 PULSADORES MINIS (PUSH BUTTON) CONECTADOS DE A1 A A5       ////
////    (HARDWARE OPCIONAL A CONTINUACIÓN)                           ////
//// 4. UN LCD DE 16x2 CONECTADO EN EL PUERTO B COMO SE DESCRIBE EN  ////
////    EL ARCHIVO LCD.C                                             ////
//// 5. 8 LEDS CONECTADOS DE B0 A B7                                 ////
////                                                                 ////
//// ESTE HARDWARE NO NECESITA ALIMENTACIÓN EXTERNA,           ////

//CICY JUNIO DE 2010
//Con este código se lee la entrada análoga A0 cada 500ms y se envía el dato
//por USB-COM Virtual a la PC, que ejecuta una rutina en excel por medio de
//Visual Basic para graficar los datos enviados. Si el número de muestras es mayor
//a 100 entonces despliega "FIN" y se termina la adquisición.

#include <18F2455.h>
#device ADC=10                //configura AD a 10 bit

#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,VREGEN,NOPBADEN
#use delay(clock=48000000)
#include <microchip_bootloader.h>   //COMENTA ESTA LINEA SI NO UTILIZAS BOOTLOADER

//#define use_portb_lcd TRUE

#include <usb_cdc.h>          //configura descriptores para trabajo en CDC
//#include <lcd.c>              //driver del LCD. Configurar pines

void main(void) {

   char c;                       //Variable caracter
   int16 valor;                  //Declara la variable para lectura de AD
   usb_cdc_init();               //Parámetros de puerto COM Virtual Ej.9600,N,8,1
   usb_init();                   //Inicializa USB
//   lcd_init();                   //Inicializa LCD
                                 
   
   setup_adc(ADC_CLOCK_INTERNAL);//Configura parámetros para AD
   setup_adc_ports(AN0);         //Selecciona la(s) terminal(es) Analogica(s)
   set_adc_channel(0);           //Selecciona el canal que se va leer
      
   while (TRUE) {                //Inicia bucle infinito
      if (usb_enumerated()) {       //Revisa si el USB ha sido enumerado
      
         if (usb_cdc_kbhit()) {        //Revisa si hay caracter en el buffer de recepción
            c=usb_cdc_getc();             //en caso afirmativo toma el caracter
            if (c=='1')                   //Revisa si el caracter corresponde a un "1"
            {                             //enviado por la PC
//            printf(lcd_putc,"\f\nFIN");   //En caso afirmativo despliega "FIN" y sale del
            break;                        //bucle infinito, terminando el proceso
            }
         }                               
         valor=read_adc();
         printf(usb_cdc_putc," %Lu",valor);    //Envía por USB el dato leído del AD
//         lcd_gotoxy(1,1);                      //Posiciona cursor de LCD en la 1a línea
//         printf(lcd_putc,"\f%Lu",valor);       // despliega dato en la 1a línea
         delay_ms(500);                        //muestrea cada 500ms la entrada análogica
      
      }
   }
}
