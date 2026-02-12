object EndBox: TEndBox
  Left = 98
  Top = 156
  BorderStyle = bsDialog
  Caption = #1050#1086#1085#1077#1094' '#1088#1072#1079#1076#1077#1083#1072
  ClientHeight = 168
  ClientWidth = 485
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
  object Delimiter1Label: TLabel
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
  object Delimiter2Label: TLabel
    Left = 375
    Top = 13
    Width = 10
    Height = 18
    Caption = '-'
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object EndWithSavingProtBtn: TButton
    Left = 80
    Top = 59
    Width = 260
    Height = 25
    Caption = #1047#1072#1082#1086#1085#1095#1080#1090#1100' '#1088#1072#1079#1076#1077#1083' '#1089' '#1089#1086#1093#1088#1072#1085#1077#1085#1080#1077#1084' '#1087#1088#1086#1090#1086#1082#1086#1083#1072
    TabOrder = 0
    OnClick = EndWithSavingProtBtnClick
  end
  object EndWithoutSavingProtBtn: TButton
    Left = 80
    Top = 98
    Width = 260
    Height = 25
    Caption = #1047#1072#1082#1086#1085#1095#1080#1090#1100' '#1088#1072#1079#1076#1077#1083' '#1073#1077#1079' '#1089#1086#1093#1088#1072#1085#1077#1085#1080#1103' '#1087#1088#1086#1090#1086#1082#1086#1083#1072
    TabOrder = 1
    OnClick = EndWithoutSavingProtBtnClick
  end
  object CancelBtn: TButton
    Left = 80
    Top = 137
    Width = 260
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 2
    OnClick = CancelBtnClick
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
    MaxLength = 23
    ParentFont = False
    TabOrder = 3
  end
  object ProtokolNamePart2Edit: TEdit
    Left = 299
    Top = 8
    Width = 70
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 4
    Text = '123456789'
  end
  object ProtokolNamePart3Edit: TEdit
    Left = 391
    Top = 8
    Width = 34
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 5
    Text = '0000'
  end
end
