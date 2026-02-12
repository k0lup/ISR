#pragma once

#include <memory>

class QWidget;
class QDialog;
struct RuntimeState;

namespace dialogs {

enum class Kind {
    About,
    CheckSPO,
    End,
    PartitionList,
    Find,
    Request,
    SP,
    Stencil,
    Title,
    Variant,
    CloseReport,
    Completing,
    Settings,
    PartitionReport,
    SelectionReport,
    ChildWindow
};

std::unique_ptr<QDialog> createDialog(Kind kind, RuntimeState *state, QWidget *parent);

} // namespace dialogs
