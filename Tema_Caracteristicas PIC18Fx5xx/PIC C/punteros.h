typedef union
{
  struct {
    unsigned BIT0:1;
    unsigned BIT1:1;
    unsigned BIT2:1;
    unsigned BIT3:1;
    unsigned BIT4:1;
    unsigned BIT5:1;
    unsigned BIT6:1;
    unsigned BIT7:1;
  };
  struct {
    unsigned B0:1;
    unsigned B1:1;
    unsigned B2:1;
    unsigned B3:1;
    unsigned B4:1;
    unsigned B5:1;
    unsigned B6:1;
    unsigned B7:1;
  };

} MY_BYTE;

MY_BYTE PORTAbits;
#BYTE PORTAbits  = 0x0F80               // Registro del puerto A


typedef union
{
  BYTE _byte[2];    //For byte access
  long _word;       //For word access
  struct {
        BYTE LO;
        BYTE HI;
  };
  struct {
    unsigned BIT0:1;
    unsigned BIT1:1;
    unsigned BIT2:1;
    unsigned BIT3:1;
    unsigned BIT4:1;
    unsigned BIT5:1;
    unsigned BIT6:1;
    unsigned BIT7:1;
    unsigned BIT8:1;
    unsigned BIT9:1;
    unsigned BIT10:1;
    unsigned BIT11:1;
    unsigned BIT12:1;
    unsigned BIT13:1;
    unsigned BIT14:1;
    unsigned BIT15:1;
  };
  struct {
    unsigned B0:1;
    unsigned B1:1;
    unsigned B2:1;
    unsigned B3:1;
    unsigned B4:1;
    unsigned B5:1;
    unsigned B6:1;
    unsigned B7:1;
    unsigned B8:1;
    unsigned B9:1;
    unsigned B10:1;
    unsigned B11:1;
    unsigned B12:1;
    unsigned B13:1;
    unsigned B14:1;
    unsigned B15:1;
  };

} MY_LONG_BYTE;


