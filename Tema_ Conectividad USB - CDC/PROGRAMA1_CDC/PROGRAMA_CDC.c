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
   #include "microchip_bootloader.h"   //lIBRERIA PARA DESCARGAR PROGRAMAS POR LA usb
   
   #include <usb_cdc.h>       // LIBRERIA PARA COMUNICARSE EN MODO CDC
  
   
void main()    //INICIO DE PROGRAMA
{
      int8  c;  
   //  setup_adc(ADC_CLOCK_INTERNAL);
    
    usb_init_cs();   //INICIALIZAMOS LAS LLIBREIAS USB 
   
     while(true)
       {
         delay_ms(1000);      // rutina para prender el led C2 cuando se desconecta de la pc
         output_high(PIN_c2);
         delay_ms(1000);

         while(input(pin_A3))
         {
            usb_task(); 
            output_low(PIN_c2);
                        
            if (usb_cdc_kbhit())
            {
               c=usb_cdc_getc(); 
              // usb_cdc_get_discard();
               switch(c)
               {
                   case '1':output_toggle(PIN_B0);
                          break;
                    case '2':OUtput_toggle(PIN_B1); 
                        break;
                    case '3':OUtput_toggle(PIN_B2); 
                        break;
                    case '4':OUtput_toggle(PIN_B3); 
                        break;
                     case '5':OUtput_toggle(PIN_B4); 
                        break;
                     case '6':OUtput_toggle(PIN_B5); 
                        break;
                     case '7':OUtput_toggle(PIN_B6); 
                        break;
                     case '8':OUtput_toggle(PIN_B7); 
                        break;
                   
                       
             }
            
          }
          if(!input(PIN_A1))
         {
        //delay_ms(10);
                 while(!input(PIN_A1));
          
          printf(USB_CDC_PUTC,"\r\nHAS PRESIONADO A1");
          delay_ms(100);
          }
          
       }
         usb_detach();
 }
}
