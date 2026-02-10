#ifndef STENCILDIALOG_H
#define STENCILDIALOG_H

#include <QDialog>

class QLineEdit;

class StencilDialog : public QDialog {
    Q_OBJECT
public:
    explicit StencilDialog(const QString &currentStencil, QWidget *parent = nullptr);

    QString stencil() const;

private:
    QLineEdit *m_edit = nullptr;
};

#endif // STENCILDIALOG_H
