//////////////////////////////////////////////////////////////////////////////
//
// ex7a.c - Example 7A from the Embedded Internet/Embedded Ethernet tutorial
//
// Displays incoming Ethernet packets over RS232
//
//////////////////////////////////////////////////////////////////////////////

#include "ccstcpip.h"

void MACDisplayHeader(MAC_ADDR *mac, int8 type) {
   int8 i;
   printf("\r\nMAC: ");
   for (i=0;i<6;i++) {
      printf("%X", mac->v[i]);
      if (i!=5)
         putc(':');
   }
   printf("  PROT:0x08%X ",type);

   if (type==MAC_IP)
      printf("[IP]");
   else if (type==MAC_ARP)
      printf("[ARP]");
}

void main(void) {
   MAC_ADDR mac;
   int8 type;

            /*
              #if STACK_USE_MCPINC
               #byte OSCTUNE=0xF9B
               OSCTUNE=0x40;   //5x PLL, /3 prescalar, no postcalar
               delay_ms(150);
              #endif
            */

printf("\r\n\nCCS TCP/IP TUTORIAL 7A\r\n");

   MACInit();

   while(TRUE) {
      if (MACGetHeader(&mac, &type)) {
         if (type!=MAC_UNKNOWN) {
            MACDisplayHeader(&mac, type);
         }
      }
   }
}
