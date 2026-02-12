#pragma once

#include <QDialog>

class QLabel;

class SimpleDialog : public QDialog {
    Q_OBJECT
public:
    explicit SimpleDialog(const QString &title, const QString &description, QWidget *parent = nullptr);

private:
    QLabel *m_label = nullptr;
};
