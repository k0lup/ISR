#ifndef PORMESSAGE_H
#define PORMESSAGE_H

#include <QByteArray>
#include <QtGlobal>

#pragma pack(push, 1)
struct PorHeader {
    quint16 LenPor = 0;
    quint16 AdrTo = 0;
    quint16 AdrFrom = 0;
    quint16 NumPor = 0;
    quint16 ReqLent = 0;
    quint16 KindofWork = 0;
    quint16 SortofWork = 0;
    quint16 LenCort = 0;
    quint16 AtomType = 0;
    quint16 AtomQuant = 0;
};
#pragma pack(pop)

struct PorMessage {
    PorHeader header;
    QByteArray payload;

    QByteArray serialize() const;
    static bool deserialize(const QByteArray &raw, PorMessage &message, QString *error = nullptr);
    static int headerSize();
};

#endif // PORMESSAGE_H
