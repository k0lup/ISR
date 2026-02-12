object StencilBox: TStencilBox
  Left = 80
  Top = 184
  BorderStyle = bsDialog
  Caption = #1058#1088#1072#1092#1072#1088#1077#1090
  ClientHeight = 98
  ClientWidth = 261
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnActivate = FormActivate
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 13
    Top = 20
    Width = 53
    Height = 13
    Caption = #1058#1088#1072#1092#1072#1088#1077#1090':'
  end
  object CancelBtn: TButton
    Left = 27
    Top = 65
    Width = 72
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 1
    OnClick = CancelBtnClick
  end
  object OKBtn: TButton
    Left = 105
    Top = 65
    Width = 72
    Height = 25
    Caption = #1054#1050
    Default = True
    TabOrder = 2
    OnClick = OKBtnClick
  end
  object StencilEdit: TMaskEdit
    Left = 72
    Top = 16
    Width = 129
    Height = 23
    EditMask = '!cccccccccccc-0000;1; '
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = 17
    ParentFont = False
    TabOrder = 0
    Text = '            -    '
  end
end
