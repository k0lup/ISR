#pragma once

#include <QByteArray>

#include "../portion.h"

namespace por_codec {

bool tryDecodeHeader(const QByteArray &packet, POR &header);
QByteArray encodePacket(const POR &header, const QByteArray &payload);
POR makeHeader(int payloadSize,
               quint8 from,
               quint8 to,
               quint8 sort,
               quint8 type,
               quint8 atomCount,
               quint8 kind,
               qint16 no,
               quint8 s1,
               quint8 s2);

} // namespace por_codec
