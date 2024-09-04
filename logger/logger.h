#ifndef LOGGER_OLD_H
#define LOGGER_OLD_H

#include <QObject>
#include <QDir>
#include <QMap>
#include <QCoreApplication>

using QStringMap = QMap<QString, QString>;

class Logger_old : public QObject
{
    Q_OBJECT
public:
    explicit Logger_old(QString filePath);

    void addLogFile(QString tag, QString filename = "");

public slots:
    void onWriteLog(QString message, QString tag = "debug.log");

private:
    QString filePath;
    QStringMap logMap;
};

static void logit(QString text, QString tag = "debug")
{
    static Logger_old logger(QDir::homePath() + "/." + QFileInfo(QCoreApplication::applicationFilePath()).fileName() + "/log/");


    logger.onWriteLog(text, tag);
}
Q_DECL_UNUSED
static void logit(std::string text, std::string tag = "debug")
{
    logit(QString::fromStdString(text), QString::fromStdString(tag));
}
#endif // LOGGER_OLD_H
