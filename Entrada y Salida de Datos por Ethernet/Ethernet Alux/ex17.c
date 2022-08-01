//////////////////////////////////////////////////////////////////////////////
//
// ex17.c - Example 17 from the Embedded Internet tutorial
//
// A modification of EX14.C that uses PPP instead of Ethernet.
//
// NOTE: You MUST change the code in ISPInit() to the login parameters of your
//       dial-up ISP.
//
//////////////////////////////////////////////////////////////////////////////

#define STACK_USE_PPP   1
#define STACK_USE_MAC   0
#define STACK_USE_ICMP  1
#define STACK_USE_TCP   1
#include "ccstcpip.h"
#include "tcpip/dlcd.c"

#define MY_HTTP_SOCKET  80

int8 HTTPSocket=INVALID_SOCKET;

void HTTPPut(char c) {
   TCPPut(HTTPSocket, c);
}

void LCDTask(void) {
   static enum {LCD_ST_WAIT=0, LCD_ST_REFRESH_BAUD=1,
      LCD_ST_REFRESH_MY_IP=2, LCD_ST_REFRESH_THEIR_IP=3} state=0;
   static TICKTYPE refresh;
   TICKTYPE current;
   static int8 last;

   current=TickGet();

   if (ppp_is_connected()) {
      switch(state) {
         case LCD_ST_WAIT:
            if (TickGetDiff(current, refresh) > (2 * TICKS_PER_SECOND)) {
               if (last) {
                  last=0;
                  state=LCD_ST_REFRESH_BAUD;
               }
               else {
                  last=1;
                  state=LCD_ST_REFRESH_MY_IP;
               }
            }
            break;

         case LCD_ST_REFRESH_BAUD:
            printf(lcd_putc,"\f%LUbps", connected_baudrate);
            state=LCD_ST_REFRESH_THEIR_IP;
            break;

         case LCD_ST_REFRESH_MY_IP:
               printf(lcd_putc,"\f%u.%u.%u.%u",
                        MY_IP_BYTE1, MY_IP_BYTE2, MY_IP_BYTE3, MY_IP_BYTE4);
               state=LCD_ST_REFRESH_THEIR_IP;
            break;

         case LCD_ST_REFRESH_THEIR_IP:
            if (TCPIsConnected(HTTPSocket)) {
               printf(lcd_putc,"\n%u.%u.%u.%u", TCB[HTTPSocket].remote.IPAddr.v[0],
                        TCB[HTTPSocket].remote.IPAddr.v[1],
                        TCB[HTTPSocket].remote.IPAddr.v[2],
                        TCB[HTTPSocket].remote.IPAddr.v[3]
                     );
            }
            else {
               printf(lcd_putc,"\nHTTP Listening");
            }
            refresh=current;
            state=LCD_ST_WAIT;
            break;

      }
   }
}

void HTTPTask(void) {
   static enum {HTTP_ST_CONNECT=0, HTTP_ST_WAIT_CONNECT=1, HTTP_ST_GET=2,
      HTTP_ST_PUT=3, HTTP_ST_DISCONNECT=4, HTTP_ST_FORCE_DISCONNECT=5} state=0;
   static TICKTYPE timeout_counter;
   TICKTYPE current;
   static char lc, lc2;
   char c;
   char content[250];
   int8 a0, a1;

   current=TickGet();

   if (!MACIsLinked()) {
      if (HTTPSocket!=INVALID_SOCKET) {
         state=HTTP_ST_FORCE_DISCONNECT;
      }
   }
   if (HTTPSocket==INVALID_SOCKET)
      state=HTTP_ST_CONNECT;

   switch(state) {
      case HTTP_ST_CONNECT:
         if (MACIsLinked()) {
            HTTPSocket=TCPListen(MY_HTTP_SOCKET);
            if (HTTPSocket!=INVALID_SOCKET) {
               state=HTTP_ST_WAIT_CONNECT;
            }
         }
         break;

      case HTTP_ST_WAIT_CONNECT:
         if (TCPIsConnected(HTTPSocket)) {
            state=HTTP_ST_GET;
            timeout_counter=current;
         }
         break;

      case HTTP_ST_GET:
         if (!TCPIsConnected(HTTPSocket)) {
            state=HTTP_ST_FORCE_DISCONNECT;
            break;
         }
         if (TCPIsGetReady(HTTPSocket)) {
            while (TCPGet(HTTPSocket, &c)) {
               if ( (c=='\n') && (lc2=='\n') ) {
                  state=HTTP_ST_PUT;
                  timeout_counter=current;
                  TCPDiscard(HTTPSocket);
                  break;
               }
               lc2=lc;
               lc=c;
            }
         }
         else if (TickGetDiff(current, timeout_counter) > ((int16)30 * TICKS_PER_SECOND))
            state=HTTP_ST_DISCONNECT;
         break;

      case HTTP_ST_PUT:
         if (!TCPIsConnected(HTTPSocket)) {
            state=HTTP_ST_FORCE_DISCONNECT;
            break;
         }
         if (TCPIsPutReady(HTTPSocket)) {
            a0=read_adc();
            set_adc_channel(1);
            delay_us(20);
            a1=read_adc();
            set_adc_channel(0);

            sprintf(content, "<HTML><BODY><H1>HELLO</H1><P>AN0 = 0x%X<BR>AN1 = 0x%X</BODY></HTML>", a0, a1);
            printf(HTTPPut, "HTTP/1.1 200 OK\r\n");
            printf(HTTPPut, "Content-Type: text/html\r\n");
            printf(HTTPPut, "Content-Length: %u\r\n",strlen(content));
            printf(HTTPPut, "\r\n");
            printf(HTTPPut, "%s", content);
            TCPFlush(HTTPSocket);
            state=HTTP_ST_GET;
            timeout_counter=current;
         }
         else if (TickGetDiff(current, timeout_counter) > ((int16)30 * TICKS_PER_SECOND))
            state=HTTP_ST_DISCONNECT;
         break;

      case HTTP_ST_DISCONNECT:
         if (TCPIsPutReady(HTTPSocket))
            state=HTTP_ST_FORCE_DISCONNECT;
         else
            break;

      case HTTP_ST_FORCE_DISCONNECT:
         TCPDisconnect(HTTPSocket);
         HTTPSocket=INVALID_SOCKET;
         state=HTTP_ST_CONNECT;
         break;
   }
}

char ppp_username[32];
char ppp_password[32];
char ppp_phonenumber[20];

void ISPInit(void) {
   sprintf(ppp_username, "me@host.com");
   sprintf(ppp_password, "password");
   sprintf(ppp_phonenumber, "5551234");
}

void main(void) {
   MODEM_RESP resp;

   MACAddrInit();
   IPAddrInit();

   ISPInit();

   setup_adc_ports(ANALOG_AN0_TO_AN1);
   set_tris_a(3);    //set A0 and A1 to input for ADC
   set_tris_b(0);
   setup_adc(ADC_CLOCK_INTERNAL);

   lcd_init();

   StackInit();

   while(TRUE) {
      if (!ppp_is_connected() && !ppp_is_connecting()) {
         printf(lcd_putc,"\fDialing");
         resp=ppp_connect(ppp_username, ppp_password, ppp_phonenumber);
         if (resp==MODEM_BUSY) {
            printf(lcd_putc,"\fBusy Signal");
            delay_ms(2000);
         }
         else if (resp==MODEM_NO_DIALTONE) {
            printf(lcd_putc,"\fNo Dialtone");
            delay_ms(2000);
         }
         else if (resp!=MODEM_CONNECTED) {
            printf(lcd_putc,"\fDial Error");
            delay_ms(2000);
         }
         else {
            printf(lcd_putc,"\f%LUbps", connected_baudrate);
            printf(lcd_putc,"\nNegotiating PPP");
         }
      }
      StackTask();
      HTTPTask();
      LCDTask();
   }
}
