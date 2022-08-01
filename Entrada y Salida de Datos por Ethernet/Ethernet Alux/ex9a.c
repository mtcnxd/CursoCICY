//////////////////////////////////////////////////////////////////////////////
//
// ex9a.c - Example 9A from the Embedded Internet/Embedded Ethernet tutorial
//
// Answers any ARP requests (over RS232) that have been sent to this IP
// address.
//
// NOTE: Change the code in IPAddrInit() to your desired IP address, which
//       is based on your network.
//
//////////////////////////////////////////////////////////////////////////////

#define STACK_USE_ARP 1
#include "ccstcpip.h"

enum {ARP_ST_IDLE=0, ARP_ST_REPLY=1} my_arp_state=0;
NODE_INFO arp_req_src;

void my_arp_task(void) {
   switch (my_arp_state) {
      case ARP_ST_REPLY:
         if ( ARPIsTxReady() ) {
            ARPPut(&arp_req_src, ARP_REPLY);
            my_arp_state=ARP_ST_IDLE;
            printf("Sent ARP Response.\r\n");
         }
         break;

      default:
         break;
   }
}

void main(void) {
   NODE_INFO src;
   int8 opCode, type;

   set_tris_b(0);
   printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 9A (ARP RECEIVE)\r\n");

   MACAddrInit();
   IPAddrInit();

   MACInit();

   while(TRUE) {
      if (MACGetHeader(&src.MACAddr, &type)) {
         if (type==MAC_ARP) {
            if (ARPGet(&arp_req_src, &opCode)) {
               printf("Got ARP Request.\r\n");
               output_toggle(USER_LED1);
               my_arp_state=ARP_ST_REPLY;
            }
         }
      }
      my_arp_task();
   }
}
