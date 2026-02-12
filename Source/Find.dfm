object FindBox: TFindBox
  Left = 77
  Top = 181
  BorderStyle = bsDialog
  Caption = #1053#1072#1081#1090#1080' '#1088#1072#1079#1076#1077#1083
  ClientHeight = 98
  ClientWidth = 209
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
    Left = 13
    Top = 23
    Width = 74
    Height = 13
    Caption = #1040#1073#1073#1088#1077#1074#1080#1072#1090#1091#1088#1072':'
  end
  object CancelBtn: TButton
    Left = 53
    Top = 65
    Width = 72
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 1
    OnClick = CancelBtnClick
  end
  object OKBtn: TButton
    Left = 131
    Top = 65
    Width = 72
    Height = 25
    Caption = #1054#1050
    Default = True
    TabOrder = 2
    OnClick = OKBtnClick
  end
  object AbbrEdit: TEdit
    Left = 98
    Top = 20
    Width = 104
    Height = 24
    CharCase = ecUpperCase
    TabOrder = 0
    OnChange = AbbrEditChange
    OnEnter = AbbrEditEnter
  end
end
