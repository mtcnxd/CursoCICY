//////////////////////////////////////////////////////////////////////////////
//
// ex14.c - Example 14 from the Embedded Internet/Embedded Ethernet tutorial
//
// A very simple webserver.
//
// NOTE: Change the code in IPAddrInit() to your desired IP address, which
//       is based on your network.
//
//////////////////////////////////////////////////////////////////////////////

#define STACK_USE_ARP   1
#define STACK_USE_ICMP  1
#define STACK_USE_TCP   1
#include "ccstcpip.h"

#if STACK_USE_CCS_PICENS
 #include "tcpip/mlcd.c"
#else
 #include "tcpip/dlcd.c"
#endif

#define MY_HTTP_SOCKET  80

int8 HTTPSocket=INVALID_SOCKET;

void LCDTask(void) {
   static TICKTYPE lastTick;
   static enum {LCD_Display=0, LCD_Wait=1} state=0;

   switch(state) {
      case LCD_Display:
         if (!MACIsLinked()) {
            printf(lcd_putc,"\fNo Ethernet\n");
         }
         else {
            printf(lcd_putc,"\f%u.%u.%u.%u\n", MY_IP_BYTE1, MY_IP_BYTE2,
               MY_IP_BYTE3, MY_IP_BYTE4);
         }

         if (HTTPSocket==INVALID_SOCKET) {
            printf(lcd_putc,"Socket Error");
         }
         else if (!TCPIsConnected(HTTPSocket)) {
            printf(lcd_putc,"Listening");
         }
         else {
            printf(lcd_putc,"\n%u.%u.%u.%u", TCB[HTTPSocket].remote.IPAddr.v[0],
                     TCB[HTTPSocket].remote.IPAddr.v[1],
                     TCB[HTTPSocket].remote.IPAddr.v[2],
                     TCB[HTTPSocket].remote.IPAddr.v[3]
                  );
         }
         lastTick=TickGet();
         state=LCD_Wait;
         break;

      case LCD_Wait:
         if (TickGetDiff(TickGet(), lastTick) >= TICKS_PER_SECOND) {
            output_toggle(USER_LED1);
            state=LCD_Display;
         }
         break;
   }
}


void HTTPPut(char c) {
   TCPPut(HTTPSocket, c);
}

void HTTPTask(void) {
   static enum {HTTP_ST_CONNECT=0, HTTP_ST_WAIT_CONNECT=1, HTTP_ST_GET=2,
      HTTP_ST_PUT=3, HTTP_ST_DISCONNECT=4} state=0;
   static TICKTYPE timeout_counter;
   static char lc, lc2;
   char c;
   char content[250];
   int8 a0;
  #if !STACK_USE_CCS_PICENS
   int8 a1;
  #endif

   if (HTTPSocket==INVALID_SOCKET)
      state=HTTP_ST_CONNECT;
   else if (!TCPIsConnected(HTTPSocket))
      state=HTTP_ST_WAIT_CONNECT;
   else if (TickGetDiff(TickGet(), timeout_counter) > TICKS_PER_SECOND*60)
      state=HTTP_ST_DISCONNECT;

   switch(state) {
      case HTTP_ST_CONNECT:
         HTTPSocket=TCPListen(MY_HTTP_SOCKET);
         if (HTTPSocket!=INVALID_SOCKET) {
            printf("\r\nHTTP: Listening");
            state=HTTP_ST_WAIT_CONNECT;
            timeout_counter=TickGet();
         }
         break;

      case HTTP_ST_WAIT_CONNECT:
         timeout_counter=TickGet();
         if (TCPIsConnected(HTTPSocket)) {
            state=HTTP_ST_GET;
            printf("\r\nHTTP: Connected");
         }
         break;

      case HTTP_ST_GET:
         if (TCPIsGetReady(HTTPSocket)) {
            while (TCPGet(HTTPSocket, &c)) {
               if ( (c=='\n') && (lc2=='\n') ) {
                  state=HTTP_ST_PUT;
                  printf("\r\nHTTP: Got Page Request");
               }
               lc2=lc;
               lc=c;
            }
         }
         break;

      case HTTP_ST_PUT:
         if (TCPIsPutReady(HTTPSocket)) {
            printf("\r\nHTTP: Putting Page Response");
            a0=read_adc();
           #if !STACK_USE_CCS_PICENS
            set_adc_channel(1);
            delay_us(20);
            a1=read_adc();
            set_adc_channel(0);
           #endif
            sprintf(content, "<HTML><HEAD><TITLE>PICNET</TITLE></HEAD><BODY><H1>HELLO</H1>");
            sprintf(&content[0]+strlen(content),"<P>AN0 = 0x%X",a0);
           #if !STACK_USE_CCS_PICENS
            sprintf(&content[0]+strlen(content),"<BR>AN1 = 0x%X",a1);
           #endif
            sprintf(&content[0]+strlen(content),"</BODY></HTML>");
            printf(HTTPPut, "HTTP/1.1 200 OK\r\n");
            printf(HTTPPut, "Content-Type: text/html\r\n");
            printf(HTTPPut, "Content-Length: %u\r\n",strlen(content));
            printf(HTTPPut, "\r\n");
            printf(HTTPPut, "%s", content);
            printf("\r\nHTTP: Flushing...");
            TCPFlush(HTTPSocket);
            printf("\r\nHTTP: Flushed!");
            state=HTTP_ST_GET;
         }
         break;

      case HTTP_ST_DISCONNECT:
         if (TCPIsPutReady(HTTPSocket)) {
            printf("\r\nHTTP: Force Disconnect");
            TCPDisconnect(HTTPSocket);
            state=HTTP_ST_WAIT_CONNECT;
         }
         break;
   }
}

void main(void) {
   MACAddrInit();
   IPAddrInit();

   init_user_io();

   lcd_init();

   StackInit();

   while(TRUE) {
      restart_wdt();
      StackTask();
      HTTPTask();
      LCDTask();
   }
}
