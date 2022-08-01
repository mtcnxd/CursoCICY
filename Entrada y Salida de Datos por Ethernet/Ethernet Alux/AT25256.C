///////////////////////////////////////////////////////////////////////////
////   Library for an ATMEL25128 or ATMEL25256                         ////
////    ATMEL25128 has 16,384 (or 13bits of addressing) locations      ////
////    ATMEL25256 has 32,768 words of 8 bits                          ////
////                                                                   ////
////   Uses software (bit banging on the port pins)                    ////
////                                                                   ////
////                     Pin Layout                                    ////
////   ATMEL EEPROM pin     to      Microchip MCU Pin                  ////
////   ----------------             -----------------                  ////
////   1 (CS)                       PortB 0                            ////
////   2 (SO)                       PortC 4                            ////
////   3 (WP)                       +5V                                ////
////   4 (GND)                      GND                                ////
////   5 (SI)                       PortC 5                            ////
////   6 (SCK)                      PortC 3                            ////
////   7 (HOLD)                     +5V                                ////
////   8 (VCC)                      +5V                                ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   b = ext_eerpom_ready();  Returns TRUE if the eeprom is ready    ////
////                            to receive opcodes                     ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef OUTPUT_DRIVE_MACRO
 #if defined(__PCH__)
   #define OUTPUT_DRIVE_MACRO(x) bit_clear(*(x/8+18),x%8)
 #else
   #define OUTPUT_DRIVE_MACRO(x) bit_clear(*(x/8+0x80),x%8)
 #endif
#endif

typedef union {
   int16 val;
   int8 b[2];
} EEPROM_ADDRESS;

#define ext_eeprom_write_enable()   ext_eeprom_set_status(2)

int8 at25256_spi_exchange(int8 out) {
 #if EEPROM_USE_SPI
   return(spi_read(out));
 #else
   int8 in=0,i;
   for(i=1;i<=8;++i) {
      output_bit(EEPROM_DI, shift_left(&out,1,0));
      output_high(EEPROM_CLK);		//data latches
      shift_left(&in,1,input(EEPROM_DO));
      output_low(EEPROM_CLK);		//back to idle
   }
   return(in);
 #endif
}

void ext_eeprom_set_status(int8 status) {
   output_low(EEPROM_SELECT);   //at25256 is cs active LOW
   at25256_spi_exchange(1);   //status
   at25256_spi_exchange(status);
   output_high(EEPROM_SELECT);   //at25256 is cs active LOW
}

int8 ext_eeprom_ready() {
   BYTE data;

   output_low(EEPROM_SELECT);

   at25256_spi_exchange(5);   //rdsr opcode
   data=at25256_spi_exchange(0);

   output_high(EEPROM_SELECT);
   return (!bit_test(data, 0));
}

void write_ext_eeprom(EEPROM_ADDRESS address, BYTE data) {
   do {} while (!(ext_eeprom_ready())); //wait until the eeprom is out of the previous write state, if applicable

   output_low(EEPROM_SELECT);
   at25256_spi_exchange(0x06);
   output_high(EEPROM_SELECT);

   output_low(EEPROM_SELECT);
   at25256_spi_exchange(0x02);   //write opcode
   at25256_spi_exchange(address.b[1]);
   at25256_spi_exchange(address.b[0]);
   at25256_spi_exchange(data);
   output_high(EEPROM_SELECT);
}


BYTE read_ext_eeprom(EEPROM_ADDRESS address) {
   BYTE data;

   do {} while (!(ext_eeprom_ready())); //wait until the eeprom is out of the previous write state, if applicable

   output_low(EEPROM_SELECT);
   at25256_spi_exchange(0x03);   //read opcode
   at25256_spi_exchange(address.b[1]);
   at25256_spi_exchange(address.b[0]);
   data=at25256_spi_exchange(0);
   output_high(EEPROM_SELECT);

   return(data);
}

void init_ext_eeprom(void) {
   int8 to=250;

   output_high(EEPROM_SELECT);   //at25256 is cs active LOW

   OUTPUT_DRIVE_MACRO(EEPROM_SELECT);
   
 #if EEPROM_USE_SPI
   setup_spi(SPI_MASTER | SPI_L_TO_H | SPI_XMIT_L_TO_H | SPI_CLK_DIV_4);
 #else
   output_low(EEPROM_DI);
   output_low(EEPROM_CLK);
 #endif

   OUTPUT_DRIVE_MACRO(EEPROM_DI);
   OUTPUT_DRIVE_MACRO(EEPROM_CLK);
   output_float(EEPROM_DO);


   while(!ext_eeprom_ready() && to)
   {
      to--;
      delay_ms(1);
   }

   ext_eeprom_write_enable();
}
