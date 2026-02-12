#pragma once

#include <QString>

namespace protocol_naming {

QString formatBisectPart(int bisectNumber);
QString buildProtocolName(const QString &part1, const QString &part2, int bisectNumber);

} // namespace protocol_naming
