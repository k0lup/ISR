#include "directiveexecutor.h"
#include "logging_categories.h"
#include <QLoggingCategory>

DirectiveExecutor::DirectiveExecutor(QObject *parent) : QObject(parent)
{

}

void DirectiveExecutor::setSection(SectionStruct section, const RequestId request_id)
{
    qCInfo(logCore) << "Вызван метод setSection";
    ExecutionPosition exec_pos;
    ExecutorMode mode = ExecutorMode::STEP;

    if (request_id != NO_REQUEST_ID) {
        qCDebug(logCore) << "Программа вызвана из директивы. Загружаем параметры запуска";


        if (!info_about_req_progs_.contains(request_id)) {
            QString failed_message = "НЕ УДАЛОСЬ ПОЛУЧИТЬ ПАРАМЕТРЫ ЗАПУСКА ПРОГРАММЫ (РАЗДЕЛ, МЕТКУ и т.д.)";
            qCCritical(logCore) << failed_message;
            qCDebug(logCore) << "Отправляем сигнал failed";
            emit failed(failed_message);
            return;
        }

        InfoAboutRequestedProgram info_about_prog = info_about_req_progs_.value(request_id);
        exec_pos.active_chapter = info_about_prog.chapter;
        if (!info_about_prog.label_nshs.isEmpty()) {
            if (info_about_prog.chapter != ChapterType::NOT_STATE) {
                QString failed_message = "УКАЗАНА МЕТКА НШС, НО ВЫБРАН ДРУГОЙ РАЗДЕЛ";
                qCCritical(logCore) << failed_message;
                qCDebug(logCore) << "Отправляем сигнал failed";
                emit failed(failed_message);
                return;
            }

            if (!section.get(info_about_prog.chapter).labels.contains(info_about_prog.label_nshs)) {
                QString warning_message = "В ЗАПРОШЕННОМ РАЗДЕЛЕ НЕТ МЕТКИ '" + info_about_prog.label_nshs + "'!";
                qCWarning(logCore) << warning_message;
                qCDebug(logCore) << "Отправляем сигнал failed";
                emit failed(warning_message);
                return;
            }

            exec_pos.active_direct_index = section.get(info_about_prog.chapter).labels.value(info_about_prog.label_nshs);
        }
        mode = info_about_prog.mode;

        qCDebug(logCore) << "Загрузили параметры запуска";
    }

    qCDebug(logCore) << "Добавляем программу в стек";

    ExecutionFrame frame;
    frame.caller_request_id = request_id;
    frame.execution_position = exec_pos;
    frame.mode = mode;
    frame.section = section;

    stack_.append(frame);
    stack_.last().execution_position.stack_index = stack_.count() - 1;

    qCDebug(logCore) << "Добавили программу в стек";

    //СТОИТ ЛИ ТУТ РЕГИСТРИРОВАТЬ ВСЕ ДИРЕКТИВЫ?

    qCInfo(logCore) << "Отправляем сигнал sectionSetted";
    emit sectionSetted(stack_.last().toGUI());

    qCInfo(logCore) << "Завершен метод setSection";
    return;
}

void DirectiveExecutor::startFrom(const int index, const ExecutorMode mode)
{
    qCInfo(logCore) << "Вызван метод startFrom";

    if (stack_.isEmpty()) {
        QString error_message = "Стек пуст, невозможно начать директиву!";
        qCCritical(logCore) << error_message;
        qCDebug(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    if (stack_.last().section.get(stack_.last().execution_position.active_chapter).directives.count() <= index) {
        QString error_message = "В активном разделе верхней программы в стеке нет такой директивы";
        qCCritical(logCore) << error_message;
        qCDebug(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    ExecutionFrame frame = stack_.last();
    frame.mode = mode;
    frame.execution_position.active_direct_index = index;

    qCDebug(logCore) << "Вызываем метод startCurrentDirective";
    startCurrentDirective();
}

void DirectiveExecutor::startCurrentDirective() {
    qCInfo(logCore) << "Вызван метод startCurrentDirective";

    if (state_ != ExecutorState::IDLE) {
        QString error_message = "Состояние исполнителя не позволяет запустить новую директиву";
        qCCritical(logCore) << error_message;
        qCDebug(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    if (stack_.isEmpty()) {
        QString error_message = "Стек пуст, невозможно начать директиву";
        qCCritical(logCore) << error_message;
        qCDebug(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    if (stack_.last().execution_position.active_direct_index == -1) {
        QString error_message = "В верхней программе стека нет активной директивы для выполнения";
        qCCritical(logCore) << error_message;
        qCDebug(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    ChapterStruct chapter = stack_.last().section.get(stack_.last().execution_position.active_chapter);
    if (chapter.directives.count() >= stack_.last().execution_position.active_direct_index) {
        QString error_message = "В активном разделе верхней программы стека количества директив меньше индекса запрошенной на выполнени директивы";
        qCCritical(logCore) << error_message;
        qCDebug(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    qCInfo(logCore) << "Переводим состояние DirectiveExecutor в RUNNING";
    state_ = ExecutorState::RUNNING;

    /*Direct* direct = new Direct(chapter.directives.at(stack_.last().execution_position.active_direct_index));
     *registerDirectives(direct);
     *direct->run();
     */

    emit directiveStarted(stack_.last().toGUI());
}

void DirectiveExecutor::onDirectiveFinished(const ExecutionPosition& exec_pos)
{
    Q_UNUSED(exec_pos)
    qCInfo(logCore) << "Вызван метод onDirectiveFinished";

    if (stack_.isEmpty() || stack_.last().execution_position.active_direct_index == -1) {
        QString error_message = "Не удалось получить ни одной запущенной директивы по сигналу завершения директивы";
        qCCritical(logCore) << error_message;
        qCDebug(logCore) << "Отправляем сигнал failed";
        emit failed(error_message);
        return;
    }

    emit directiveFinished(stack_.last().toGUI());

    bool need_start_next_dir = (stack_.last().mode == ExecutorMode::AUTO);

    if (stack_.last().section.get(stack_.last().execution_position.active_chapter).directives.count() >= stack_.last().execution_position.active_direct_index + 1) {
        stack_.last().execution_position.active_direct_index = -1;
        qCDebug(logCore) << "Заврешено выполнение программы, устанавливаем индекс активной директивы на -1";
        if (stack_.last().caller_request_id != NO_REQUEST_ID) {
            qCInfo(logCore) << "Заврешено выполнение программы, вызванной другой директивой, закрываем программу, удаляя ее из стека";
            emit programFinished(true);
            stack_.removeLast();
            need_start_next_dir = true;
        }
    }

    state_ = ExecutorState::IDLE;
    if (need_start_next_dir) {

    }
}


