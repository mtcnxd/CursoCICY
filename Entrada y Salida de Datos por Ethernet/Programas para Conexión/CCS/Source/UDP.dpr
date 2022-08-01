program UDP;

uses
  Forms,
  UDP_mainU in 'UDP_mainU.pas' {FrmUDPMain};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TFrmUDPMain, FrmUDPMain);
  Application.Run;
end.
