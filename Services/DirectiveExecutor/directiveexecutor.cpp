#include "Services/DirectiveExecutor/directiveexecutor.h"

#include "Services/Directives/commanddirective.h"
#include "Services/Directives/dokladdirective.h"
#include "Services/Directives/insructionsdirective.h"
#include "Services/Directives/mainoperdirecitve.h"
#include "Services/Directives/startsectiondirective.h"
#include "Services/Directives/variantdirective.h"

#include <Logger/logger.h>
#include "logging_categories.h"

DirectiveExecutor::DirectiveExecutor(const AppConfig& cfg, QObject* parent) :
    QObject(parent), cfg_(cfg)
{
    qCInfo(logCore) << "Вызван и выполнен коструктор DirectiveExecutor";
}

void DirectiveExecutor::setSection(const QString& section_name, const SectionStruct& section_struct, const RequestId request_id)
{
    StartSectionDirective* caller = nullptr;

    if (request_directives_.contains(request_id)) {
        caller = dynamic_cast<StartSectionDirective*>(request_directives_.value(request_id).data());
        request_directives_.remove(request_id);
        if (!caller) {
            emit failed("ДИРЕКТИВА ДЛЯ КОТОРОЙ ПОЛУЧЕН ОТВЕТ УЖЕ УНИЧТОЖЕНА!");
        }
    }

    qCInfo(logCore) << "Вызван метод setSection";
    ExecuteFrame frame;
    frame.section_name = section_name;

    const QVector<ChapterType> chapter_types = {
        ChapterType::STATE,
        ChapterType::NOT_STATE,
        ChapterType::PRI
    };

    for (const ChapterType chapter_type : chapter_types) {
        QStringList error_messages;

        auto directives =
            createDirectiveFromChapter(section_struct.get(chapter_type), error_messages);

        if (!error_messages.isEmpty()) {
            qCWarning(logCore) << "Выявлена ошибка при подготовке раздела к исполнению:\n" + error_messages.join("\n");

            qCInfo(logCore) << "Отправляем сигнал requestFailed";
            emit requestFailed(request_id, error_messages.join("\n"));

            if (caller) {
                qCInfo(logCore) << "Уведомляем вызывающую директиву о закрытии вызванной программы";
                caller->onReqStartProgResult(false);
            }

            ChapterStruct chapter = section_struct.get(chapter_type);
            for (auto& directive : chapter.directives) {
                directive.status = DirectStatus::LOAD;
            }

            return;
        }

        frame.directives_for_execute[chapter_type] = std::move(directives);
    }

    ExecuteParam execute_param = getParamFromStartDirective(caller);
    frame.active_chapter = execute_param.active_chapter;

    if (!execute_param.label.isEmpty()) {
        if (frame.active_chapter != ChapterType::NOT_STATE) {
            QString error_message = "Недопустимо использование метки раздела в разделах отличных от НШС!";
            qCCritical(logCore) << "Выявлена ошибка при подготовке раздела к исполнению:\n" + error_message;

            qCInfo(logCore) << "Отправляем сигнал requestFailed";
            emit requestFailed(request_id, error_message);

            if (caller) {
                qCInfo(logCore) << "Уведомляем вызывающую директиву о закрытии вызванной программы";
                caller->onReqStartProgResult(false);
            }

            qCInfo(logCore) << "Отправляем сигнал failed";
            emit failed(error_message);
            return;
        }

        if (!section_struct.get(ChapterType::NOT_STATE).hasLabel(execute_param.label)) {
            QString error_message = QString("Заданной метки '%1' не существует").arg(execute_param.label);
            qCWarning(logCore) << "Выявлена ошибка при подготовке раздела к исполнению:\n" + error_message;

            qCInfo(logCore) << "Отправляем сигнал requestFailed";
            emit requestFailed(request_id, error_message);

            if (caller) {
                qCInfo(logCore) << "Уведомляем вызывающую директиву о закрытии вызванной программы";
                caller->onReqStartProgResult(false);
            }

            return;
        } else {
            int cur_dir_index = section_struct.get(ChapterType::NOT_STATE).blockByLabel(execute_param.label).num_dirs_for_block.first();
            if (cur_dir_index < 0 || cur_dir_index >= section_struct.get(ChapterType::NOT_STATE).directives.count()) {
                QString error_message = QString("Индекс директивы '%1' по заданной метке '%2' недопустим!")
                        .arg(execute_param.label).arg(cur_dir_index);

                qCWarning(logCore) << "Выявлена ошибка при подготовке раздела к исполнению:\n" + error_message;

                qCInfo(logCore) << "Отправляем сигнал requestFailed";
                emit requestFailed(request_id, error_message);

                if (caller) {
                    qCInfo(logCore) << "Уведомляем вызывающую директиву о закрытии вызванной программы";
                    caller->onReqStartProgResult(false);
                }

                return;

            }

            frame.current_directive_index = cur_dir_index;
        }
    }

    frame.caller = caller;
    frame.section_struct = section_struct;
    frame.id_program = getNextID();

    sections_.append(frame);
    qCInfo(logCore) << "Добавили раздел в стек вызовов";

    qCInfo(logCore) << "Отправляем сигнал requestCompleted";
    emit requestCompleted(request_id);
    qCInfo(logCore) << "Отправляем сигнал sectionChanged";
    emit sectionChanged(frame.toGUI());

    qCInfo(logCore) << "Завершен метод setSection";
    return;
}

void DirectiveExecutor::startFrom(const ExecutionSnapshot& section_data, const RequestId request_id)
{
    qCInfo(logCore) << "Вызван метод startFrom";

    if (sections_.isEmpty()) {
        QString error_message = "Нет открытых разделов, выполнение директивы невозможно";
        qCCritical(logCore) << error_message;

        qCInfo(logCore) << "Отправлен сиганл requestFailed";
        emit requestFailed(request_id, error_message);
        qCInfo(logCore) << "Отправлен сиганл failed";
        emit failed(error_message);
        return;
    }

    if (sections_.last().id_program != section_data.exec_frame_id) {
        Direct* current_directive = sections_.last().directives_for_execute[sections_.last().active_chapter].at(sections_.last().current_directive_index);
        if (current_directive->isRunned()) {
            QString error_message = "Невозможно запустить новую директиву, пока выполняется другая";
            qCWarning(logCore) << error_message;

            qCInfo(logCore) << "Отправлен сиганл requestFailed";
            emit requestFailed(request_id, error_message);
            return;
        }
    }

    int section_index = sections_.count() - 1;
    while (section_index >= 0) {
        if (sections_[section_index].id_program == section_data.exec_frame_id) {
            break;
        }
        section_index -= 1;
    }

    if (section_index < 0) {
        QString error_message = "Не удалось найти раздел, который содержит директиву для запуска";
        qCCritical(logCore) << error_message;

        qCInfo(logCore) << "Отправлен сиганл requestFailed";
        emit requestFailed(request_id, error_message);
        qCInfo(logCore) << "Отправлен сиганл failed";
        emit failed(error_message);
        return;
    }

    if (section_index != sections_.count() - 1) {
        qCDebug(logCore) << "Перемещаем раздел на верхний уровень стека";
        ExecuteFrame frame = sections_[section_index];
        sections_.remove(section_index);
        sections_.append(frame);
    }

    sections_.last().active_chapter = section_data.active_chpater;

    if ((sections_.last().directives_for_execute[section_data.active_chpater].count() <= section_data.active_dir_index) ||
            section_data.active_dir_index < 0) {
        QString error_message = "Индекс директивы для запуска недопустим";
        qCWarning(logCore) << error_message;

        qCInfo(logCore) << "Отправлен сиганл requestFailed";
        emit requestFailed(request_id, error_message);
        return;
    }

    sections_.last().current_directive_index = section_data.active_dir_index;

    qCInfo(logCore) << "Отправляем сигнал requestCompleted";
    emit requestCompleted(request_id);  //Отправляем сигнал тут, чтобы не было необходимости ждать завершения директивы, т.к. она может потянуть за собой вызов новой программы и т.д.
                                        //При этом не отправляем сигнал sectionsChanged для перерисовки GUI, т.к. он отправится в ::continueExecute();

    qCInfo(logCore) << "Вызываем метод continueExecute";
    continueExecute();


    qCInfo(logCore) << "Завершен метод startFrom";
    return;
}

void DirectiveExecutor::setExecuteMode(const ExecuteMode& execute_mode, const RequestId request_id)
{
    qCInfo(logCore) << "Вызван метод setExecuteMode";

    execute_mode_ = execute_mode;

    switch (execute_mode_) {
    case (ExecuteMode::AUTO) : qCInfo(logCore) << "Режим исполнения переведен в АВТО"; break;
    case (ExecuteMode::STEP) : qCInfo(logCore) << "Режим исполнения переведен в ШАГ"; break;
    }

    qCInfo(logCore) << "Отправляем сигнал requestCompleted";
    emit requestCompleted(request_id);

    qCInfo(logCore) << "Завершен метод setExecuteMode";
}

QVector<Direct*> DirectiveExecutor::createDirectiveFromChapter(const ChapterStruct& chapter, QStringList& error_messages)
{
    qCInfo(logCore) << "Вызван метод createDirectiveFromChapter";
    error_messages.clear();
    QVector<Direct*> result;

    for (int index = 0; index < chapter.directives.count(); ++index) {
        Direct* current_directive = createDirect(chapter.directives[index]);
        if (!current_directive) {
            QString error_message = QString("Ошибка при создании директивы №%1 - Не удалось распознать тип!").arg(index + 1);
            qCWarning(logCore) << error_message;
            continue;
        }
        if (!current_directive->isValid()) {
            QString error_message = QString("В директиве №%1 обнаружены ошибки:\n%2").arg(index + 1).arg(current_directive->getErrorValid().join("\n"));
            qCWarning(logCore) << error_message;

            error_messages.append(error_message);
        }
        result.append(current_directive);
    }

    if (!error_messages.isEmpty()) {
        QString log_message = "При выполнении метода createDirectiveFromChapter() были обнаружены проблемы при проверке директив!";
        qCWarning(logCore) << log_message;

        result.clear();
    }

    qCInfo(logCore) << "Завершен метод createDirectiveFromChapter";
    return result;
}


//СДЕЛАТЬ ТУТ DIRECT ЧЕРЕЗ QPOINTER
Direct* DirectiveExecutor::createDirect(const DirectiveStruct& directive)
{
    qCInfo(logCore) << "Вызван метод createDirect";
    Direct* result = nullptr;

    switch (directive.type) {
    case DirectType::COMMAND            : result = new CommandDirective(directive, this);           break;
    case DirectType::DOKLAD             : result = new DokladDirective(directive, this);            break;
    case DirectType::INSTRUCTIONS       : result = new InsructionsDirective(directive, this);       break;
    case DirectType::MAIN_OPERATION     : result = new MainOperDirecitve(directive, this);          break;
    case DirectType::START_SECTION      : result = new StartSectionDirective(directive, this);      break;
    case DirectType::VARIANT            : result = new VariantDirective(directive, this);           break;
    default:
    {
        QString error_message = "Неопознанный тип директивы";
        qCWarning(logCore) << error_message;
        break;
    }
    }

    if (result) {
        QObject::connect(result, &Direct::showWindow, this, [this, result](const WidgetTypes& types, const WidgetInfo& info){
            const RequestId req_id = RequestIdGenerator::next();
            this->request_directives_.insert(req_id, result);
            emit requestShowWindow(types, info, req_id);
        });
        QObject::connect(result, &Direct::sendMessageToPris, this, [this, result](const PRISMessage& message){
            const RequestId req_id = RequestIdGenerator::next();
            this->request_directives_.insert(req_id, result);
            emit requestSendDataToPris(message, req_id);
        });
        QObject::connect(result, &Direct::requestStartProgram, this, [this, result](const ExecuteParam& params){
           const RequestId req_id = RequestIdGenerator::next();
           this->request_directives_.insert(req_id, result);
           emit requestLoadSection(params, req_id);
        });
        QObject::connect(result, &Direct::finished, this, [this, result](const Direct::ResultDirective& result_dir){
            onDirectiveFinished(result_dir, result);
        });
        QObject::connect(result, &Direct::failed, this, [this, result](const QString& message){
            onDirectiveFailed(message, result);
        });
    }

    qCInfo(logCore) << "Завершен метод createDirect";
    return result;
}

ExecuteParam DirectiveExecutor::getParamFromStartDirective(StartSectionDirective* directive)
{
    Q_UNUSED(directive);
    qCInfo(logCore) << "Вызван метод getParamFromStartDirective";
    ExecuteParam exec_param;

    qCInfo(logCore) << "Завершен метод getParamFromStartDirective";
    return exec_param;
}

void DirectiveExecutor::continueExecute()
{
    qCInfo(logCore) << "Вызван метод continueExecute";
    ExecuteFrame& exec_prog = sections_.last();

    if (exec_prog.current_directive_index >= exec_prog.directives_for_execute[exec_prog.active_chapter].count()
            || exec_prog.current_directive_index < 0) {
        QString error_message = "Недопустимый индекс для запуска директивы";
        qCCritical(logCore) << error_message;

        qCInfo(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    Direct* directive_for_runned = exec_prog.directives_for_execute[exec_prog.active_chapter][exec_prog.current_directive_index];
    if (directive_for_runned->isRunned()) {
        QString error_message = "Невозможно запустить уже запущенную директиву";
        qCCritical(logCore) << error_message;

        qCInfo(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    exec_prog.section_struct.get(exec_prog.active_chapter).directives[exec_prog.current_directive_index].status = DirectStatus::RUNNING;
    directive_for_runned->run();

    qCInfo(logCore) << "Завершен метод continueExecute";
}

void DirectiveExecutor::onWindowResponse(const WindowResultData& result, const RequestId source_request_id)
{
    if (!request_directives_.contains(source_request_id)) {
        emit failed("ДИРЕКТИВА ДЛЯ ОТПРАВКИ ДАННЫХ ИЗ GUI НЕ НАЙДЕНА!");
        return;
    }

    Direct* directive = request_directives_.value(source_request_id);
    if (!directive) {
        emit failed("ДИРЕКТИВА ДЛЯ КОТОРОЙ ПОЛУЧЕН ОТВЕТ УЖЕ УНИЧТОЖЕНА!");
        return;
    }
    request_directives_.remove(source_request_id);
    directive->onWindowResult(result);
}

void DirectiveExecutor::onPRISResponse(const PRISResultData& result, const RequestId source_request_id)
{
    if (!request_directives_.contains(source_request_id)) {
        emit failed("ДИРЕКТИВА ДЛЯ ОТПРАВКИ ДАННЫХ ИЗ PRIS НЕ НАЙДЕНА!");
        return;
    }

    Direct* directive = request_directives_.value(source_request_id);
    if (!directive) {
        emit failed("ДИРЕКТИВА ДЛЯ КОТОРОЙ ПОЛУЧЕН ОТВЕТ УЖЕ УНИЧТОЖЕНА!");
        return;
    }
    request_directives_.remove(source_request_id);
    directive->onPRISResult(result);
}

void DirectiveExecutor::onLoadSectionFailed(const QString& error_messages, RequestId source_request_id)
{
    if (!request_directives_.contains(source_request_id)) {
        emit failed("ДИРЕКТИВА ДЛЯ ОТПРАВКИ ДАННЫХ ИЗ LOAD_SECTION НЕ НАЙДЕНА!");
        return;
    }

    StartSectionDirective* directive = dynamic_cast<StartSectionDirective*>(request_directives_.value(source_request_id).data());
    request_directives_.remove(source_request_id);
    if (!directive) {
        emit failed("ДИРЕКТИВА ДЛЯ КОТОРОЙ ПОЛУЧЕН ОТВЕТ УЖЕ УНИЧТОЖЕНА!");
        return;
    }
    //request_directives_.remove(source_request_id);
    if (!error_messages.isEmpty()) {
        QString message = "Не удалось загрузить раздел:\n" + error_messages;
        qCWarning(logCore) << message;
        qCInfo(logCore) << "Отправляем сигнал warning()";
        emit warning(message);
        qCInfo(logCore) << "Уведомляем директиву-caller о завершении программы с false";
        directive->onReqStartProgResult(false);
        return;
    }
}

void DirectiveExecutor::onDirectiveFinished(const Direct::ResultDirective& result, Direct* direct)
{
    if (direct != getCurrentExecuteDirect().data()) {
        QString message = "Не удалось сопоставить директиву отправившую сигнал о завершении с текущей выполняемой директивой!";
        qCCritical(logCore) << message;
        qCInfo(logCore) << "Отправляем сигнал failed";
        return;
    }

    ExecuteFrame& active_section = sections_.last();
    const bool has_next_directive = active_section.current_directive_index + 1 < active_section.directives_for_execute[active_section.active_chapter].count();

    if (result.type == Direct::RESULT_DIRECTIVE_TYPES::NOT_COMPLETED) {
        active_section.section_struct.get(active_section.active_chapter).directives[active_section.current_directive_index].status = DirectStatus::CURRENT;
    } else if (result.type == Direct::RESULT_DIRECTIVE_TYPES::ERROR) {
        QString message = "При выполнении директивы возникла ошибка:\n" + result.message;
        qCWarning(logCore) << message;
        qCInfo(logCore) << "Отправляем сигнал warning()";
        emit warning(message);
        active_section.section_struct.get(active_section.active_chapter).directives[active_section.current_directive_index].status = DirectStatus::FAILED;
    } else if (result.type == Direct::RESULT_DIRECTIVE_TYPES::OK) {
        active_section.section_struct.get(active_section.active_chapter).directives[active_section.current_directive_index].status = DirectStatus::FINISHED;

        if (has_next_directive) {
            active_section.section_struct.get(active_section.active_chapter).directives[active_section.current_directive_index + 1].status = DirectStatus::CURRENT;
            active_section.current_directive_index += 1;
            if (execute_mode_ == ExecuteMode::AUTO) {
                continueExecute();
            }
        } else {
            //ПРОГРАММА ЗАВЕРШЕНА
        }
    }

    emit this->sectionChanged(active_section.toGUI());
    return;
}
void DirectiveExecutor::onDirectiveFailed(const QString& message, Direct* direct)
{
    if (direct != getCurrentExecuteDirect().data()) {
        QString message = "Не удалось сопоставить директиву отправившую сигнал о завершении с текущей выполняемой директивой!";
        qCCritical(logCore) << message;
        qCInfo(logCore) << "Отправляем сигнал failed";
        return;
    }

    ExecuteFrame& active_section = sections_.last();
    active_section.section_struct.get(active_section.active_chapter).directives[active_section.current_directive_index].status = DirectStatus::FAILED;
    emit this->sectionChanged(active_section.toGUI());
    emit failed(message);
}

QPointer<Direct> DirectiveExecutor::getCurrentExecuteDirect()
{
    QPointer<Direct> result;
    if (sections_.isEmpty()) {
        return result;
    }

    if (sections_.last().current_directive_index < 0 || sections_.last().current_directive_index >= sections_.last().directives_for_execute[sections_.last().active_chapter].count()) {
        emit failed("Недопустимый индекс директивы для исполнения!");
        return result;
    }

    if (sections_.last().section_struct.get(sections_.last().active_chapter).directives[sections_.last().current_directive_index].status == DirectStatus::RUNNING) {
        result = sections_.last().directives_for_execute[sections_.last().active_chapter][sections_.last().current_directive_index];
    }

    return result;
}

void DirectiveExecutor::onSetLoadedSectionRequested(const QString& section_name, const RequestId source_request_id)
{
    for (int i = 0; i < sections_.size(); ++i) {
        if (sections_[i].section_name == section_name) {
            emit sendSectionForView(sections_[i].toGUI());
            return;
        }
    }

    emit requestFailed(source_request_id, "Искомой секции не найдено!");
}

void DirectiveExecutor::onStatusSectionsRequested(const QString& section_name, const RequestId request_id)
{
    for (int i = 0; i < sections_.size(); ++i) {
        if (sections_[i].section_name == section_name) {
            emit sendStatusSection(true, request_id);
            return;
        }
    }

    emit sendStatusSection(false, request_id);
}
