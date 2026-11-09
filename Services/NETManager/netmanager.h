#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>
#include <QTcpSocket>

#include "Services/NETManager/netStruct.h"
#include "Services/ConfigReader/configtypes.h"

class NETManager : public QObject
{
    Q_OBJECT
public:
    explicit NETManager(const AppConfig& cfg, QObject *parent = nullptr);
public slots:
    void sendMessageToPris(const NETPrisMessage& message);
    void sendMessageToProt(const NETProtMessage& message);

    void onPrisAnwer();
signals:
    void sendMessageFromPris(const NETPrisAnswer& message);
    void failed(const QString& error_message);
private:
    struct ConnectInfo
    {
        QString ip;
        int port;
    };

    enum class State
    {
        Initialization,
        Ready,
        Error
    };

private:
    void setNetworkConnections();
    void onAttemptConnectionFinished();
    void setState(const State new_state);
    QString getStateName(const State state) const;
private slots:
    void onFailedDetected(const QString& message);
private:
    QTcpSocket* pris_socket_ = nullptr;
    QTcpSocket* prot_socket_ = nullptr;

    ConnectInfo pris_connect_info;
    ConnectInfo prot_connect_info;

    State state_ = State::Initialization;

    const AppConfig& cfg_;
};

#endif // NETMANAGER_H
