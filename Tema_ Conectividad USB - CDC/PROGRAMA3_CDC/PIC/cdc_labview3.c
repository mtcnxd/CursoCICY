//PROGRAMA PARA COMUNICAR LABVIEM CON EL ALUX MEDIANTE UN PUERTO SERIE 
//FECHA:  7 JULIO 2011              TARJETA ALUX 1.1
//PROGRAMADO: DEPTO. INSTRUMENTACION
// ING JUAN MANUEL MOLINA AMARO mail   jmolina@cicy.mx
// En ESTE EJERCICIO PASAMOS LA RUTINA OBTENER_DATO A UN ARCHIVO MICDC.H
// MANEJANDO EN ESTA FORMA PUNTEROS
//LUGAR: MERIDA, YUCATÁN.

   #include <18f2550.h>  //DIRECTIVA PARA ESPECIFICAR EL CHIP A UTILIZAR
   #fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL2,CPUDIV1,VREGEN,NOPBADEN  
   #device ADC=8                //configura AD a 8 bit
   #use delay(clock=48000000) //FRECUENCIA A CONSIDERAR EN LOS COMANDOS DE RETARDO                                              
 //DIRECTIVAS 
   #include <microchip_bootloader.h>
   #include <lcd.c>
   #define LCD_DATA_PORT getenv("SFR:PORTB")
   #include <usb_desc_cdc.h>   //DESCRIPTOR
   #include <usb_cdc.h>       // LIBRERIA PARA COMUNICARSE EN MODO CDC
   #include "micdc.h"
   #BYTE PORTA  = 0x0F80               // Registro del puerto A
   
   char dato[35],d[4],*di;
   
void main(void)    //INICIO DE PROGRAMA
{
    
    
    char d1,d2,c;
    int8 pot=0,pot2=0;
    lcd_init();
 
   setup_ccp2(CCP_PWM);             // Configure CCP2 as a PWM
   setup_timer_2(T2_DIV_BY_16, 64, 1);    // The cycle time will be (1/clock)*4*t2div*(period+1)
                                    // In this program clock=48000000 and period=127
                                    // In this program clock=48000000 and period=127
                                    // (1/48000000)*4*4*128 =  42.66 uS or 23.437 Khz
   usb_init_cs();   //INICIALIZAMOS LAS LLIBREIAS USB 
   setup_adc(ADC_CLOCK_INTERNAL);  
   setup_adc_ports(AN0);
   printf(lcd_putc,"Bienvenidos!!!");  // desplegamos las lecturas de los pots

      
     while(true)
       {
         delay_ms(1000);      // rutina para prender el led C2 cuando se desconecta de la pc
         output_high(PIN_c2);
         delay_ms(1000);

         while(input(pin_A3)||(input(pin_A1)))   //condición que desconecta el puerto cdc
             {
              usb_task(); 
              output_low(PIN_c2);
              if (USB_CDC_kbhit())           //condición que indica que se ha recibido un caracter
                {
                 Obtener_Dato_CDC(dato);        // rutina loclizada micdc.h encargada de obtener los caracteres de la pc
                 di=&dato[0];
                 d1= dato_txt_num(di);
                 di=&dato[3];
                 d2=dato_txt_num(di);
           
                 set_pwm2_duty(d2);
                 lcd_gotoxy(1,1);
                 printf(lcd_putc,"               ");
                 lcd_gotoxy(1,1);
                  printf(lcd_putc,"   %u    %u",d1,d2);
               
             setup_adc_ports(AN0_TO_AN4);     // se configuran los pines desde A0 hasta A4 como
             set_adc_channel(0);          // entradas Analógicas seleccionamos el canal 0 (A0)   
             pot=read_adc();            // se lee A0 y se guarda la captura en pot
             delay_ms(10);
             set_adc_channel(4);        //se seleciona el canal 4 (A4) y se lee guardando 
             pot2=read_adc();           //el resultado en pot2
             delay_ms(10);
             setup_adc_ports(AN0);     // con esta instrucción liberamos los pines del puerto A excepto
             c=porta;                      // el A0. Para poder usar el teclado de A1 hasta A4
               //  itoa(c,10,d);
               //   c&&0x3F;
            printf(usb_cdc_putc,"%03u%03u%03u\r\n",pot,pot2,c); // se manda a la pc Las lecturas de los dos pot y del puertoA
            delay_ms(10);
            lcd_gotoxy(1,2);        // seleccionamos la segunda fila del display
            printf(lcd_putc,"%03u   %03u   %03u",pot,pot2, c);  // desplegamos las lecturas de los pots
       }
             }
         usb_detach();
 }
}
