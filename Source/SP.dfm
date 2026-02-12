object SPBox: TSPBox
  Left = 91
  Top = 139
  BorderStyle = bsDialog
  Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1087#1088#1086#1090#1086#1082#1086#1083
  ClientHeight = 89
  ClientWidth = 528
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
  object Label2: TLabel
    Left = 11
    Top = 15
    Width = 81
    Height = 13
    Caption = #1048#1084#1103' '#1087#1088#1086#1090#1086#1082#1086#1083#1072':'
  end
  object Label7: TLabel
    Left = 288
    Top = 12
    Width = 11
    Height = 22
    Caption = '.'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label1: TLabel
    Left = 453
    Top = 15
    Width = 18
    Height = 20
    Caption = '-'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object ProtokolNamePart1Edit: TEdit
    Left = 106
    Top = 12
    Width = 179
    Height = 23
    AutoSelect = False
    CharCase = ecUpperCase
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 22
    ParentFont = False
    TabOrder = 2
    OnChange = ProtokolNamePart1EditChange
  end
  object ProtokolNamePart2Edit: TEdit
    Left = 301
    Top = 12
    Width = 146
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
    Text = '123456789'
  end
  object ProtokolNamePart3Edit: TEdit
    Left = 469
    Top = 11
    Width = 26
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 2
    ParentFont = False
    ReadOnly = True
    TabOrder = 4
    Text = '00'
  end
  object CancelBtn: TButton
    Left = 265
    Top = 59
    Width = 72
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 0
    OnClick = CancelBtnClick
  end
  object OKBtn: TButton
    Left = 343
    Top = 59
    Width = 72
    Height = 25
    Caption = #1054#1050
    Default = True
    TabOrder = 1
    OnClick = OKBtnClick
  end
end
