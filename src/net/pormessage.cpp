#include "pormessage.h"

#include <QString>
#include <cstring>

QByteArray PorMessage::serialize() const
{
    PorMessage copy = *this;
    copy.header.LenPor = static_cast<quint16>(headerSize() + copy.payload.size());

    QByteArray out;
    out.resize(copy.header.LenPor);
    std::memcpy(out.data(), &copy.header, headerSize());
    if (!copy.payload.isEmpty()) {
        std::memcpy(out.data() + headerSize(), copy.payload.constData(), copy.payload.size());
    }
    return out;
}

bool PorMessage::deserialize(const QByteArray &raw, PorMessage &message, QString *error)
{
    if (raw.size() < headerSize()) {
        if (error) *error = QStringLiteral("Недостаточно данных для заголовка POR");
        return false;
    }

    std::memcpy(&message.header, raw.constData(), headerSize());
    if (raw.size() < message.header.LenPor || message.header.LenPor < headerSize()) {
        if (error) *error = QStringLiteral("Некорректная длина POR: %1").arg(message.header.LenPor);
        return false;
    }

    message.payload = raw.mid(headerSize(), message.header.LenPor - headerSize());
    return true;
}

int PorMessage::headerSize()
{
    return static_cast<int>(sizeof(PorHeader));
}
