#include "sectionsloader.h"
#include "logging_categories.h"
#include <QLoggingCategory>
#include "Services/ConfigReader/configtypes.h"
#include <QDir>
#include <cmath>
#include "diireader.h"

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

SectionsLoader::SectionsLoader(const AppConfig& cfg, QObject *parent) :
    QObject(parent),
    cfg_(cfg)
{
    qCDebug(logCore) << "Инициализирован SectionsLoader";

    qRegisterMetaType<SectionStruct>("SectionStruct");
    QObject::connect(this, &SectionsLoader::requestDiiFileRead, &dii_reader_, &DiiReader::onReadFileRequested);
    QObject::connect(&dii_reader_, &DiiReader::fileReaded, this, &SectionsLoader::onDiiFileReaded);
    QObject::connect(&dii_reader_, &DiiReader::failed, this, [this](const quint64, const QString& message) {
        emit failed(message, current_load_request_id);
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

    QString folder = cfg_.sections_folder_path;
    QDir dir(expandUserPath(folder));

    QStringList sections = cfg_.sections_paths;
    //QSet<QString> down_sections;
    for (auto& section : sections) {
        section = dir.filePath(section + QString(".SET"));
    }

    if (sections.isEmpty()) {
        emit failed("Список разделов пуст", current_load_request_id);
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
            emit failed(error.toString(), current_load_request_id);
            return;
        }
        sections_names_.unite(new_sections);
    }

    folder = cfg_.spo_path;

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

void SectionsLoader::onExecutorRequestLoadSection(const ExecuteParam& params, const RequestId request_id)
{
    need_send_failed_to_executor_ = true;
    onLoadSectionRequested(params.section_name, request_id);
}

void SectionsLoader::onLoadSectionRequested(const QString &section_name, const RequestId source_request_id) {
    current_load_request_id = source_request_id;
    if (!sections_names_.contains(section_name)) {
        emit failed("Запрошена загрузка несуществующего раздела!", current_load_request_id);
        return;
    }

    const RequestId request_id = RequestIdGenerator::next();
    request_section_.insert(request_id, section_name);
    emit requestStatusSection(section_name, request_id);
}

void SectionsLoader::onStatusSectionReceived(bool is_load, RequestId source_request_id)
{
    if (!request_section_.contains(source_request_id)) {
        emit failed("Получен результат, но по request_id отстуствует секция", current_load_request_id);
        return;
    }

    QString section_name = request_section_.value(source_request_id);
    request_section_.remove(source_request_id);
    if (section_name.isEmpty()) {
        emit failed("Пустое название секции по source_id!", current_load_request_id);
        return;
    }

    if (is_load) {
        emit message(QString("Секция '%1' уже загружена").arg(section_name));
        return;
    }

    QString folder = cfg_.spo_path;
    QDir dir(expandUserPath(folder));
    dir = dir.filePath(section_name);

    QString section_set_filepath = dir.filePath(section_name + QString(".SET"));
    qDebug() << "section_set_filepath: " << section_set_filepath;
    ErrorReadFile error;
    error.filePath = section_set_filepath;
    error.line = -1;
    error.message = "";

    if (!set_reader_.readFile(section_set_filepath, error)) {
        emit failed(error.toString(), current_load_request_id);
        return;
    }

    SetFileData data = set_reader_.getFileData();
    if (!data.abbreviation.contains(section_name)) {
        emit failed("в файле структуры раздела нет аббревиатуры этого раздела. Файл '" + section_set_filepath + "'", current_load_request_id);
        return;
    }

    QString dii_file_path;
    QStringList dip_dirs;

    if (data.structure.contains(section_name)) {
        dii_file_path = data.structure.value(section_name);
    }

    dii_file_path = dir.filePath(dii_file_path + ".DII");

    dip_dirs = data.directory.value(section_name);

    /*Section section;
    section.section_path = dir.path();
    section.section_name = section_name;
    section.active_chapter_type = ChapterType::INCORRECT;
    section.dii_file_path = dii_file_path;
    section.dip_dirs = dip_dirs;

    loaded_section_.clear();
    loaded_section_ = section;*/

    RequestId request_id_dii = RequestIdGenerator::next();
    request_dii_.insert(request_id_dii, section_name);

    emit requestDiiFileRead(request_id_dii, dii_file_path);

    //sections_.append(section);
    //emit sectionLoaded(section_name, sections_.last());
}

void SectionsLoader::onDiiFileReaded(const quint64 &source_request_id, const SectionStruct &data) {
    if (!request_dii_.contains(source_request_id)) {
        QString error_message = "Ошибка в номере ответа на запрос при чтении dii файла";
        qCCritical(logCore) << error_message;
        emit failed(error_message, current_load_request_id);
    }
    QString section_name = request_dii_.value(source_request_id);
    request_dii_.remove(source_request_id);
    if (section_name.isEmpty()) {
        QString error_message = "Пустое имя секции по source_request_id";
        qCCritical(logCore) << error_message;
        emit failed(error_message, current_load_request_id);
    }
    //loaded_section_.dii_file = data;
    //loaded_section_.active_chapter_type = ChapterType::STATE;
    //loaded_section_.num_command = {0, 0, 0};

    //sections_.append(loaded_section_);
    emit sectionLoaded(section_name, data, current_load_request_id);
    current_load_request_id = RequestIdGenerator::NO_REQUEST_ID;
    need_send_failed_to_executor_ = false;
    //loaded_section_.clear();
}

void SectionsLoader::onFailedDetected(const QString& message, const RequestId source_request_id)
{
    if (need_send_failed_to_executor_) {
        emit sendAnswerToExecutor(message, source_request_id);
    }

    need_send_failed_to_executor_ = false;
    current_load_request_id = RequestIdGenerator::NO_REQUEST_ID;
}
