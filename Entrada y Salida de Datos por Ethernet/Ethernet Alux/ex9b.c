//////////////////////////////////////////////////////////////////////////////
//
// ex9b.c - Example 9B from the Embedded Internet/Embedded Ethernet tutorial
//
// Uses ARP to poll the Ethernet network to determine allocated IP address.
// Will display results over RS232.
//
// NOTE: Change the code in IPAddrInit() to your desired IP address, which
//       is based on your network.
//
//////////////////////////////////////////////////////////////////////////////

#define STACK_USE_ARP 1
#include "ccstcpip.h"

int8 TIMER1_COUNTER=0;

#int_timer1
void isr_timer1(void) {
   TIMER1_COUNTER++;
}

#define is_arp_timeout() TIMER1_COUNTER>=5
void arp_timeout_clear(void) {
   disable_interrupts(INT_TIMER1);
   set_timer1(0);
   TIMER1_COUNTER=0;
   enable_interrupts(INT_TIMER1);
}

enum {ARP_ST_ASK=0, ARP_ST_ASK_WAIT=1, APR_ST_RESP_WAIT=2} my_arp_state=0;
IP_ADDR arp_ask_ip;

void my_arp_task(void) {
   NODE_INFO remoteNode;

   switch(my_arp_state) {
      case ARP_ST_ASK:
         arp_ask_ip.v[3]++;
         remoteNode.IPAddr = arp_ask_ip;
         my_arp_state=ARP_ST_ASK_WAIT;

      case ARP_ST_ASK_WAIT:
         if (ARPIsTxReady()) {
            output_toggle(USER_LED1);
            printf("\r\n%U.%U.%U.%U <-> ",
                        arp_ask_ip.v[0], arp_ask_ip.v[1],
                        arp_ask_ip.v[2], arp_ask_ip.v[3]
                     );
            ARPPut(&remoteNode, ARP_REQUEST);
            arp_timeout_clear();
            my_arp_state=APR_ST_RESP_WAIT;
         }
         break;

      default:
         if (is_arp_timeout()) {
            printf("NO RESPONSE");
            my_arp_state=ARP_ST_ASK;
         }
         break;
   }
}

void main(void) {
   NODE_INFO src;
   NODE_INFO arp_resp_src;
   int8 opCode, type;

   set_tris_b(0);
   printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 9B (ARP ASK)\r\n");

   MACAddrInit();
   IPAddrInit();

   MACInit();

   arp_ask_ip.v[0]=MY_IP_BYTE1;
   arp_ask_ip.v[1]=MY_IP_BYTE2;
   arp_ask_ip.v[2]=MY_IP_BYTE3;
   arp_ask_ip.v[3]=0;

   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
   arp_timeout_clear();
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);

   while(TRUE) {
      if (MACGetHeader(&src.MACAddr, &type)) {
         if (type==MAC_ARP) {
            if (ARPGet(&arp_resp_src, &opCode)) {
               if (  opCode==ARP_REPLY &&
                     (memcmp(&arp_resp_src.IPAddr, &arp_ask_ip, 4)==0)
                  )
                  {
                     printf("%X:%X:%X:%X:%X:%X",
                        arp_resp_src.MACAddr.v[0], arp_resp_src.MACAddr.v[1],
                        arp_resp_src.MACAddr.v[2], arp_resp_src.MACAddr.v[3],
                        arp_resp_src.MACAddr.v[4], arp_resp_src.MACAddr.v[5]
                     );
                     my_arp_state=ARP_ST_ASK;
                   }
            }
         }
      }
      my_arp_task();
   }
}
