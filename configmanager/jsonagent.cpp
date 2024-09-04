#include "../configmanager/jsonagent.h"

#include <QCoreApplication>

#define DATEFORMAT "dd.MM.yyyy"
#define TIMEFORMAT "hh:mm"

JsonAgent::JsonAgent(QObject *parent) : QObject(parent) {}

void JsonAgent::write(const QVariantList &list, const QString &fileName) const
{
    QJsonArray array;

    foreach (QVariant map, list) {
        array.append(QJsonObject::fromVariantMap(map.toMap()));
    }

    writeFile(QJsonDocument(array), fileName);
}

void JsonAgent::write(const QVariantMap &map, const QString &fileName) const
{
    writeFile(QJsonDocument(QJsonObject::fromVariantMap(map)), fileName);
}

bool JsonAgent::writeFile(const QJsonDocument &doc, const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return false;
    }

    file.write(doc.toJson());
    file.close();
    return true;
}

bool JsonAgent::saveDataToLocalDir(const QByteArray &data, const QString &fileName)
{
    QString defPath = QDir::homePath() + "/." + QFileInfo(QCoreApplication::applicationFilePath())
                                          .fileName() + "/" + fileName;
    QFile file(defPath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return false;
    }

    file.write(data);
    file.close();
    return true;
}

QVariantMap JsonAgent::read(const QString &fileName)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(readFile(fileName).toUtf8());
    if (jsonDoc.isEmpty()) {

        return QVariantMap();
    }
    return jsonDoc.toVariant().toMap();
}

QVariantList JsonAgent::readToList(const QString &fileName)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(readFile(fileName).toUtf8());
    if (jsonDoc.isEmpty()) {

        return QVariantList();
    }
    return jsonDoc.toVariant().toList();
}

QString JsonAgent::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return "";
    }
    QString fileContent = file.readAll();
    file.close();
    return(fileContent);
}
