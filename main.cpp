#include <QCoreApplication>
#include <QObject>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>

#include "configmanager/configmanager.h"
#include "configmanager/jsonagent.h"
#include "logger/logger_boost_qtwrap.h"
#include "client.h"

const int DEFAULT_HTTP_SERVER_PORT = 8099;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    const QString workPath = configManager()->getDefaultPath();
    if (!QDir(workPath).exists())
        QDir().mkdir(workPath);

    // setup logger
    QString loggerPath = workPath + "/log/";
    if (!QDir(loggerPath).exists())
        QDir().mkdir(loggerPath);

    QVariantMap log_map = JsonAgent::read(workPath + "/conf_log.json");
    setup_logger(loggerPath, log_map);

    QString log_tag = "KA_states";
    logit("start client", log_tag, Logger::severity_level::normal);

    // setup paths to settings
    const QString CONFIG_PATH = workPath + "config.json";

    QString settingsPath = QString(CONFIG_PATH);

    // setup websocket
    QVariantMap wsServer = JsonAgent::read(settingsPath).value("ws_server").toMap();
    QString ipSerw = wsServer.value("ip").toString();
    int portSerw = wsServer.value("port").toInt();
    if (ipSerw.isEmpty()) {
        logit("server ip is missing\n client closed", log_tag, Logger::severity_level::error);

        return 0;
    }
    else if (!portSerw) {
        portSerw = DEFAULT_HTTP_SERVER_PORT;
        logit("server port is missing, using default", log_tag, Logger::severity_level::warning);
    }

    QUrl url;
    url.setScheme("ws");
    url.setHost(ipSerw);
    url.setPort(portSerw);
    bool debug = false;
    Client client(url, debug);

    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&]() {
        logit("client closed", log_tag, Logger::severity_level::normal);
    });
    QObject::connect(&client, &Client::closed, &a, &QCoreApplication::quit);

    return a.exec();
}
