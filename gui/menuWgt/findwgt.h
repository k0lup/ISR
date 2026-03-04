#ifndef FINDWGT_H
#define FINDWGT_H
#include <QWidget>
#include <QTextLine>
#include <QString>

class FindWgt : public QWidget
{
public:
    FindWgt(QObject* parent = nullptr);
signals:
    void searchSection(const QString& section_name);
private:
    QTextLine* name_for_search_text_line_ = nullptr;
};

#endif // FINDWGT_H
