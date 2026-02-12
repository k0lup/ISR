#include "protocol_naming.h"

namespace protocol_naming {

QString formatBisectPart(int bisectNumber) {
    if (bisectNumber < 0) {
        bisectNumber = 0;
    }
    return QStringLiteral("%1").arg(bisectNumber, 2, 10, QLatin1Char('0'));
}

QString buildProtocolName(const QString &part1, const QString &part2, int bisectNumber) {
    QString name = part1 + QStringLiteral(".") + part2;
    if (bisectNumber > 1) {
        name += QStringLiteral("-") + formatBisectPart(bisectNumber);
    }
    return name;
}

} // namespace protocol_naming
