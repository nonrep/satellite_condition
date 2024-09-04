#ifndef LOGGER_BOOST_QTWRAP_H
#define LOGGER_BOOST_QTWRAP_H

#include "logger_boost.h"
#include <QString>
#include <QList>
#include <QStringList>
#include <QVariantMap>

inline void setup_logger(const QString& path, const QVariantMap map =
                            {{"format", "%Message%"},
                            {"rotation_size", "100"},
                            {"max_files", "100"}})
{
    std::map<std::string, std::string> conf_map;
    for (auto it = map.constBegin(); it != map.constEnd();  ++it ) {
        conf_map.insert(make_pair(it.key().toStdString(), it.value().toString().toStdString()));
    }

    Logger::get().lock()->setup_logger(path.toStdString(), conf_map);
}

inline void logit(QString message, QString tag, int level = 0)
{
    Logger::get().lock()->logit(message.toStdString(), tag.toStdString(), Logger::severity_level(level));
}

#endif
