unit UDP_example;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs,ExtCtrls, StdCtrls, Buttons, System.ComponentModel;

type
  TForm1 = class(TForm)
    ScrPortEdit: TEdit;
    DestPortEdit: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    IncommingData: TMemo;
    OutgoingData: TMemo;
    Label3: TLabel;
    Label4: TLabel;
    SendButt: TBitBtn;
    Bevel1: TBevel;
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.nfm}

end.
