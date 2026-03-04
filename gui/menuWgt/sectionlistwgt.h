#ifndef SECTIONLISTWGT_H
#define SECTIONLISTWGT_H
#include <QDialog>
#include <QListWidget>
#include <QString>
#include <QStringList>
#include "findwgt.h"

class SectionListWgt : public QDialog
{
    Q_OBJECT
public:
    SectionListWgt(const QStringList& sections, QWidget* parent = nullptr);
    SectionListWgt(QWidget *parent = nullptr);

    int exec() override;
    QString getSelectedSection() const;
    void setSections(const QStringList& sections);
signals:
    void sectionSelected(const QString& section_name);
public slots:
    void searchSection(const QString& section_name);
    void onSearhBtnClicked();
    void onSelectBtnClicked();
private:
    QListWidget* list_of_sections_ = nullptr;
    FindWgt* find_wgt_ = nullptr;

    QString selected_section_; //выбранный раздел по кнопке выбрать
};

#endif // SECTIONLISTWGT_H
