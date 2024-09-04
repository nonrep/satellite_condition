#ifndef JSONAGENT_H
#define JSONAGENT_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QVariantList>
#include <QDir>

class JsonAgent : public QObject
{
    Q_OBJECT

public:
    explicit JsonAgent(QObject *parent = 0);

    static QVariantMap read(const QString &fileName);

    static QVariantList readToList(const QString &fileName);

    void write(const QVariantList &list, const QString &fileName) const;

    void write(const QVariantMap &map, const QString &fileName) const;

    bool writeFile(const QJsonDocument &doc, const QString &fileName) const;

    static bool saveDataToLocalDir(const QByteArray &data, const QString &fileName);
public slots:
    static QString readFile(const QString &fileName);
};

#endif // JSONAGENT_H
