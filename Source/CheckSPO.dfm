object CheckSPOBox: TCheckSPOBox
  Left = 261
  Top = 185
  BorderStyle = bsDialog
  Caption = #1050#1086#1085#1090#1088#1086#1083#1100' '#1057#1055#1054
  ClientHeight = 148
  ClientWidth = 259
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
  object CloseBtn: TButton
    Left = 114
    Top = 117
    Width = 59
    Height = 25
    Caption = #1047#1072#1082#1088#1099#1090#1100
    TabOrder = 0
    OnClick = CloseBtnClick
  end
  object CheckBtn: TButton
    Left = 182
    Top = 117
    Width = 72
    Height = 25
    Caption = #1055#1088#1086#1074#1077#1088#1080#1090#1100
    TabOrder = 1
    OnClick = CheckBtnClick
  end
  object GroupBox1: TGroupBox
    Left = 7
    Top = 7
    Width = 247
    Height = 98
    Caption = #1055#1088#1086#1074#1077#1088#1103#1090#1100':'
    TabOrder = 2
    object AdjustmentCheckBox: TCheckBox
      Left = 13
      Top = 20
      Width = 157
      Height = 13
      Caption = #1060#1072#1081#1083' '#1085#1072#1089#1090#1088#1086#1081#1082#1080' '#1088#1072#1079#1076#1077#1083#1072
      TabOrder = 0
      OnClick = AnyCheckBoxClick
      OnKeyPress = AnyCheckBoxKeyPress
    end
    object StructureCheckBox: TCheckBox
      Left = 13
      Top = 39
      Width = 157
      Height = 14
      Caption = #1060#1072#1081#1083' '#1089#1090#1088#1091#1082#1090#1091#1088#1099' '#1088#1072#1079#1076#1077#1083#1072
      TabOrder = 1
      OnClick = AnyCheckBoxClick
    end
    object CPKUProgramsCheckBox: TCheckBox
      Left = 13
      Top = 59
      Width = 209
      Height = 13
      Caption = #1060#1072#1081#1083#1099' '#1087#1088#1086#1075#1088#1072#1084#1084' '#1088#1072#1079#1076#1077#1083#1072' '#1074' '#1062#1055#1050#1059
      TabOrder = 2
      OnClick = AnyCheckBoxClick
    end
    object SSBVSProgramsCheckBox: TCheckBox
      Left = 13
      Top = 78
      Width = 202
      Height = 14
      Caption = #1060#1072#1081#1083#1099' '#1087#1088#1086#1075#1088#1072#1084#1084' '#1088#1072#1079#1076#1077#1083#1072' '#1074' '#1057#1057' '#1041#1042#1057
      TabOrder = 3
      OnClick = AnyCheckBoxClick
    end
  end
end
