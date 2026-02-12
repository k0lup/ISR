object CloseRep: TCloseRep
  Left = 192
  Top = 107
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = #1047#1072#1082#1088#1099#1090#1080#1077' '#1088#1072#1079#1076#1077#1083#1072' '#1056#1069#1055
  ClientHeight = 81
  ClientWidth = 300
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object ListBox1: TListBox
    Left = 8
    Top = 16
    Width = 153
    Height = 49
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 15
    Items.Strings = (
      '1234567890123456'
      '1234567890123456'
      '1234567890123456')
    ParentFont = False
    TabOrder = 0
  end
  object Button1: TButton
    Left = 176
    Top = 8
    Width = 97
    Height = 25
    Caption = #1047#1072#1082#1088#1099#1090#1100' '#1088#1072#1079#1076#1077#1083
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 184
    Top = 40
    Width = 81
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 2
    OnClick = Button2Click
  end
end
