#include "netmanager.h"
#include <QTimer>

#include <Logger/logger.h>
#include "logging_categories.h"

NETManager::NETManager(const AppConfig& cfg, QObject *parent) : QObject(parent), cfg_(cfg)
{
    qCInfo(logNet) << "Вызван конструктор NETManager";
    pris_socket_ = new QTcpSocket(this);
    prot_socket_ = new QTcpSocket(this);

    pris_connect_info.ip = cfg_.ip_server;
    pris_connect_info.port = cfg_.pris_port;

    prot_connect_info.ip = cfg_.ip_server;
    prot_connect_info.port = cfg_.prot_port;


    setNetworkConnections();

    QObject::connect(this, &NETManager::failed, this, &NETManager::onFailedDetected);
    qCInfo(logNet) << "Завершен конструктор NETManager";
}

void NETManager::setNetworkConnections()
{
    qCInfo(logNet) << "Вызван метод setNetworkConnections()";

    if (state_ != State::Initialization) {
        emit failed("Вызван метод подключение к серверу в недопустимом состоянии!");
    }

    if (pris_socket_->state() != QAbstractSocket::ConnectedState || prot_socket_->state() != QAbstractSocket::ConnectedState) {
        QTimer* timer = new QTimer(this);
        timer->setInterval(1000);

        QObject::connect(timer, &QTimer::timeout, this, [this, timer, attempts = 0]() mutable {
            ++attempts;


            if (pris_socket_->state() != QAbstractSocket::ConnectedState) {
                qCInfo(logNet) << QString("Пытаюсь установить соединение с PRIS (%1 : %2)").arg(pris_connect_info.ip).arg(pris_connect_info.port);
                pris_socket_->connectToHost(pris_connect_info.ip, pris_connect_info.port);
            }

            if (prot_socket_->state() != QAbstractSocket::ConnectedState) {
                qCInfo(logNet) << QString("Пытаюсь установить соединение с PROT (%1 : %2)").arg(prot_connect_info.ip).arg(prot_connect_info.port);
                prot_socket_->connectToHost(prot_connect_info.ip, prot_connect_info.port);
            }

            if (pris_socket_->state() == QAbstractSocket::ConnectedState && prot_socket_->state() == QAbstractSocket::ConnectedState) {
                qCInfo(logNet) << QString("Соединение успешно установлено!");
                timer->stop();
                timer->deleteLater();
                onAttemptConnectionFinished();
                return;
            }
            if (attempts >= 5) {
                qCWarning(logNet) << "Не удалось установить соединение";
                timer->stop();
                timer->deleteLater();
                onAttemptConnectionFinished();
                return;
            }
        });
    }

    qCInfo(logNet) << "Завершен метод setNetworkConnections()";
}

void NETManager::onAttemptConnectionFinished()
{
    qCInfo(logNet) << "Вызван метод onAttemptConnectionFinished()";
    if (pris_socket_->state() != QAbstractSocket::ConnectedState || prot_socket_->state() != QAbstractSocket::ConnectedState) {
        if (pris_socket_->state() != QAbstractSocket::ConnectedState) {
            QString error_message = QString("Не удалось подключиться к серверу %1 по порту %2 (ПРИС)").arg(pris_connect_info.ip).arg(pris_connect_info.port);
            qCCritical(logNet) << error_message;
            emit failed(error_message);
        }

        if (prot_socket_->state() != QAbstractSocket::ConnectedState) {
            QString error_message = QString("Не удалось подключиться к серверу %1 по порту %2 (ПРОТ)").arg(prot_connect_info.ip).arg(prot_connect_info.port);
            qCCritical(logNet) << error_message;
            emit failed(error_message);
        }
        return;
    }

    setState(State::Ready);
    qCInfo(logNet) << "Завершен метод onAttemptConnectionFinished()";
}

void NETManager::setState(const State new_state)
{
    qCInfo(logNet) << QString("Переходим из состояния %1 в состояние %2").arg(getStateName(state_)).arg(getStateName(new_state));

    state_ = new_state;
}

QString NETManager::getStateName(const State state) const
{
    switch (state) {
    case State::Initialization  : return "Initialization";
    case State::Ready           : return "Ready";
    case State::Error           : return "Error";
    }
}

void NETManager::onFailedDetected(const QString& message)
{
    qCCritical(logNet) << "Критическая ошибка: " + message;
    setState(State::Error);
}

void NETManager::sendMessageToPris(const NETPrisMessage& message)
{

}

void NETManager::sendMessageToProt(const NETProtMessage& message)
{

}

void NETManager::onPrisAnwer()
{
    NETPrisAnswer pris_answer;
    //получение ответа от ПРИС
    emit sendMessageFromPris(pris_answer);

}
