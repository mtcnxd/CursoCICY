//////////////////////////////////////////////////////////////////////////////
//
// ex12.c - Example 12 from the Embedded Internet/Embedded Ethernet tutorial
//
// A UDP example.  The PIC can talk to a PC using UDP, where the PC is using
// the CCS provided example program UDP.EXE
//
// NOTE: Change the code in IPAddrInit() to your desired IP address, which
//       is based on your network.
//
//////////////////////////////////////////////////////////////////////////////

#define STACK_USE_ICMP  1
#define STACK_USE_ARP   1
#define STACK_USE_UDP   1

#define MAX_SOCKETS                     6
#define MAX_UDP_SOCKETS                 4

#include "ccstcpip.h"

#if STACK_USE_CCS_PICENS
 #include "tcpip/mlcd.c"
#elif STACK_USE_CCS_PICEEC
 #include "tcpip/elcd.c"
#else
 #include "tcpip/dlcd.c"
#endif

#define UDP_SOURCE_PORT 1024
#define UDP_DESTINATION_PORT  1025
#define UDP_NUM_LISTEN_SOCKETS   2

UDP_SOCKET last_rx_socket=INVALID_UDP_SOCKET;

void UDPRxTask(void) {
   static enum {UDP_RX_OPEN=0, UDP_RX_LISTEN=1, UDP_RX_BLAH=2}
            state[UDP_NUM_LISTEN_SOCKETS]={0};
   static UDP_SOCKET socket[UDP_NUM_LISTEN_SOCKETS]={INVALID_UDP_SOCKET};
   int8 i;
   char c;

   for (i=0;i<UDP_NUM_LISTEN_SOCKETS;i++) {
      if (socket[i]==INVALID_UDP_SOCKET)
         state[i]=UDP_RX_OPEN;

      switch (state[i]) {
         case UDP_RX_OPEN:
            socket[i]=UDPOpen(UDP_SOURCE_PORT,NULL,INVALID_UDP_SOCKET);
            if (socket[i]!=INVALID_UDP_SOCKET) {
               printf("\r\nUDP SOCKET %U OPEN FOR LISTENING", socket[i]);
               state[i]=UDP_RX_LISTEN;
            }
            break;

         case UDP_RX_LISTEN:
            if (UDPIsGetReady(socket[i])) {
               last_rx_socket=socket[i];
               printf("\r\nGOT [IP:%U.%U.%U.%U",
                  UDPSocketInfo[socket[i]].remoteNode.IPAddr.v[0],
                  UDPSocketInfo[socket[i]].remoteNode.IPAddr.v[1],
                  UDPSocketInfo[socket[i]].remoteNode.IPAddr.v[2],
                  UDPSocketInfo[socket[i]].remoteNode.IPAddr.v[3]
               );
               printf(" SP:%LU DP:%LU S:%U] - ",
                  UDPSocketInfo[socket[i]].remotePort,
                  UDPSocketInfo[socket[i]].localPort,
                  socket[i]
               );
               printf(lcd_putc,"\f%U.%U.%U.%U\n",
                  UDPSocketInfo[socket[i]].remoteNode.IPAddr.v[0],
                  UDPSocketInfo[socket[i]].remoteNode.IPAddr.v[1],
                  UDPSocketInfo[socket[i]].remoteNode.IPAddr.v[2],
                  UDPSocketInfo[socket[i]].remoteNode.IPAddr.v[3]
               );
               while(UDPGet(&c)) {
                  putc(c);
                  lcd_putc(c);
               }
            }
            break;
      }
   }
}

void UDPTxTask(void) {
   static int8 button;
   static enum {UDP_TX_OPEN=2, UDP_TX_WAIT=0, UDP_TX_DEBOUNCE=1,
      UDP_TX_ISREADY=3} state=0;
   static int8 tx_socket;

   switch (state) {
      case UDP_TX_WAIT:
         if (last_rx_socket!=INVALID_UDP_SOCKET) {
            if (BUTTON1_PRESSED()) {
               state=UDP_TX_DEBOUNCE;
               button=1;
            }
           #if defined(BUTTON2_PRESSED())
            if (BUTTON2_PRESSED()) {
               state=UDP_TX_DEBOUNCE;
               button=2;
            }
           #endif
         }
         break;

      case UDP_TX_DEBOUNCE:
         if (button==1 && !BUTTON1_PRESSED())
            state=UDP_TX_OPEN;
        #if defined(BUTTON2_PRESSED())
         else if (button==2 && !BUTTON2_PRESSED())
            state=UDP_TX_OPEN;
        #endif
         break;

      case UDP_TX_OPEN:
         tx_socket=UDPOpen(UDP_SOURCE_PORT, &UDPSocketInfo[last_rx_socket].remoteNode, UDP_DESTINATION_PORT);
         if (tx_socket==INVALID_UDP_SOCKET)
         {
            printf("\r\nCan't open UDP socket for transmit");
            state=UDP_TX_WAIT;
         }
         else
         {
            printf("\r\nStart Transmit UDP BUTTON%U", button);
            state=UDP_TX_ISREADY;
         }
         break;

      case UDP_TX_ISREADY:
         if (UDPIsPutReady(tx_socket)) {
            printf(UDPPut, "\r\nBUTTON%U  ",button);
            UDPFlush();
            UDPClose(tx_socket);
            state=UDP_TX_WAIT;
            printf("\r\nStart Transmit UDP BUTTON%U OK", button);
         }
         break;
   }
}

void main(void) {
   printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 12 (UDP)\r\n");

   MACAddrInit();
   IPAddrInit();

   init_user_io();

   lcd_init();

   printf(lcd_putc,"\fCCS UDP TUTORIAL\nLISTENING");

   StackInit();

   while(TRUE) {
      StackTask();
      UDPTxTask();
      UDPRxTask();
   }
}
