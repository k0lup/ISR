#include "dialog_manager.h"

#include <QDialog>

#include "../dialogs/dialogs.h"

namespace {
struct Item {
    const char *key;
    const char *legacyClassName;
    dialogs::Kind kind;
};

constexpr Item kItems[] = {
    {"About", "TAboutBox", dialogs::Kind::About},
    {"CheckSPO", "TCheckSPOBox", dialogs::Kind::CheckSPO},
    {"End", "TEndBox", dialogs::Kind::End},
    {"PartitionList", "TPartitionListBox", dialogs::Kind::PartitionList},
    {"Find", "TFindBox", dialogs::Kind::Find},
    {"Request", "TRequestBox", dialogs::Kind::Request},
    {"SP", "TSPBox", dialogs::Kind::SP},
    {"Stencil", "TStencilBox", dialogs::Kind::Stencil},
    {"Title", "TTitleBox", dialogs::Kind::Title},
    {"Variant", "TVariantBox", dialogs::Kind::Variant},
    {"CloseReport", "TCloseRep", dialogs::Kind::CloseReport},
    {"Completing", "TCompletingBox", dialogs::Kind::Completing},
    {"Settings", "TSettingBox", dialogs::Kind::Settings},
    {"PartitionReport", "TPartisionRep", dialogs::Kind::PartitionReport},
    {"SelectionReport", "TSelectionRep", dialogs::Kind::SelectionReport},
    {"ChildWindow", "TMDIChild", dialogs::Kind::ChildWindow},
};
} // namespace

DialogManager::DialogManager(RuntimeState *state, QWidget *owner)
    : QObject(owner), m_state(state), m_owner(owner) {}

DialogManager::~DialogManager() {
    qDeleteAll(m_dialogs);
}

void DialogManager::createAll() {
    for (const auto &item : kItems) {
        const QString key = QString::fromLatin1(item.key);

        if (m_dialogs.contains(key)) {
            continue;
        }

        auto dialog = dialogs::createDialog(item.kind, m_state, m_owner);
        auto *raw = dialog.release();
        raw->setObjectName(key);
        m_dialogs.insert(key, raw);
        m_legacyClassNames.insert(key, QString::fromLatin1(item.legacyClassName));

        emit log(QStringLiteral("Dialog created: %1 (legacy=%2)").arg(key, QString::fromLatin1(item.legacyClassName)));
    }
}

void DialogManager::showDialog(const QString &key) {
    auto it = m_dialogs.find(key);
    if (it == m_dialogs.end()) {
        emit log(QStringLiteral("Dialog not found: %1").arg(key));
        return;
    }

    it.value()->show();
    it.value()->raise();
    it.value()->activateWindow();
}

QDialog *DialogManager::dialog(const QString &key) const {
    return m_dialogs.value(key, nullptr);
}

QStringList DialogManager::keys() const {
    QStringList out;
    out.reserve(static_cast<int>(sizeof(kItems) / sizeof(kItems[0])));
    for (const auto &item : kItems) {
        out.push_back(QString::fromLatin1(item.key));
    }
    return out;
}

QString DialogManager::legacyClassName(const QString &key) const {
    return m_legacyClassNames.value(key);
}
