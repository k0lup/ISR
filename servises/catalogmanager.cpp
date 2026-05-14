#include "catalogmanager.h"
#include <QSet>

CatalogManager::CatalogManager(QObject *parent)
    : QObject{parent}
{

}

void CatalogManager::addCatalog(const quint64 request_id, const QString &section, const QString& path, DirectType direct_type) {
    QString error_message;
    bool res{false};

    switch (direct_type) {
    case (DirectType::SPO_SECTION):                 res = addSpoPath(section, path, error_message);                      break;
    case (DirectType::PUBLIC_PRIORITY_SECTION):     res = addPublicPrioritySection(section, path, error_message);        break;
    case (DirectType::PRIVATE_SECTION):             res = addPrivateSection(section, path, error_message);               break;
    }

    if (res) {
        emit requestFailed(request_id, error_message);
    } else {
        emit requestCompleted(request_id);
    }
}

void CatalogManager::removeSectionsPaths(const quint64 request_id, const QString &section) {
    QString error_message;

    if (!spo_sections_.keys().contains(section) &&
            !public_priority_sections_.keys().contains(section) &&
            !private_sections_.keys().contains(section)) {
        error_message = "Раздел " + section + " не содержит 'своих' каталогов";
        emit requestFailed(request_id, error_message);
        return;
    }

    spo_sections_.remove(section);
    public_priority_sections_.remove(section);
    private_sections_.remove(section);

    emit requestCompleted(request_id);
}

void CatalogManager::requestSectionsPaths(const quint64 request_id, const QString& section) {
    QString error_message;
    QStringList paths;

    paths.append(public_priority_sections_.values());
    for (const auto& list : public_priority_sections_) {
        paths.append(list);
    }


    QStringList paths_for_append;
    paths_for_append = spo_sections_.value(section);
    paths_for_append.append(private_sections_.value(section));

    for (const auto& path : paths_for_append) {
        if (!paths.contains(path)) {
            paths.append(path);
        }
    }

    if (paths.isEmpty()) {
        error_message = "Раздел " + section + " не содержит 'своих' каталогов";
        emit requestFailed(request_id, error_message);
        return;
    }

    emit sectionsReady(request_id, section, paths);
}

bool CatalogManager::addSpoPath(const QString &section, const QString &path, QString &error_message) {
    error_message.clear();

    if (!spo_sections_.contains(section)) {
        spo_sections_.insert(section, QStringList());
    }

    spo_sections_[section].append(path);
    return true;
}

bool CatalogManager::addPrivateSection(const QString &section, const QString &path, QString &error_message) {
    error_message.clear();

    if (!private_sections_.contains(section)) {
        private_sections_.insert(section, QStringList());
    }

    private_sections_[section].append(path);
    return true;
}

bool CatalogManager::addPublicPrioritySection(const QString &section, const QString &path, QString &error_message) {
    error_message.clear();

    if (!public_priority_sections_.contains(section)) {
        public_priority_sections_.insert(section, QStringList());
    }

    public_priority_sections_[section].append(path);
    return true;
}
