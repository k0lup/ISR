//---------------------------------------------------------------------------

#ifndef AdjNamesH
#define AdjNamesH
//---------------------------------------------------------------------------
class AdjustmentNames {
    // исходная строка ("РАЗДЕЛЫ=")
    AnsiString AdjustmentsString;
    // позиция в AdjustmentsString
    // это позиция очередного имени
    // если имен больше нет, nPosition = -1
    // если AdjustmentsString пуста, nPosition не определена
    int nPosition;
    int StringLength;
public:
    __fastcall AdjustmentNames(AnsiString TransAdjustmentsString);
    // сбросить отсчет файлов в начальное состояние
    void __fastcall Reset();
    // получить очередное имя файла настройки
    // если файла больше нет, возвращается пустая строка
    AnsiString __fastcall GetAdjustmentName();
};

// элемент списка
struct ListElement {
    // аббревиатура
    AnsiString String;
    // следующий элемент
    ListElement* Next;
};

// список файлов настройки пользователя
class StrList {
    // первый элемент списка
    ListElement* Header;
    int nElementCount;
public:
    // NULL означает пустой список
    StrList() {nElementCount = 0; Header = NULL;}
    ~StrList() {ReleaseList();}
    // первый элемент списка
    ListElement* First() {return (Header);}
    int GetElementCount() {return (nElementCount);}
    // добавляет элемент в начало списка
    void Prepend(AnsiString String);
    // очистить список и освободить память
    void ReleaseList();
};

#endif
