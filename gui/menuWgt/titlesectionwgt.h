#ifndef TITLESECTIONWGT_H
#define TITLESECTIONWGT_H
#include <QDialog>
#include <QString>
#include <QLineEdit>

struct TitleSection {
    QString name_section;
    QString name_of_prot;
    QString traf;
    QString rep;
    QString operator_pu_ais;
    QString operator_ais;
    QString operator_tkck_ss_bvs;
    QString operator_ntk_si;
    QString operator_bs;

    void clear() {*this = {};}
    void setSectionName(const QString& section_name) {
        name_section = section_name;
        name_of_prot = section_name;
    }
    QString getString() {
        return QString("{Заголовок раздела: %1},\n"
                       "{Имя протокола: %2.%3},\n"
                       "{РЭП: %4},\n"
                       "{Оператор ПУ АИС: %5},\n"
                       "{Оператор АИС: %6},\n"
                       "{Оператор ТКЦК СС БВС: %7},\n"
                       "{Оператор НТК СИ: %8},\n"
                       "{Оператор БС: %9}")
                .arg(name_section)
                .arg(name_of_prot)
                .arg(traf)
                .arg(rep)
                .arg(operator_pu_ais)
                .arg(operator_ais)
                .arg(operator_tkck_ss_bvs)
                .arg(operator_ntk_si)
                .arg(operator_bs);
    }
};

class TitleSectionWgt : public QDialog
{
public:
    TitleSectionWgt(const QString& title_of_section, QWidget* parent = nullptr);
    TitleSectionWgt(QWidget* parent = nullptr);
    int exec(const QString& section_name);

    TitleSection getTitleOfSection() const;
private:
    //TitleSection title_of_sections_;

    QLineEdit* name_section_field_          = nullptr;
    QLineEdit* name_prot_field_             = nullptr;
    QLineEdit* traf_prot_field_             = nullptr;
    QLineEdit* rep_field_                   = nullptr;
    QLineEdit* operator_pu_ais_field_       = nullptr;
    QLineEdit* operator_ais_field_          = nullptr;
    QLineEdit* operator_tkck_ss_bvs_field_  = nullptr;
    QLineEdit* operator_ntk_si_field_       = nullptr;
    QLineEdit* operator_bs_field_           = nullptr;

    using QDialog::exec;

    void clearWgt();
private slots:
    void setSectionName(const QString& section_name);
    void onOkBtnClicked();
};

#endif // TITLESECTIONWGT_H
