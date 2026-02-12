//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AdjNames.h"

// установка исходной строки
__fastcall AdjustmentNames::AdjustmentNames(AnsiString TransAdjustmentsString)
{
    AdjustmentsString = TransAdjustmentsString;
    StringLength = AdjustmentsString.Length();
    Reset();
}

// сбросить отсчет файлов в начальное состояние
void __fastcall AdjustmentNames::Reset()
{
    nPosition = 1;
}

// получить очередное имя файла настройки
AnsiString __fastcall AdjustmentNames::GetAdjustmentName()
{
    // если исходная строка пуста или позиция за границей строки
    if (StringLength == 0 || nPosition == -1) {
        return ("");
    }
    // текущая позиция для прохода строки
    int nCurrentPosition = nPosition;
    // возвращаемая строка
    AnsiString ReturnString;
    // цикл по строке
    while (1) {
        // если ';'
        if (AdjustmentsString[nCurrentPosition] == ';') {
            ReturnString = AdjustmentsString.SubString(nPosition, nCurrentPosition - nPosition);
            // если в строке еще есть символы
            if (nCurrentPosition < StringLength) {
                // выделение следующего имени начнется со след. символа
                nPosition = nCurrentPosition + 1;
            }
            else {
                // больше имен нет
                nPosition = -1;
            }
            // выйдем
            break;
        }
        // если дошли до конца строки
        if (nCurrentPosition == StringLength) {
            ReturnString = AdjustmentsString.SubString(nPosition, nCurrentPosition - nPosition + 1);
            // больше имен нет
            nPosition = -1;
            // выйдем
            break;
        }
        // перейдем к след. символу
        nCurrentPosition++;
    }
    // вернем результат
    return (ReturnString);
}

// добавляет элемент в начало списка
void StrList::Prepend(AnsiString String)
{
    ListElement* Temp = new ListElement;
    Temp->Next = Header;
    Temp->String = String;
    Header = Temp;
    nElementCount++;
}

// очистить список и освободить память
void StrList::ReleaseList()
{
    ListElement* Temp;
    while (Header != NULL) {
        Temp = Header;
        Header = Header->Next;
        delete Temp;
    }
    nElementCount = 0;
}

//---------------------------------------------------------------------------
#pragma package(smart_init)

