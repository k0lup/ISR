#ifndef ISRCONTROLLER_H
#define ISRCONTROLLER_H

#include <QObject>
#include <QVector>

#include "appconfig.h"
#include "sporepository.h"
#include "../net/pormessage.h"

class PrisTransport;
class ProtocolLogger;
class DirectiveHandler;

class IsrController : public QObject {
    Q_OBJECT
public:
    explicit IsrController(QObject *parent = nullptr);

    bool initialize(const QStringList &args, QString &error);

    const AppConfig &config() const;
    QVector<SectionInfo> sections() const;
    QString currentSection() const;
    QString stencil() const;

    bool selectSection(const QString &abbr, QString &error);
    bool setStencil(const QString &value, QString &error);
    bool setHeader(const QString &headerText, QString &error);

signals:
    void sectionChanged(const QString &abbr);
    void statusChanged(const QString &text);
    void switchSectionTab(const QString &name);

private:
    AppConfig m_config;
    QVector<SectionInfo> m_sections;
    QString m_currentSection;
    QString m_stencil;

    PrisTransport *m_transport = nullptr;
    ProtocolLogger *m_logger = nullptr;
    DirectiveHandler *m_directiveHandler = nullptr;

    QString stencilFilePath() const;
    void loadStencil();
    void sendSimpleAck(quint16 kind, quint16 sort, quint16 atomType, bool ok, quint16 adrTo);

    friend class DirectiveHandler;
};

#endif // ISRCONTROLLER_H
