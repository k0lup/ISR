object RequestBox: TRequestBox
  Left = 79
  Top = 172
  BorderStyle = bsDialog
  Caption = 'RequestBox'
  ClientHeight = 219
  ClientWidth = 384
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 13
  object MakeButton: TButton
    Left = 182
    Top = 189
    Width = 124
    Height = 25
    Caption = #1042#1099#1087#1086#1083#1085#1080#1090#1100' '#1086#1087#1077#1088#1072#1094#1080#1102
    TabOrder = 2
    OnClick = MakeButtonClick
  end
  object CancelBtn: TButton
    Left = 315
    Top = 189
    Width = 60
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 1
    OnClick = CancelBtnClick
  end
  object OperationTextMemo: TMemo
    Left = 7
    Top = 7
    Width = 371
    Height = 169
    Color = clBtnFace
    Ctl3D = True
    ParentCtl3D = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
end
