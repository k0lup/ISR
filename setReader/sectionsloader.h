#ifndef SECTIONSLOADER_H
#define SECTIONSLOADER_H

#include <QObject>
#include <QVector>
#include <QSet>
#include <QString>
#include <QFile>
#include <QList>
#include <memory>
#include <atomic>
#include "config/app_config.h"
#include "setfilesreader.h"
#include "ErrorReadFileStruct.h"

enum class ChapterType {
    STATE = 0,
    NON_STATE = 1,
    ACCIDENT = 2,
    NOT_LOAD = -1
};

struct Chapter {
    QStringList command_list;
};

struct Section {
    QString section_name;
    QString dii_file_path;
    QString set_file_path;
    QStringList dip_dirs;

    ChapterType active_chapter_type = ChapterType::NOT_LOAD;
    QList<Chapter> chapters;
    QList<int> num_command;
};

Q_DECLARE_METATYPE(Section)

class SectionsLoader : public QObject
{
    Q_OBJECT
public:
    SectionsLoader(std::shared_ptr<const AppConfig> cfg, QObject *parent = nullptr);

    QSet<QString> getSectionsNames();
public slots:
    void start();
    void cancel();

    void onLoadSectionRequested(const QString& section_name);

signals:
    void progress(int percent);
    void message(const QString& text);
    void failed(const QString& error_message);
    void finished();


    void sectionLoaded(const QString& section_name, Section section);
private:
    QSet<QString> readMasterSectionsFile(const QString& file_path, ErrorReadFile& error);

    std::atomic_bool cancel_requested_{false};
    std::shared_ptr<const AppConfig> cfg_;
    QVector<Section> sections_;
    QSet<QString> sections_names_;
    SetFilesReader set_reader_;
};

#endif // SECTIONSLOADER_H
