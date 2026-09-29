#ifndef TRAFARETDIALOG_H
#define TRAFARETDIALOG_H
#include <QDialog>
class TrafaretLineEdit;
class QPushButton;

class TrafaretDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TrafaretDialog(QWidget* parent = nullptr);

    int exec(const QString& traf);
    QString getTrafText() const;
private slots:
    void onOkBtnClicked();
    void onCancelBtnClicked();
private:
    using QDialog::exec;
    void createWindow();

    bool isValid(QString text) const;
private:
    QString traf_;

    TrafaretLineEdit* traf_line_edit_ = nullptr;
    QPushButton* cancel_btn_ = nullptr;
    QPushButton* ok_btn_ = nullptr;
};

#endif // TRAFARETDIALOG_H
