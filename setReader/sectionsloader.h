#ifndef SECTIONSLOADER_H
#define SECTIONSLOADER_H

#include <QObject>
#include <QVector>
#include <QSet>
#include <QString>
#include <QFile>
#include <memory>
#include "config/app_config.h"
#include "setfilesreader.h"
#include "ErrorReadFileStruct.h"

struct Section {
    QString section_name;
    QFile dii_file;
};

class SectionsLoader : public QObject
{
    Q_OBJECT
public:
    SectionsLoader(std::shared_ptr<const AppConfig> cfg, QObject *parent = nullptr);

    QSet<QString> getSectionsNames();
public slots:
    void start();
    void cancel();

signals:
    void progress(int percent);
    void message(const QString& text);
    void errorMessage(const QString& text);
    void finished();

private:
    QSet<QString> readMasterSectionsFile(const QString& file_path, ErrorReadFile& error);

    bool cancel_requested_ = false;
    std::shared_ptr<const AppConfig> cfg_;
    QVector<Section> sections_;
    QSet<QString> sections_names_;
    SetFilesReader set_reader_;
};

#endif // SECTIONSLOADER_H
