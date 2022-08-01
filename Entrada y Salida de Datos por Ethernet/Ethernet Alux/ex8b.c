//////////////////////////////////////////////////////////////////////////////
//
// ex8b.c - Example 8B from the Embedded Internet/Embedded Ethernet tutorial
//
// Broadcasts an Ethernet frame (with a UDP/IP payload) every second.
// Use Ethereal to view network traffic.
//
//////////////////////////////////////////////////////////////////////////////

#include "ccstcpip.h"

void main(void) {
   NODE_INFO node;

   printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 8B\r\n");

   IPAddrInit();
   MACAddrInit();

   MACInit();

   memset(&node.MACAddr.v[0], 0xFF, sizeof(MAC_ADDR));

   node.IPAddr.v[0]=192;
   node.IPAddr.v[1]=168;
   node.IPAddr.v[2]=100;
   node.IPAddr.v[3]=8;

   while(TRUE) {
      if (IPIsTxReady(TRUE)) {
         IPPutArray(ExampleUDPPacket, sizeof(ExampleUDPPacket));
         IPPutHeader(&node, IP_PROT_UDP, sizeof(ExampleUDPPacket));
         MACFlush();
         output_toggle(USER_LED1);
         putc('.');
         
         delay_ms(1000);
      }
   }
}
