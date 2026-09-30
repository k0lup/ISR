#include "trafaretlineedit.h"

TrafaretLineEdit::TrafaretLineEdit(QWidget* parent) : QLineEdit(parent)
{
    QLineEdit::setText("............-....");

    setCursorPosition(0);

    setSelection(0, 1);
}

QString TrafaretLineEdit::trafaretText() const
{
    return text();
}

bool TrafaretLineEdit::isEditablePosition(int pos) const
{
    return
            pos >=0 &&
            pos < text().size() &&
            pos != SeparatorPos;
}

bool TrafaretLineEdit::isAllowedCharacter(int pos, QChar ch) const
{
    if (pos < SeparatorPos) {
        return ch.isLetterOrNumber();
    }

    if (pos > SeparatorPos) {
        return ch.isDigit();
    }

    return false;
}

int TrafaretLineEdit::nextEditablePosition(int pos) const
{
    ++pos;

    if (pos == SeparatorPos) {
        ++pos;
    }

    if (pos >= text().size()) {
        return -1;
    }

    return pos;
}

int TrafaretLineEdit::previousEditablePosition(int pos) const
{
    --pos;

    if (pos == SeparatorPos) {
        --pos;
    }

    if (pos < 0) {
        return -1;
    }

    return pos;
}

void TrafaretLineEdit::replaceCharacter(int pos, QChar ch)
{
    if (!isEditablePosition(pos)) {
        return;
    }

    if (!isAllowedCharacter(pos, ch)) {
        return;
    }

    QString value = text();
    value[pos] = ch;

    QLineEdit::setText(value);
    setSelection(pos, 1);
}

void TrafaretLineEdit::moveRight()
{
    int pos = cursorPosition();

    if (hasSelectedText()) {
        pos = selectionStart();
    }

    const int next = nextEditablePosition(pos);

    if (next >= 0) {
        setSelection(next, 1);
    }
}

void TrafaretLineEdit::moveLeft()
{
    int pos = cursorPosition();

    if (hasSelectedText()) {
        pos = selectionStart();
    }

    const int previous = previousEditablePosition(pos);

    if (previous >= 0) {
        setSelection(previous, 1);
    }
}

void TrafaretLineEdit::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        moveLeft();
        return;
    case Qt::Key_Right:
        moveRight();
        return;
    case Qt::Key_Backspace: {
        int pos = hasSelectedText() ? selectionStart() : cursorPosition();
        const int previous = previousEditablePosition(pos);

        if (previous >= 0) {
            replaceCharacter(previous, '.');
        }

        return;
    }
    case Qt::Key_Delete: {
        const int pos = hasSelectedText() ? selectionStart() : cursorPosition();

        if (isEditablePosition(pos)) {
            replaceCharacter(pos, '.');
        }

        return;
    }

    default:
        break;
    }

    const QString enternedText = event->text();

    if (enternedText.size() == 1) {
        const QChar ch = enternedText.at(0);

        int pos = hasSelectedText() ? selectionStart() : cursorPosition();

        if (pos == SeparatorPos) {
            pos = nextEditablePosition(pos);
        }

        if (isEditablePosition(pos) && isAllowedCharacter(pos, ch)) {
            replaceCharacter(pos, ch);

            const int next = nextEditablePosition(pos);

            if (next >= 0) {
                setSelection(next, 1);
            }

            return;
        }
    }
}

void TrafaretLineEdit::mousePressEvent(QMouseEvent* event)
{
    QLineEdit::mousePressEvent(event);

    int pos = cursorPosition();

    if (pos >= text().size()) {
        pos = text().size() -1;
    }

    if (pos == SeparatorPos) {
        ++pos;
    }

    if (isEditablePosition(pos)) {
        setSelection(pos, 1);
    }
}

void TrafaretLineEdit::setTrafaretTextText(QString text_for_set)
{
    if (text_for_set.isEmpty() || text_for_set.count() > text().count()) {
        return;
    }

    while (text_for_set.count() < text().count()) {
        text_for_set.prepend('.');
    }

    if (text_for_set[SeparatorPos] != text()[SeparatorPos]) {
        return;
    }

    QString order_num = text_for_set.right(4);
    for (const QChar& ch : order_num) {
        if (!ch.isDigit()) {
            return;
        }
    }

    for (int pos = 0; pos < text_for_set.count(); ++pos) {
        if (!isEditablePosition(pos)) {
            continue;
        }

        if (!isAllowedCharacter(pos, text_for_set[pos])) {
            return;
        }
    }

    QLineEdit::setText(text_for_set);
}
