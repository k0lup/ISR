#include "stencil_rules.h"

namespace stencil_rules {

bool isValid(const QString &value, QString *errorMessage) {
    const QString trimmed = value.trimmed();
    if (trimmed.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Трафарет не может быть пустым");
        }
        return false;
    }

    // Legacy-inspired invalid set from VCL implementation.
    static const QString kInvalidChars = QStringLiteral(" !<>#$*?/|:\\-\"");
    for (const QChar ch : trimmed) {
        if (kInvalidChars.contains(ch)) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("Недопустимый символ: '%1'").arg(ch);
            }
            return false;
        }
    }

    return true;
}

} // namespace stencil_rules
