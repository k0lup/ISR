#include "executor.h"
#include <QLoggingCategory>
#include "logger/logging_categories.h"

Executor::Executor(QObject* parent) :
    QObject(parent)
{
    state_ = ExecutorState::IDLE;
    mode_ = ExecutorMode::STEP;
    cur_index_ = -1;
    call_stack.clear();
}

void Executor::startFrom(const int index, const ExecutorMode mode) {
    qCDebug(logCore) << QString("Получили запрос на выполение директивы %1").arg(index);
    if (call_stack.isEmpty()) {
        emit failed(QString("Нет загруженных разделов!"));
        return;
    }
    if (index < 0 || index > call_stack.last().section.dii_file.chapters[static_cast<int>(call_stack.last().section.active_chapter_type)].directives.count()) {
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
        Direct* directive = call_stack.last().section.dii_file.chapters[static_cast<int>(call_stack.last().section.active_chapter_type)].directives[cur_index_];
        last_connections_.clear();
        last_connections_.append(QObject::connect(directive, &Direct::requestStartProgram, this, &Executor::onRequestedStartProgram));
        last_connections_.append(QObject::connect(directive, &Direct::showWindow, this, &Executor::requestShowWindow));
        last_connections_.append(QObject::connect(directive, &Direct::sendMessageToPris, this, &Executor::requestSendDataToPris));
        last_connections_.append(QObject::connect(directive, &Direct::finished, this, &Executor::onDirectiveFinished));

        last_connections_.append(QObject::connect(this, &Executor::windowResponse, directive, &Direct::onWindowResult));
        last_connections_.append(QObject::connect(this, &Executor::PRISResponse, directive, &Direct::onPRISResult));
        last_connections_.append(QObject::connect(this, &Executor::progStarted, directive, &Direct::onReqStartProgResult));

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
        if (mode_ == ExecutorMode::AUTO && cur_index_ < call_stack.last().section.dii_file.chapters[static_cast<int>(call_stack.last().section.active_chapter_type)].directives.count()) {
            startDirective();
        }
    }
}

/*void Executor::setDirectives(const QVector<Direct *> &directives) {
    this->directives_ = directives;
}*/

void Executor::addSection(const Section& section) {
    if (!requested_start_program_info_.abbrev.isEmpty()) {
        if (section.section_name != requested_start_program_info_.abbrev) {
            emit failed(QString("Была запрошена загрузка секции '%1', а получена секция '%2'!")
                        .arg(requested_start_program_info_.abbrev)
                        .arg(section.section_name));
            emit progStarted(false);
            requested_start_program_info_.clear();
            return;
        }
    }

    CallStackObject call_stack_object;
    call_stack_object.section = section;
    call_stack_object.mode = ExecutorMode::STEP;
    call_stack_object.state = ExecutorState::IDLE;
    call_stack_object.cur_index = -1;
    call_stack_object.last_connections_.clear();

    if (!requested_start_program_info_.abbrev.isEmpty()) {
        ChapterType chapter_type;
        if (requested_start_program_info_.section == "ШТАТ") {
            chapter_type = ChapterType::STATE;
        } else if (requested_start_program_info_.section == "ПРИ") {
            chapter_type = ChapterType::ACCIDENT;
        } else if (requested_start_program_info_.section == "НШС") {
            chapter_type = ChapterType::NON_STATE;
        } else {
            chapter_type = ChapterType::INCORRECT;
            emit failed(QString("Недопустимая секция для загрузки. Раздел '%1', секция '%2'").arg(requested_start_program_info_.abbrev).arg(requested_start_program_info_.section));
            emit progStarted(false);
            requested_start_program_info_.clear();
            return;
        }

        ExecutorMode executor_mode;
        if (requested_start_program_info_.mode == "АВТ") {
            executor_mode = ExecutorMode::AUTO;
        } else if (requested_start_program_info_.mode == "ШАГ") {
            executor_mode = ExecutorMode::STEP;
        } else {
            executor_mode = ExecutorMode::ERROR;
            emit failed(QString("Недопустимый режим работы для раздела '%1'").arg(requested_start_program_info_.section));
            emit progStarted(false);
            requested_start_program_info_.clear();
            return;
        }

        //ДОБАВИТЬ БЛОК НШС
        call_stack_object.section.active_chapter_type = chapter_type;
        call_stack_object.mode = executor_mode;
    }

    call_stack.append(call_stack_object);

    emit showSection(section);

    if (!requested_start_program_info_.abbrev.isEmpty()) {
        requested_start_program_info_.clear();
        emit progStarted(true);
    }
}

void Executor::onRequestedStartProgram(const START_SECTION_PARAMS& params) {
    if (!requested_start_program_info_.abbrev.isEmpty()) {
        emit failed(QString("Еще не была выполнена загрузка предыдущей секции - '%1', а уже запрошена загрузка секции '%2'")
                    .arg(requested_start_program_info_.abbrev).arg(params.section));
        emit progStarted(false);
        return;
    }

    requested_start_program_info_ = params;
    emit requestStartProgram(params.abbrev);
}
