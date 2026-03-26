#ifndef ENVLOADER_H
#define ENVLOADER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMap>

class EnvLoader {
public:
    static void load(const QString &filePath = ".env") {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty() || line.startsWith('#'))
                continue;
            int eq = line.indexOf('=');
            if (eq == -1) continue;
            QString key = line.left(eq).trimmed();
            QString val = line.mid(eq + 1).trimmed();
            qputenv(key.toUtf8(), val.toUtf8());
        }
    }

    static QString get(const QString &key) {
        return QString::fromUtf8(qgetenv(key.toUtf8()));
    }
};

#endif // ENVLOADER_H
