#include "dialogs.h"

#include <QDialog>

#include "checkspo_dialog.h"
#include "child_window_dialog.h"
#include "completing_dialog.h"
#include "about_dialog.h"
#include "close_report_dialog.h"
#include "selection_report_dialog.h"
#include "partition_report_dialog.h"
#include "stencil_dialog.h"
#include "partition_list_dialog.h"
#include "find_dialog.h"
#include "sp_dialog.h"
#include "settings_dialog.h"
#include "end_dialog.h"
#include "request_dialog.h"
#include "simple_dialog.h"
#include "title_dialog.h"
#include "variant_dialog.h"

namespace dialogs {

std::unique_ptr<QDialog> createDialog(Kind kind, RuntimeState *state, QWidget *parent) {
    switch (kind) {
    case Kind::About:
        return std::make_unique<AboutDialog>(state, parent);
    case Kind::CheckSPO:
        return std::make_unique<CheckSpoDialog>(state, parent);
    case Kind::End:
        return std::make_unique<EndDialog>(state, parent);
    case Kind::PartitionList:
        return std::make_unique<PartitionListDialog>(state, parent);
    case Kind::Find:
        return std::make_unique<FindDialog>(state, parent);
    case Kind::Request:
        return std::make_unique<RequestDialog>(state, parent);
    case Kind::SP:
        return std::make_unique<SpDialog>(state, parent);
    case Kind::Stencil:
        return std::make_unique<StencilDialog>(state, parent);
    case Kind::Title:
        return std::make_unique<TitleDialog>(state, parent);
    case Kind::Variant:
        return std::make_unique<VariantDialog>(state, parent);
    case Kind::CloseReport:
        return std::make_unique<CloseReportDialog>(state, parent);
    case Kind::Completing:
        return std::make_unique<CompletingDialog>(state, parent);
    case Kind::Settings:
        return std::make_unique<SettingsDialog>(state, parent);
    case Kind::PartitionReport:
        return std::make_unique<PartitionReportDialog>(state, parent);
    case Kind::SelectionReport:
        return std::make_unique<SelectionReportDialog>(state, parent);
    case Kind::ChildWindow:
        return std::make_unique<ChildWindowDialog>(state, parent);
    }

    return std::make_unique<QDialog>(parent);
}

} // namespace dialogs
