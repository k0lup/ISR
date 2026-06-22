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
#include "diireader.h"

struct Section {
    QString section_path;
    QString section_name;
    QString dii_file_path;
    QString set_file_path;
    QStringList dip_dirs;

    ChapterType active_chapter_type = ChapterType::INCORRECT;
    //QList<Chapter> chapters;
    DiiFile dii_file;
    QList<int> num_command;

    void clear() {
        section_path.clear();
        section_name.clear();
        dii_file_path.clear();
        set_file_path.clear();
        dip_dirs.clear();
        active_chapter_type = ChapterType::INCORRECT;
        dii_file.clear();
        num_command.clear();
    }
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
    void onDiiFileReaded(const quint64& request_id, const DiiFile& data);

signals:
    void progress(int percent);
    void message(const QString& text);
    void failed(const QString& error_message);
    void finished();


    void sectionLoaded(const QString& section_name, Section section);
    void requestDiiFileRead(const quint64 request_id, const QString& file_name);
private:
    QSet<QString> readMasterSectionsFile(const QString& file_path, ErrorReadFile& error);

    std::atomic_bool cancel_requested_{false};
    std::shared_ptr<const AppConfig> cfg_;
    QVector<Section> sections_;
    QSet<QString> sections_names_;
    SetFilesReader set_reader_;
    DiiReader dii_reader_;

    Section loaded_section_;

    static quint64 request_id;
};

#endif // SECTIONSLOADER_H
