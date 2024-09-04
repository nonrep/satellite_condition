#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

#include "logger.h"

#define MAX_SIZE 100000

Logger_old::Logger_old(QString filePath) : filePath(filePath)
{
    QDir dir;
    if (!dir.mkpath(filePath)) {
        qDebug() << "ERROR creating path: " << filePath;
    }
}

void Logger_old::addLogFile(QString tag, QString filename)
{
    QString fullPath = filePath + "%1.log";
    logMap[tag] = filename.isEmpty() ? fullPath.arg(tag) : fullPath.arg(filename.remove(".log"));
}

void Logger_old::onWriteLog(QString message, QString tag)
{
    QString filePath = logMap[tag];

    if(filePath.isEmpty()) {
        addLogFile(tag);
        filePath = logMap[tag];
    }

    QFileInfo fileInfo(filePath);

    if (fileInfo.size() > MAX_SIZE) {
        QString backupFileName(QString("%1.old").arg(filePath));
        QFileInfo backupFileInfo(backupFileName);

        if (backupFileInfo.exists())
            QFile::remove(backupFileName);
        QFile::rename(filePath, backupFileName);
    }

    QFile logFile(filePath);

    logFile.open(QFile::Append);
    if (!logFile.isOpen()) {
        qDebug() << "ERROR can't open log file " << logFile.errorString().toLatin1().data();
        return;
    }

    QTextStream stream(&logFile);
    stream << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz ")
           << message << "\n";

    logFile.close();
}
