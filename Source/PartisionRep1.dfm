object PartisionRep: TPartisionRep
  Left = 226
  Top = 110
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1057#1087#1080#1089#1086#1082' '#1088#1072#1079#1076#1077#1083#1086#1074' '#1056#1069#1055
  ClientHeight = 289
  ClientWidth = 273
  Color = 10930928
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 13
  object ListBox1: TListBox
    Left = 16
    Top = 8
    Width = 137
    Height = 273
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
      '1234567890123456')
    ParentFont = False
    Sorted = True
    TabOrder = 0
  end
  object LoadBtn: TButton
    Left = 176
    Top = 24
    Width = 75
    Height = 25
    Caption = #1047#1072#1075#1088#1091#1079#1080#1090#1100
    TabOrder = 1
    OnClick = LoadBtnClick
  end
  object Button2: TButton
    Left = 176
    Top = 64
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 2
    OnClick = Button2Click
  end
end
