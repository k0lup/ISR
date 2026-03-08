#include "services/sections_catalog_service.h"

#include <QMetaObject>
#include <QStringList>

#include "setReader/sectionsloader.h"

namespace isr {

SectionsCatalogService::SectionsCatalogService(std::shared_ptr<const AppConfig> cfg, QObject* parent)
    : QObject(parent)
    , cfg_(std::move(cfg))
{
}

SectionsCatalogService::~SectionsCatalogService()
{
    cancel();
    cleanupWorker();
}

void SectionsCatalogService::start()
{
    if (running_) {
        return;
    }

    running_ = true;
    thread_ = new QThread(this);
    loader_ = new SectionsLoader(cfg_);
    loader_->moveToThread(thread_);

    connect(thread_, &QThread::started, loader_, &SectionsLoader::start);
    connect(loader_, &SectionsLoader::progress, this, &SectionsCatalogService::progress);
    connect(loader_, &SectionsLoader::message, this, &SectionsCatalogService::message);
    connect(loader_, &SectionsLoader::failed, this, &SectionsCatalogService::onLoaderFailed);
    connect(loader_, &SectionsLoader::finished, this, &SectionsCatalogService::onLoaderFinished);

    thread_->start();
}

void SectionsCatalogService::cancel()
{
    if (loader_) {
        QMetaObject::invokeMethod(loader_, "cancel", Qt::QueuedConnection);
    }
}

void SectionsCatalogService::onLoaderFinished()
{
    QStringList sections;
    if (loader_) {
        const auto set = loader_->getSectionsNames();
        sections.reserve(set.size());
        for (const auto& section : set) {
            sections.push_back(section);
        }
        sections.sort(Qt::CaseInsensitive);
    }

    running_ = false;
    emit finished(sections);
    cleanupWorker();
}

void SectionsCatalogService::onLoaderFailed(const QString& error)
{
    running_ = false;
    emit failed(error);
    cleanupWorker();
}

void SectionsCatalogService::cleanupWorker()
{
    if (thread_ == nullptr) {
        return;
    }

    if (loader_) {
        loader_->disconnect(this);
        loader_->deleteLater();
        loader_.clear();
    }

    thread_->quit();
    thread_->wait();
    thread_->deleteLater();
    thread_ = nullptr;
}

} // namespace isr
