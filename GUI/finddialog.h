#ifndef FINDDIALOG_H
#define FINDDIALOG_H
#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class FindDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindDialog(QString title_window, QString label_name, QWidget* parent = nullptr);

    int exec() override;
    QString getTxtForSearch() const;
private slots:
    void onCancelBtnClicked();
    void onOkBtnClicked();
    void onTxtInLineEditChanged(const QString& text);
private:
    void createWindow();
private:
    QString txt_for_search_;

    QLabel* label_;
    QLineEdit* line_edit_;
    QPushButton* cancel_button_;
    QPushButton* ok_btn_;
};

#endif // FINDDIALOG_H
