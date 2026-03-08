#pragma once

#include <QString>

namespace isr {

struct TitleSectionData {
    QString nameSection;
    QString protocolName;
    QString traf;
    QString rep;
    QString operatorPuAis;
    QString operatorAis;
    QString operatorTkckSsBvs;
    QString operatorNtkSi;
    QString operatorBs;

    bool isValid() const
    {
        return !nameSection.trimmed().isEmpty();
    }
};

} // namespace isr
