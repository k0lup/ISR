object SettingBox: TSettingBox
  Left = 51
  Top = 141
  BorderStyle = bsDialog
  Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1072
  ClientHeight = 93
  ClientWidth = 350
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
  object CancelBtn: TButton
    Left = 194
    Top = 63
    Width = 73
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 0
    OnClick = CancelBtnClick
  end
  object OKBtn: TButton
    Left = 272
    Top = 63
    Width = 73
    Height = 25
    Caption = #1054#1050
    TabOrder = 1
    OnClick = OKBtnClick
  end
  object Panel1: TPanel
    Left = 7
    Top = 7
    Width = 338
    Height = 46
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 2
    object AllowBackCrossingCheckBox: TCheckBox
      Left = 7
      Top = 13
      Width = 325
      Height = 14
      Caption = #1056#1072#1079#1088#1077#1096#1080#1090#1100' '#1087#1077#1088#1077#1093#1086#1076#1099' '#1085#1072#1079#1072#1076' '#1087#1088#1080' '#1087#1077#1088#1077#1084#1077#1097#1077#1085#1080#1080' '#1087#1086' '#1089#1077#1082#1094#1080#1080
      TabOrder = 0
    end
  end
  object Button1: TButton
    Left = 8
    Top = 64
    Width = 113
    Height = 25
    Caption = #1050#1086#1085#1092#1080#1075#1091#1088#1072#1094#1080#1103' '#1084#1077#1085#1102
    TabOrder = 3
    OnClick = Button1Click
  end
end
