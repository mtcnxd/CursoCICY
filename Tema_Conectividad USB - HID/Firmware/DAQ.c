/////////////////////////////////////////////////////////////////////////
////                            DAQ.c                                ////
////                                                                 ////
//// Este ejemplo muestra como desarrollar un sencillo dispositivo   ////
//// USB con el PIC18F2550 (para la serie 18Fxx5x)                   ////
////                                                                 ////
//// Este ejemplo se limita a recibir un paquete de 64 bytes de la   ////
//// PC, mostrar en el puerto b el dato [0],                         ////
//// el PWM1 en dato [4] y dato [5] y PWM2 en dato [6] y dato [7]    ////
//// introducir en el                                                ////
//// arreglo la informacion que se recibe del puerto A en dato [1] y ////
//// del ADC configurado a 10 bits en dato [2] y dato [3]            ////
//// y devolver el paquete a la PC                                   ////
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
//// Molina Amaro Juan Manuel  jmolina@cicy.mx                       ////
//// Pool Balam Gabriel  gabriel.pool@cicy.mx                        ////
//// Torres Sanchez Gamaliel  gtorres@cicy.mx                        ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////

// Por favor, complete los 3 pasos siguientes ...

// Paso 1:
// Cambie la siguiente instrucción de acuerdo al PIC que utilice PIC18F2455/2550/4455/4550
#include <18F2455.h>
#device ADC=10                      //Descomente ésta opción en caso de usar el ADC a 10bits
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL2,CPUDIV1,VREGEN,MCLR,NOPBADEN

// Paso 2: Ajuste el fusible del PLL de acuerdo al Xtal que utilice
// No olvide que PLL1 = Para un Xtal de 4Mhz
//               PLL2 = Para un Xtal de 8Mhz
//               PLL3 = Para un Xtal de 12Mhz
//               PLL4 = Para un Xtal de 16Mhz
//               PLL5 = Para un Xtal de 20Mhz , etc.
#use delay(clock=48000000)
//#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#include <pic18_usb.h>              //Microchip PIC18Fxx5x Hardware layer for CCS's PIC USB driver
#include "usb_desc_hid.h"         //descriptors del Pic USB
#include <usb.c>                    //handles usb setup tokens and get descriptor reports
#include <microchip_bootloader.h>         //Si NO está usando el bootloader en el PIC es IMPORTANTE que comente
                                    //ésta opción, de lo contrario el programa NO FUNCIONA.

#define  LEDV    PIN_B6             // Se cambia el nombre por uno más sencillo de recordar
#define  LEDR    PIN_B7
#define  LED_ON  output_high
#define  LED_OFF output_low

// Direcciones de memoria válidas para PIC18F2455/2550/4455/4550 (no olvide que son la misma familia)
// Esto es con el fin de poder escribir directamente en ellos sin usar instrucciones como intermediarios.
#BYTE TRISA  = 0x0F92               // Registro de control de E/S del puerto A
#BYTE TRISB  = 0x0F93               // Registro de control de E/S del puerto B
#BYTE PORTA  = 0x0F80               // Registro del puerto A
#BYTE PORTB  = 0x0F81               // Registro del puerto B
#BYTE ADCON0  = 0x0FC2              // Registro de control del ADC
#BYTE ADCON1  = 0x0FC1              // Registro de control del ADC
#BYTE CMCON   = 0x0FB4              // Registro del modulo comparador

int8 dato[64];
unsigned int16 entrada,valor_pwm1, valor_pwm2;

void main(void) {

   LED_ON(LEDV);                    //encendemos led en RB6 para indicar presencia de energia
   LED_OFF(LEDR);

   usb_init();                      // inicializamos el USB
   usb_task();                      // habilita periferico usb e interrupciones
   usb_wait_for_enumeration();      // esperamos hasta que el PicUSB sea configurado por el host

   LED_OFF(LEDV);
   LED_ON(LEDR);                    // encendemos led en RB7 al establecer contacto con la PC

   TRISA  = 0x0FF;                  // Se declara el puerto A como entradas (instrucción opcional)
   TRISB  = 0x00;                   // Se declara el puerto B como salidas (instrucción opcional)
   ADCON1 = 0x0F;                   // Se configura al ADC para entradas digitales (apagar el ADC)
   CMCON  = 0x07;                   // Se configuran los comparadores para entradas digitales (apagar los comparadores)

    //***************  CONFIGURACIÓN DEL ADC   **************   
   setup_comparator(NC_NC_NC_NC);   // Se configuran los comparadores para entradas digitales (apagar los comparadores, igual que instrucción anterior)
   setup_adc_ports(AN0);            // Se selecciona AN0 como entrada analógica y las demás como digitales
   setup_adc( VSS_VDD );            // Se indica el rango de voltaje que tendrá la entrada análoga
   set_adc_channel( 0 );            // Indica de que pin se hará la conversion
   setup_adc( ADC_CLOCK_div_16 );   // Indica la frecuencia que se usará el reloj del ADC ADC_CLOCK_DIV_16

    //***************  CONFIGURACIÓN DEL PWM   **************   
   setup_ccp1(CCP_PWM);             // Configure CCP1 as a PWM
   setup_ccp2(CCP_PWM);             // Configure CCP2 as a PWM
   setup_timer_2(T2_DIV_BY_4, 127, 1);    // The cycle time will be (1/clock)*4*t2div*(period+1)
                                    // In this program clock=48000000 and period=127
   valor_pwm1 = 0;                  // (1/48000000)*4*4*128 =  42.66 uS or 23.437 Khz
   set_pwm1_duty(valor_pwm1);       // Se ajusta el pwm1 al valor mínimo
   valor_pwm2 = 0;
   set_pwm2_duty(valor_pwm2);       // Se ajusta el pwm2 al valor mínimo

   while (TRUE)
   {
      if(usb_enumerated())          // si el Pic está configurado via USB
      {
         if (usb_kbhit(1))          // si el endpoint de salida contiene datos del host
         {
            usb_get_packet(1, dato, 64);        // cojemos el paquete de tamaño 8bytes del EP1 y almacenamos en dato
            portb = dato[0];                    // Se muestra al byte completo en el puerto B
            valor_pwm1 = dato[5];
            valor_pwm1 = valor_pwm1 << 8;
            valor_pwm1 = valor_pwm1 + dato[4];  
            set_pwm1_duty(valor_pwm1);       // Se actualiza el valor del PWM1
            valor_pwm2 = dato[7];
            valor_pwm2 = valor_pwm2 << 8;
            valor_pwm2 = valor_pwm2 + dato[6];  
            set_pwm2_duty(valor_pwm2);       // Se actualiza el valor del PWM2
            dato[1] = porta;                    // Se lee el puerto A y se almacena en el vector
            read_adc(ADC_START_ONLY);           // Se inicla la conversión del ADC
            while (bit_test(ADCON0,1));         // Se espera a que el ADC concluya la conversión
            entrada = read_adc(ADC_READ_ONLY);  // El ADC concluye la conversión y se almacena el resultado en una variable de 16 bits
            dato[2]= make8(entrada,0);          // Se divide el dato en 2 partes de 8 bits y se almacena la parte baja en una localidad del arreglo
            dato[3] = make8(entrada,1);         // Se divide el dato en 2 partes de 8 bits y se almacena la parte alta en una localidad del arreglo
            usb_put_packet(1, dato, 64, USB_DTS_TOGGLE); //y enviamos el mismo paquete de tamaño 64bytes del EP1 al PC
         }
       }
    }
}

//Paso 3: Compile y grabe en el PIC
