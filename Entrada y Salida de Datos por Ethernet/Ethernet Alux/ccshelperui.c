///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                       CCSHELPERUI.C                               ////
////                                                                   ////
//// More Helper functions and definitions for using CCS's example     ////
//// programs   These routines are more focused on the user interface  ////
//// needed to get and save setting from the user.                     ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// RELEASE HISTORY:                                                  ////
////                                                                   ////
//// November 20th, 2006 -                                             ////
////    Added support for EX_ST_UDP.C example                          ////
////    Added DNS                                                      ////
////    SMTP Example uses DNS to resolve IP                            ////
////                                                                   ////
////    May 24, 2004: Initial Public Release                           ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2004 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////



void ui_disp_ip_user(IP_ADDR *ip) {
   fprintf(USER,"%U.%U.%U.%U",ip->v[0],ip->v[1],ip->v[2],ip->v[3]);
}

void ui_settings(void) {
#if STACK_USE_MAC
   int1 disp=TRUE;
#endif
#if STACK_USE_TELNET
   int8 i;
#endif
#ifdef CCS_UDP_EXAMPLE
   IP_ADDR scratchIP;
#endif

#if STACK_USE_PPP
   if (ppp_is_connected())
   {
      fprintf(USER,"\r\n\nIP Address: "); ui_disp_ip_user(&AppConfig.MyIPAddr);
     #if STACK_USE_DNS
      fprintf(USER,"\r\nDNS: "); ui_disp_ip_user(&AppConfig.PrimaryDNSServer);
     #endif
      fprintf(USER,"\r\nBaud Rate: %LU",connected_baudrate);
   }
   else {
      fprintf(USER,"\r\n\nNot connected to ISP");
   }
   fprintf(USER,"\r\nISP Username: %s",ppp_username);
   fprintf(USER,"\r\nISP Password: %s",ppp_password);
   fprintf(USER,"\r\nISP Phonenumber: %s", ppp_phonenumber);
#elif STACK_USE_MAC
   fprintf(USER,"\r\n\nMAC Address: %X:%X:%X:%X:%X:%X",AppConfig.MyMACAddr.v[0],AppConfig.MyMACAddr.v[1],AppConfig.MyMACAddr.v[2],AppConfig.MyMACAddr.v[3],AppConfig.MyMACAddr.v[4],AppConfig.MyMACAddr.v[5]);
  #if STACK_USE_DHCP
   fprintf(USER,"\r\nDHCP Enabled: ");
   if (read_ext_eeprom(EE_NIC_DHCP)) {
      fprintf(USER,"TRUE");
      fprintf(USER,"\r\nDHCP Bound: ");
      if (DHCPIsBound()) {fprintf(USER,"TRUE");}
      else {fprintf(USER,"FALSE"); disp=FALSE;}
   }
   else {
      fprintf(USER,"FALSE");
   }
  #else
   disp=TRUE;
  #endif
   if (disp) {
      fprintf(USER,"\r\nIP Address: ");  ui_disp_ip_user(&AppConfig.MyIPAddr);
      fprintf(USER,"\r\nGateway: ");  ui_disp_ip_user(&AppConfig.MyGateway);
      fprintf(USER,"\r\nNetmask: "); ui_disp_ip_user(&AppConfig.MyMask);
     #if STACK_USE_DNS
      fprintf(USER,"\r\nDNS: "); ui_disp_ip_user(&AppConfig.PrimaryDNSServer);
     #endif
   }
#endif

#ifdef CCS_EMAIL_EXAMPLE
  fprintf(USER,"\r\nSMTP Server Hostname: %s", SMTP_HOST_NAME);
  fprintf(USER,"\r\nSMTP Server Port: %LU",SMTP_PORT);
  fprintf(USER,"\r\nE-Mail To: %s", SMTP_TO_ADDR);
  fprintf(USER,"\r\nE-Mail From: %s", SMTP_FROM_ADDR);
  fprintf(USER,"\r\nE-Mail Subject: %s", SMTP_SUBJECT);
  fprintf(USER,"\r\nE-Mail Body: %s", SMTP_BODY);
#endif

#ifdef CCS_UDP_EXAMPLE
   read_ext_eeprom_ip(EE_UDP_DEST_IP, &scratchIP);
   fprintf(USER,"\r\nUDP Destination IP: ");  ui_disp_ip_user(&scratchIP);
   fprintf(USER,"\r\nUDP Destination Port: %LU", read_ext_eeprom16(EE_UDP_DEST_PORT));
   fprintf(USER,"\r\nUDP Source Port: %LU", read_ext_eeprom16(EE_UDP_SRC_PORT));
#endif

#if STACK_USE_TELNET
   for (i=0;i<TELNET_MAX_CONNECTIONS;i++) {
      fprintf(USER,"\r\nTelnet %U: ",i);
      if (TelnetIsOpen(i)) {
         fprintf(USER,"OPEN ST=%X S=%U IP=",TCB[telnet_active_socket[i]].smState,telnet_active_socket[i]);
         ui_disp_ip_user(&TCB[telnet_active_socket[i]].remote.IPAddr);
         fprintf(USER," RP=%LU",TCB[telnet_active_socket[i]].remotePort);
      }
      else {
         fprintf(USER,"LISTENING ST=%X",TCB[telnet_active_socket[i]].smState);
      }
   }
#endif
}

void fget_string(char* s, int max) {
   int len=0;
   char c;

   --max;

   while ((c=*s)!=0)
   {
      fputc(c, USER);
      len++;
      s++;
   }

   do {
     c=fgetc(USER);
     if(c==8) {  // Backspace
        if(len>0) {
          len--;
          s--;
          fputc(c,USER);
          fputc(' ',USER);
          fputc(c,USER);
        }
     } else if ((c>=' ')&&(c<='~'))
       if(len<max) {
         *s++ = c;
         len++;
         fputc(c,USER);
       }
   } while(c!=13);
   *s = 0;
}

int fget_int(int8 initial) {
  char s[4];
  signed int i;

  sprintf(s, "%U", initial);

  fget_string(s, 4);

  i=atol(s);
  return(i);
}

int16 fget_long(int16 initial) {
   char s[6];
   int16 i;

   sprintf(s, "%LU", initial);

   fget_string(s, 6);

   i=atol(s);
   return(i);
}

BYTE fgethex(int initial) {
   char hex[5];
   int8 ret;

   hex[0]='0';
   hex[1]='x';

   sprintf(&hex[2], "%X", initial);

   fget_string(&hex[2],3);
   ret=atol(hex);
   return(ret);
}

void ui_ip_to_ee(int16 address) {
   IP_ADDR ip;
   int i;
   read_ext_eeprom_ip(address, &ip);
   for (i=0;i<4;i++) {
      fprintf(USER, "  BYTE%U: ",i);
      ip.v[i]=fget_int(ip.v[i]);
      fprintf(USER,"\r\n");
   }
   write_ext_eeprom_ip(address, &ip);
}

#if STACK_USE_PPP
void ui_setup(void) {
   char str[64];

   fprintf(USER,"\r\n\nISP USERNAME: ");
   read_ext_eeprom_string(EE_ISP_USERNAME, str, sizeof(str)-1);
   fget_string(str,64);
   write_ext_eeprom_string(EE_ISP_USERNAME, str);

   fprintf(USER,"\r\nISP PASSWORD: ");
   read_ext_eeprom_string(EE_ISP_PASSWORD, str, sizeof(str)-1);
   fget_string(str,64);
   write_ext_eeprom_string(EE_ISP_PASSWORD,str);

   fprintf(USER,"\r\nISP PHONENUMBER: ");
   read_ext_eeprom_string(EE_ISP_PHONENUMBER, str, sizeof(str)-1);
   fget_string(str,64);
   write_ext_eeprom_string(EE_ISP_PHONENUMBER,str);

   fprintf(USER,"\r\n\nSaved to EEPROM.  Resetting...");

   reset_cpu();
}
#elif STACK_USE_MAC
void ui_setup(void) {
   char dhcp;
   int8 i;
   int16 addy;

   fprintf(USER,"\r\n\nMAC ADDRESS:");
   addy=EE_NIC_MAC;
   for(i=0;i<6;i++) {
     fprintf(USER, "\r\n  BYTE%U: ",i);
     dhcp=fgethex(read_ext_eeprom(addy));
     write_ext_eeprom(addy++,dhcp);
   }


   fprintf(USER,"\r\n\nUSE DHCP? (Y/N): ");
   do {
      dhcp=fgetc(USER);
      dhcp=toupper(dhcp);
   } while ((dhcp!='Y')&&(dhcp!='N'));
   fputc(dhcp,USER);

   if (dhcp=='N') {
      write_ext_eeprom(EE_NIC_DHCP,0);

      fprintf(USER,"\r\n\nIP ADDRESS:\r\n");
      ui_ip_to_ee(EE_NIC_IP);

      fprintf(USER,"\r\nGATEWAY:\r\n");
      ui_ip_to_ee(EE_NIC_GATEWAY);

      fprintf(USER,"\r\nNETMASK:\r\n");
      ui_ip_to_ee(EE_NIC_NETMASK);

     #if STACK_USE_DNS
      fprintf(USER, "\r\nDNS:\r\n");
      ui_ip_to_ee(EE_DNS);
     #endif
   }
   else {
      write_ext_eeprom(EE_NIC_DHCP,1);
   }

   fprintf(USER,"\r\n\nSaved to EEPROM.  Resetting...");

   reset_cpu();
}
#endif

#ifdef CCS_EMAIL_EXAMPLE
void ui_mail_settings(void) {
   int16 port;
   char str[64];

   fprintf(USER,"\r\n\nSMTP HOST NAME: ");
   read_ext_eeprom_string(EE_SMTP_HOSTNAME, str, sizeof(str)-1);
   fget_string(str,64);
   write_ext_eeprom_string(EE_SMTP_HOSTNAME, str);

   fprintf(USER,"\r\nSMTP PORT (USUALLY 25): ");
   port=fget_long(read_ext_eeprom16(EE_SMTP_PORT));
   write_ext_eeprom16(EE_SMTP_PORT, port);
   fprintf(USER,"\r\n");

   fprintf(USER,"\r\nTo: ");
   read_ext_eeprom_string(EE_SMTP_TO, str, sizeof(str)-1);
   fget_string(str,64);
   write_ext_eeprom_string(EE_SMTP_TO,str);

   fprintf(USER,"\r\nFrom: ");
   read_ext_eeprom_string(EE_SMTP_FROM, str, sizeof(str)-1);
   fget_string(str,64);
   write_ext_eeprom_string(EE_SMTP_FROM,str);

   fprintf(USER,"\r\nSubject: ");
   read_ext_eeprom_string(EE_SMTP_SUBJECT, str, sizeof(str)-1);
   fget_string(str,64);
   write_ext_eeprom_string(EE_SMTP_SUBJECT,str);

   fprintf(USER,"\r\nBody: ");
   read_ext_eeprom_string(EE_SMTP_BODY, str, sizeof(str)-1);
   fget_string(str,64);
   write_ext_eeprom_string(EE_SMTP_BODY,str);

   init_smtp_settings();
}
#endif

#ifdef CCS_UDP_EXAMPLE
void ui_udp_settings(void)
{
   int16 port;

   fprintf(USER,"\r\n\nUDP Destination IP:\r\n");
   ui_ip_to_ee(EE_UDP_DEST_IP);

   fprintf(USER,"\r\nUDP Destination Port (I will transmit to this port): ");
   port=fget_long(read_ext_eeprom16(EE_UDP_DEST_PORT));
   write_ext_eeprom16(EE_UDP_DEST_PORT, port);
   fprintf(USER,"\r\n");

   fprintf(USER,"\r\nUDP Source Port (I will listen to this port): ");
   port=fget_long(read_ext_eeprom16(EE_UDP_SRC_PORT));
   write_ext_eeprom16(EE_UDP_SRC_PORT, port);
   fprintf(USER,"\r\n");

   fprintf(USER,"\r\n\nSaved to EEPROM.  Resetting...");

   reset_cpu();
}
#endif

void ui_menu(void) {
   fprintf(USER,"\r\n\nD) Display Settings");
   #if STACK_USE_PPP
   fprintf(USER,"\r\nC) Connect Modem / PPP");
   fprintf(USER,"\r\nH) Hangup Modem / PPP");
   #endif
   fprintf(USER,"\r\nS) Configure TCP/IP (Will Reset Board)");
  #if STACK_USE_CCS_PICENS
   fprintf(USER,"\r\nT) Test Hardware (Will Reset Board)");
  #endif
   #ifdef CCS_EMAIL_EXAMPLE
   fprintf(USER,"\r\nM) Configure SMTP");
   fprintf(USER,"\r\nE) Send E-Mail");
   #endif
   #ifdef CCS_UDP_EXAMPLE
   fprintf(USER,"\r\nU) Configure UDP");
   #endif
   fprintf(USER,"\r\nZ) Clear EEPROM (Caution!)");
}

void ui_handle(void)
{
   char c;
   c=fgetc(USER);
   c=toupper(c);
   switch(c) {
      #if STACK_USE_PPP
      case 'H':   if (ppp_is_connected()) {
                     fprintf(USER,"\r\n\nHanging up\r\n");
                     ppp_init(); //ppp_init() will reset the PPP and the MODEM forcing a hangup
                  }
                  else {
                     fprintf(USER,"\r\n\nNot Connected\r\n");
                  }
                  break;

      case 'C':   if (ppp_is_connected()) {
                     fprintf(USER,"\r\n\nAlready connected!\r\n");
                  }
                  else {
                     fprintf(USER,"\r\n\nConnecting\r\n");
                     picdem_ppp_connect();
                  }
                  break;
      #endif

   #ifdef CCS_EMAIL_EXAMPLE
      case 'M':   ui_mail_settings();
                  break;

      case 'E':   my_smtp_send();
                  fprintf(USER, "\r\n\nSending E-Mail.  Watch LCD for status.\r\n\n");
                  break;
   #endif

   #ifdef CCS_UDP_EXAMPLE
      case 'U':   ui_udp_settings();
                  break;
   #endif


      case 'T':   hardware_test();
                  break;

      case 'D':   ui_settings();
                  break;

      case 'S':   ui_setup();
                  break;

      case 'Z':   clear_ee();
                  fprintf(USER,"\r\nEEPROM Cleared\r\n");
                  break;
   }
   ui_menu();
}
