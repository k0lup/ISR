#ifndef SECTIONLISTDIALOG_H
#define SECTIONLISTDIALOG_H

#include <QDialog>
#include <QVector>

#include "../core/sporepository.h"

class QListWidget;
class QLineEdit;

class SectionListDialog : public QDialog {
    Q_OBJECT
public:
    explicit SectionListDialog(const QVector<SectionInfo> &sections, QWidget *parent = nullptr);

    QString selectedSection() const;

private slots:
    void onFind();

private:
    QVector<SectionInfo> m_sections;
    QListWidget *m_listWidget = nullptr;
    QLineEdit *m_findEdit = nullptr;
};

#endif // SECTIONLISTDIALOG_H
