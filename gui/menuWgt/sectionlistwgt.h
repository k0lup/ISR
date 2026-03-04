#ifndef SECTIONLISTWGT_H
#define SECTIONLISTWGT_H
#include <QWidget>
#include <QListWidget>
#include <QString>
#include "findwgt.h"

class SectionListWgt : public QWidget
{
    Q_OBJECT
public:
    SectionListWgt(QObject* parent = nullptr);
signals:
    void sectionSelected(const QString& section_name);
public slots:
    void searchSection(const QString& section_name);
private:
    QListWidget* list_of_sections_ = nullptr;
    FindWgt* find_wgt_ = nullptr;
};

#endif // SECTIONLISTWGT_H
