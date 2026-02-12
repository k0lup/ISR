object TitleBox: TTitleBox
  Left = 122
  Top = 172
  BorderStyle = bsDialog
  Caption = #1047#1072#1075#1086#1083#1086#1074#1086#1082' '#1088#1072#1079#1076#1077#1083#1072
  ClientHeight = 304
  ClientWidth = 486
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
  object Label1: TLabel
    Left = 8
    Top = 16
    Width = 102
    Height = 13
    Caption = #1047#1072#1075#1086#1083#1086#1074#1086#1082' '#1088#1072#1079#1076#1077#1083#1072':'
  end
  object Label2: TLabel
    Left = 8
    Top = 48
    Width = 81
    Height = 13
    Caption = #1048#1084#1103' '#1087#1088#1086#1090#1086#1082#1086#1083#1072':'
  end
  object Label3: TLabel
    Left = 8
    Top = 142
    Width = 77
    Height = 13
    Caption = #1054#1087#1077#1088#1072#1090#1086#1088' '#1040#1048#1057':'
  end
  object Label4: TLabel
    Left = 8
    Top = 80
    Width = 25
    Height = 13
    Caption = #1056#1069#1055':'
  end
  object Label5: TLabel
    Left = 8
    Top = 111
    Width = 96
    Height = 13
    Caption = #1054#1087#1077#1088#1072#1090#1086#1088' '#1055#1059' '#1040#1048#1057':'
  end
  object Label6: TLabel
    Left = 7
    Top = 174
    Width = 125
    Height = 13
    Caption = #1054#1087#1077#1088#1072#1090#1086#1088' '#1058#1050#1062#1050' '#1057#1057' '#1041#1042#1057':'
  end
  object Label7: TLabel
    Left = 327
    Top = 45
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
  object Label9: TLabel
    Left = 8
    Top = 237
    Width = 69
    Height = 13
    Caption = #1054#1087#1077#1088#1072#1090#1086#1088' '#1041#1057':'
  end
  object Label8: TLabel
    Left = 8
    Top = 206
    Width = 95
    Height = 13
    Caption = #1054#1087#1077#1088#1072#1090#1086#1088' '#1053#1058#1050' '#1057#1048':'
  end
  object ProtokolNamePart1Edit: TEdit
    Left = 145
    Top = 45
    Width = 179
    Height = 23
    AutoSelect = False
    CharCase = ecUpperCase
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
    OnChange = ProtokolNamePart1EditChange
  end
  object OperatorPUAISEdit: TEdit
    Left = 145
    Top = 108
    Width = 263
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 35
    ParentFont = False
    TabOrder = 6
  end
  object RAPEdit: TEdit
    Left = 145
    Top = 76
    Width = 264
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 35
    ParentFont = False
    TabOrder = 5
  end
  object OperatorAISEdit: TEdit
    Left = 145
    Top = 139
    Width = 263
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 35
    ParentFont = False
    TabOrder = 7
  end
  object OperatorTKCKEdit: TEdit
    Left = 145
    Top = 171
    Width = 263
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 35
    ParentFont = False
    TabOrder = 8
  end
  object OKBtn: TButton
    Left = 336
    Top = 273
    Width = 73
    Height = 25
    Caption = #1054#1050
    TabOrder = 1
    OnClick = OKBtnClick
  end
  object CancelBtn: TButton
    Left = 258
    Top = 273
    Width = 73
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 0
    OnClick = CancelBtnClick
  end
  object ProtokolNamePart2Edit: TEdit
    Left = 340
    Top = 45
    Width = 138
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
  end
  object OperatorNTKSIEdit: TEdit
    Left = 145
    Top = 202
    Width = 263
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 35
    ParentFont = False
    TabOrder = 9
  end
  object OperatorBSEdit: TEdit
    Left = 145
    Top = 234
    Width = 263
    Height = 23
    AutoSelect = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 35
    ParentFont = False
    TabOrder = 10
  end
  object TitleEdit: TEdit
    Left = 145
    Top = 13
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
    TabOrder = 2
    OnChange = TitleEditChange
  end
end
