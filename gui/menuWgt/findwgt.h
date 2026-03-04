#ifndef FINDWGT_H
#define FINDWGT_H
#include <QDialog>
#include <QLineEdit>
#include <QString>

class FindWgt : public QDialog
{
    Q_OBJECT
public:
    FindWgt(QWidget* parent = nullptr);

    QString getNameForSearch() const;

    int exec() override;
signals:
    void searchTextEntered(const QString& section_name);
public slots:
    void onOkClicked();
private:
    QLineEdit* name_for_search_text_line_ = nullptr;
    QString text_for_search_;
};

#endif // FINDWGT_H
