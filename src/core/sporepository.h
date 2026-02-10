#ifndef SPOREPOSITORY_H
#define SPOREPOSITORY_H

#include <QString>
#include <QVector>

#include "appconfig.h"

struct SectionInfo {
    QString abbreviation;
    QString setFilePath;
};

class SpoRepository {
public:
    QVector<SectionInfo> loadSections(const AppConfig &config, QString &error) const;
    int findByPrefix(const QVector<SectionInfo> &sections, const QString &prefix) const;
};

#endif // SPOREPOSITORY_H
