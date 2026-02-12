#include "report_slots.h"

#include "runtime_state.h"

namespace report_slots {

bool isReportOpen(const RuntimeState &state, const QString &name) {
    return state.partisionRepName == name || state.secondRepName == name || state.thirdRepName == name;
}

int freeSlotIndex(const RuntimeState &state) {
    if (state.partisionRepName.isEmpty()) return 0;
    if (state.secondRepName.isEmpty()) return 1;
    if (state.thirdRepName.isEmpty()) return 2;
    return -1;
}

void assignToFreeSlot(RuntimeState &state, const QString &name, int *assignedSlot) {
    const int slot = freeSlotIndex(state);
    if (slot == 0) state.partisionRepName = name;
    else if (slot == 1) state.secondRepName = name;
    else if (slot == 2) state.thirdRepName = name;

    if (slot >= 0 && !state.openReports.contains(name)) {
        state.openReports.append(name);
    }
    if (assignedSlot) {
        *assignedSlot = slot;
    }
}

int clearReport(RuntimeState &state, const QString &name) {
    if (state.partisionRepName == name) {
        state.partisionRepName.clear();
        state.openReports.removeAll(name);
        return 0;
    }
    if (state.secondRepName == name) {
        state.secondRepName.clear();
        state.openReports.removeAll(name);
        return 1;
    }
    if (state.thirdRepName == name) {
        state.thirdRepName.clear();
        state.openReports.removeAll(name);
        return 2;
    }
    return -1;
}

QString slotName(const RuntimeState &state, int index) {
    if (index == 0) return state.partisionRepName;
    if (index == 1) return state.secondRepName;
    if (index == 2) return state.thirdRepName;
    return {};
}

} // namespace report_slots
