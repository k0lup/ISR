#include "config_loader.h"

#include <QFile>
#include <QTextStream>
#include "encodingdetector.h"

ConfigLoader::ConfigLoader(Options opt)
    : opt_(opt)
{
}

bool ConfigLoader::loadFile(const QString& path, RawConfig& out, ConfigError* error) const
{
    QFile f(path);
    EncodingDetector::Encoding encoding = EncodingDetector::detectFile(path);

    if (encoding == EncodingDetector::Encoding::ERROR) {
        setError(error, path, -1, "Не удалось установить кодировку файла");
    }
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(error, path, -1, "Не удалось открыть файл");
        return false;
    }

    QTextStream in(&f);
    in.setCodec(EncodingDetector::codecName(encoding));

    int lineNo = 0;
    while (!in.atEnd()) {
        ++lineNo;
        const QString rawLine = in.readLine();

        if (!parseLine(rawLine, lineNo, path, out, error)) {
            // parseLine уже заполнил error при strict/фатальной ошибке
            if (opt_.strict) return false;
            // если не strict — просто пропускаем проблемную строку
        }
    }

    return true;
}

bool ConfigLoader::parseLine(const QString& line,
                             int lineNo,
                             const QString& filePath,
                             RawConfig& out,
                             ConfigError* error) const
{
    QString s = line;
    s = s.trimmed();

    if (s.isEmpty())
        return true;

    if (opt_.allowComments && isCommentLine(s))
        return true;

    const int eqPos = s.indexOf('=');
    if (eqPos < 0) {
        if (opt_.strict)
            setError(error, filePath, lineNo, "Ожидался символ '='");
        return !opt_.strict;
    }

    const QString key = trimmed(s.left(eqPos));
    if (key.isEmpty()) {
        if (opt_.strict)
            setError(error, filePath, lineNo, "Пустой ключ слева от '='");
        return !opt_.strict;
    }

    const QString valuesPart = trimmed(s.mid(eqPos + 1));

    QStringList values;
    if (opt_.keepEmptyValues) {
        // Оставляем пустые куски: "a=1;;2" -> ["1", "", "2"]
        values = valuesPart.split(';', QString::KeepEmptyParts);
        for (QString& v : values) v = v.trimmed();
    } else {
        // Пропускаем пустые: "a=1;;2" -> ["1", "2"]
        values = valuesPart.split(';', QString::SkipEmptyParts);
        for (QString& v : values) v = v.trimmed();
        values.removeAll(QString());
    }

    // Если значений нет — ключ можно “зафиксировать” пустым списком при желании.
    // Здесь: если values пустой — просто создадим пустой список, чтобы key считался присутствующим.
    if (!out.contains(key))
        out.insert(key, QStringList{});

    if (!values.isEmpty())
        out[key].append(values);

    return true;
}

bool ConfigLoader::isCommentLine(const QString& trimmedLine) const
{
    return trimmedLine.startsWith('#') || trimmedLine.startsWith("//") || trimmedLine.startsWith("/*");
}

void ConfigLoader::setError(ConfigError* error,
                            const QString& filePath,
                            int lineNo,
                            const QString& message)
{
    if (!error) return;
    error->filePath = filePath;
    error->line = lineNo;
    error->message = message;
}

QString ConfigLoader::trimmed(const QString& s)
{
    return s.trimmed();
}
