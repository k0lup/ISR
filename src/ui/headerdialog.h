#ifndef HEADERDIALOG_H
#define HEADERDIALOG_H

#include <QDialog>

class QLineEdit;

class HeaderDialog : public QDialog {
    Q_OBJECT
public:
    explicit HeaderDialog(QWidget *parent = nullptr);

    QString headerText() const;

private:
    QLineEdit *m_edit = nullptr;
};

#endif // HEADERDIALOG_H
