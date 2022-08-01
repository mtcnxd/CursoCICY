///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                          EX_ST_UDP.C                              ////
////                                                                   ////
//// An example program demonstrating using Microchip's TCP/IP stack   ////
//// with CCS's PCH C Compiler.                                        ////
////                                                                   ////
//// Communication between two Ethernet development boards is          ////
//// demonstrated by using UDP.  Configuration of each board is        ////
//// accomplished over the serial link.                                ////
////                                                                   ////
//// Use the serial interface to configure the destination UDP IP      ////
//// address and UDP destination port.  You can also configure what    ////
//// UDP port to listen to for incoming packets.                       ////
////                                                                   ////
//// The transmitted packet contains two bytes.  The first byte        ////
//// contains the ADC value, the second byte is the current button     ////
//// state (inverted).                                                 ////
////                                                                   ////
//// If it receives a packet, it assumes that the first byte is an ADC ////
//// value from another unit, and the second byte is a button state    ////
//// from another unit.  The received ADC value will be displayed on   ////
//// the first line of the LCD.  The second LED will show the last     ////
//// received state of the other unit's button state (so if you press  ////
//// the button on UNITA, the LED on UNITB will go on and vice versa). ////
////                                                                   ////
//// The first line of the LCD will display Ethernet/PPP status.  If   ////
//// the unit is listening for packets, it will display it's IP        ////
//// adress on the first line of the LCD.  Once it receives 1 packet   ////
//// it will display the first byte received (the ADC value) on the    ////
//// first line of the LCD.                                            ////
////                                                                   ////
//// The second line of the LCD shows the current TX status, whether   ////
//// or not a UDP socket has been opened.  It will either say          ////
//// 'CONNECTING' or 'CONNECTED'.  Remember that UDP is a              ////
//// connectionless transfer protocol, there really is no              ////
//// 'CONNECTED' state with UDP.  When it says 'CONNECTED' this        ////
//// simply means that ARP has been resolved, and a valid UDP          ////
//// socket has been opened from the TCP/IP stack.  Even though        ////
//// it is 'CONNECTED' you may be sending packets to nowhere.          ////
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
//// November 20th, 2006 -                                             ////
////    Initial Release.                                               ////
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

#define STACK_USE_DNS         TRUE   //we shall use DNS in this example
#define STACK_USE_ICMP        TRUE   //for ping
#define STACK_USE_UDP         TRUE
#define STACK_USE_IP_GLEANING FALSE

#use rs232(baud=9600, xmit=PIN_C6,rcv=PIN_C7, STREAM=USER)
#define debug_printf   printf

#include "tcpip/stacktsk.c"    //include Microchip's TCP/IP stack

#define CCS_UDP_EXAMPLE   TRUE

#include "ccshelper.c"   //include CCS's PICDEM.net/PICNET helper functions
#include "ccshelperui.c"

enum
{
   UDP_TX_SM_ARP_REQ=0,
   UDP_TX_SM_ARP_RES,
   UDP_TX_SM_CONNECT,
   UDP_TX_SM_CONNECTED,
   UDP_TX_SM_5S_DELAY
} ge_UDPTXSM = 0;

UDP_SOCKET g_UDPRXSocket = INVALID_UDP_SOCKET;

struct
{
   int8 adc;
   int8 button;
   int1 isValid;
} UDPRXPacket = {0};

void LCDTask(void)
{
   static TICKTYPE lastTick;
   if (TickGetDiff(TickGet(), lastTick) > (TICKS_PER_SECOND/2))
   {
      lastTick = TickGet();
      if (!MACIsLinked())
      {
        #if STACK_USE_MAC
         printf(lcd_putc,"\fNO ETHERNET");
        #else
         printf(lcd_putc,"\fNO PPP");
        #endif
      }
     #if STACK_USE_DHCP
      else if (read_ext_eeprom(EE_NIC_DHCP) && !DHCPIsBound())
      {
         printf(lcd_putc,"\fDHCP NOT BOUND");
      }
     #endif
      else
      {
         if (g_UDPRXSocket == INVALID_UDP_SOCKET)
            printf(lcd_putc,"\fRX: ERROR");
         else if (UDPRXPacket.isValid)
         {
            printf(lcd_putc, "\fRX: ADC=%X", UDPRXPacket.adc);
         }
         else
         {
            printf(lcd_putc,"\f%u.%u.%u.%u\n", MY_IP_BYTE1, MY_IP_BYTE2,
                  MY_IP_BYTE3, MY_IP_BYTE4);
         }

         if (ge_UDPTXSM == UDP_TX_SM_CONNECTED)
            printf(lcd_putc,"\nTX: CONNECTED");
         else
            printf(lcd_putc,"\nTX: CONNECTING");
      }
   }
}

void MyUDPTXTask(void)
{
   static UDP_SOCKET socket;
   static NODE_INFO remote;
   static TICKTYPE lastTick;
   TICKTYPE currTick;

   currTick=TickGet();

   switch (ge_UDPTXSM)
   {
      default:
      case UDP_TX_SM_ARP_REQ:
   #if STACK_USE_ARP
         read_ext_eeprom_ip(EE_UDP_DEST_IP, &remote.IPAddr);
         if (ARPIsTxReady())
         {
            ARPResolve(&remote.IPAddr);
            lastTick=currTick;
            ge_UDPTXSM=UDP_TX_SM_ARP_RES;
         }
         break;

      case UDP_TX_SM_ARP_RES:
         if (ARPIsResolved(&remote.IPAddr, &remote.MACAddr))
            ge_UDPTXSM=UDP_TX_SM_CONNECT;
         else if (TickGetDiff(currTick, lastTick) > (TICKS_PER_SECOND * 2))
            ge_UDPTXSM=UDP_TX_SM_ARP_REQ;
         break;
   #endif

      case UDP_TX_SM_CONNECT:
         socket=UDPOpen(
               read_ext_eeprom16(EE_UDP_DEST_PORT)+read_ext_eeprom16(EE_UDP_SRC_PORT), //the source port, not important for the TX task.  just make it different
               &remote,
               read_ext_eeprom16(EE_UDP_DEST_PORT)
            );
         lastTick = currTick;
         if (socket != INVALID_UDP_SOCKET)
            ge_UDPTXSM = UDP_TX_SM_CONNECTED;
         else
            ge_UDPTXSM = UDP_TX_SM_5S_DELAY;
         break;

      case UDP_TX_SM_CONNECTED:
         if (TickGetDiff(currTick, lastTick) >= (TICKS_PER_SECOND / 3))
         {
            if (UDPIsPutReady(socket))
            {
               lastTick = currTick;
               UDPPut(read_adc());
               UDPPut(input(USER_BUTTON1));
               UDPFlush();
            }
         }
         if (!MACIsLinked())
         {
            UDPClose(socket);
            ge_UDPTXSM = UDP_TX_SM_5S_DELAY;
         }
         break;

      case UDP_TX_SM_5S_DELAY:
         if (TickGetDiff(currTick,lastTick) >= (5 * TICKS_PER_SECOND))
            ge_UDPTXSM = UDP_TX_SM_ARP_REQ;
         break;
   }
}

void MyUDPRXTask(void)
{
   static NODE_INFO remote;
   static TICKTYPE lastTick;
   TICKTYPE currTick;

   currTick=TickGet();

   if (g_UDPRXSocket == INVALID_UDP_SOCKET)
      g_UDPRXSocket = UDPOpen(read_ext_eeprom16(EE_UDP_SRC_PORT),NULL,INVALID_UDP_SOCKET);
   else if (UDPIsGetReady(g_UDPRXSocket))
   {
      UDPGet(&UDPRXPacket.adc);
      UDPGet(&UDPRXPacket.button);
      UDPRXPacket.isValid = TRUE;
      UDPDiscard();
   }
}

void splash(void)
{
   printf("\r\n\n\nEX_ST_UDP.C - UDP Transmit/Receive Example\r\n\n");

   UDPRXPacket.isValid = FALSE;

   ui_settings();
   ui_menu();

   LCDTask();
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

      if (UDPRXPacket.isValid)
      {
         if (UDPRXPacket.button)
            output_high(USER_LED2);
         else
            output_low(USER_LED2);
      }

      if (kbhit(USER))
         ui_handle();

      #if STACK_USE_PPP
      if (ppp_is_connected())
      {
      #endif
         StackTask();
         MyUDPTXTask();
         MyUDPRXTask();
         LCDTask();
      #if STACK_USE_PPP
      }
      #endif
   }
}
