#ifndef SECTIONLISTWGT_H
#define SECTIONLISTWGT_H
#include <QDialog>
#include <QListWidget>
#include <QString>
#include <QStringList>
#include "findwgt.h"

enum class MODE_SECTION_LIST_WGT {
    FOR_SELECT,
    FOR_LOAD
};

struct SectionSelected {
    QString section_name;
    MODE_SECTION_LIST_WGT mode;
};

class SectionListWgt : public QDialog
{
    Q_OBJECT
public:
    SectionListWgt(const QStringList& sections, QWidget* parent = nullptr);
    SectionListWgt(QWidget *parent = nullptr);

    int exec(const MODE_SECTION_LIST_WGT mode = MODE_SECTION_LIST_WGT::FOR_SELECT); //параметр mode определяет возможность виджета - выбрать головной раздел {FOR_SELECT} или загрузить новый {FOR_LOAD}
                                                                                    //при {FOR_LOAD} будет доступна возможность загрузки, но не будет доступен выбор
                                                                                    //при {FOR_SELECT} будет доступна возможность выбора, но не будет доступна загрузка
    SectionSelected getSection() const;
    void setSections(const QStringList& sections);
signals:
    void sectionSelected(const QString& section_name);
    void failed(const QString& error_message);
public slots:
    void searchSection(const QString& section_name);
    void onSearhBtnClicked();
    void onAccpetBtnClicked();  //срабатывает при нажатии на загрузить/выбрать в зависимости от режима
private:
    QListWidget* list_of_sections_ = nullptr;
    FindWgt* find_wgt_ = nullptr;

    QString selected_section_; //выбранный раздел по кнопке выбрать

    QPushButton* select_btn_ = nullptr;
    QPushButton* load_btn_ = nullptr;

    MODE_SECTION_LIST_WGT mode_ = MODE_SECTION_LIST_WGT::FOR_SELECT;

    using QDialog::exec;
};

#endif // SECTIONLISTWGT_H
