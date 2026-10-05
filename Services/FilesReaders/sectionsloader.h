#ifndef SECTIONSLOADER_H
#define SECTIONSLOADER_H

#include <QObject>
#include <QVector>
#include <QSet>
#include <QString>
#include <QFile>
#include <QList>
#include <atomic>
#include "Services/ConfigReader/configtypes.h"
#include "setfilesreader.h"
#include "diireader.h"
#include "Services/Programs/programtypes.h"
#include "Services/RequestIDGenerator/requestIDGenerator.h"
#include "ErrorReadFileStruct.h"
#include "Services/DirectiveExecutor/executiontypes.h"

class SectionsLoader : public QObject
{
    Q_OBJECT
public:
    SectionsLoader(const AppConfig& cfg, QObject *parent = nullptr);

    QSet<QString> getSectionsNames();
public slots:
    void start();
    void cancel();

    void onLoadSectionRequested(const QString& section_name, const RequestId request_id);
    void onDiiFileReaded(const quint64& source_request_id, const SectionStruct& data);

    void onStatusSectionReceived(bool is_load, RequestId source_request_id);

    void onExecutorRequestLoadSection(const ExecuteParam& params, const RequestId request_id);
signals:
    void progress(int percent);
    void message(const QString& text);
    void failed(const QString& error_message, const RequestId source_request_id);
    void finished();

    void requestStatusSection(const QString& section_name, RequestId request_id);

    void sendAnswerToExecutor(const QString& error_message, RequestId source_request_id);
    void sectionLoaded(const QString& section_name, const SectionStruct& section, const RequestId source_request_id);
    void requestDiiFileRead(const quint64 request_id, const QString& file_name);

private slots:
    void onFailedDetected(const QString& message, const RequestId request_id);
private:
    QSet<QString> readMasterSectionsFile(const QString& file_path, ErrorReadFile& error);

    std::atomic_bool cancel_requested_{false};
    const AppConfig& cfg_;
    QSet<QString> sections_names_;
    SetFilesReader set_reader_;
    DiiReader dii_reader_;

    QMap<RequestId, QString> request_section_;
    QMap<RequestId, QString> request_dii_;

    int current_load_request_id = RequestIdGenerator::NO_REQUEST_ID;
    bool need_send_failed_to_executor_ = false;
};

#endif // SECTIONSLOADER_H
