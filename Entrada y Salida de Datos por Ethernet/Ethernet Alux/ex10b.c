//////////////////////////////////////////////////////////////////////////////
//
// ex10b.c - Example 10B from the Embedded Internet/Embedded Ethernet tutorial
//
// Answer any Ping (ICMP) requests sent to this unit.
//
// NOTE: Change the code in IPAddrInit() to your desired IP address, which
//       is based on your network.
//
//////////////////////////////////////////////////////////////////////////////

#define STACK_USE_ICMP  1
#define STACK_USE_ARP   1
#include "ccstcpip.h"

void main(void) {
   MACAddrInit();
   IPAddrInit();

   printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 10B (ICMP RESPOND FROM STACKTASK)\r\n");

   StackInit();

   while(TRUE) {
      StackTask();
   }
}
