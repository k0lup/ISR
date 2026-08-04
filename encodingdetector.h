#ifndef ENCODINGDETECTOR_H
#define ENCODINGDETECTOR_H

#include <QByteArray>
#include <QString>

class EncodingDetector
{
public:
    enum class Encoding
    {
        UTF8,
        WINDOWS1251,
        ERROR
    };

    static Encoding detect(const QByteArray& data);
    static Encoding detectFile(const QString& file_path);

    static const char* codecName(Encoding encoding);
    static QString encodingName(Encoding encoding);

private:
    static bool isValidUtf8(const QByteArray &data);
};

#endif // ENCODINGDETECTOR_H
