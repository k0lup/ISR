#include "sectionsloader.h"
#include "logger/logging_categories.h"
#include <QLoggingCategory>
#include "config/app_config.h"
#include <QDir>
#include <cmath>

static QString expandUserPath(const QString &path)
{
    if (path.startsWith("~"))
    {
        QString home = QDir::homePath();
        QString newPath = path;
        newPath.replace(0, 1, home);
        return QDir::cleanPath(newPath);
    }

    return QDir::cleanPath(path);
}

SectionsLoader::SectionsLoader(std::shared_ptr<const AppConfig> cfg, QObject *parent) :
    QObject(parent),
    cfg_(cfg)
{
    qCDebug(logCore) << "Инициализирован SectionsLoader";
}

void SectionsLoader::cancel() {
    qCInfo(logCore) << "Запрошена останов чтения разделов";
    cancel_requested_ = true;
}

void SectionsLoader::start() {
    qCInfo(logCore) << "Запрошен старт чтения разделов";

    qCInfo(logCore) << "Читаем основные set файлы разделов (из файла настройки)";
    ErrorReadFile error;

    QString folder = cfg_->sections_folder_path;
    QDir dir(expandUserPath(folder));

    QStringList sections = cfg_->sections_paths;
    //QSet<QString> down_sections;
    for (auto& section : sections) {
        section = dir.filePath(section + QString(".SET"));
    }

    for (int i = 0; i < sections.size(); ++i) {
        const QString& file_name = sections.at(i);
        int percent_progress = std::lround(100.0 * (i + 1) / sections.size());
        emit progress(percent_progress);
        QSet new_sections = readMasterSectionsFile(file_name, error);
        if (!error.message.isEmpty())
            return;
        sections_names_.unite(new_sections);
    }

    folder = cfg_->spo_path;

    dir.setCurrent(folder);
    for (const auto& section : sections_names_) {
        QDir section_dir(dir.filePath(section));
        QString filePath  = section_dir.filePath(section + QString(".SET"));
    }
    qCInfo(logCore) << "Закончили чтение списка разделов";
    emit finished();
    return;
    //qCInfo(logCore) << "Читаем set файлы разделов (уже самих разделов)";
}

QSet<QString> SectionsLoader::readMasterSectionsFile(const QString& file_path, ErrorReadFile &error) {
    error.filePath = file_path;
    error.line = -1;
    error.message = "";
    QSet<QString> result;

    qCInfo(logCore) << QString("Читаем файл %1").arg(file_path);
    if (!set_reader_.readFile(file_path, error)) {
        emit errorMessage(error.toString());
        qCWarning(logCore) << error.toString();
        return QSet<QString>();
    }
    SetFileData data = set_reader_.getFileData();
    if (!data.directory.isEmpty() || !data.structure.isEmpty()) {
        error.message = QString("В файле (%1) обнаружены данные в столбцах кроме аббревиатуры!").arg(file_path);
        emit errorMessage(error.toString());
        qCWarning(logCore()) << error.toString();
        return QSet<QString>();
    }
    for (const auto& section_name : data.abbreviation) {
        result.insert(section_name);
    }
    qCInfo(logCore) << QString("Закончили читать файл %1").arg(file_path);
    return result;
}

QSet<QString> SectionsLoader::getSectionsNames() {
    return sections_names_;
}
