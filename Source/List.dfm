object PartitionListBox: TPartitionListBox
  Left = 82
  Top = 197
  ActiveControl = PartitionListBox
  BorderStyle = bsDialog
  Caption = #1057#1087#1080#1089#1086#1082' '#1088#1072#1079#1076#1077#1083#1086#1074
  ClientHeight = 284
  ClientWidth = 300
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
  object PartitionListBox: TListBox
    Left = 8
    Top = 1
    Width = 185
    Height = 275
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 15
    Items.Strings = (
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '1234567890123456'
      '123456789012345678901')
    ParentFont = False
    Sorted = True
    TabOrder = 0
    OnDblClick = PartitionListBoxDblClick
  end
  object ChooseBtn: TButton
    Left = 211
    Top = 10
    Width = 73
    Height = 25
    Caption = #1042#1099#1073#1088#1072#1090#1100
    Default = True
    TabOrder = 1
    OnClick = ChooseBtnClick
    IsControl = True
  end
  object CancelBtn: TButton
    Left = 211
    Top = 106
    Width = 73
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 3
    OnClick = CancelBtnClick
  end
  object FindBtn: TButton
    Left = 211
    Top = 75
    Width = 73
    Height = 25
    Caption = #1053#1072#1081#1090#1080
    TabOrder = 2
    OnClick = FindBtnClick
  end
  object LoadBtn: TButton
    Left = 211
    Top = 42
    Width = 73
    Height = 25
    Caption = #1047#1072#1075#1088#1091#1079#1080#1090#1100
    TabOrder = 4
    OnClick = ChooseBtnClick
  end
end
