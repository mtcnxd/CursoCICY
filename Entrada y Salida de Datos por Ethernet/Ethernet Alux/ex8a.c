//////////////////////////////////////////////////////////////////////////////
//
// ex8a.c - Example 8A from the Embedded Internet/Embedded Ethernet tutorial
//
// Displays incoming Ethernet packets over RS232.
// If Ethernet payload contains an IP packet, display IP information.
//
//////////////////////////////////////////////////////////////////////////////

#include "ccstcpip.h"

void IPDisplayHeader(void) {
   IP_ADDR dest_ip;
   NODE_INFO node;
   int8 prot;
   int16 len;

   if (IPGetHeader(&dest_ip, &node, &prot, &len)) {
      printf("\r\n  DEST: %U.%U.%U.%U  SRC: %U.%U.%U.%U  LEN: %LU  PROT: %X ",
         dest_ip.v[0], dest_ip.v[1], dest_ip.v[2], dest_ip.v[3],
         node.IPAddr.v[0], node.IPAddr.v[1], node.IPAddr.v[2], node.IPAddr.v[3],
         len, prot);
      if (prot==IP_PROT_ICMP) {printf("[ICMP]");}
      else if (prot==IP_PROT_TCP) {printf("[TCP]");}
      else if (prot==IP_PROT_UDP) {printf("[UDP]");}
   }
   else {
      printf("\r\n  [MALFORMED IP]");
   }
}

void MACDisplayHeader(MAC_ADDR *mac, int8 type) {
   int8 i;
   printf("\r\nMAC: ");
   for (i=0;i<6;i++) {
      printf("%X", mac->v[i]);
      if (i!=5)
         putc(':');
   }
   printf("  PROT:0x08%X ",type);

   if (type==MAC_IP) {
      printf("[IP]");
      IPDisplayHeader();
   }
   else if (type==MAC_ARP)
      printf("[ARP]");
}

void main(void) {
   MAC_ADDR mac;
   int8 type;

   printf("\r\n\nCCS TCP/IP TUTORIAL, EXAMPLE 8A\r\n");

   MACAddrInit();

   MACInit();

   while(TRUE) {
      if (MACGetHeader(&mac, &type)) {
         if (type!=MAC_UNKNOWN) {
            MACDisplayHeader(&mac, type);
         }
      }
   }
}
