#ifndef TITLEOFSECTIONDIALOG_H
#define TITLEOFSECTIONDIALOG_H
#include <QDialog>
#include "Services/GUI/guitypes.h"
class QLineEdit;
class QPushButton;

class TitleOfSectionDialog : public QDialog
{
    Q_OBJECT
public:
    TitleOfSectionDialog(QWidget *parent = nullptr);

    int exec(const QString& title_section, const QString& traf);
    TitleOfSectionDescription getDescription() const;
private:
    using QDialog::exec;

    void createWindow();
    void clearAllLineEdits();
private slots:
    void onCancelBtnClicked();
    void onOkBtnClicked();
private:
    TitleOfSectionDescription description_;

    QLineEdit* title_of_section_line_edit_      = nullptr;
    QLineEdit* name_of_prot_line_edit_          = nullptr;
    QLineEdit* traf_line_edit_                  = nullptr;
    QLineEdit* rep_line_edit_                   = nullptr;
    QLineEdit* operator_pu_ais_line_edit_       = nullptr;
    QLineEdit* operator_ais_line_edit_          = nullptr;
    QLineEdit* operator_tkck_ss_bvs_line_edit_  = nullptr;
    QLineEdit* operator_ntk_si_line_edit_       = nullptr;
    QLineEdit* operator_bs_line_edit_           = nullptr;

    QPushButton* ok_btn_        = nullptr;
    QPushButton* cancel_btn     = nullptr;
};

#endif // TITLEOFSECTIONDIALOG_H
