object FrmTCPMain: TFrmTCPMain
  Left = 0
  Top = 0
  Width = 376
  Height = 419
  Caption = 'TCP/IP Server'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 55
    Top = 8
    Width = 52
    Height = 13
    Caption = 'IP Address'
  end
  object Label2: TLabel
    Left = 191
    Top = 8
    Width = 42
    Height = 13
    Caption = 'TCP Port'
  end
  object Label3: TLabel
    Left = 16
    Top = 96
    Width = 69
    Height = 13
    Caption = 'Incoming Data'
  end
  object Label4: TLabel
    Left = 16
    Top = 224
    Width = 70
    Height = 13
    Caption = 'Outgoing Data'
  end
  object Label5: TLabel
    Left = 55
    Top = 48
    Width = 92
    Height = 13
    Caption = 'Remote IP Address'
  end
  object Label6: TLabel
    Left = 191
    Top = 48
    Width = 82
    Height = 13
    Caption = 'Remote TCP Port'
  end
  object IP_Address: TEdit
    Left = 55
    Top = 24
    Width = 121
    Height = 21
    ReadOnly = True
    TabOrder = 0
    Text = '127.0.0.1'
  end
  object Port: TEdit
    Left = 191
    Top = 24
    Width = 121
    Height = 21
    TabOrder = 1
    Text = '1000'
  end
  object IncommingData: TMemo
    Left = 11
    Top = 112
    Width = 350
    Height = 105
    Color = clInfoBk
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object SendData: TMemo
    Left = 11
    Top = 240
    Width = 350
    Height = 105
    ScrollBars = ssVertical
    TabOrder = 3
  end
  object Button1: TButton
    Left = 54
    Top = 352
    Width = 75
    Height = 25
    Action = ConnectAct
    TabOrder = 4
  end
  object SendButt: TButton
    Left = 238
    Top = 352
    Width = 75
    Height = 25
    Action = SendAct
    TabOrder = 5
  end
  object RemoteAdd: TEdit
    Left = 55
    Top = 64
    Width = 121
    Height = 21
    Color = clInfoBk
    Enabled = False
    ReadOnly = True
    TabOrder = 6
  end
  object RemotePort: TEdit
    Left = 191
    Top = 64
    Width = 121
    Height = 21
    Color = clInfoBk
    Enabled = False
    ReadOnly = True
    TabOrder = 7
  end
  object Button2: TButton
    Left = 146
    Top = 352
    Width = 75
    Height = 25
    Action = DisconnectAct
    TabOrder = 8
  end
  object ActionManager1: TActionManager
    Left = 80
    Top = 128
    StyleName = 'XP Style'
    object ConnectAct: TAction
      Caption = 'Connect'
      OnExecute = ConnectActExecute
      OnUpdate = ConnectActUpdate
    end
    object SendAct: TAction
      Caption = 'Send'
      OnExecute = SendActExecute
      OnUpdate = SendActUpdate
    end
    object DisconnectAct: TAction
      Caption = 'Disconnect'
      OnExecute = DisconnectActExecute
      OnUpdate = DisconnectActUpdate
    end
  end
  object TcpServer1: TTcpServer
    OnAccept = TcpServer1Accept
    Left = 112
    Top = 184
  end
end
