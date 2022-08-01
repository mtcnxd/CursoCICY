///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                      EX_ST_WESERVER.C                             ////
////                                                                   ////
//// An example program demonstrating using Microchip's TCP/IP stack   ////
//// with CCS's PCH C Compiler.                                        ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// ABOUT CCS TCP/IP EXAMPLES:                                        ////
////                                                                   ////
//// Connect your prototype board to serial cable, and open a serial   ////
//// connection at 9600bps.  With this link you can change all the     ////
//// ethernet and PPP settings.  You can also see the current status   ////
//// of the TCP/IP link.  Pressing any key will bring up the menu.     ////
////                                                                   ////
//// If you are using PPP, you will need to initiate and disconnect a  ////
//// PPP connection using this serial link.  Until PPP is connected    ////
//// the board will not be connected to TCP/IP.                        ////
////                                                                   ////
//// If the LCD is enabled in the code (it is enabled by default), the ////
//// last line displayed will be IP address of the unit.  If your      ////
//// ISP / Network doesn't filter ICMP/PING you can use the PING       ////
//// utility to test your connection.  If the last line doesn't        ////
//// display the IP address then the unit is connected (PPP not        ////
//// connected, ethernet cable not connected, a problem with ethernet  ////
//// DHCP, etc).                                                       ////
////                                                                   ////
//// CAUTION: All settings are saved to the external EEPROM.  If you   ////
//// are using the Microchip PICDEM.net board that has an example      ////
//// file-system with webpages it will be deleted/corrupted by these   ////
//// examples.                                                         ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// RELEASE HISTORY:                                                  ////
////                                                                   ////
//// Sep 20, 2006 -                                                    ////
////     Updated and tested with Microchip's 3.02 stack.               ////
////                                                                   ////
//// Apr 28, 2006 -                                                    ////
////     Tested on PCH 3.249                                           ////
////     Fixed broken link for CCS logo                                ////
////     Added test routine                                            ////
////     Changed to 18F4620                                            ////
////                                                                   ////
//// Mar 06, 2006 -                                                    ////
////     Updated for CCS PICENS Board                                  ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

//quick auto configure for different tcp/ip demo boards
#define STACK_USE_CCS_PICNET        0  //PICNET rev 0-2 board sold by CCS.  Has a 56K modem and a 10BaseT Network IC and 18F6620
#define STACK_USE_CCS_PICNET3       0  //PICNET rv 3 and up board sold by CCS.  Has a 56K modem and a 10BaseT Network IC and 18F8622
#define STACK_USE_CCS_PICENS        1  //CCS PICENS (Embedded Ethernet) development kit.  18F4620 + ENC28J60
#define STACK_USE_CCS_PICEEC        0  //CCS PICEEC (Embedded Ethernet) development kit.  18F67J60

//You must set one (and only) one of these to TRUE to tell the stack what hardware layer you are using
#define STACK_USE_MAC   1      //use the nic card
#define STACK_USE_PPP   0      //use PPP on modem
#define STACK_USE_SLIP  0      //use slip (NOT TESTED)

/// *** END USER CONFIGURABLE DEFINITIONS

#if (STACK_USE_CCS_PICENS+STACK_USE_CCS_PICNET3+STACK_USE_CCS_PICNET+STACK_USE_CCS_PICEEC>1)
 #error You can only use one hardware definition
#endif

#if (STACK_USE_CCS_PICENS+STACK_USE_CCS_PICNET3+STACK_USE_CCS_PICNET+STACK_USE_CCS_PICEEC==0)
 #error You must define one hardware definition
#endif

#if STACK_USE_CCS_PICNET3
 #undef STACK_USE_CCS_PICNET
 #define STACK_USE_CCS_PICNET 1
#endif

#if STACK_USE_CCS_PICNET3
 #include <18f6722.h>
 #use delay(clock=40000000
 #fuses H4
 #fuses CCP2C1
 #fuses MCLR
#elif STACK_USE_CCS_PICNET
 #include <18f6720.h>
 #use delay(clock=20000000) //CCS picnet is 20Mhz
 #fuses HS
 #fuses CCP2C1
#elif STACK_USE_CCS_PICENS
 #include <18f4620.h>
 #use delay(clock=40000000)
 #fuses H4
 #fuses NOFCMEN
 #fuses NOXINST
 #fuses NOIESO
 #fuses NOPBADEN
 #fuses MCLR
#elif STACK_USE_CCS_PICEEC
 #include <18F67J60.H>
 #use delay(clock=25M)
 #fuses HS
 #fuses NOIESO
 #fuses NOFCMEN
 #fuses PRIMARY
 #fuses ETHLEDNOEMB
#endif

#fuses NOWDT
#fuses NOPROTECT
#fuses NODEBUG
#fuses NOSTVREN

#if !STACK_USE_CCS_PICEEC
 #fuses PUT
 #fuses NOLVP
 #fuses NOBROWNOUT
 #fuses NOCPD
 #fuses WRT
 #fuses NOWRTD
 #fuses NOEBTR
 #fuses NOCPB
 #fuses NOEBTRB
 #fuses WRTB
#endif

#define STACK_USE_PICDEM_LCD     TRUE     //set to true to put out information on the LCD screen

#if STACK_USE_MAC
 #define STACK_USE_DHCP  TRUE  //for auto resolution of IP address
 #define STACK_USE_ARP   TRUE  //needed to resolve MAC addresses of IP addresses
 #define STACK_USE_UDP   TRUE  //needed for dhcp
#endif

#if STACK_USE_CCS_PICENS && STACK_USE_MAC
 #define STACK_USE_MCPENC  TRUE     //tell the stack to use the ENC28J60 driver
#else
 #define STACK_USE_MCPENC  FALSE    //tell the stack to use the Realtek driver
#endif

#if STACK_USE_CCS_PICEEC && STACK_USE_MAC
 #define STACK_USE_MCPINC  TRUE
#else
 #define STACK_USE_MCPINC  FALSE
#endif

#define STACK_USE_ICMP        TRUE   //Enabled for ping support
#define STACK_USE_TCP         TRUE   //To send TCP packets (talk to HTTP clients)
#define STACK_USE_HTTP        TRUE   //Enable the CCS webserver
#define STACK_USE_IP_GLEANING FALSE  //Disable Microchip's propietary auto-IP configuration

#include "tcpip/stacktsk.c"    //include Microchip's TCP/IP stack

#include "ccshelper.c"   //include CCS's PICDEM.net/PICNET helper functions
#include "ccshelperui.c"

/////***** WEB PAGES STORED INTO PROGRAM MEMORY *****/////

//The following escape characters are used:
//  %0 - AN0
//  %1 - AN1
//  %2 - BUTTON0
//  %3 - BUTTON1
//  %4 - Current LCD Message
//  %5 - CHECKED if LED1 is ON
//  %6 - CHECKED if LED1 is OFF
//  %7 - CHECKED if LED2 is ON
//  %8 - CHECKED if LED2 is OFF
//  %F - Message stack

//here is this examples / page
const char  HTML_INDEX_PAGE[]=
"<HTML><BODY BGCOLOR=#FFFFFF TEXT=#000000>
<IMG SRC=\"http://www.ccsinfo.com/images/CCSlogotiny.gif\">
<BR/>
<H1>CCS HTTP/CGI EXAMPLE</H1>
<P><B><I>%F</I></B></P>
<FORM METHOD=POST>
<TABLE BORDER=0>
<TR><TH>LCD:</TH><TD COLSPAN=4><INPUT TYPE=\"text\" NAME=\"lcd\" size=20 maxlength=16 value=\"%4\"></TD></TR>
<TR><TH>LED1:</TH><TD><INPUT type=\"radio\" name=\"led1\" value=1 %5></TD><TD>ON</TD>
   <TD><INPUT type=\"radio\" name=\"led1\" value=0 %6></TD><TD>OFF</TD></TR>
<TR><TH>LED2:</TH><TD><INPUT type=\"radio\" name=\"led2\" value=1 %7></TD><TD>ON</TD>
   <TD><INPUT type=\"radio\" name=\"led2\" value=0 %8></TD><TD>OFF</TD></TR>
</TABLE>
<INPUT TYPE=\"hidden\" name=\"msgStack\" value=\"LCD/LEDs changed!\">
<BR/><INPUT TYPE=\"submit\"></FORM>
<P><A HREF=\"/analog\">Analog Readings</A></P>
</BODY></HTML>";

const char  HTML_ANALOG_PAGE[]=
"<HTML><BODY BGCOLOR=#FFFFFF TEXT=#000000>
<IMG SRC=\"http://www.ccsinfo.com/images/CCSlogotiny.gif\">
<H1>PICNET ADC READINGS</H1>
<P>%0</P>
<BR/>%1
<P>%2</P>
<BR/>%3
<P><A HREF=\"/\">Change LCD/LEDs</A></P>
</BODY></HTML>";

/////***** END OF WEB PAGES STORED INTO PROGRAM MEMORY *****/////

char g_HTTPMessageStack[20]="";
char g_HTTPLCDMessage[20]="CCS Webserver";
int8 g_LEDState[2]={0,0};

//this is a callback function to the HTTP stack.  see http.c
//this demo provides to web "pages", an index (/) and an about page (/about)
int32 http_get_page(char *file_str)
{
   int32 file_loc=0;
   static char index[]="/";
   static char about[]="/analog";

   if (stricmp(file_str,index)==0)
      file_loc=label_address(HTML_INDEX_PAGE);

   else if (stricmp(file_str,about)==0)
      file_loc=label_address(HTML_ANALOG_PAGE);

   return(file_loc);
}

//this isn't a required callback, this is an application function being
//used by http_format_char.
int http_format_char_checked(int8 flag, char *str, int max)
{
   int len=0;

   if (flag)
   {
      sprintf(str,"CHECKED");
      len = 7;
   }

   return(len);
}

//this is a callback function to the HTTP stack. see http.c
//The following escape characters are used:
//  %0 - AN0
//  %1 - AN1
//  %2 - BUTTON0
//  %3 - BUTTON1
//  %4 - Current LCD Message
//  %5 - CHECKED if LED1 is ON
//  %6 - CHECKED if LED1 is OFF
//  %7 - CHECKED if LED2 is ON
//  %8 - CHECKED if LED2 is OFF
//  %F - Message stack
int8 http_format_char(int32 file, char id, char *str, int8 max_ret) {
   char new_str[20];
   int8 len=0;
   int8 i;

   *str=0;

   switch(id) {
      case '0':
         set_adc_channel(ADC_CHAN_0);
         delay_us(100);
         i=read_adc();
         sprintf(new_str,"<B>AN%U = </B>0x%X",ADC_CHAN_0,i);
         len=strlen(new_str);
         break;

      case '1':
        #if defined(ADC_CHAN_1)
         set_adc_channel(ADC_CHAN_1);
         delay_us(100);
         i=read_adc();
         sprintf(new_str,"<B>AN%U = </B>0x%X",ADC_CHAN_1,i);
         len=strlen(new_str);
        #else
         len = 0;
        #endif
         break;

      case '2':
         sprintf(new_str,"<B>BTN1 = </B>");
         if (input(USER_BUTTON1))
            sprintf(&new_str[14], "UP");
         else
            sprintf(&new_str[14], "DOWN");
         len=strlen(new_str);
         break;

      case '3':
        #if defined(USER_BUTTON2)
         sprintf(new_str,"<B>BTN2 = </B>");
         if (input(USER_BUTTON2))
            sprintf(&new_str[14], "UP");
         else
            sprintf(&new_str[14], "DOWN");
         len=strlen(new_str);
        #else
         len = 0;
        #endif
         break;

      case '4':
         strncpy(new_str, g_HTTPLCDMessage, sizeof(new_str));
         new_str[sizeof(new_str)-1] = 0;
         len = strlen(new_str);
         break;

      case '5':
         len = http_format_char_checked(g_LEDState[0], new_str, sizeof(new_str));
         break;

      case '6':
         len = http_format_char_checked(!g_LEDState[0], new_str, sizeof(new_str));
         break;

      case '7':
         len = http_format_char_checked(g_LEDState[1], new_str, sizeof(new_str));
         break;

      case '8':
         len = http_format_char_checked(!g_LEDState[1], new_str, sizeof(new_str));
         break;

      case 'F':
         strncpy(new_str, g_HTTPMessageStack, sizeof(new_str));
         new_str[sizeof(new_str)-1] = 0;
         len = strlen(new_str);
         g_HTTPMessageStack[0] = 0;
         break;

      default:
         len = 0;
   }

   if (len)
      strncpy(str, new_str, max_ret);
   else
      *str=0;

   return(len);
}

//this is a callback function to the HTTP stack. see http.c
//in this example it verifies that "pwd" is "master", if it is
//then it sets led1 and led2 ("led1" and "led2") based on their value
//and changes the lcd screen ("lcd").
void http_exec_cgi(int32 file, char *key, char *val) {
   static char led1_key[]="led1";
   static char led2_key[]="led2";
   static char lcd_key[]="lcd";
   static char msgStack_key[]="msgStack";
   int8 v;

   if (stricmp(key,led1_key)==0) {
      v=atoi(val);
      g_LEDState[0]=v;
   }

   if (stricmp(key,led2_key)==0) {
      v=atoi(val);
      g_LEDState[1]=v;
   }

   if (stricmp(key, msgStack_key)==0)
   {
      strncpy(g_HTTPMessageStack, val, sizeof(g_HTTPMessageStack));
      g_HTTPMessageStack[sizeof(g_HTTPMessageStack)-1] = 0;
   }

#if STACK_USE_PICDEM_LCD
   if (stricmp(key,lcd_key)==0) {
      strncpy(g_HTTPLCDMessage, val, sizeof(g_HTTPLCDMessage));
      g_HTTPLCDMessage[sizeof(g_HTTPLCDMessage)-1] = 0;
      printf(lcd_putc,"\f%s",val);
   }
#endif
}

void splash(void) {
  #IF STACK_USE_PICDEM_LCD
   printf(lcd_putc,"\fCCS Webserver");
  #ENDIF
   fprintf(USER,"\r\n\r\nCCS ex_st_webserver\r\n");
   ui_settings();
   ui_menu();
}

void main(void) {
   init();
   splash();

   while (TRUE) {
      restart_wdt();

      if (kbhit(USER)) {
         ui_handle();
      }

      //every second: refresh current IP, blink LED
      if (second_counter>=1) {
         display_ip_lcd();
        #if defined(USER_LED3)
         output_toggle(USER_LED3);
        #endif
         second_counter=0;
      }

      //just something to test the buttons
     #if STACK_USE_CCS_PICNET
      if (!input(USER_BUTTON1))
      {
         output_low(USER_LED1);
      }
      else
      {
         if (g_LEDState[0])
            output_low(USER_LED1);
         else
            output_high(USER_LED1);
      }
      if (!input(USER_BUTTON2))
      {
         output_low(USER_LED2);
      }
      else
      {
         if (g_LEDState[1])
            output_low(USER_LED2);
         else
            output_high(USER_LED2);
      }
     #elif STACK_USE_CCS_PICENS
      if (g_LEDState[0])
         output_low(USER_LED1);
      else
         output_high(USER_LED1);
      if (g_LEDState[1])
         output_low(USER_LED2);
      else
         output_high(USER_LED2);
      if (!input(USER_BUTTON1)) output_low(USER_LED3);
     #endif

      #if STACK_USE_PPP
      if (ppp_is_connected()) {
      #endif
      StackTask();            //Microchip's TCP/IP stack handler.  Must be called routinely.
      #if STACK_USE_PPP
      }
      #endif
   }
}
