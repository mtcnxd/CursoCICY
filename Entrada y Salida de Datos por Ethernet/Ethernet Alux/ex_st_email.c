///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                         EX_ST_EMAIL.C                             ////
////                                                                   ////
//// An example program demonstrating using Microchip's TCP/IP stack   ////
//// with CCS's PCH C Compiler.                                        ////
////                                                                   ////
//// You can send an e-mail by either pressing the button or using     ////
//// the serial interface.  Make sure your TCP/IP is connected         ////
//// properly before trying to send an e-mail.  You will see debug     ////
//// information about the SMTP session using the serial link.         ////
////                                                                   ////
//// Use the serial link to setup the SMTP settings, such as the       ////
//// IP address of the STMP server, the TCP/IP port to use, and        ////
//// the body, subject, to and from address of the e-mail.             ////
////                                                                   ////
//// If you are having problems, try telneting into your SMTP server   ////
//// and manually sending an e-mail using the proper SMTP commands     ////
//// to make sure they work.  To fight the war on spam, many SMTP      ////
//// servers are locking down to prevent un-authorized users.          ////
////                                                                   ////
//// LCD display is not used you are using a Microchip PICDEM.net      ////
//// board with a PIC18F452 because there wasn't enough program memory ////
//// to fit this.                                                      ////
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
//// Mar 06, 2006 -                                                    ////
////     Updated for CCS PICENS Board                                  ////
////                                                                   ////
//// Mar 06, 2004 -                                                    ////
////     Initial Public Release                                        ////
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
#define STACK_USE_PPP   0     //use PPP on modem
#define STACK_USE_SLIP  0    //use slip (NOT TESTED)

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

#define STACK_USE_UDP         TRUE   //needed for dhcp & dns
#define STACK_USE_DNS         TRUE   //we shall use DNS in this example
#define STACK_USE_ICMP        TRUE   //for ping
#define STACK_USE_TCP         TRUE   //To send TCP packets (talk to HTTP clients)
#define STACK_USE_SMTP        TRUE   //enable the CCS SMTP Engine
#define STACK_USE_IP_GLEANING FALSE

#use rs232(baud=9600, xmit=PIN_C6,rcv=PIN_C7, STREAM=USER)
#define debug_printf   printf

#include "tcpip/stacktsk.c"    //include Microchip's TCP/IP stack

#define CCS_EMAIL_EXAMPLE TRUE   //used by CCSHELPER.C
void my_smtp_send(void);

#include "ccshelper.c"   //include CCS's PICDEM.net/PICNET helper functions
#include "ccshelperui.c"

int8 emails_sent=0;

enum {
   MYSMTP_STATE_IDLE=0, MYSMTP_STATE_BUTTON_DEBOUNCE=1,
   MYSMTP_STATE_CONNECT=2, MYSMTP_STATE_GET_DNS=3,
   MYSMTP_STATE_CONNECT_NOW=4, MYSMTP_STATE_SEND=5,
   MYSMTP_STATE_SEND_WAIT=6
} mysmtp_state=0;

void LCDTask(void) {
   static TICKTYPE lastTick;
   TICKTYPE currTick;
   static enum {LCD_Display=0, LCD_Wait=1} state=0;

   currTick=TickGet();

   switch(state) {
      case LCD_Display:
            lcd_putc('\f');
           #if STACK_USE_MAC
            if (!MACIsLinked())
               printf(lcd_putc,"NO ETHERNET LINK");
            #if STACK_USE_DHCP
            else if ( (!DHCPIsBound() && read_ext_eeprom(EE_NIC_DHCP) ) || !read_ext_eeprom(EE_NIC_DHCP) )
               printf(lcd_putc,"DHCP NOT BOUND");
            #endif
           #elif STACK_USE_PPP
            if (ppp_is_connecting()) {
               printf(lcd_putc,"PPP CONNECTING");
            }
            else if (!ppp_is_connected()) {
               printf(lcd_putc,"NOT CONNECTED");
            }
           #endif
            else
            {
               if ((mysmtp_state==MYSMTP_STATE_CONNECT_NOW)||(mysmtp_state==MYSMTP_STATE_GET_DNS))
               {
                  printf(lcd_putc,"Resolving DNS");
               }
               else
               {
                  switch (smtp_state)
                  {
                     case SMTP_STATE_WAITING:
                        printf(lcd_putc,"SMTP Idle");
                        break;

                     case SMTP_STATE_ARP_REQ:
                     case SMTP_STATE_ARP_WAIT:
                     case SMTP_STATE_CONNECT:
                     case SMTP_STATE_CONNECT_WAIT:
                        printf(lcd_putc,"Connecting");
                        break;

                     case SMTP_STATE_EHLO:
                     case SMTP_STATE_MAIL_FROM:
                     case SMTP_STATE_RCPT_TO:
                     case SMTP_STATE_DATA_START:
                     case SMTP_STATE_DO_CMD:
                        printf(lcd_putc,"Sending SMTP Cmd");
                        break;

                     case SMTP_STATE_DO_CMD_GET_RESP:
                        printf(lcd_putc,"Read SMTP Response");
                        break;

                     case SMTP_STATE_PUT_HEADER:
                     case SMTP_STATE_PUT_BODY:
                        printf(lcd_putc,"Sending body/headers");
                        break;

                     case SMTP_STATE_FINISH_EMAIL:
                     case SMTP_STATE_FINISH_EMAIL_WAIT:
                     case SMTP_STATE_CLOSE:
                     case SMTP_STATE_FORCE_CLOSE:
                        printf(lcd_putc, "Closing");
                        break;
                  }
               }
            }
            if (SMTPLastError())
            {
               //see SMTP_EC in smtp.h for a list of errors
               printf(lcd_putc,"\nERR: %u", SMTPLastError());
            }
            else
            {
               printf(lcd_putc,"\nEmails Sent: %U", emails_sent);
            }
         lastTick=currTick;
         state=LCD_Wait;
         break;

      case LCD_Wait:
         if (TickGetDiff(currTick,lastTick) > (TICKS_PER_SECOND/4))
            state=LCD_Display;
         break;
   }
}

void my_smtp_send(void)
{
   mysmtp_state=MYSMTP_STATE_CONNECT;
}

void MySMTPTask(void) {
   static TICKTYPE lastTick;
   TICKTYPE currTick;
   static IP_ADDR SMTP_ADDR;

   currTick=TickGet();

   switch(mysmtp_state) {
      case MYSMTP_STATE_IDLE:
         if (!input(USER_BUTTON1)) {
            lastTick=currTick;
            mysmtp_state=MYSMTP_STATE_BUTTON_DEBOUNCE;
         }
         break;

      case MYSMTP_STATE_BUTTON_DEBOUNCE:
         if (TickGetDiff(currTick,lastTick) > (TICKS_PER_SECOND/4)) {
            if (!input(USER_BUTTON1))
               mysmtp_state=MYSMTP_STATE_CONNECT;
            else
               mysmtp_state=MYSMTP_STATE_IDLE;
         }
         break;

      case MYSMTP_STATE_CONNECT:
         fprintf(USER,"\r\n\nFinding IP address for %s", SMTP_HOST_NAME);
         DNSResolve(SMTP_HOST_NAME);
         lastTick = currTick;
         mysmtp_state = MYSMTP_STATE_GET_DNS;
         break;

      case MYSMTP_STATE_GET_DNS:
         if (DNSIsResolved(&SMTP_ADDR))
         {
            fprintf(USER,"\r\nIP address is resolved to ");
            ui_disp_ip_user(&SMTP_ADDR);
            mysmtp_state = MYSMTP_STATE_CONNECT_NOW;
         }
         else if (TickGetDiff(currTick, lastTick) > (TICKS_PER_SECOND * 30))
         {
            fprintf(USER, "\r\nDNS failure %X.", smDNS);
            mysmtp_state = MYSMTP_STATE_IDLE;
         }
         break;

      case MYSMTP_STATE_CONNECT_NOW:
         if (SMTPConnect(&SMTP_ADDR, SMTP_PORT, SMTP_FROM_ADDR, SMTP_TO_ADDR, SMTP_SUBJECT))
         {
            printf("\r\nOpening socket with SMTP server...");
            mysmtp_state=MYSMTP_STATE_SEND;
         }
         else
         {
            printf("\r\nSMTP engine is busy!  Try again in a few moments.");
            mysmtp_state=MYSMTP_STATE_IDLE;
         }
         break;

      case MYSMTP_STATE_SEND:
         if (SMTPLastError()) {
            printf("\r\nSMTP Error %X", SMTPLastError());
            mysmtp_state=MYSMTP_STATE_IDLE;
         }
         else if (SMTPIsPutReady()) {
            printf("\r\nSMTP Connection accepted, now sending body of email.");
            printf(SMTPPut, "%s", SMTP_BODY);
            SMTPDisconnect();
            mysmtp_state=MYSMTP_STATE_SEND_WAIT;
         }
         break;

      case MYSMTP_STATE_SEND_WAIT:
         if (SMTPIsFree())
         {
            if (!SMTPLastError())
            {
               emails_sent++;
               printf("\r\nE-mail accepted by SMTP server!");
            }
            else
               printf("\r\nE-Mail rejected by SMTP server! %X", SMTPLastError());
            printf("\r\n\n");
            mysmtp_state=MYSMTP_STATE_IDLE;
         }
         break;
   }
}

void splash(void)
{
   ui_settings();
   ui_menu();
}


void main(void) {
   init();
   splash();

   while(TRUE) {
      restart_wdt();

      //every second: refresh current IP, blink LED
      if (second_counter>=1) {
         output_toggle(USER_LED1);
         second_counter=0;
      }

      if (kbhit(USER))
         ui_handle();

      #if STACK_USE_PPP
      if (ppp_is_connected())
      {
      #endif
         StackTask();
         MySMTPTask();
      #if STACK_USE_PPP
      }
      #endif
      LCDTask();
   }
}
