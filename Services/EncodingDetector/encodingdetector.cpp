#include "encodingdetector.h"

#include <QFile>

EncodingDetector::Encoding EncodingDetector::detect(const QByteArray &data)
{
    if (data.startsWith("\xEF\xBB\xBF")) {
        return Encoding::UTF8;
    }

    if (isValidUtf8(data)) {
        return Encoding::UTF8;
    }

    return Encoding::WINDOWS1251;
}

EncodingDetector::Encoding EncodingDetector::detectFile(const QString &file_path)
{
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly)) {
        return Encoding::ERROR;
    }

    return detect(file.readAll());
}

const char* EncodingDetector::codecName(Encoding encoding)
{
    switch (encoding) {
    case Encoding::UTF8 :           return "UTF-8";
    case Encoding::WINDOWS1251 :    return "Windows-1251";
    case Encoding::ERROR :          return "Error";
    }
}

QString EncodingDetector::encodingName(Encoding encoding) {
    switch (encoding) {
    case Encoding::UTF8 :           return QStringLiteral("UTF-8");
    case Encoding::WINDOWS1251 :    return QStringLiteral("Windows-1251");
    case Encoding::ERROR :          return QStringLiteral("Error");
    }
}

bool EncodingDetector::isValidUtf8(const QByteArray &data)
{
    const auto *bytes = reinterpret_cast<const unsigned char *>(data.constData());

    const int size = data.size();

    for (int i = 0; i < size;) {
        const unsigned char first = bytes[i];

        if (first <= 0x7F) {
            ++i;
            continue;
        }

        int continuationCount = 0;
        unsigned int codePoint = 0;

        if ((first & 0xE0) == 0xC0) {
            continuationCount = 1;
            codePoint = first & 0x1F;

            if (first < 0xC2) {
                return false;
            }
        } else if ((first & 0xF0) == 0xE0) {
            continuationCount = 2;
            codePoint = first & 0x0F;
        } else if ((first & 0xF8) == 0xF0) {
            continuationCount = 3;
            codePoint = first & 0x07;

            if (first > 0xF4) {
                return false;
            }
        } else {
            return false;
        }

        if (i + continuationCount >= size) {
            return false;
        }

        for (int j = 1; j <= continuationCount; ++j) {
            const unsigned char continuation = bytes[i + j];

            if ((continuation & 0xC0) != 0x80) {
                return false;
            }

            codePoint = (codePoint << 6) | static_cast<unsigned int>(continuation & 0x3F);
        }

        if (continuationCount == 1 & codePoint < 0x80) {
            return false;
        }
        if (continuationCount == 2 & codePoint < 0x800) {
            return false;
        }
        if (continuationCount == 3 && codePoint < 0x10000) {
            return false;
        }

        if (codePoint >= 0xD800 && codePoint <=0xDFFF) {
            return false;
        }
        if (codePoint > 0x10FFFF) {
            return false;
        }

        i += continuationCount + 1;
    }

    return true;
}
