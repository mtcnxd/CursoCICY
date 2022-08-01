//PROGRAMA PARA COMUNICAR LA PC CON EL V DRIVE UTILIZANDO COMO INTERFACE LA TARJETA ALUX 1.0
//PROGRAMÓ: JULIO LUGO
//FECHA: 01 JULIO 2010
//LUGAR: MERIDA, YUCATÁN.

//SI UTILIZAMOS UN PIC18F4550 SUSTITUIR #include <18F2455.h> POR #include<18F4550>
   #include <18F2550.h>                               //DIRECTIVA PARA ESPECIFICAR EL CHIP A UTILIZAR
   #fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL2,CPUDIV1,VREGEN,NOPBADEN   //FUSIBLES
   
//DEFINIMOS LA CANTIDAD DE BITS UTILIZADOS POR EL ADC
   #device ADC=8
   
//FRECUENCIA A CONSIDERAR EN LOS COMANDOS DE RETARDO                                                   
   #use delay(clock=48000000)

//DIRECTIVAS PARA CARGA CON EL BOOTLOADER
   #include <usb_bootloader.h>
   
   //#use    RS232  (UART1, BAUD=9600, PARITY=N, BITS=8, STOP=1, DISABLE_INTS )  //SIN DISABLE INTS SE ATORA DESPUES DE RECIBIR EL 1ER CARACTER
   #use rs232(UART1, baud=9600, DISABLE_INTS, errors)
   
   #include <usb_desc_cdc.h>
   #include <usb_cdc.h>


void main(void) 
{
   char c;
       output_low(PIN_C0);     //Reset DF-Bluetooth a VCC.
      delay_ms(2000);
      output_high(PIN_C0);     //Reset DF-Bluetooth a VCC.
   
      usb_init_cs();
   
      while (TRUE)    {
      usb_task();

      if (kbhit())        {
         c=getc();
         usb_cdc_putc(c);
      }
      
      if (usb_cdc_kbhit()) {
         c=usb_cdc_getc();
         putc(c);
      }
   }
}
