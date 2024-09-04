#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QSettings>
#include <QCoreApplication>

#include <mutex>

#include "../configmanager/jsonagent.h"

class ConfigurationManager
{
public:
    static std::shared_ptr<ConfigurationManager> instance()
    {
        std::lock_guard<std::mutex> lock(m_cfgMutex);
        if (m_cfgInstance == nullptr) {
            m_cfgInstance = std::shared_ptr<ConfigurationManager>(new ConfigurationManager);
        }
        return m_cfgInstance;
    }

    ~ConfigurationManager() = default;

    ConfigurationManager(const ConfigurationManager &) = delete;
    ConfigurationManager& operator=(const ConfigurationManager &) = delete;

    /**
     * \brief Получение полного пути к файлу по ключу
     *
     * \param path - имя пользовательского файла
     * \return - путь к файлу
     */
    QString getPath(const QString &path) const
    {
        return getFilePath(path);
    }

    /**
     * \brief Получение полного пути к файлу по ключу
     *
     * \param path - имя пользовательского файла
     * \return - путь к файлу
     */
    QString getFilePath(const QString &name) const
    {
        return m_paths.value(name);
    }

    /**
     * \brief Получение содержимого файла sql.json
     *
     * \return - контейнер с содержимым файла
     */
    QVariantMap getRequests() const
    {
        return getMap("requests");
    }

    /**
     * \brief Получение по имени файла содержимого в формате QVariantMap
     *
     * \param fileName - имя пользовательского файла
     * \return - контейнер с содержимым файла
     */
    QVariantMap getMapFromFile(const QString &fileName) const
    {
        if (fileName.isEmpty())
            return QVariantMap();
        return JsonAgent::read(getDefaultPath() + fileName);
    }

    /**
     * \brief Получение по ключу из контейнера с путями m_paths содержимого в формате QVariantMap
     *
     * \param key - ключ из m_paths
     * \return - контейнер с содержимым файла
     */
    QVariantMap getMap(const QString &key) const
    {
        if (m_paths.isEmpty() || key.isEmpty())
            return QVariantMap();
        const QString &fileName = m_paths.value(key);
        return JsonAgent::read(fileName);
    }

    /**
     * \brief Получение содержимого конфига config.json
     *
     * \return - контейнер с содержимым файла
     */
    QVariantMap getConfig() const
    {
        return getMap("config");
    }

    /**
     * \brief Получение содержимого конфига conf_log.json
     *
     * \return - контейнер с содержимым файла
     */
    QVariantMap getConfLog() const
    {
        return getMap("conf_log");
    }

    /**
     * \brief Получение пути к каталогу log
     *
     * \return - путь к log
     */
    QString getLogDir() const
    {
        QString loggerPath = m_paths.value("log");
        if (!QDir(loggerPath).exists())
            QDir().mkdir(loggerPath);
        return loggerPath;
    }

    /**
     * \brief Получение параметров баз данных из файла настроек
     *
     * \return - параметры бд
     */
    QVariantMap getDatabases() const
    {
        QSettings settings(m_paths.value("settings"), QSettings::NativeFormat);
        if (settings.childGroups().isEmpty())
            return QVariantMap();
        QVariantMap databases;
        settings.beginGroup("databases");
        const QStringList groups = settings.childGroups();

        if (groups.isEmpty()) {
            QVariantMap dataBase;
            const auto allKeys = settings.allKeys();
            for (const QString &name : allKeys)
                dataBase.insert(name, settings.value(name, ""));
            databases.insert("defaultGroup", dataBase);
        } else {
            for (const QString &group : groups) {
                QVariantMap dataBase;
                settings.beginReadArray(group);
                const auto allKeys = settings.allKeys();
                for (const QString &name : allKeys)
                    dataBase.insert(name, settings.value(name, ""));
                settings.endArray();
                databases.insert(group, dataBase);
            }
        }
        settings.endGroup();
        return databases;
    }

    /**
     * \brief Получение свойства из настроек
     *
     * \param grName - группа
     * \param vlName - название свойства
     * \return - значение свойства
     */
    QString getProperty(const QString &grName, const QString &vlName) const
    {
        QSettings settings(m_paths.value("settings"), QSettings::NativeFormat);
        settings.beginGroup(grName);
        return settings.value(vlName, "").toString();
    }

    /**
     * \brief Установка свойства настройках
     *
     * \param grName - группа
     * \param vlName - название свойства
     * \param value - значение свойства
     */
    void setProperty(const QString &grName, const QString &vlName, const QVariant &value) const
    {
        QSettings settings(m_paths.value("settings"), QSettings::NativeFormat);
        settings.beginGroup(grName);
        settings.setValue(vlName, QVariant::fromValue(value));
        settings.endGroup();
    }

    /**
     * \brief чтение файла в массив символов
     *
     * \param filePath - путь к файлу
     * \return - массив символов
     */
    static QByteArray readFile(const QString &filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return QByteArray();
        }
        QByteArray blob = file.readAll();
        file.close();
        return blob;
    }

    /**
     * \brief Возвращает путь к папке конфигурации заданный по умолчанию
     *
     * \return - путь к папке
     */
    const QString &getDefaultPath() const
    {
        return m_defaultPath;
    }
    
    /**
     * \brief Получение контейнера из файла
     *
     * \param fileName - название файла
     * \return - содержимое файла в контейнере QVariantMap
     */
    QVariantMap getDataFromJson(const QString &fileName) const
    {
        return JsonAgent::read(QString("%1%2").arg(m_defaultPath, fileName));
    }

private:
    static std::shared_ptr<ConfigurationManager> m_cfgInstance;
    static std::mutex m_cfgMutex;

    QString m_defaultPath;
    QMap<QString, QString> m_paths;

    void setupManager()
    {
        QString path = QDir::homePath() + "/." + QFileInfo(QCoreApplication::applicationFilePath()).fileName();
        //Определение в .pro-файле проекта переменной CONFIG_PATH поменяет значение path
#ifdef CONFIG_PATH
        path = CONFIG_PATH;
#endif

        if (!QDir(path).exists())
            QDir().mkpath(path);
        m_defaultPath = path + "/";

        m_paths.insert("protobuf", m_defaultPath + "config.probuf");
        m_paths.insert("requests", m_defaultPath + "sql.json");
        m_paths.insert("settings", m_defaultPath + "settings.config");
        m_paths.insert("config", m_defaultPath + "config.json");
        m_paths.insert("conf_log", m_defaultPath + "conf_log.json");
        QSettings settings(m_paths.value("settings"), QSettings::NativeFormat);

        settings.beginGroup("trackwriter_path");
        m_paths.insert("trackwriter", settings.value("path", "").toString());

        m_paths.insert("log", m_defaultPath + "log/");

        m_paths.insert("tracks", m_defaultPath +".trackwriter/" + "track_points");
        m_paths.insert("xrandr", m_defaultPath + "xrandr.sh");
        m_paths.insert("refresh", m_defaultPath + "refresh.sh");
        m_paths.insert("mcc",getProperty("mcc_configuration","url"));
    }

    ConfigurationManager()
    {
        setupManager();
    };

protected:
};

inline std::shared_ptr<ConfigurationManager> configManager()
{
    return ConfigurationManager::instance();
}

#endif
