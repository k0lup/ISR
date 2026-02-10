#include "sporepository.h"

#include <QDir>
#include <QFileInfo>

QVector<SectionInfo> SpoRepository::loadSections(const AppConfig &config, QString &error) const
{
    QVector<SectionInfo> sections;
    const QDir dir(config.sectionsRoot);
    if (!dir.exists()) {
        error = QStringLiteral("Каталог разделов недоступен: %1").arg(config.sectionsRoot);
        return sections;
    }

    if (!config.sections.isEmpty()) {
        for (const QString &abbr : config.sections) {
            const QString filePath = dir.filePath(abbr + QStringLiteral(".set"));
            if (QFileInfo::exists(filePath)) {
                sections.push_back({abbr, filePath});
            }
        }
    } else {
        const QFileInfoList files = dir.entryInfoList({QStringLiteral("*.set")}, QDir::Files, QDir::Name);
        for (const QFileInfo &fi : files) {
            sections.push_back({fi.completeBaseName(), fi.absoluteFilePath()});
        }
    }

    if (sections.isEmpty()) {
        error = QStringLiteral("Список разделов пуст");
    }
    return sections;
}

int SpoRepository::findByPrefix(const QVector<SectionInfo> &sections, const QString &prefix) const
{
    const QString normalized = prefix.trimmed().toUpper();
    if (normalized.isEmpty()) {
        return -1;
    }

    for (int i = 0; i < sections.size(); ++i) {
        if (sections.at(i).abbreviation.toUpper().startsWith(normalized)) {
            return i;
        }
    }
    return -1;
}
