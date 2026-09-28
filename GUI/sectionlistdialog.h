#ifndef SECTIONLISTDIALOG_H
#define SECTIONLISTDIALOG_H
#include <QDialog>
#include <QStringList>

#include <QListWidget>
#include <QPushButton>
//class QListWidget;
//class QPushButton;

class SectionListDialog : public QDialog
{
    Q_OBJECT
public:
    enum class Mode
    {
        SELECT,
        LOAD
    };
public:
    SectionListDialog(QWidget* parent = nullptr);

    void setSections(const QStringList& sections_list);
    int exec(const Mode mode);
signals:
    void sectionSelected(const QString& sections_name);
    void loadSectionRequested(const QString& section_name);
private slots:
    void onSelectButtonClicked();
    void onLoadButtonClicked();
    void onFindButtonClicked();
    void onCancelButtonClicked();
private:
    using QDialog::exec;                                        //Чтобы убрать warning из-за сокрытия базового QDialog::exec нашим переопределением.

    void createWidgets();
    void createLayouts();
    void createConnections();
private:
    QListWidget* section_list_ = nullptr;

    QPushButton* select_button_ = nullptr;
    QPushButton* load_button_ = nullptr;
    QPushButton* find_button_ = nullptr;
    QPushButton* cancel_button_ = nullptr;

    QStringList sections_list;
};

#endif // SECTIONLISTDIALOG_H
