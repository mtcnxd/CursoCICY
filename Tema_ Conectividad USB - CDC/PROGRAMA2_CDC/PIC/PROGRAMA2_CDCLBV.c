//PROGRAMA PARA COMUNICAR MATLAB CON EL ALUX MEDIANTE UN PUERTO SERIE 
//FECHA:  7 JULIO 2011              TARJETA ALUX 1.1
//PROGRAMADO: DEPTO. INSTRUMENTACION
// ING JUAN MANUEL MOLINA AMARO mail   jmolina@cicy.mx
//LUGAR: MERIDA, YUCATÁN.

   #include <18F2550.h>  //DIRECTIVA PARA ESPECIFICAR EL CHIP A UTILIZAR

   #fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL2,CPUDIV1,VREGEN,NOPBADEN  
//FRECUENCIA A CONSIDERAR EN LOS COMANDOS DE RETARDO                                                   
   #use delay(clock=48000000) 
//DIRECTIVAS 
   #include <microchip_bootloader.h>   //lIBRERIA PARA DESCARGAR PROGRAMAS POR LA usb
   #include <usb_cdc.h>       // LIBRERIA PARA COMUNICARSE EN MODO CDC
   #include "micdc.h"
   #BYTE PORTA  = 0x0F80
   #BYTE PORTB =0x0F81 
   #BYTE TRISA  = 0x0F92 
   #BYTE TRISB  = 0x0F93  
   
void main()    //INICIO DE PROGRAMA
{
      char i, c[4], *p;  
       TRISA  = 0x0FF;
       TRISB= 0x00;
       
    usb_init_cs();   //INICIALIZAMOS LAS LLIBREIAS USB 
   
     while(true)
       {
                
            usb_task(); // crea la conexion CDC
                                  
            if (usb_cdc_kbhit())  //si se detecta c. en el bufer
            {
               i=0;             // se hace un while para recibir 3 C
               while(i<3){      // labview esta configurado para que envie 3
               if (usb_cdc_kbhit()){
                  c[i]=usb_cdc_getc(); //proceso de recepcion de datos
                  c[i+1]=0;
                  i++;
               }
               }
              p=c;              // se carga el inicio de los datos en p
              PORTB=(dato_txt_num(p)); // se pasa a la rutina de transformacion
            }                        //  de string a numero y se saca por el 
                                    // puerto B
     // se crea un arreglo de if para muestrear los botones del puerto A                              
          if (!input(pin_A1))
              {
               while(!input(pin_A1));
               delay_ms(100);
               printf(usb_cdc_putc,"  Has presionado A1");
              }
              if (!input(pin_A2))
              {
               while(!input(pin_A2));
               delay_ms(100);
               printf(usb_cdc_putc,"  Has presionado A2");
              }
              if (!input(pin_A3))
              {
               while(!input(pin_A3));
               delay_ms(100);
               printf(usb_cdc_putc,"  Has presionado A3");
              }
              if (!input(pin_A4))
              {
               while(!input(pin_A1));
               delay_ms(100);
               printf(usb_cdc_putc,"  Has presionado A4");
              }
       
         
 }
 usb_detach();
}
