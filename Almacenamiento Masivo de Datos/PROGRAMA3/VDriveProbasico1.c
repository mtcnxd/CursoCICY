/* PROGRAMA PARA EJEMPLO DEMOSTRATIVO DE LA COMUNICACION DEL MODULO
   VDRIVE CON LA TARJETA ALUX 1.1 Y LA PC
PROGRAMADO: DEPTO. INSTRUMENTACION
           ING. GAMALIEL TORRES SÁNCHEZ         gtorres@cicy.mx       
           ING. GABRIEL JESÚS POOL BALAM        gabriel.pool@cicy.mx  
           ING. JULIO ALEJANDRO LUGO JIMENEZ    jalugo@cicy.mx        
           ING. JUAN MANUEL MOLINA AMARO        jmolina@cicy.mx  
           LUGAR: MERIDA, YUCATÁN.
*/          
/*SE MODIFICA LA LIBRERIA LCD.C Y SE RENOMBRA UNA COPIA LCD1.C
DONDE SE AGREGA UNA RUTINA PARA DESPLEGAR LOS DATOS ALMACENADOS EN UN BUFER DE 250 BYTES
LA RUTINA ES "DISPLAY_DATO(parametro1, parametro2)" variable es el nombre o puntero
donde se encuentran alojados los datos. el segundo parametro es la pausa para poder leer el display
antes de ser sobre escrito.*/

//SI UTILIZAMOS UN PIC18F4550 SUSTITUIR #include <18F2455.h> POR #include<18F4550>
   #include <18F2455.h>  //DIRECTIVA PARA ESPECIFICAR EL CHIP A UTILIZAR
  

  #fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL2,CPUDIV1,VREGEN,NOPBADEN   //FUSIBLES   USBDIV
   
//DEFINIMOS LA CANTIDAD DE BITS UTILIZADOS POR EL ADC
// #device ADC=8 //descomentar esta fila si usa el ADC
   
//FRECUENCIA A CONSIDERAR EN LOS COMANDOS DE RETARDO                                                   
   #use delay(clock=48000000)

//DIRECTIVAS 
   #include <usb_bootloader.h>
   #include <string.h>
   #include <lcd1.c>//RUTINAS PARA USO DEL LCD DEL CCS
   #include <usb_desc_cdc.h>   //DESCRIPTOR
   #include <usb_cdc.h>
//configuramos el puerto 
   #use rs232(UART1, baud=9600, XMIT=PIN_C6, RCV=PIN_C7, DISABLE_INTS)
   #define ON                 output_high    // DEFINE COMO OUTPUT_HIGH A ON
   #define OFF                output_low     //DEFINE COMO OUTPUT_LOW A OFF
   
   unsigned char dato[250];             //CREAMOS LA VARIABLES CONTENEDORAS DE 250 BYTES
   int1 act_lcd = 0;
   char c,nombre;                  
   unsigned int band1,l;
   UNSIGNED INT32 k;
   unsigned int16 j=1;
   long d;
   long menuC[12][12]={"SCS","ECS","IPA","IPH","SBD-DIVISOR","FWV","E","e","DIR","CLF","salir"};//LEAMOS EL MENU DE COMANDOS PARA DESPLEGAR
   
  
    
void put_enter(){putc(0x0D);}          //lista comandos de configuracion del vdrive
VOID PUT_SCS(){putc(0x10);PUTC(0X0D);} //Switches to the shortened command set
VOID PUT_ECS(){putc(0x11);PUTC(0X0D);} //Switches to the extended command set
VOID PUT_IPA(){putc(0x90);PUTC(0X0D);} //Input numbers in ASCII
VOID PUT_IPH(){putc(0x91);PUTC(0X0D);} //Input numbers in HEX
//VOID PUT_SBS(){putc(0x14);PUTC(0x20);PUTC(VB1);PUTC(VB2);PUTC(VB3);PUTC(0X0D);}
VOID PUT_FWV(){putc(0x13);PUTC(0X0D);} //DEVUELVE LA VERSION DE FIRMWARE
VOID P_E(){putc(0x45);PUTC(0X0D);}     //DEVUELVE UNA E SE USA PARA SINCRONIZAR
VOID PUT_e(){putc(0x65);PUTC(0X0D);}   //DEVUELVE UNA e SE USA PARA SINCRONIZAR


VOID PUT_DIR(){putc(0x01);PUTC(0X0D);}                                          //DEVUELVE EL DIRECTORIO DE LA MEMORIA FLASH
VOID PUT_DIR_FILE(){PUTC(0x01);PUTC(0x20);PUTC(0x0D);}                          //DEVUELVE EL ARCHIVO SOLICITADO Y SU TAMAÑO
VOID PUT_CD_FILE(){PUTC(0x02);PUTC(0x20);PUTC(0x0D);}                           // ABRE UN DIRECTORIO
VOID PUT_CD_(){PUTC(0x02);PUTC(0x20);PUTC(0x2E);PUTC(0x2E);PUTC(0x0D);}         //CIERRA UN NIVEL DE DIRECTORIO
VOID PUT_RD_FILE(){PUTC(0x04);PUTC(0x20); putc(0x48);putc(0x4f);putC(0x4c);
                  PUTC(0x41);PUTC(0x2e);PUTC(0x54);PUTC(0x58);PUTC(0x54); PUTC(0x0D);}//LEE EL CONTENIDO COMPLETO DE UN ARCHIVO
VOID PUT_DLD_FILE(){PUTC(0x05);PUTC(0x20); putc(0x48);putc(0x4f);putC(0x4c);PUTC(0x41);PUTC(0x2e);
                  PUTC(0x54);PUTC(0x58);PUTC(0x54); PUTC(0x0D);}                     // BORRA EL SUBDIRECTORIO
VOID PUT_MKD_FILE(){PUTC(0x06);PUTC(0x20); putc(0x48);putc(0x4f);putC(0x4c);PUTC(0x41);PUTC(0x2e);
                   PUTC(0x54);PUTC(0x58);PUTC(0x54); PUTC(0x0D);}                   //CREA UN DIRECTORIO
VOID PUT_DLF_FILE(){PUTC(0x07);PUTC(0x20); putc(0x48);putc(0x4f);putC(0x4c);PUTC(0x41);PUTC(0x2e);
                   PUTC(0x54);PUTC(0x58);PUTC(0x54); PUTC(0x0D);}                  //BORRA UN ARCHIVO
VOID PUT_WRF(UNSIGNED INT BYTE3,BYTE2,BYTE1,BYTE0){ PUTC(0x08);PUTC(0x20);PUTC(BYTE3);PUTC(BYTE2);PUTC(BYTE1);PUTC(BYTE0);PUTC(0x0D);} // ESCRIBE EN EL ARCHIVO EL NUMERO BYTES ESPESIFCADO POR NB
VOID PUT_RDF(UNSIGNED INT BYTE3,BYTE2,BYTE1,BYTE0){PUTC(0x0B);PUTC(0x20);PUTC(BYTE3);PUTC(BYTE2);PUTC(BYTE1);PUTC(BYTE0); PUTC(0x0D);}
VOID PUT_OPR_FILE(){PUTC(0x0E);PUTC(0x20); putc(0x48);putc(0x4f);putC(0x4c);PUTC(0x41);PUTC(0x2e);
                   PUTC(0x54);PUTC(0x58);PUTC(0x54); PUTC(0x0D);}
VOID PUT_OPW_FILE(){PUTC(0x09);PUTC(0x20); putc(0x48);putc(0x4f);putC(0x4c);PUTC(0x41);PUTC(0x2e);
                   PUTC(0x54);PUTC(0x58);PUTC(0x54); PUTC(0x0D);}                
                   
VOID PUT_CLF(CHAR NOMBRE) {printf("CLF %s\r",NOMBRE);}

// RUTINA PARA OBTENER EL DATO DEL PUERTO SERIAL
VOID ObtenerDato(void)      
{  j=1;k=0;                            
   while(j!=0)
   {
      While(!kbhit()){j++; delay_us(40);if(j>=50000){j=0;break;}} //COMIENZA UN DELAY  POCO MAYOR A 2 S  EN ESPERA
      if(j!=0)                                                     //SI SE VENCE EL DELAY Y NO LLEGÓ DATO SE SALE
      {                                                             
      c=getc();
      if(c!='\r'){dato[k]=c;  k++; dato[k]=0; act_lcd=1; j=1;}       // SI LLEGO DATO SE ALMACENA EN DATO
      }
   }              
                 
 }   

// RUTINA EJEMPLO PARA INTRODUCIR COMANDOS CORTOS AL VDRIVE DESDE EL MICRO

void menu_config()
{
   char L1=1;
   char L2=1;
   printf(lcd_putc,"\fmenu de config");    //SE NOTIFICA QUE A ENTRADO AL MENU DE CONFIGURACION
   printf(lcd_putc,"\nA3 alux  A1 PC");    // SE PIDE AL USUARIO UNA DECISION DE ENTRADA DE COMANDOS
   delay_ms(500);                          //DESDE LA PC O DESDE  LA TARJETA ALUX  
   while((INPUT(PIN_A1))&&(INPUT(PIN_A3))){};// ESPERA INDEFINIDAMENTE UNA RESPUESTA 
   IF(!INPUT(PIN_A1)) GOTO PC;               // SI SE HA SELECCIONADO PC SE HACE UN SALTO A LA RUTINA DE EN CUESTION
   l=0;                                     // SI SE HA SELECCIONADO ALUX
   while((input(PIN_A1))||(input(PIN_A3)))   //CONDION PARA SALIR DE MENU CONFIG  
   {
      d = menuC[l]; 
      printf(lcd_putc,"\n     %s",d);                                                  // SE INICIA LA RUTINA DEL MENU DESDE EL ALUX
      if((!input(PIN_A5))&&(l==0)){PUT_SCS(); ObtenerDato();Display_Dato(DATO,300);}   //SE AVANZA EN LOS COMANDOS CON A1
      if((!input(PIN_A5))&&(l==1)){PUT_ECS(); ObtenerDato();Display_Dato(DATO,300);}   //DE FORMA CICLICA Y SE DEFINE A A5 COMO ENTER
      if((!input(PIN_A5))&&(l==2)){PUT_IPA(); ObtenerDato();Display_Dato(DATO,300);}   //DESPUES DE ENVIAR EL COMANDO SE LANZA LA RUTINA
      if((!input(PIN_A5))&&(l==3)){PUT_IPH(); ObtenerDato();Display_Dato(DATO,300);}   //OBTENER DATO PARA CAPTURAR EL SUCESO ENVIADO POR 
      if((!input(PIN_A5))&&(l==5)){PUT_FWV(); ObtenerDato();Display_Dato(DATO,300);}   //EL MONITOR DE COMANDOS Y LA RUTINA DISPLAY DATO
      if((!input(PIN_A5))&&(l==6)){P_E(); ObtenerDato();Display_Dato(DATO,300);}       // LEER EL SUCESO
      if((!input(PIN_A5))&&(l==7)){PUT_e(); ObtenerDato();Display_Dato(DATO,300);}
      if((!input(PIN_A5))&&(l==8)){PUT_DIR(); ObtenerDato();Display_Dato(DATO,300);}
      if((!input(PIN_A5))&&(l==9)){PUT_CLF("HOLA2.TXT"); ObtenerDato();Display_Dato(DATO,300);}
      if((!input(PIN_A5))&&(l==10)){break;}
               
      if (!input(PIN_A1)) {                                         
                           delay_ms(300);                                                               
                           if (l>9) {l=0;} else { l++;}
                           printf(lcd_putc,"\n                ");                    }
    }
   GOTO FIN;

   PC:   // INICIA LA RUTINA PARA INTRODUCIR DATOS DESDE EL PC
   while((input(PIN_A1))||(input(PIN_A3)))    // CONDICION PARA SALIR DE MENU CONFIG
   {
     usb_task();           // SE INICIALIZA LA CONEXION USB CON LA PC
      //**********************************************************************//
      // SE RECIBE CARACTERES DE RESPUESTA DEL VDRIVE                         //
      //**********************************************************************//
      
      if (kbhit()) {                   //SE RECIBIO UN CARACTER DEL VDRIVE...
          
         c=getc();                     //SE ALMACENA EN C
         
         if (c=='\r') {                //SE RECIBIO FINAL DEL COMANDO
            usb_cdc_putc("\r\n");      //ENVIAR A LA PC UN SALTO DE LINEA
            L2=1;
         }
         else {
            usb_cdc_putc(c);           //ENVIAR C A LA PC
            if (L2==1) printf(LCD_PUTC,"\n                ");  //LIMPIA LINEA 2
            lcd_gotoxy(L2++,2);        //ENVIA EL CURSOR AL INICIO DE LA LINEA 2
            if (c==0x5C) c=0;          //SUSTITUYE CARACTER 0x5C POR EL 0
            printf(LCD_PUTC,"%c",c);   //ENVIA AL LCD EL CARACTER C          
         }
      }
         
      //**********************************************************************//
      // SE ENVIAN CARACTERES DE COMANDO AL VDRIVE                            //
      //**********************************************************************//
 
      if (usb_cdc_kbhit()) 
      {                                  //SE RECIBIO UN CARACTER DE LA PC...
         c=usb_cdc_getc();             //SE ALMACENA EN C
         if (c!='\n') 
         {
           putc(c); 
           if (c=='\r') 
           {
              L1=1;
            }
            else 
             {
               if (L1==1) printf(LCD_PUTC,"\f");  //BORRA TODO
               lcd_gotoxy(L1++,1);
               printf(LCD_PUTC,"%c",c);
            }
           }
      }
      
      //**********************************************************************//
      // FINAL DE LAS RUTINAS                                                 //
      //**********************************************************************//
      
   }
 usb_detach();

 FIN:
BREAK;
}
// INICIA PROGRAMA PRINCIPAL

void main(void)    //INICIO DE PROGRAMA
{
   lcd_init();
   printf(lcd_putc,"\fPRUEBA 01");
// PRINTF("SCS\r");
  PUTC(0x11);
  PUTC(0x0D);
  obtenerDato();
  display_dato(dato,500);
  PUTC(0x91);
  PUTC(0x0D);
  obtenerDato();
  display_dato(dato,500);
  PUTC(0x09);
  PUTC(0x20);
  delay_ms(1);
  PUTC(0x48);PUTC(0x4F);PUTC(0X4C);PUTC(0x41);PUTC(0x2E);PUTC(0x54);PUTC(0x58);PUTC(0x54);
  PUTC(0x0D);
  obtenerDato();
  display_dato(dato,500);
  PUTC(0X08); 
  PUTC(0X20); 
  PUTC(0X4F);
  obtenerDato();
  display_dato(dato,500);
  PUTC(0x0A);
  PUTC(0x20);
  delay_ms(1);
  PUTC(0X48);PUTC(0x4F);PUTC(0X4C);PUTC(0x41);PUTC(0x2E);PUTC(0x54);PUTC(0x58);PUTC(0x54);
  PUTC(0x0D);
     obtenerDato();
  display_dato(dato,500);
  printf(lcd_putc,"\fCORRECTO");
  PUTC(0x12); 
  PUTC(0x0D);
  obtenerDato();
  display_Dato(dato,500);
}









