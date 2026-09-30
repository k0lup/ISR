#ifndef CONTROLSPODIALOG_H
#define CONTROLSPODIALOG_H
#include <QDialog>
class QCheckBox;
class QPushButton;

class ControlSpoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ControlSpoDialog(QWidget* parent = nullptr);
signals:
    void requestSettingsFileCheck();
    void requestStructFileCheck();
    void requestProgFilesCheck();
public slots:
    void onSettingFileChecked(const bool status, const QString& errors = "");
    void onStructFileChecked(const bool status, const QString& errors = "");
    void onProgFilesChecked(const bool status, const QString& errors = "");
private slots:
    void onCloseBtnClicked();
    void onCheckBtnClicked();
    void onCheckOptionsChanges();
private:
    void createWindow();
private:
    struct CheckParams{
        bool need_check_settins;
        bool need_check_struct;
        bool need_check_progs;
    };
private:
    QCheckBox* settings_check_box_ = nullptr;
    QCheckBox* struct_check_box_ = nullptr;
    QCheckBox* prog_check_box_ = nullptr;

    QPushButton* close_btn_ = nullptr;
    QPushButton* check_btn_ = nullptr;

    CheckParams check_params_;
};

#endif // CONTROLSPODIALOG_H
