//////////////////////////////////////////////////////////////////////////////
//
// ex15.c - Example 15 from the Embedded Internet/Embedded Ethernet tutorial
//
// A webserver that accepts GET and POST commands.
//
// NOTE: Change the code in IPAddrInit() to your desired IP address, which
//       is based on your network.
//
//////////////////////////////////////////////////////////////////////////////

#define STACK_USE_ICMP  1
#define STACK_USE_ARP   1
#define STACK_USE_TCP   1
#define STACK_USE_HTTP  1
#include "ccstcpip.h"

#if STACK_USE_CCS_PICENS
 #include "tcpip/mlcd.c"
#else
 #include "tcpip/dlcd.c"
#endif

//here is this examples / page
const char  HTML_INDEX_PAGE[]="
<HTML><BODY BGCOLOR=#FFFFFF TEXT=#000000>
<IMG SRC=\"http://www.ccsinfo.com/pix/CCSlogotiny.gif\"><P>
<H1>CCS HTTP/CGI EXAMPLE</H1>
<FORM METHOD=GET>
<P>LCD: <INPUT TYPE=\"text\" NAME=\"lcd\" size=20 maxlength=16>
<BR>LED1:<INPUT type=\"radio\" name=\"led1\" value=1>ON &nbsp; &nbsp; &nbsp;
<INPUT type=\"radio\" name=\"led1\" value=0 checked>OFF
<BR>LED2:<INPUT type=\"radio\" name=\"led2\" value=1>ON &nbsp; &nbsp; &nbsp;
<INPUT type=\"radio\" name=\"led2\" value=0 checked>OFF
<BR><INPUT TYPE=\"submit\"></FORM>
<P><A HREF=\"/analog\">Analog Readings</A>
</BODY></HTML>
";

const char  HTML_ANALOG_PAGE[]="
<HTML><BODY BGCOLOR=#FFFFFF TEXT=#000000>
<IMG SRC=\"http://www.ccsinfo.com/pix/CCSlogotiny.gif\">
<H1>PICNET ADC READINGS</H1>
<P>%0
<BR>%1
<P><A HREF=\"/\">Change LCD/LEDs</A>
</BODY></HTML>
";

//this is a callback function to the HTTP stack.  see http.c
//this demo provides to web "pages", an index (/) and an about page (/about)
int32 http_get_page(char *file_str) {
   int32 file_loc=0;
   static char index[]="/";
   static char about[]="/analog";

   printf("\r\nRequest %s ",file_str);

   if (stricmp(file_str,index)==0)
      file_loc=label_address(HTML_INDEX_PAGE);

   else if (stricmp(file_str,about)==0)
      file_loc=label_address(HTML_ANALOG_PAGE);

   if (file_loc)
      printf("(FILE=%LU)",file_loc);
   else
      printf("(File Not Found)");

   return(file_loc);
}

//this is a callback function to the HTTP stack. see http.c
// this demo provides handling for two formatting chars, %0 and %1.
//  %0 is ADC for channel 0, %1 is ADC for channel 1.
int8 http_format_char(int32 file, char id, char *str, int8 max_ret) {
   char new_str[20];
   int8 len=0;
   int8 i;

   *str=0;

   switch(id) {
      case '0':
         set_adc_channel(0);
         delay_us(100);
         i=read_adc();
         sprintf(new_str,"<B>AN0 = </B>0x%X",i);
         len=strlen(new_str);
         break;

      case '1':
        #if STACK_USE_CCS_PICNET
         set_adc_channel(1);
         delay_us(100);
         i=read_adc();
         sprintf(new_str,"<B>AN1 = </B>0x%X",i);
         len=strlen(new_str);
        #else
         len=0;
        #endif
         break;

   }

   if (len) {
      if (len>max_ret) {len=max_ret;}
      memcpy(str,new_str,len);
   }

   return(len);
}

//this is a callback function to the HTTP stack. see http.c
//in this example it verifies that "pwd" is "master", if it is
//then it sets led1 and led2 ("led1" and "led2") based on their value
//and changes the lcd screen ("lcd").
void http_exec_cgi(int32 file, char *key, char *val) {
   static char led1_key[]="led1";
   static char led2_key[]="led2";
   static char lcd_key[]="lcd";
   int8 v;

   printf("\r\nCGI FILE=%LD KEY=%S VAL=%S", file, key, val);

   if (stricmp(key,led1_key)==0) {
      v=atoi(val);
      if (v) {output_low(USER_LED1);}
      else {output_high(USER_LED1);}
   }

   if (stricmp(key,led2_key)==0) {
      v=atoi(val);
      if (v) {output_low(USER_LED2);}
      else {output_high(USER_LED2);}
   }

   if (stricmp(key,lcd_key)==0) {
      printf(lcd_putc,"\f%s",val);
   }
}

void main(void) {
   MACAddrInit();
   IPAddrInit();

   init_user_io();

   lcd_init();

   printf(lcd_putc,"\fCCS CGI Example\nWaiting");
   printf("\r\n\nCCS CGI Example\r\nWaiting");

   set_adc_channel(0);

   StackInit();

   while(TRUE) {
      StackTask();
   }
}
