//////////////////////////////////////////////////////////////////////////////
//
// ex13.c - Example 13 from the ALUX 1.1
//
// A TCP example, where the PIC acts as a TCP client.  Connects to a PC running
// the CCS provided example program TCPSERVER.EXE.  TCPSERVER.EXE listens to
// port 1000 for TCP connections.
//
// NOTE: You MUST change the code in ServerAddrInit() to specify the IP address
//       of the PC running TCPSERVER.EXE 192.168.0.100
//
// NOTE: Change the code in IPAddrInit() to your desired IP address, which
//       is based on your network.  192.168.0.110
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////                     PROGRAMA: ex13.c  (CCS)                     ////
////                                                                 ////
//// Modificado para utilizarse con la tarjeta de desarrollo ALUX 1.1////
////                                                                 ////
//// Realizado con el compilador CCS PCWH 4.065 con Xtal de 8.0 MHz  ////
////                                                                 ////
//// Por:                                                            ////
////      ING. GAMALIEL TORRES SÁNCHEZ         gtorres@cicy.mx       ////
////      ING. GABRIEL JESÚS POOL BALAM        gabriel.pool@cicy.mx  ////
////      ING. JULIO ALEJANDRO LUGO JIMENEZ    jalugo@cicy.mx        ////
////      ING. JUAN MANUEL MOLINA AMARO        jmolina@cicy.mx       ////
////                                                                 ////
////     CENTRO DE INVESTIGACIÓN CIENTÍFICA DE YUCATÁN, A. C.        ////
////        MERIDA, YUCATAN A 10 DE SEPTIEMBRE DE 2010               ////
////                                                                 ////
//// ESTE PROGRAMA FUE REALIZADO PARA USARSE CON LA TARJETA DE       ////
//// DESARROLLO PROPIA BASADA EN UN MICROCONTROLADOR PIC18F2550.     ////
//// LA TARJETA CUENTA CON EL SIGUIENTE HARDWARE:                    ////
//// 1. CRISTAL DE 8 MHz.                                            ////
//// 2. POTENCIÓMETRO DE 10 K OHMS CONECTADO EN EL PUERTO A0         ////
//// 3. 5 PULSADORES MINIS (PUSH BUTTON) CONECTADOS DE A1 A A5       ////
////    (HARDWARE OPCIONAL A CONTINUACIÓN)                           ////
//// 4. UN LCD DE 16x2 CONECTADO EN EL PUERTO B COMO SE DESCRIBE EN  ////
////    EL ARCHIVO LCD.C EN LA LÍNEA 23. PERO EN EL PUERTO B         ////
////    Y AGREGANDO LA DIRECTIVA (#define  use_portb_lcd TRUE )ANTES ////
////    DE LA LLAMADA A #include <lcd.c>                             ////
////    SI NO SE CUENTA CON LCD, COMENTAR LAS INSTRUCCIONES          //// 
////    REFERENTES AL LCD, SINO EL CODIGO SE DETIENE.                ////
//// 5. 8 LEDS CONECTADOS DE B0 A B7                                 ////
////                                                                 ////
//// TODO ESTE HARDWARE NO NECESITA ALIMENTACIÓN EXTERNA,            ////
//// LA OBTIENE DEL PUERTO USB.                                      ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// LA INTERFAZ DE ETHERNET UTILIZADA ES EL MODULO ETHERNET         ////
//// VSHIELD V.1.0 PARA ARDUINO                                      ////
////                                                                 ////
//// LAS CONEXIONES DEL ALUX AL MODULO SON LAS SIGUIENTES:           ////
////                                                                 ////
////   ALUX 1.1          MODULO                                      ////
////      C1   ->    DIG 10   ~cs                                    ////
////      C0   ->    ~RESET   ~reset                                 ////
//// sdo  C7   ->    DIG 11   sdi                                    ////
//// sdi  B0   <-    DIG 12   sdo                                    ////
//// sck  B1   ->    DIG 13   sck                                    ////
//// ~int B2   <-    DIG 02   ~int                                   ////
////                                                                 ////
////  El software considera B3 <- wol (wake on lan) pero el módulo   ////
////  no tiene esta señal de salida. Así que tampoco se consideró    ////
////  en las modificaciones a los valores de TRISB.                  ////
////                                                                 ////
////  ARCHIVOS MODIFICADOS:                                          ////
////  ex13.c, ccstcpip.h, hardware.h, enc28j60.c, stacktsk.c         ////
////                                                                 ////
////  VARIOS:                                                        ////
////  bootloader original de CCS se modifico para trabajar a 32MHz   ////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

//use this example with the CCS provided TCPSERVER.EXE

#define STACK_USE_ICMP  1
#define STACK_USE_ARP   1
#define STACK_USE_TCP   1
#include "ccstcpip.h"

/*#if STACK_USE_CCS_PICENS       //SE ELIMINÓ EL LCD PORQUE EL F2550 UTILIZA EL
// #include "tcpip/mlcd.c"       //B0, B1 Y B2 PARA COMUNICACION SPI
#elif STACK_USE_CCS_PICEEC
 #include "tcpip/elcd.c"
#else
 #include "tcpip/dlcd.c"
#endif*/

//IP address of the PC running TCPSERVER.EXE
IP_ADDR server;

#define EXAMPLE_TCP_PORT   (int16)1000

//this function is called by MyTCPTask() when the specified socket is connected
//to the PC running the TCPSERVER.EXE demo.
//returns TRUE if BUTTON2 was pressed, therefore we must disconnect the socket
int8 TCPConnectedTask(TCP_SOCKET socket) {
   char c;
   static int8 counter;
   char str[20];
   static int8 button1_held;

   if (TCPIsGetReady(socket)) {
      //printf(lcd_putc,"\n                 \n");     //SE ELIMINA LCD
      while (TCPGet(socket, &c)) {
         //lcd_putc(c);                               //SE ELIMINA LCD
      };                                              //SE AGREGA ";"
   }

//when button 1 is pressed: send message over TCP
//when button 2 is pressed: disconnect socket
   if (BUTTON1_PRESSED() && !button1_held && TCPIsPutReady(socket)) {
      button1_held=TRUE;
      sprintf(str,"BUTTON C=%U",counter++);
      TCPPutArray(socket,str,strlen(str));
      TCPFlush(socket);
   }
   if (!BUTTON1_PRESSED()) {
      button1_held=FALSE;
   }
  #if defined(BUTTON2_PRESSED())
   if (BUTTON2_PRESSED()) {
      return(TRUE);
   }
  #endif
   return(FALSE);
}

void MyTCPTask() {
   static TICKTYPE lastTick;
   static TCP_SOCKET socket=INVALID_SOCKET;
   static enum {
      MYTCP_STATE_NEW=0, MYTCP_STATE_ARP_REQ=1, MYTCP_STATE_ARP_WAIT=2,
      MYTCP_STATE_CONNECT=3, MYTCP_STATE_CONNECT_WAIT=4,
      MYTCP_STATE_CONNECTED=5, MYTCP_STATE_DISCONNECT=6,
      MYTCP_STATE_FORCE_DISCONNECT=7
   } state=0;
   static NODE_INFO remote;
   TICKTYPE currTick;
   int8 dis;

   currTick=TickGet();

   switch (state) {
      case MYTCP_STATE_NEW:
         memcpy(&remote.IPAddr, &server, sizeof(IP_ADDR));
         //printf(lcd_putc,"\nARP REQUEST     ");     //SE ELIMINA LCD
         state=MYTCP_STATE_ARP_REQ;

      case MYTCP_STATE_ARP_REQ:
         if (ARPIsTxReady()) {
            ARPResolve(&remote.IPAddr);
            lastTick=currTick;
            state=MYTCP_STATE_ARP_WAIT;
         }
         break;

      case MYTCP_STATE_ARP_WAIT:
         if (ARPIsResolved(&remote.IPAddr, &remote.MACAddr)) {
            state=MYTCP_STATE_CONNECT;
            //printf(lcd_putc,"\nCONNECTING      ");  //SE ELIMINA LCD
         }
         else if (TickGetDiff(currTick, lastTick) > (TICKS_PER_SECOND * 2)) {
            state=MYTCP_STATE_ARP_REQ;
         }
         break;

      case MYTCP_STATE_CONNECT:
         socket=TCPConnect(&remote, EXAMPLE_TCP_PORT);
         if (socket!=INVALID_SOCKET) {
            lastTick=TickGet();
            state=MYTCP_STATE_CONNECT_WAIT;
         }
         else {
            //printf(lcd_putc,"\nSOCKET ERROR    ");  //SE ELIMINA LCD
         }
         break;

      case MYTCP_STATE_CONNECT_WAIT:
         if (TCPIsConnected(socket)) {
            state=MYTCP_STATE_CONNECTED;
            //printf(lcd_putc,"\nCONNECTED!      ");  //SE ELIMINA LCD
         }
         else if (TickGetDiff(currTick, lastTick) > (TICKS_PER_SECOND * 10)) {
            state=MYTCP_STATE_FORCE_DISCONNECT;
         }
         break;

      case MYTCP_STATE_CONNECTED:
         if (TCPIsConnected(socket)) {
            dis=TCPConnectedTask(socket);
            if (dis) {
               state=MYTCP_STATE_DISCONNECT;
               lastTick=currTick;
            }
         }
         else {
            //printf(lcd_putc,"\nDISCONNECTED    ");  //SE ELIMINA LCD
            state=MYTCP_STATE_CONNECT;
         }
         break;

      case MYTCP_STATE_DISCONNECT:
         //printf(lcd_putc,"\nDISCONNECTING   ");     //SE ELIMINA LCD
         if (TCPIsPutReady(socket)) {
            state=MYTCP_STATE_FORCE_DISCONNECT;
         }
         else if (TickGetDiff(currTick, lastTick) > (TICKS_PER_SECOND * 10)) {
            state=MYTCP_STATE_FORCE_DISCONNECT;
         }
         break;

      case MYTCP_STATE_FORCE_DISCONNECT:
         TCPDisconnect(socket);
         state=MYTCP_STATE_CONNECT;
         break;
   }
}

void ServerAddrInit(void) {
   //IP address of the PC running TCPSERVER.EXE    //SE MODIFICO IP DE SERVER
   server.v[0]=192;                                //IP DE LA PC (SERVER).
   server.v[1]=168;
   server.v[2]=0;
   server.v[3]=111;
}

void main(void) {
   //printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 13 (TCP CLIENT)\r\n");  //SE ELIMINA SERIAL

   MACAddrInit();
   IPAddrInit();

   ServerAddrInit();

   init_user_io();

   //lcd_init();           //SE ELIMINA LCD

   //printf(lcd_putc,"\fCCS TCP TUTORIAL\nINIT");     //SE ELIMINA LCD

   StackInit();

   while(TRUE) {
      StackTask();
      MyTCPTask();
   }
}
