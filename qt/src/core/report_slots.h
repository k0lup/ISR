#pragma once

#include <QString>

struct RuntimeState;

namespace report_slots {

bool isReportOpen(const RuntimeState &state, const QString &name);
int freeSlotIndex(const RuntimeState &state); // 0..2 or -1
void assignToFreeSlot(RuntimeState &state, const QString &name, int *assignedSlot = nullptr);
int clearReport(RuntimeState &state, const QString &name); // slot index or -1
QString slotName(const RuntimeState &state, int index);

} // namespace report_slots
