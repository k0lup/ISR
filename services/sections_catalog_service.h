#pragma once

#include <QObject>
#include <QPointer>
#include <QThread>
#include <memory>

#include "config/app_config.h"

class SectionsLoader;

namespace isr {

class SectionsCatalogService : public QObject
{
    Q_OBJECT
public:
    explicit SectionsCatalogService(std::shared_ptr<const AppConfig> cfg, QObject* parent = nullptr);
    ~SectionsCatalogService() override;

    void start();
    void cancel();

signals:
    void progress(int value);
    void message(const QString& text);
    void finished(const QStringList& sections);
    void failed(const QString& error);

private slots:
    void onLoaderFinished();
    void onLoaderFailed(const QString& error);

private:
    void cleanupWorker();

private:
    std::shared_ptr<const AppConfig> cfg_;
    QThread* thread_ = nullptr;
    QPointer<SectionsLoader> loader_;
    bool running_ = false;
};

} // namespace isr
