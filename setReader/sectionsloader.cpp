#include "sectionsloader.h"
#include "logger/logging_categories.h"
#include <QLoggingCategory>
#include "config/app_config.h"
#include <QDir>
#include <cmath>
#include "diireader.h"

quint64 SectionsLoader::request_id = 0;

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

    qRegisterMetaType<DiiFile>("DiiFile");
    QObject::connect(this, &SectionsLoader::requestDiiFileRead, &dii_reader_, &DiiReader::onReadFileRequested);
    QObject::connect(&dii_reader_, &DiiReader::fileReaded, this, &SectionsLoader::onDiiFileReaded);
    QObject::connect(&dii_reader_, &DiiReader::failed, this, [this](const quint64, const QString& message) {
        emit failed(message);
    });
}

void SectionsLoader::cancel() {
    qCInfo(logCore) << "Запрошена останов чтения разделов";
    cancel_requested_.store(true);
}

void SectionsLoader::start() {
    qCInfo(logCore) << "Запрошен старт чтения разделов";
    cancel_requested_.store(false);
    sections_names_.clear();

    qCInfo(logCore) << "Читаем основные set файлы разделов (из файла настройки)";
    ErrorReadFile error;

    QString folder = cfg_->sections_folder_path;
    QDir dir(expandUserPath(folder));

    QStringList sections = cfg_->sections_paths;
    //QSet<QString> down_sections;
    for (auto& section : sections) {
        section = dir.filePath(section + QString(".SET"));
    }

    if (sections.isEmpty()) {
        emit failed("Список разделов пуст");
        return;
    }

    for (int i = 0; i < sections.size(); ++i) {
        if (cancel_requested_.load()) {
            qCInfo(logCore) << "Пользователем запрошена остановка загрузки";
            emit message("Загрузка отменена");
            emit finished();
            return;
        }

        const QString& file_name = sections.at(i);
        int percent_progress = std::lround(100.0 * (i + 1) / sections.size());
        emit progress(percent_progress);
        QSet<QString> new_sections = readMasterSectionsFile(file_name, error);
        if (!error.message.isEmpty()) {
            emit failed(error.toString());
            return;
        }
        sections_names_.unite(new_sections);
    }

    folder = cfg_->spo_path;

    dir = QDir(folder);
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
        qCWarning(logCore) << error.toString();
        return QSet<QString>();
    }
    SetFileData data = set_reader_.getFileData();
    if (!data.directory.isEmpty() || !data.structure.isEmpty()) {
        error.message = QString("В файле (%1) обнаружены данные в столбцах кроме аббревиатуры!").arg(file_path);
        qCWarning(logCore) << error.toString();
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

void SectionsLoader::onLoadSectionRequested(const QString &section_name) {
    if (!sections_names_.contains(section_name)) {
        emit failed("Запрошена загрузка несуществующего раздела!");
        return;
    }

    auto it = std::find_if(sections_.begin(), sections_.end(),
        [&](const Section& sec) { return sec.section_name == section_name; });

    if (it != sections_.end()) {
        emit failed("Запрошена загрузка уже загруженного раздела!");
        return;
    }

    QString folder = cfg_->spo_path;
    QDir dir(expandUserPath(folder));
    dir = dir.filePath(section_name);

    QString section_set_filepath = dir.filePath(section_name + QString(".SET"));
    qDebug() << "section_set_filepath: " << section_set_filepath;
    ErrorReadFile error;
    error.filePath = section_set_filepath;
    error.line = -1;
    error.message = "";

    if (!set_reader_.readFile(section_set_filepath, error)) {
        emit failed(error.toString());
        return;
    }

    SetFileData data = set_reader_.getFileData();
    if (!data.abbreviation.contains(section_name)) {
        emit failed("в файле структуры раздела нет аббревиатуры этого раздела. Файл '" + section_set_filepath + "'");
        return;
    }

    QString dii_file_path;
    QStringList dip_dirs;

    if (data.structure.contains(section_name)) {
        dii_file_path = data.structure.value(section_name);
    }

    dii_file_path = dir.filePath(dii_file_path + ".DII");

    dip_dirs = data.directory.value(section_name);

    Section section;
    section.section_path = dir.path();
    section.section_name = section_name;
    section.active_chapter_type = ChapterType::INCORRECT;
    section.dii_file_path = dii_file_path;
    section.dip_dirs = dip_dirs;

    loaded_section_.clear();
    loaded_section_ = section;

    emit requestDiiFileRead(request_id++, dii_file_path);

    //sections_.append(section);
    //emit sectionLoaded(section_name, sections_.last());
}

void SectionsLoader::onDiiFileReaded(const quint64 &request_id, const DiiFile &data) {
    if (request_id != this->request_id - 1) {
        QString error_message = "Ошибка в номере ответа на запрос при чтении dii файла";
        qCCritical(logCore) << error_message;
        emit failed(error_message);
    }

    loaded_section_.dii_file = data;
    loaded_section_.active_chapter_type = ChapterType::STATE;
    loaded_section_.num_command = {0, 0, 0};

    sections_.append(loaded_section_);
    emit sectionLoaded(loaded_section_.section_name, sections_.last());
    loaded_section_.clear();
}
