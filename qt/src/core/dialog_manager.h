#pragma once

#include <QObject>
#include <QHash>
#include <QString>
#include <QStringList>

class QDialog;
class QWidget;
struct RuntimeState;

class DialogManager final : public QObject {
    Q_OBJECT
public:
    explicit DialogManager(RuntimeState *state, QWidget *owner);
    ~DialogManager() override;

    void createAll();
    void showDialog(const QString &key);
    QDialog *dialog(const QString &key) const;
    QStringList keys() const;
    QString legacyClassName(const QString &key) const;

signals:
    void log(const QString &line);

private:
    RuntimeState *m_state = nullptr;
    QWidget *m_owner = nullptr;
    QHash<QString, QDialog *> m_dialogs;
    QHash<QString, QString> m_legacyClassNames;
};
