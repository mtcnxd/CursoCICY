
//DEFINICIÓN DE OTRAS DIRECTIVAS
   #use fast_io(a)                                    //PARA QUE EL COMPILADOR NO GENERE CÓDIGO ADICIONAL EN LLAMADAS AL PTO A
   #use fast_io(b)                                    //PARA QUE EL COMPILADOR NO GENERE CÓDIGO ADICIONAL EN LLAMADAS AL PTO B

//DEFINICION DE UNA  ESTRUCTURA
typedef union {                                            //SE DEFINE UNA UNION PARA PODER MANEJAR UNA MISMA VARIABLE
   unsigned char _byte;                            //POR BYTE Y TAMBIEN POR BIT INDIVIDUAL
   struct {
     short _0:1;
     short _1:1;
     short _2:1;
     short _3:1;
     short _4:1;
     short _5:1;
     short _6:1;
     short _7:1;
   };
}DUAL;

DUAL PORTA,PORTB,PORTC;

#byte PORTA = 0xF80                                //SE ASIGNA FORZA A PORTA A TOMAR LA DIR DE 0xF80 (PUERTO A EN EL PIC18F2455) 
#byte PORTB = 0xF81                                //SE FORZA A PORTB A TOMAR LA DIR DE 0xF81        (PUERTO B EN EL PIC18F2455) 
#byte PORTC = 0xF82                                //SE FORZA A PORTB A TOMAR LA DIR DE 0xF81        (PUERTO B EN EL PIC18F2455) 
#byte TRISA = 0xF92
#byte TRISB = 0xF93
#byte TRISC = 0xF94
#byte ADCON1 = 0xFC1


void Alux_LCD_Init(void) {

         lcd_init();                                //INICIALIZAMOS EL LCD
}

unsigned int16 Alux_ADC_1(void) {
unsigned int8 BK_TRISA,BK_ADCON1;
      
        BK_TRISA=TRISA;
        BK_ADCON1=ADCON1;
        TRISA=TRISA|0x01;
        setup_adc(ADC_CLOCK_INTERNAL);             //ESTABLECE A INTERNO, EL RELOJ DEL ADC
        setup_adc_ports(AN0);                      //CONFIGURA EL PIN A0 COMO ENTRADA ANALÓGICA (AN0)
        set_adc_channel(0);                        //SELECCIONA EL CANAL AN0
        delay_us(10);
        ADCON1=BK_ADCON1;
        TRISA=BK_TRISA;
        
return  read_adc();
}

unsigned int16 Alux_ADC_2(void) {
unsigned int8 BK_TRISA,BK_ADCON1;

        BK_TRISA=TRISA;
        BK_ADCON1=ADCON1;
        TRISA=TRISA|0x20;
        setup_adc(ADC_CLOCK_INTERNAL);             //ESTABLECE A INTERNO, EL RELOJ DEL ADC
        setup_adc_ports(AN0_TO_AN4);                   //CONFIGURA EL PIN A0 A ENTRADA ANALÓGICA (AN0)
        set_adc_channel(4);                            //SELECCIONA EL CANAL AN4 PARA LA SIGUIENTES LECTURAS
        delay_us(10);
        ADCON1=BK_ADCON1;
        TRISA=BK_TRISA;

return  read_adc();
}

