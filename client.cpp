#include "client.h"

Client::Client(const QUrl &url, bool debug, QObject *parent) :
    QObject(parent),
    m_debug(debug)
{
    if (m_debug)
        qDebug() << "websocket server:" << url.toString();
    logit("websocket connecting to " + url.toString(), log_tag, Logger::severity_level::normal);

    connect(&m_webSocket, &QWebSocket::connected, this, &Client::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &Client::onDisconnected);
    m_webSocket.open(url);
}

void Client::onConnected()
{
    if (m_debug)
        qDebug() << "websocket connected";
    logit("websocket connected", log_tag, Logger::severity_level::normal);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived,
            this, &Client::onBinaryMessageReceived);

    //create input json
    QJsonObject json;
    json.insert("target", "satellites");
    json.insert("action", "list");
    QJsonDocument doc(json);
    QByteArray arr = doc.toJson();
    m_webSocket.sendBinaryMessage(arr);
    logit("request sended", log_tag, Logger::severity_level::normal);
}

void Client::onBinaryMessageReceived(QByteArray message)
{
    //message into QJsonObject
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QJsonObject json = doc.object();

    if (m_debug)
        qDebug() <<"response from back: " << json << Qt::endl;

    if (json["action"].toString() == "list" && json["code"].toInt() == 200
        && json["target"].toString() == "satellites")
    {
        logit("message received", log_tag, Logger::severity_level::normal);
        QList states = {"A1", "A2", "B1", "B2", "C1", "C2"};
        const int statesSize = states.size();
        QRandomGenerator random;

        //create request body header
        QJsonObject requestHeader;
        requestHeader.insert("target", "satellites");
        requestHeader.insert("action", "condition");

        QJsonArray satellites = json["satellites"].toArray();
        int size = satellites.size();

        logit("number of satellites: " + QString::number(size), log_tag, Logger::severity_level::normal);
        logit("start sending data", log_tag, Logger::severity_level::normal);
        for (int i = 0; i < satellites.size(); i++)
        {
            QJsonObject data;
            data.insert("satellite", satellites[i].toString());
            data.insert("condition", states[random.generate() % statesSize]);

            QJsonObject request(requestHeader);
            request.insert("data", data);
            QJsonDocument doc(request);

            QByteArray arr = doc.toJson();
            QTimer::singleShot(1000 * i, this, [=] () {
                m_webSocket.sendBinaryMessage(arr);
            });
        }
        QTimer::singleShot(1000 * (satellites.size() + 1), this, [=] () {
            logit("sending data completed", log_tag, Logger::severity_level::normal);
            m_webSocket.close();
        });
    }
}

void Client::onDisconnected()
{
    logit("websocket disconnected", log_tag, Logger::severity_level::normal);
    if (m_debug)
        qDebug() << "websocket disconnected";
    this->closed();
}

