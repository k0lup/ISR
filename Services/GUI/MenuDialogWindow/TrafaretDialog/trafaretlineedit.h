#ifndef TRAFARETLINEEDIT_H
#define TRAFARETLINEEDIT_H
#include <QLineEdit>
#include <QKeyEvent>
#include <QMouseEvent>

class TrafaretLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit TrafaretLineEdit(QWidget* parent = nullptr);

    QString trafaretText() const;
    void setTrafaretTextText(QString text);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    using QLineEdit::setText;
    using QLineEdit::text;
    static constexpr int SeparatorPos = 12;

    bool isEditablePosition(int pos) const;
    bool isAllowedCharacter(int pos, QChar ch) const;

    int nextEditablePosition(int pos) const;
    int previousEditablePosition(int pos) const;

    void moveRight();
    void moveLeft();
    void replaceCharacter(int pos, QChar ch);
};

#endif // TRAFARETLINEEDIT_H
