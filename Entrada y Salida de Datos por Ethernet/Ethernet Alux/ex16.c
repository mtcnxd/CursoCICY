//////////////////////////////////////////////////////////////////////////////
//
// ex16.c - Example 16 from the Embedded Internet/Embedded Ethernet tutorial
//
// Uses SMTP to send an e-mail when the button is pressed.
//
// NOTE: It is very important to change the constant EMAIL_SMTP_SERVER to
//       the IP address of your SMTP server!
//
// NOTE: EMail contents can be changed by changing the following defines:
//             EMAIL_FROM_ADDRESS
//             EMAIL_TO_ADDRESS
//             EMAIL_SUBJECT
//             EMAIL_BODY
//
// NOTE: Change the code in IPAddrInit() to your desired IP address, which
//       is based on your network.
//
//////////////////////////////////////////////////////////////////////////////


#define STACK_USE_ICMP  1
#define STACK_USE_ARP   1
#define STACK_USE_TCP   1
#define STACK_USE_SMTP  1
#include "ccstcpip.h"

#if STACK_USE_CCS_PICENS
 #include "tcpip/mlcd.c"
#else
 #include "tcpip/dlcd.c"
#endif

#define EMAIL_SMTP_SERVER     0x3DE6AAD8
#define EMAIL_FROM_ADDRESS    "test@ccsinfo.com"
#define EMAIL_TO_ADDRESS      "darren@ccsinfo.com"
#define EMAIL_SUBJECT         "TEST SMTP ON PIC"
#define EMAIL_BODY            "HELLO E-MAIL WORLD!!!"

int8 emails_sent=0;

void LCDTask(void) {
   static TICKTYPE lastTick;
   TICKTYPE currTick;
   static enum {LCD_Display=0, LCD_Wait=1} state=0;

   currTick=TickGet();

   switch(state) {
      case LCD_Display:
            lcd_putc('\f');

            switch (smtp_state) {
               case SMTP_STATE_WAITING:
                  printf(lcd_putc,"SMTP Idle");
                  break;

               case SMTP_STATE_ARP_REQ:
               case SMTP_STATE_ARP_WAIT:
                  printf(lcd_putc,"ARP Finding MAC");
                  break;

               case SMTP_STATE_CONNECT:
               case SMTP_STATE_CONNECT_WAIT:
                  printf(lcd_putc, "TCP Connecting");
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
                  printf(lcd_putc,"Sending headers");
                  break;

               case SMTP_STATE_PUT_BODY:
                  printf(lcd_putc,"Sending body");
                  break;

               case SMTP_STATE_FINISH_EMAIL:
               case SMTP_STATE_FINISH_EMAIL_WAIT:
                  printf(lcd_putc, "Finishing e-mail");
                  break;

               case SMTP_STATE_CLOSE:
               case SMTP_STATE_FORCE_CLOSE:
                  printf(lcd_putc,"Closing TCP");
                  break;

               default:
                  printf(lcd_putc,"STATE = %U", smtp_state);
                  break;
            }
            lcd_putc('\n');
            if (SMTPLastError()) {
               switch (SMTPLastError()) {
                  case SMTP_EC_CLOSE_PREVIOUS:
                     printf(lcd_putc,"ERR: Close Previous");
                     break;

                  case SMTP_EC_MAC_TX_FAIL:
                     printf(lcd_putc,"ERR: MAC TX Buffer");
                     break;

                  case SMTP_EC_ARP_FAIL:
                     printf(lcd_putc,"ERR: No ARP Answer");
                     break;

                  case SMTP_EC_INVALID_SOCKET:
                     printf(lcd_putc,"ERR: Invalid Socket");
                     break;

                  case SMTP_EC_CONNECT_FAIL:
                     printf(lcd_putc,"ERR: TCP Conn Fail");
                     break;

                  case SMTP_EC_BAD_WELCOME:
                     printf(lcd_putc,"ERR: bad welcome msg");
                     break;

                  case SMTP_EC_BAD_EHLO:
                     printf(lcd_putc,"ERR: ehlo me");
                     break;

                  case SMTP_EC_BAD_MAILFROM:
                     printf(lcd_putc,"ERR: mail from:");
                     break;

                  case SMTP_EC_BAD_RCPTTO:
                     printf(lcd_putc,"ERR: rcpt to:");
                     break;

                  case SMTP_EC_BAD_DATACMD:
                     printf(lcd_putc,"ERR: data");
                     break;

                  case SMTP_EC_PUT_HEADER:
                     printf(lcd_putc,"ERR: headers");
                     break;

                  case SMTP_EC_BODY_NOT_ACCEPTED:
                     printf(lcd_putc,"ERR: data end");
                     break;

                  default:
                     printf(lcd_putc,"ERR: %u", SMTPLastError());
                     break;
               }
            }
            else {
               printf(lcd_putc,"Emails Sent: %U", emails_sent);
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

IP_ADDR smtp_server;
char from[80];
char to[80];
char subject[80];
char body[80];

void MySMTPTask(void) {
   static TICKTYPE lastTick;
   TICKTYPE currTick;

   static enum {
      MYSMTP_STATE_IDLE=0, MYSMTP_STATE_BUTTON_DEBOUNCE=1,
      MYSMTP_STATE_SEND=2, MYSMTP_STATE_SEND_WAIT=3
   } state=0;

   currTick=TickGet();

   switch(state) {
      case MYSMTP_STATE_IDLE:
         if (BUTTON1_PRESSED()) {
            lastTick=currTick;
            state=MYSMTP_STATE_BUTTON_DEBOUNCE;
         }
         break;

      case MYSMTP_STATE_BUTTON_DEBOUNCE:
         if (!BUTTON1_PRESSED() && (TickGetDiff(currTick,lastTick) > (TICKS_PER_SECOND/4))) {
            if (SMTPConnect(&smtp_server, 25, from, to, subject))
               state=MYSMTP_STATE_SEND;
         }
         break;

      case MYSMTP_STATE_SEND:
         if (SMTPLastError()) {
            state=MYSMTP_STATE_IDLE;
         }
         else if (SMTPIsPutReady()) {
            printf(SMTPPut, "%s", body);
            SMTPDisconnect();
            state=MYSMTP_STATE_SEND_WAIT;
         }
         break;

      case MYSMTP_STATE_SEND_WAIT:
         if (SMTPIsFree()) {
            if (!SMTPLastError()) {
               emails_sent++;
            }
            state=MYSMTP_STATE_IDLE;
         }
         break;
   }
}

void MYSMTPInit(void) {
   //set the IP address of your SMTP server
   //smtp_server.v[0]=216;
   //smtp_server.v[1]=170;
   //smtp_server.v[2]=230;
   //smtp_server.v[3]=61;

   smtp_server.Val=EMAIL_SMTP_SERVER;

   //set the sender and receiver e-mail address
   sprintf(from, EMAIL_FROM_ADDRESS);
   sprintf(to, EMAIL_TO_ADDRESS);

   //set the subject line
   sprintf(subject, EMAIL_SUBJECT);

   //the body of the e-mail
   sprintf(body, EMAIL_BODY);
}

void main(void) {
   MACAddrInit();
   IPAddrInit();
   MYSMTPInit();

   init_user_io();

   lcd_init();

   StackInit();

   while(TRUE) {
      StackTask();
      MySMTPTask();
      LCDTask();
   }
}
