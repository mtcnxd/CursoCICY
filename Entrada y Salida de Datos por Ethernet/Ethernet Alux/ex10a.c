//////////////////////////////////////////////////////////////////////////////
//
// ex10a.c - Example 10A from the Embedded Internet/Embedded Ethernet tutorial
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

enum {ARP_ST_IDLE=0, ARP_ST_REPLY=1} my_arp_state=0;
NODE_INFO arp_req_src;

void my_arp_task(void) {
   switch (my_arp_state) {
      case ARP_ST_REPLY:
         if ( ARPIsTxReady() ) {
            ARPPut(&arp_req_src, ARP_REPLY);
            my_arp_state=ARP_ST_IDLE;
            printf("Sent ARP Response\r\n");
         }
         break;

      default:
         break;
   }
}

char my_icmp_data[64];
enum {ICMP_ST_IDLE=0, ICMP_ST_REPLY=1} my_icmp_state=0;
int16 ICMP_Id, ICMP_Seq;
int8 ICMP_Size;
ICMP_CODE iCode;
NODE_INFO icmp_node;

void my_icmp_task(void) {
   switch(my_icmp_state) {
      case ICMP_ST_REPLY:
         if (ICMPIsTxReady()) {
            ICMPPut(&icmp_node, ICMP_ECHO_REPLY, my_icmp_data, ICMP_Size,
                        ICMP_Id, ICMP_Seq);
            printf("Sent ICMP Response\r\n");
            my_icmp_state = ICMP_ST_IDLE;
         }
         break;

      default:
         break;
   }
}

void main(void) {
   IP_ADDR localIP;
   ICMP_CODE icode;
   NODE_INFO src;
   int8 opCode, type, protocol;
   int16 len;

   set_tris_b(0);
   printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 10A (ICMP RESPOND)\r\n");

   MACAddrInit();
   IPAddrInit();

   MACInit();

   while(TRUE) {
      if (MACGetHeader(&src.MACAddr, &type)) {
         if (type==MAC_ARP) {
            if (ARPGet(&arp_req_src, &opCode)) {
               output_toggle(USER_LED1);
               my_arp_state=ARP_ST_REPLY;
               printf("Got ARP Request\r\n");
            }
         }
         else if (type==MAC_IP) {
            if (IPGetHeader(&localIP,&src,&protocol,&len)) {
               if (protocol == IP_PROT_ICMP) {
                  ICMP_Size=len;
                  if (ICMPGet(&iCode, my_icmp_data, &ICMP_Size,
                        &ICMP_Id, &ICMP_Seq)) {
                        if (iCode == ICMP_ECHO_REQUEST) {
                           printf("Got ICMP Request\r\n");
                           memcpy(&icmp_node,&src,sizeof(NODE_INFO));
                           output_toggle(USER_LED2);
                           my_icmp_state=ICMP_ST_REPLY;
                        }
                  }
               }
            }
         }
      }
      my_icmp_task();
      my_arp_task();
   }
}
