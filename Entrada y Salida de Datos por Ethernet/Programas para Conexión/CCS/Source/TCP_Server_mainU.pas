unit TCP_Server_mainU;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Sockets, StdCtrls, ActnList, XPStyleActnCtrls,
  ActnMan, ExtCtrls, winsock;

const
  WM_SOCKET_CLOSED = WM_USER + $37;

type
  {Hack class to start listening when a client disconnects}
  TconnectionMon = class(Tthread)
  private
    Flisten: boolean;
    TcpServer1: TTcpServer;
    procedure Execute; override;
  end;


  TFrmTCPMain = class(TForm)
    IP_Address: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Port: TEdit;
    IncommingData: TMemo;
    Label3: TLabel;
    SendData: TMemo;
    Label4: TLabel;
    Button1: TButton;
    SendButt: TButton;
    ActionManager1: TActionManager;
    ConnectAct: TAction;
    SendAct: TAction;
    TcpServer1: TTcpServer;
    Label5: TLabel;
    RemoteAdd: TEdit;
    RemotePort: TEdit;
    Label6: TLabel;
    DisconnectAct: TAction;
    Button2: TButton;
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure DisconnectActExecute(Sender: TObject);
    procedure DisconnectActUpdate(Sender: TObject);
    procedure SendActExecute(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure ConnectActExecute(Sender: TObject);
    procedure TcpServer1Accept(Sender: TObject; ClientSocket: TCustomIpClient);
    procedure SendActUpdate(Sender: TObject);
    procedure ConnectActUpdate(Sender: TObject);
  private
    { Private declarations }
    Fconnected: boolean;
    outBuff: string;
    ClientConnection: TconnectionMon;
    FClientSocket: TCustomIpClient;
    procedure DoSocketClosed(var msg: tmessage); message WM_SOCKET_CLOSED;
    procedure DoDisconnect(sender: Tobject);
    procedure DoNewData(Sender: TObject; Buf: pchar; var DataLen: Integer);
    procedure onReciveData(Sender: TObject; Buf: pchar; var DataLen: Integer);
  public
    { Public declarations }
  end;

var
  FrmTCPMain: TFrmTCPMain;

//************************************************************
implementation
//************************************************************

{$R *.dfm}

{----------------------------------------------------------------*
* Update Action captions                                         *
*----------------------------------------------------------------}
procedure TFrmTCPMain.ConnectActUpdate(Sender: TObject);
const
  CAPTIONS: array[boolean] of string[10] = ('Listen', 'Close');
begin
  (sender as Taction).caption := CAPTIONS[TcpServer1.Listening or Fconnected];
end;

procedure TFrmTCPMain.SendActUpdate(Sender: TObject);
begin
  (Sender as Taction).enabled := Fconnected and
                                 (SendData.Text <> '');
end;

{----------------------------------------------------------*
* DoNewData:                                               *
*   add new data to the memo                               *
*----------------------------------------------------------}
procedure TFrmTCPMain.DoNewData(Sender: TObject; Buf: pchar;
  var DataLen: Integer);
var
  Astr: string;
  index: integer;
  Achar: Pchar;
begin
  Astr := '';
  Achar := buf;
  for index := 1 to datalen do
    begin
      Astr := astr + Achar^;
      INC(Achar);
    end;
  self.IncommingData.lines.add(Astr);
end;

{----------------------------------------------------------*
* TcpServer1Accept:                                        *
*   Accepts the connection request, process data and loop  *
*   until disconnect.                                      *
*----------------------------------------------------------}
procedure TFrmTCPMain.TcpServer1Accept(Sender: TObject;
  ClientSocket: TCustomIpClient);
var
  buffsize: integer;
  buff: array[0..255] of char;
  tempbuff: string;
begin
  Fconnected := TRUE;
  FclientSocket := clientSocket;
  self.RemoteAdd.Text := clientsocket.RemoteHost;
  self.RemotePort.text := clientsocket.RemotePort;
  tcpserver1.RemoteHost := clientsocket.remotehost;
  tcpserver1.remoteport := clientsocket.remoteport;
  IncommingData.Lines.add(formatdatetime('HH:NN:SS.zz', now) + #9 +
                               'Connection Accepted: ' + ClientSocket.RemoteHost +
                               ':' + clientsocket.RemotePort);
  clientsocket.OnReceive := DoNewData;
  ClientSocket.OnDisconnect := DoDisconnect;

  // request a message if the socket is disconnected
  WSAAsyncSelect(clientSocket.Handle, self.Handle, WM_SOCKET_CLOSED, FD_CLOSE);

  // loop until connection is complete
  while Fconnected and (ClientSocket.Connected) do
    begin
      sleep(0);
      if clientsocket.WaitForData(50)
        then
          begin
            buffsize:= clientsocket.ReceiveBuf(buff[0], sizeof(buff));
          end;
      if outbuff <> ''
        then
          begin
            TempBuff := outbuff;
            Outbuff := '';
            clientsocket.SendBuf(Pchar(tempbuff)^, length(tempbuff))

          end;

      application.ProcessMessages;
    end;
  FclientSocket := nil;
end;

{------------------------------------------------------------*
* ConnectActExecute:                                         *
*   Open port to recive connections                          *
*------------------------------------------------------------}
procedure TFrmTCPMain.ConnectActExecute(Sender: TObject);
begin
  if (TcpServer1.listening or fconnected)
    then clientConnection.Suspend;
  TCPServer1.LocalPort := self.port.text;
  TCPserver1.OnReceive := OnReciveData;
  TCPServer1.Active := not (TcpServer1.Listening or Fconnected);
  self.IP_Address.text := tcpserver1.LocalHostAddr;
  self.Port.text := TCPServer1.localport;
  ClientConnection.TCPServer1 := TcpServer1;
  ClientConnection.Flisten := TCPServer1.active;
  if clientconnection.flisten
    then ClientConnection.Resume;
end;


{-----------------------------------------------------------*
* FormCreate event:                                         *
*   Init memory                                             *
*-----------------------------------------------------------}
procedure TFrmTCPMain.FormCreate(Sender: TObject);
begin
  ClientConnection := TconnectionMon.Create(TRUE);
end;

{------------------------------------------------------------*
*  FormDestroy event:                                        *
*    When form is destroyed release memory                   *
*------------------------------------------------------------}
procedure TFrmTCPMain.FormDestroy(Sender: TObject);
begin
  FreeAndNil(ClientConnection);
end;

{-------------------------------------------------------------*
* OnReciveData:                                               *
*   When data is recived display it in the memo               *
*-------------------------------------------------------------}
procedure TFrmTCPMain.onReciveData(Sender: TObject; Buf: pchar;
  var DataLen: Integer);
var
  Astr: string;
  index: integer;
  Achar: Pchar;
begin
  Astr := '';
  Achar := buf;
  for index := 1 to datalen do
    begin
      Astr := astr + Achar^;
      INC(Achar);
    end;
  self.IncommingData.lines.add(Astr);
end;

{--------------------------------------------------------*
* SendActExecute:                                        *
*   Moves the data from the memo to the buffer of data to*
*   send out                                             *
*--------------------------------------------------------}
procedure TFrmTCPMain.SendActExecute(Sender: TObject);
begin
  OutBuff := self.SendData.text;
  SendData.text := '';
end;

{------------------------------------------------------------*
* doDisconnect:                                              *
*   Event on disconnect.  This resets the captions to no     *
*   conection settings                                       *
*------------------------------------------------------------}
procedure TFrmTCPMain.DoDisconnect(sender: Tobject);
begin
  IncommingData.Lines.add(formatdatetime('HH:NN:SS.zz', now) + #9 +
                               'Disconnected: ' + RemoteAdd.text +
                               ':' + RemotePort.text);
  Fconnected := FALSE;
  RemoteAdd.text := '';
  RemotePort.text := '';
end;

{------------------------------------------------------------*
* doSocketClosed:                                            *
*   Handles a callback message when the socket is disconnect *
*------------------------------------------------------------}
procedure TFrmTCPMain.DoSocketClosed(var msg: tmessage);
begin
  Fconnected := FALSE;
end;

{-------------------------------------------------------------*
* DisconnectActUpdate:                                        *
*   Enables the disconnect option when we have a connection   *
*-------------------------------------------------------------}
procedure TFrmTCPMain.DisconnectActUpdate(Sender: TObject);
begin
  DisconnectAct.Enabled := Fconnected;
end;

{-------------------------------------------------------------*
* DisconnectActExecute:                                       *
    Disconnects the current connection                        *
*-------------------------------------------------------------}
procedure TFrmTCPMain.DisconnectActExecute(Sender: TObject);
begin
  if assigned(Fclientsocket)
    then FClientSocket.Disconnect;
end;

{-------------------------------------------------------------*
* FormClose:                                                  *
*   When the form closes, we'll make sure that any connection *
*   we have is disconnected                                   *
*-------------------------------------------------------------}
procedure TFrmTCPMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  if assigned(FClientSocket)
    then FclientSocket.Disconnect;
end;


{ TconnectionMon }


{----------------------------------------------------------*
* Execute method:                                          *
*   Just loop and keep the server active while waiting     *
*   for connections                                        *
*----------------------------------------------------------*}
procedure TconnectionMon.Execute;
begin
  inherited;
  repeat
    if Flisten and (not TcpServer1.active)
      then TcpServer1.Active := TRUE;
    sleep(200);
  until terminated;
end;

end. {Unit}
