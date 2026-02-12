#include "por_codec.h"

#include <QtEndian>
#include <cstring>

namespace por_codec {

bool tryDecodeHeader(const QByteArray &packet, POR &header) {
    if (packet.size() < static_cast<int>(sizeof(POR))) {
        return false;
    }

    POR raw{};
    memcpy(&raw, packet.constData(), sizeof(POR));

    header = raw;
    header.lenPor = qFromLittleEndian(raw.lenPor);
    header.numPor = qFromLittleEndian(raw.numPor);
    header.lenArt = qFromLittleEndian(raw.lenArt);
    header.lenCort = qFromLittleEndian(raw.lenCort);

    return true;
}

QByteArray encodePacket(const POR &header, const QByteArray &payload) {
    POR raw = header;
    raw.lenPor = qToLittleEndian(header.lenPor);
    raw.numPor = qToLittleEndian(header.numPor);
    raw.lenArt = qToLittleEndian(header.lenArt);
    raw.lenCort = qToLittleEndian(header.lenCort);

    QByteArray packet;
    packet.resize(static_cast<int>(sizeof(POR)) + payload.size());
    memcpy(packet.data(), &raw, sizeof(POR));
    memcpy(packet.data() + static_cast<int>(sizeof(POR)), payload.data(), payload.size());
    return packet;
}

POR makeHeader(int payloadSize,
               quint8 from,
               quint8 to,
               quint8 sort,
               quint8 type,
               quint8 atomCount,
               quint8 kind,
               qint16 no,
               quint8 s1,
               quint8 s2) {
    POR header{};
    header.lenPor = static_cast<quint16>(payloadSize + sizeof(POR));
    header.adrTo = to;
    header.adrFrom = from;
    header.numPor = no;
    header.dop1 = s1;
    header.dop2 = s2;
    header.lenArt = static_cast<quint16>(payloadSize + 8);
    header.kindOfWork = kind;
    header.sortOfWork = sort;
    header.lenCort = static_cast<quint16>(payloadSize);
    header.atomType = type;
    header.atomQuant = atomCount;
    return header;
}

} // namespace por_codec
