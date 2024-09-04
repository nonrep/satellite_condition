#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QPair>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QTimer>

#include "logger/logger_boost_qtwrap.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(const QUrl &url, bool debug = false, QObject *parent = nullptr);

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void onBinaryMessageReceived(QByteArray message);

private:
    QWebSocket m_webSocket;
    bool m_debug;
    QString log_tag = "KA_states";
    QByteArray m_arr;
};

#endif // CLIENT_H
