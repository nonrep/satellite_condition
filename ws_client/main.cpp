#include <QCoreApplication>
#include <QJsonDocument>
#include <QWebSocket>
#include <QString>

//#include "ws_client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QWebSocket socket;
    socket.open(QUrl(QStringLiteral("ws://192.168.34.65:8099")));

    QObject::connect(&socket, &QWebSocket::connected,[&socket]() {

        QVariantMap satellites;
        satellites["501"] = "A1";
        satellites["502"] = "A2";
        satellites["503"] = "C2";

        QVariantMap conditions;
        conditions["satellite_conditions"] = satellites;
        conditions["set_conditions"] = "true";

        QJsonDocument document = QJsonDocument::fromVariant(conditions);
        QByteArray byteArray = document.toJson();

        socket.sendBinaryMessage(byteArray);
    });

    // получение
    QObject::connect(&socket, &QWebSocket::binaryMessageReceived,[&a](const QByteArray &message) {
        qDebug() << "Message received:" << message;
        return a.exit();
    });


return a.exec();
}
