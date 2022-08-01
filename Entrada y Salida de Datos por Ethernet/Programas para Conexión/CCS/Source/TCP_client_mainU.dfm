object FrmTCPMain: TFrmTCPMain
  Left = 0
  Top = 0
  Width = 399
  Height = 374
  Caption = 'TCP/IP Client'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 67
    Top = 8
    Width = 52
    Height = 13
    Caption = 'IP Address'
  end
  object Label2: TLabel
    Left = 203
    Top = 8
    Width = 42
    Height = 13
    Caption = 'TCP Port'
  end
  object Label3: TLabel
    Left = 16
    Top = 56
    Width = 69
    Height = 13
    Caption = 'Incoming Data'
  end
  object Label4: TLabel
    Left = 16
    Top = 184
    Width = 70
    Height = 13
    Caption = 'Outgoing Data'
  end
  object IP_Address: TEdit
    Left = 67
    Top = 24
    Width = 121
    Height = 21
    TabOrder = 0
    Text = '127.0.0.1'
  end
  object Port: TEdit
    Left = 203
    Top = 24
    Width = 121
    Height = 21
    TabOrder = 1
    Text = '1000'
  end
  object IncommingData: TMemo
    Left = 11
    Top = 72
    Width = 374
    Height = 105
    Color = clInfoBk
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object SendData: TMemo
    Left = 11
    Top = 200
    Width = 374
    Height = 105
    ScrollBars = ssVertical
    TabOrder = 3
  end
  object Button1: TButton
    Left = 106
    Top = 312
    Width = 75
    Height = 25
    Action = ConnectAct
    TabOrder = 4
  end
  object SendButt: TButton
    Left = 210
    Top = 312
    Width = 75
    Height = 25
    Action = SendAct
    TabOrder = 5
  end
  object TcpClient1: TTcpClient
    OnConnect = TcpClient1Connect
    OnDisconnect = TcpClient1Disconnect
    OnReceive = TcpClient1Receive
    OnError = TcpClient1Error
    Left = 152
    Top = 168
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
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 200
    Top = 184
  end
end
