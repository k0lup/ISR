#include "findwgt.h"
#include <QLabel>
#include <QPushButton>

FindWgt::FindWgt(QObject* parent) :
    QWidget(parent)
{
    QLabel *label = new QLabel("Аббревиатура:", this);
    name_for_search_text_line_ = new QTextLine(this);

    QPushButton *cancelBtn = new QPushButton("Отмена", this);
    QPushButton *okBtn = new QPushButton("Ок", this);
}


//Достроить окно findWgt
//Сделать findWgt всегда самостоятельным виджетом (даже при наличии родителя)
//Реализовать sectionListWgt
//Вынести чтение файлов настройки .set в отдельный поток и проверить виджет загрузки ProgressBar
