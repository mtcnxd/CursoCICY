
//#include <18F2550.h> 
#include <string.h>
#INCLUDE <stdlib.h>

//////////define rutinas///////
//#define Obtener_Dato_CDC
//#define Dato_TXT_NUM
//#use rs232(STREAM=UARTBLUE,baud=9600,XMIT=PIN_C6,RCV=PIN_c7, DISABLE_INTS)




//////////////////// RUTINA PARA RECIBIR CARACTERES DE LA PC POR CDC    ///////////////////////////////////////
//#if defined (Obtener_Dato_CDC(char *dato))
//! void Obtener_Dato_CDC(char *dato)
//!          { 
//!          char k=0, c;   // CONTADOR DE MEMORIA Y CONTENEDOR DE CARACTER
//!           unsigned int16 j=1; // CONTADOR Y BANDERA DE CARACTER RECIBIDO
//!             
//!            while(j !=0)   //CONDICION DE SALIDA 
//!                   {             
//!                  while(!USB_CDC_kbhit())        //loop de tiempo de espera para un caracter
//!                          {
//!                          j++;                  // INCREMENTAMOS CONTADOR 
//!                          delay_us(40);
//!                          if(j>100){                                //DETERMINA EL TIMPO MAXIMO DE ESPERA
//!                                    j=0;
//!                                    BREAK;
//!                                    }      //si no se recibio caracter termina la espera
//!                          }
//!                          
//!                    if(j!=0){                      //BANDERA Si j no es cero es porque se recibio caracter
//!                              c=usb_cdc_getc();   //se carga en c el caracter
//!                              if(c != '\r'){      // SI ES UN RETORNO DE CARRO TERMINADOR ES DESCRIMINADO
//!                                                  //SE PUEDE COLOCAR AQUI EL MARCADOR DE DATOS SI SE REQUIERE
//!                                              dato[k]=c; 
//!                                             k++;     //carga caracter en dato
//!                                              dato[k]=0; // coloca terminador
//!                                              j=1;   
//!                                           }                  
//!                            }
//!                    if(k>34) j=0;      //se lleno el bufer de recepcion
//!                   }
//!                  
//!          BREAK;       
//!          }
 /////////////////// TERMINA RUTINA PARA RECIBIR CARACTERES//////////////////////
 

 
 
 
// #endif
 
// #if defined (Dato_TXT_NUM)
  /////////////////// RUTINA PARA CONVERTIR TEXTO NUMERICO A NÚMERO//////////////
char DATO_TXT_NUM(CHAR *di)
 {
    char i,dout,d[4];     //SE DECLARAN LAS VARIABLES A UTILIZAR I PARA EL CICLO FOR
    for(i=0;i<=2;i++)     //DOUT CONTENEDOR DE SALIDA DE LA FUNCION;D[4] ARRAY DE CUATRO       
     {                    //CARACTERES PARA EXTRAER UN BYTE DE DE BUFER DATO       
      if(*(di+i)==32)     //EL ARGUMENTO DI CONTINE CONTIENE LA DIRECCION DE INICIO
        {                 // DE LOS TRES CARACTERES QUE FORMAN EL BYTE 
        d[i]=48;         //SI POR ALGUNA RAZON LLEGA UN ESPACIO EN LUGAR DE NUMERO
       }                      //LA FUNCION ATOL NOS DEVOLVERÁ UN DATO ERRONEO POR TAL MOTIVO
       Else{ d[i]=*(di+i);}    //CHECAMOS SI ES 32 VALOR DECIMAL DEL ESPACIO LO CAMBIAMOS POR EL
     }                        //VALOR 48 VALOR DECIMAL DEL CERO
     d[3]=0;                  //AL TERMINAR DE ESTRAER LOS TRES CARACTERES DEL BYTE HAY QUE FINALIZAR
     dout=atol(d);            // LA CADENA AGREGANDO UN 0
     return dout;
 }
 //#endif
 //////////////////////AGREGAR RUTINAS PARA CDC ////////////////////////////////
