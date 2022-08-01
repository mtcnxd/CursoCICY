//////////////////////////////////////////////////////////////////////////////
//
// ex11.c - Example 11 from the Embedded Internet/Embedded Ethernet tutorial
//
// Network configuration is done automatically using DHCP.  Results, including
// the assigned IP address, is displayed on the LCD.
//
// Note: This will only work on a network that has a DHCP server.
//
//////////////////////////////////////////////////////////////////////////////

#define STACK_USE_ICMP  1
#define STACK_USE_ARP   1
#define STACK_USE_DHCP  1
#include "ccstcpip.h"

#if STACK_USE_CCS_PICENS
 #include "tcpip/mlcd.c"
#elif STACK_USE_CCS_PICEEC
 #include "tcpip/elcd.c"
#else
 #include "tcpip/dlcd.c"
#endif

void LCDTask(void) {
   static TICKTYPE lastTick;
   static enum {LCD_Display=0, LCD_Wait=1} state=0;

   switch(state) {
      case LCD_Display:
         printf(lcd_putc,"\fCCS TCP/IP Tut.");
         if (!MACIsLinked()) {
            printf(lcd_putc,"\nNo Ethernet");
         }
         else if (!DHCPIsBound()) {
            printf(lcd_putc,"\nDCHP Not Bound");
         }
         else {
            printf(lcd_putc,"\n%u.%u.%u.%u", MY_IP_BYTE1, MY_IP_BYTE2,
               MY_IP_BYTE3, MY_IP_BYTE4);
         }
         lastTick=TickGet();
         state=LCD_Wait;
         break;

      case LCD_Wait:
         if (TickGetDiff(TickGet(), lastTick) >= TICKS_PER_SECOND)
            state=LCD_Display;
         break;
   }
}

void main(void) {
   MACAddrInit();

   printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 11 (DHCP)\r\n");
   printf("Initializing LCD...\r\n");

   lcd_init();

   printf("LCD ready, DHCP status will be displayed on LCD!\r\n");

   StackInit();

   while(TRUE) {
      StackTask();
      LCDTask();
   }
}
