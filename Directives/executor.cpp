#include "executor.h"
#include <QLoggingCategory>
#include "logger/logging_categories.h"

Executor::Executor(QObject* parent) :
    QObject(parent)
{
    state_ = ExecutorState::IDLE;
    mode_ = ExecutorMode::STEP;
    cur_index_ = -1;
    directives_.clear();
}

void Executor::startFrom(const int index, const ExecutorMode mode) {
    qCDebug(logCore) << QString("Получили запрос на выполение директивы %1").arg(index);
    if (index < 0 || index > directives_.count()) {
        emit failed(QString("НЕДОПУСТИМЫЙ ИНДЕКС ЗАПРАШИВАЕМОЙ ДЛЯ ЗАПУСКА ДИРЕТИВЫ: %1").arg(index));
        return;
    }

    cur_index_ = index;
    mode_ = mode;
    startDirective();
}

void Executor::startDirective() {
        qCDebug(logCore) << QString("Получили запрос на выполение очередной директивы");
        state_ = ExecutorState::RUNNING;
        emit curDirectiveChanged(cur_index_);
        Direct* directive = directives_.at(cur_index_);
        last_connections_.clear();
        last_connections_.append(QObject::connect(directive, &Direct::requestStartProgram, this, &Executor::requestStartProgram));
        last_connections_.append(QObject::connect(directive, &Direct::showWindow, this, &Executor::requestShowWindow));
        last_connections_.append(QObject::connect(directive, &Direct::sendMessageToPris, this, &Executor::requestSendDataToPris));
        last_connections_.append(QObject::connect(directive, &Direct::finished, this, &Executor::onDirectiveFinished));

        last_connections_.append(QObject::connect(this, &Executor::windowResponse, directive, &Direct::onWindowResult));
        last_connections_.append(QObject::connect(this, &Executor::PRISResponse, directive, &Direct::onPRISResult));
        last_connections_.append(QObject::connect(this, &Executor::StartProgResponse, directive, &Direct::onReqStartProgResult));

        directive->start();
}

void Executor::onDirectiveFinished(const Direct::ResultDirective& result) {
    qCDebug(logCore) << QString("Получили сигнал о завершении директивы");
    for (auto& connection : last_connections_) {
        QObject::disconnect(connection);
    }
    last_connections_.clear();
    state_ = ExecutorState::IDLE;
    emit directiveFinished(result, cur_index_);
    cur_index_++;
    if (result.type == Direct::RESULT_DIRECTIVE_TYPES::OK) {
        if (mode_ == ExecutorMode::AUTO && cur_index_ < directives_.count()) {
            startDirective();
        }
    }
}

void Executor::setDirectives(const QVector<Direct *> &directives) {
    this->directives_ = directives;
}
