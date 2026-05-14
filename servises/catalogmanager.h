#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include <QObject>
#include <QMap>
#include <QStringList>

enum class DirectType {
    SPO_SECTION,
    PUBLIC_PRIORITY_SECTION,
    PRIVATE_SECTION
};



class CatalogManager : public QObject
{
    Q_OBJECT
public:
    explicit CatalogManager(QObject *parent = nullptr);

public slots:
    void addCatalog(const quint64 request_id, const QString& section, const QString& path, DirectType direct_type);
    void removeSectionsPaths(const quint64 request_id, const QString& section);
    void requestSectionPaths(const quint64 request_id, const QString& section);

signals:
    void sectionsChanged(const QString& section);

    void sectionsReady(quint64 request_id, const QString& section, const QStringList& paths);
    void requestCompleted(quint64 request_id);
    void requestFailed(quint64 request_id, const QString& error);
private:
    bool addSpoPath(const QString& section, const QString& path, QString& error_message);
    bool addPrivateSection(const QString& section, const QString& path, QString& error_message);
    bool addPublicPrioritySection(const QString& section, const QString& path, QString& error_message);
private:
    QMap<QString, QStringList> spo_sections_;
    QMap<QString, QStringList> public_priority_sections_;
    QMap<QString, QStringList> private_sections_;
};

#endif // CATALOGMANAGER_H
