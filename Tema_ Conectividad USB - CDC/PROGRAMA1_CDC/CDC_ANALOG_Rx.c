//************************APLICACI�N USB-COM VIRTUAL***********************

//// Realizado con el compilador CCS PCWH 4.099 con Xtal de 8.0 MHz  ////
////                                                                 ////
//// Por: ING. JULIO ALEJANDRO LUGO JIMENEZ    jalugo@cicy.mx        ////
////      ING. GAMALIEL TORRES S�NCHEZ         gtorres@cicy.mx       ////
////      ING. GABRIEL JES�S POOL BALAM        gabriel.pool@cicy.mx  ////
////                                                                 ////
////     CENTRO DE INVESTIGACI�N CIENT�FICA DE YUCAT�N, A. C.        ////
////        MERIDA, YUCATAN A 4 DE JUNIO DE 2010                  ////
////                                                                 ////
//// ESTE PROGRAMA FUE REALIZADO PARA USARSE CON LA TARJETA DE       ////
//// DESARROLLO PROPIA BASADA EN UN MICROCONTROLADOR PIC18F2455.     ////
//// LA TARJETA CUENTA CON EL SIGUIENTE HARDWARE:                    ////
//// 1. CRISTAL DE 8 MHz.                                            ////
//// 2. POTENCI�METRO DE 10 K OHMS CONECTADO EN EL PUERTO A0         ////
//// 3. 5 PULSADORES MINIS (PUSH BUTTON) CONECTADOS DE A1 A A5       ////
////    (HARDWARE OPCIONAL A CONTINUACI�N)                           ////
//// 4. UN LCD DE 16x2 CONECTADO EN EL PUERTO B COMO SE DESCRIBE EN  ////
////    EL ARCHIVO LCD.C                                             ////
//// 5. 8 LEDS CONECTADOS DE B0 A B7                                 ////
////                                                                 ////
//// ESTE HARDWARE NO NECESITA ALIMENTACI�N EXTERNA,           ////

//CICY JUNIO DE 2010
//Con este c�digo se lee la entrada an�loga A0 cada 500ms y se env�a el dato
//por USB-COM Virtual a la PC, que ejecuta una rutina en excel por medio de
//Visual Basic para graficar los datos enviados. Si el n�mero de muestras es mayor
//a 100 entonces despliega "FIN" y se termina la adquisici�n.

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
   usb_cdc_init();               //Par�metros de puerto COM Virtual Ej.9600,N,8,1
   usb_init();                   //Inicializa USB
//   lcd_init();                   //Inicializa LCD
                                 
   
   setup_adc(ADC_CLOCK_INTERNAL);//Configura par�metros para AD
   setup_adc_ports(AN0);         //Selecciona la(s) terminal(es) Analogica(s)
   set_adc_channel(0);           //Selecciona el canal que se va leer
      
   while (TRUE) {                //Inicia bucle infinito
      if (usb_enumerated()) {       //Revisa si el USB ha sido enumerado
      
         if (usb_cdc_kbhit()) {        //Revisa si hay caracter en el buffer de recepci�n
            c=usb_cdc_getc();             //en caso afirmativo toma el caracter
            if (c=='1')                   //Revisa si el caracter corresponde a un "1"
            {                             //enviado por la PC
//            printf(lcd_putc,"\f\nFIN");   //En caso afirmativo despliega "FIN" y sale del
            break;                        //bucle infinito, terminando el proceso
            }
         }                               
         valor=read_adc();
         printf(usb_cdc_putc," %Lu",valor);    //Env�a por USB el dato le�do del AD
//         lcd_gotoxy(1,1);                      //Posiciona cursor de LCD en la 1a l�nea
//         printf(lcd_putc,"\f%Lu",valor);       // despliega dato en la 1a l�nea
         delay_ms(500);                        //muestrea cada 500ms la entrada an�logica
      
      }
   }
}
