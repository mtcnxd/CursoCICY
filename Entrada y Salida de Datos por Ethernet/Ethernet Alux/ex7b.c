//////////////////////////////////////////////////////////////////////////////
//
// ex7b.c - Example 7B from the Embedded Internet/Embedded Ethernet tutorial
//
// Broadcasts an Ethernet frame (with a UDP/IP payload) every second.
// Use Ethereal to view network traffic.
//
//////////////////////////////////////////////////////////////////////////////

#include "ccstcpip.h"

void main(void) {
   MAC_ADDR mac_dest;

   printf("\r\n\nCCS TCP/IP TUTORIAL 7B\r\n");

   MACAddrInit();

   MACInit();

   mac_dest.v[0]=0xFF;
   mac_dest.v[1]=0xFF;
   mac_dest.v[2]=0xFF;
   mac_dest.v[3]=0xFF;
   mac_dest.v[4]=0xFF;
   mac_dest.v[5]=0xFF;

   while(TRUE) {
      if (MACIsTxReady(TRUE)) {
         MACPutArray(ExampleIPDatagram, sizeof(ExampleIPDatagram));
         MACPutArray(ExampleUDPPacket, sizeof(ExampleUDPPacket));

         MACPutHeader(
               &mac_dest, ETHER_IP,
               sizeof(ExampleIPDatagram) + sizeof(ExampleUDPPacket)
            );

         MACFlush();

         output_toggle(USER_LED2);
         delay_ms(1000);
      }
   }
}
