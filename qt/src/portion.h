#pragma once

#include <QtGlobal>

#pragma pack(push, 1)
struct POR {
    quint16 lenPor;
    quint8 adrTo;
    quint8 adrFrom;
    qint16 numPor;
    quint8 dop1;
    quint8 dop2;

    quint16 lenArt;
    quint8 kindOfWork;
    quint8 sortOfWork;

    quint16 lenCort;
    quint8 atomType;
    quint8 atomQuant;
};
#pragma pack(pop)

static_assert(sizeof(POR) == 16, "POR packet header size mismatch");
