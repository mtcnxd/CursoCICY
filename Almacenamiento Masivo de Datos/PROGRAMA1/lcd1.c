///////////////////////////////////////////////////////////////////////////////
////   LCD.C MODIFICADO POR ING MOLINA  A LCD1.C                                   ////
////                 Driver for common LCD modules                         ////
////                                                                       ////
////  lcd_init()   Must be called before any other function.               ////
////                                                                       ////
////  lcd_putc(c)  Will display c on the next position of the LCD.         ////
////                 \a  Set cursor position to upper left                 ////
////                 \f  Clear display, set cursor to upper left           ////
////                 \n  Go to start of second line                        ////
////                 \b  Move back one position                            ////
////              If LCD_EXTENDED_NEWLINE is defined, the \n character     ////
////              will erase all remanining characters on the current      ////
////              line, and move the cursor to the beginning of the next   ////
////              line.                                                    ////
////              If LCD_EXTENDED_NEWLINE is defined, the \r character     ////
////              will move the cursor to the start of the current         ////
////              line.                                                    ////
////                                                                       ////
////  lcd_gotoxy(x,y) Set write position on LCD (upper left is 1,1)        ////
////                                                                       ////
////  lcd_getc(x,y)   Returns character at position x,y on LCD             ////
////                                                                       ////
////  CONFIGURATION                                                        ////
////  The LCD can be configured in one of two ways: a.) port access or     ////
////  b.) pin access.  Port access requires the entire 7 bit interface     ////
////  connected to one GPIO port, and the data bits (D4:D7 of the LCD)     ////
////  connected to sequential pins on the GPIO.  Pin access                ////
////  has no requirements, all 7 bits of the control interface can         ////
////  can be connected to any GPIO using several ports.                    ////
////                                                                       ////
////  To use port access, #define LCD_DATA_PORT to the SFR location of     ////
////  of the GPIO port that holds the interface, -AND- edit LCD_PIN_MAP    ////
////  of this file to configure the pin order.  If you are using a         ////
////  baseline PIC (PCB), then LCD_OUTPUT_MAP and LCD_INPUT_MAP also must  ////
////  be defined.                                                          ////
////                                                                       ////
////  Example of port access:                                              ////
////     #define LCD_DATA_PORT getenv("SFR:PORTD")                         ////
////                                                                       ////
////  To use pin access, the following pins must be defined:               ////
////     LCD_ENABLE_PIN                                                    ////
////     LCD_RS_PIN                                                        ////
////     LCD_RW_PIN                                                        ////
////     LCD_DATA4                                                         ////
////     LCD_DATA5                                                         ////
////     LCD_DATA6                                                         ////
////     LCD_DATA7                                                         ////
////                                                                       ////
////  Example of pin access:                                               ////
////     #define LCD_ENABLE_PIN  PIN_E0                                    ////
////     #define LCD_RS_PIN      PIN_E1                                    ////
////     #define LCD_RW_PIN      PIN_E2                                    ////
////     #define LCD_DATA4       PIN_D4                                    ////
////     #define LCD_DATA5       PIN_D5                                    ////
////     #define LCD_DATA6       PIN_D6                                    ////
////     #define LCD_DATA7       PIN_D7                                    ////
////                                                                       ////
///////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

// define the pinout.
// only required if port access is being used.
typedef struct  
{                            // This structure is overlayed
   BOOLEAN enable;           // on to an I/O port to gain
   BOOLEAN rs;               // access to the LCD pins.
   BOOLEAN rw;               // The bits are allocated from
   BOOLEAN unused;           // low order up.  ENABLE will
   int     data : 4;         // be LSB pin of that port.
  #if defined(__PCD__)       // The port used will be LCD_DATA_PORT.
   int    reserved: 8;
  #endif
} LCD_PIN_MAP;

// this is to improve compatability with previous LCD drivers that accepted
// a define labeled 'use_portb_lcd' that configured the LCD onto port B.
#if ((defined(use_portb_lcd)) && (use_portb_lcd==TRUE))
 #define LCD_DATA_PORT getenv("SFR:PORTB")
#endif

#if defined(__PCB__)
   // these definitions only need to be modified for baseline PICs.
   // all other PICs use LCD_PIN_MAP or individual LCD_xxx pin definitions.
/*                                    EN, RS,   RW,   UNUSED,  DATA  */
 const LCD_PIN_MAP LCD_OUTPUT_MAP =  {0,  0,    0,    0,       0};
 const LCD_PIN_MAP LCD_INPUT_MAP =   {0,  0,    0,    0,       0xF};
#endif

////////////////////// END CONFIGURATION ///////////////////////////////////
 char new_car[8]={0,16,8,4,2,1,0,0};    //escribe caracter '\'
#ifndef LCD_ENABLE_PIN
   #define lcd_output_enable(x) lcdlat.enable=x
   #define lcd_enable_tris()   lcdtris.enable=0
#else
   #define lcd_output_enable(x) output_bit(LCD_ENABLE_PIN, x)
   #define lcd_enable_tris()  output_drive(LCD_ENABLE_PIN)
#endif

#ifndef LCD_RS_PIN
   #define lcd_output_rs(x) lcdlat.rs=x
   #define lcd_rs_tris()   lcdtris.rs=0
#else
   #define lcd_output_rs(x) output_bit(LCD_RS_PIN, x)
   #define lcd_rs_tris()  output_drive(LCD_RS_PIN)
#endif

#ifndef LCD_RW_PIN
   #define lcd_output_rw(x) lcdlat.rw=x
   #define lcd_rw_tris()   lcdtris.rw=0
#else
   #define lcd_output_rw(x) output_bit(LCD_RW_PIN, x)
   #define lcd_rw_tris()  output_drive(LCD_RW_PIN)
#endif

// original version of this library incorrectly labeled LCD_DATA0 as LCD_DATA4,
// LCD_DATA1 as LCD_DATA5, and so on.  this block of code makes the driver
// compatible with any code written for the original library
#if (defined(LCD_DATA0) && defined(LCD_DATA1) && defined(LCD_DATA2) && defined(LCD_DATA3) && !defined(LCD_DATA4) && !defined(LCD_DATA5) && !defined(LCD_DATA6) && !defined(LCD_DATA7))
   #define  LCD_DATA4    LCD_DATA0
   #define  LCD_DATA5    LCD_DATA1
   #define  LCD_DATA6    LCD_DATA2
   #define  LCD_DATA7    LCD_DATA3
#endif

#ifndef LCD_DATA4
#ifndef LCD_DATA_PORT
   #if defined(__PCB__)
      #define LCD_DATA_PORT      0x06     //portb
      #define set_tris_lcd(x)   set_tris_b(x)
   #else
     #if defined(PIN_D0)
      #define LCD_DATA_PORT      getenv("SFR:PORTD")     //portd
     #else
      #define LCD_DATA_PORT      getenv("SFR:PORTB")     //portb
     #endif
   #endif   
#endif

#if defined(__PCB__)
   LCD_PIN_MAP lcd, lcdlat;
   #byte lcd = LCD_DATA_PORT
   #byte lcdlat = LCD_DATA_PORT
#elif defined(__PCM__)
   LCD_PIN_MAP lcd, lcdlat, lcdtris;
   #byte lcd = LCD_DATA_PORT
   #byte lcdlat = LCD_DATA_PORT
   #byte lcdtris = LCD_DATA_PORT+0x80
#elif defined(__PCH__)
   LCD_PIN_MAP lcd, lcdlat, lcdtris;
   #byte lcd = LCD_DATA_PORT
   #byte lcdlat = LCD_DATA_PORT+9
   #byte lcdtris = LCD_DATA_PORT+0x12
#elif defined(__PCD__)
   LCD_PIN_MAP lcd, lcdlat, lcdtris;
   #word lcd = LCD_DATA_PORT
   #word lcdlat = LCD_DATA_PORT+2
   #word lcdtris = LCD_DATA_PORT-0x02
#endif
#endif   //LCD_DATA4 not defined

#ifndef LCD_TYPE
   #define LCD_TYPE 2           // 0=5x7, 1=5x10, 2=2 lines
#endif

#ifndef LCD_LINE_TWO
   #define LCD_LINE_TWO 0x40    // LCD RAM address for the second line
#endif

#ifndef LCD_LINE_LENGTH
   #define LCD_LINE_LENGTH 20
#endif

BYTE const LCD_INIT_STRING[4] = {0x20 | (LCD_TYPE << 2), 0xc, 1, 6};
                             // These bytes need to be sent to the LCD
                             // to start it up.

BYTE lcd_read_nibble(void);

BYTE lcd_read_byte(void)
{
   BYTE low,high;

 #if defined(__PCB__)
   set_tris_lcd(LCD_INPUT_MAP);
 #else
  #if (defined(LCD_DATA4) && defined(LCD_DATA5) && defined(LCD_DATA6) && defined(LCD_DATA7))
   output_float(LCD_DATA4);
   output_float(LCD_DATA5);
   output_float(LCD_DATA6);
   output_float(LCD_DATA7);
  #else
   lcdtris.data = 0xF;
  #endif
 #endif
        
   lcd_output_rw(1);
   delay_cycles(1);
   lcd_output_enable(1);
   delay_cycles(1);
   high = lcd_read_nibble();
      
   lcd_output_enable(0);
   delay_cycles(1);
   lcd_output_enable(1);
   delay_us(1);
   low = lcd_read_nibble();
      
   lcd_output_enable(0);

 #if defined(__PCB__)
   set_tris_lcd(LCD_OUTPUT_MAP);
 #else
  #if (defined(LCD_DATA4) && defined(LCD_DATA5) && defined(LCD_DATA6) && defined(LCD_DATA7))
   output_drive(LCD_DATA4);
   output_drive(LCD_DATA5);
   output_drive(LCD_DATA6);
   output_drive(LCD_DATA7);
  #else
   lcdtris.data = 0x0;
  #endif
 #endif

   return( (high<<4) | low);
}

BYTE lcd_read_nibble(void)
{
  #if (defined(LCD_DATA4) && defined(LCD_DATA5) && defined(LCD_DATA6) && defined(LCD_DATA7))
   BYTE n = 0x00;

   /* Read the data port */
   n |= input(LCD_DATA4);
   n |= input(LCD_DATA5) << 1;
   n |= input(LCD_DATA6) << 2;
   n |= input(LCD_DATA7) << 3;
   
   return(n);
  #else
   return(lcd.data);
  #endif
}

void lcd_send_nibble(BYTE n)
{
  #if (defined(LCD_DATA4) && defined(LCD_DATA5) && defined(LCD_DATA6) && defined(LCD_DATA7))
   /* Write to the data port */
   output_bit(LCD_DATA4, bit_test(n, 0));
   output_bit(LCD_DATA5, bit_test(n, 1));
   output_bit(LCD_DATA6, bit_test(n, 2));
   output_bit(LCD_DATA7, bit_test(n, 3));
  #else      
   lcdlat.data = n;
  #endif
      
   delay_cycles(1);
   lcd_output_enable(1);
   delay_us(2);
   lcd_output_enable(0);
}

void lcd_send_byte(BYTE address, BYTE n)
{
  #if defined(__PCB__)
   set_tris_lcd(LCD_OUTPUT_MAP);
  #else
   lcd_enable_tris();
   lcd_rs_tris();
   lcd_rw_tris();
  #endif

   lcd_output_rs(0);
   while ( bit_test(lcd_read_byte(),7) ) ;
   lcd_output_rs(address);
   delay_cycles(1);
   lcd_output_rw(0);
   delay_cycles(1);
   lcd_output_enable(0);
   lcd_send_nibble(n >> 4);
   lcd_send_nibble(n & 0xf);
}

#if defined(LCD_EXTENDED_NEWLINE)
unsigned int8 g_LcdX, g_LcdY;
#endif

void lcd_init(void) 
{
   BYTE i;

 #if defined(__PCB__)
   set_tris_lcd(LCD_OUTPUT_MAP);
 #else
  #if (defined(LCD_DATA4) && defined(LCD_DATA5) && defined(LCD_DATA6) && defined(LCD_DATA7))
   output_drive(LCD_DATA4);
   output_drive(LCD_DATA5);
   output_drive(LCD_DATA6);
   output_drive(LCD_DATA7);
  #else
   lcdtris.data = 0x0;
  #endif
   lcd_enable_tris();
   lcd_rs_tris();
   lcd_rw_tris();
 #endif

   lcd_output_rs(0);
   lcd_output_rw(0);
   lcd_output_enable(0);
    
   delay_ms(15);
   for(i=1;i<=3;++i)
   {
       lcd_send_nibble(3);
       delay_ms(5);
   }
   
   lcd_send_nibble(2);
   for(i=0;i<=3;++i)
      lcd_send_byte(0,LCD_INIT_STRING[i]);

  #if defined(LCD_EXTENDED_NEWLINE)
   g_LcdX = 0;
   g_LcdY = 0;
  #endif
}

void lcd_gotoxy(BYTE x, BYTE y)
{
   BYTE address;
   
   if(y!=1)
      address=LCD_LINE_TWO;
   else
      address=0;
     
   address+=x-1;
   lcd_send_byte(0,0x80|address);

  #if defined(LCD_EXTENDED_NEWLINE)
   g_LcdX = x - 1;
   g_LcdY = y - 1;
  #endif
}

void lcd_putc(char c)
{
   switch (c)
   {
      case '\a'   :  lcd_gotoxy(1,1);     break;

      case '\f'   :  lcd_send_byte(0,1);
                     delay_ms(2);
                    #if defined(LCD_EXTENDED_NEWLINE)
                     g_LcdX = 0;
                     g_LcdY = 0;
                    #endif
                     break;

     #if defined(LCD_EXTENDED_NEWLINE)
      case '\r'   :  lcd_gotoxy(1, g_LcdY+1);   break;
      case '\n'   :
         while (g_LcdX++ < LCD_LINE_LENGTH)
         {
            lcd_send_byte(1, ' ');
         }
         lcd_gotoxy(1, g_LcdY+2);
         break;
     #else
      case '\n'   : lcd_gotoxy(1,2);        break;
     #endif
     
      case '\b'   : lcd_send_byte(0,0x10);  break;
     
     #if defined(LCD_EXTENDED_NEWLINE)
      default     : 
         if (g_LcdX < LCD_LINE_LENGTH)
         {
            lcd_send_byte(1, c);
            g_LcdX++;
         }
         break;
     #else
      default     : lcd_send_byte(1,c);     break;
     #endif
   }
}
 
char lcd_getc(BYTE x, BYTE y)
{
   char value;

   lcd_gotoxy(x,y);
   while ( bit_test(lcd_read_byte(),7) ); // wait until busy flag is low
   lcd_output_rs(1);
   value = lcd_read_byte();
   lcd_output_rs(0);
   
   return(value);
}
 void Set_Char(char dir, char new_car[8] ) {
   unsigned char i;

   //El lcd consta de 8 espacios para caracteres personalizables
   //Ejemplo: modificar caracter ascii: 0x5C
   //dir debe estar entre 0 y 7 (CG RAM DATA)
   //new_car es un arreglo (8 bytes) donde cada byte corresponde con una linea de 5 bits
   //del nuevo caracter.
   //Character Code       Address            CG RAM Character Pattern
   //(DD RAM Data)     (CG RAM Data)            (CG RAM Data)
   //                    bits 5 al 3              bits 2 al 0
   //                0,8,16,24,32,40,48,56     0,1,2,3,4,5,6,7
                   
   lcd_send_byte(0,0x40+dir*8);           //0x40 para seleccionar CG RAM (0x40) en lugar de DD RAM (0x80)
   delay_us(100);
   for(i=0;i<8;i++) {                      
   lcd_send_byte(1,new_car[i]);             //Escribe cada lines del nuevo caracter en CG RAM, la dir se autoincrementa
   delay_us(100);
   }
   lcd_send_byte(0,0x80);                   //seleccionar DD RAM (0x80)
   delay_us(100);
}
                                             //RUTINA PARA PANTALLA DE 16X2
void Display_Dato(char *dato2,INT16 MS) {   //ESTA ES UNA RUTINA PARA DESPLEGAR DATOS(NECESITA LA VARIBALE,Y UN NUMERO X ms  PARA EL DELAY ENTRE PANTALLAS 
char *str1,*str2;                            //SE DEFINEN LOS PUNTEROS
char car='\f',car2;                          //SE DEFINE EL CARACTER DE LIMPIEZA DE PANTALLA Y CAMBIO DE LINEA
unsigned int k;
   k=strlen(dato2);                          //SE OBTIENE LA LONGITUD DE LA CADENA
   str2=&dato2[k];                           //SE ASIGNA AL PUNTERO EL FINAL DE LA CADENA
   for(str1=dato2; str1<=str2; str1+=16) {   //SE CREA UN CICLO FOR DESDE EL INICIO HASTA EL FINAL DE LA CADENA CON INCREMENTOS POR FILA
      car2=str1[16];                         //SALVAMOS EL CARACTER DE LA POSICION 17 DE LA CADENA PARA INTRODUCIR UN FINAL (0)
      str1[16]=0;                            // INTRODUCIMOS UN CERO COMO FINAL DE CADENA
      printf(LCD_PUTC,"%c%s",car,str1);      //SE IMPRIMEN LOS PRIMEROS 16 CARACTERES DE LA CADENA
      str1[16]=car2;                         //RECUPERAMOS EL CARACTER ORIGINAL DE LA POSICION 17
      if (car=='\n') car='\f'; else car='\n';//SE HACE EL CAMBIO DE LINEA
      delay_ms(MS);                          // TIEMPO DE ESPERA ANTES DE BORRAR EL TEXTO   
   }
  }
