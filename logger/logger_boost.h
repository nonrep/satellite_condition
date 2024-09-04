#pragma once
#define BOOST_LOG_DYN_LINK 1

#ifndef LOGGER_BOOST_H
#define LOGGER_BOOST_H

#include <iostream>

#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#include <boost/filesystem/operations.hpp>

#include <stdio.h>
#include <iomanip>
#include <ctime>
#include <time.h>
#include <list>
#include <string>
#include <map>
#include <unistd.h>
#include <string>
#include <fstream>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <cstdint>

using namespace std;

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

namespace AttributeKeyWord {

BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "TagString", std::string);
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string);
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int);

}

class Logger {

public:
    enum severity_level
    {
        debug,
        trace,
        normal,
        notification,
        warning,
        error,
        critical
    };

    static std::weak_ptr<Logger> get()
    {
        static auto logger = std::shared_ptr<Logger>(new Logger);
        return logger;
    };

    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;

    Logger& operator=(const Logger &) = delete;
    Logger& operator=(Logger &&) = delete;

    inline void setup_logger(const std::string& path, const std::map<std::string, std::string> conf_map =
                                                      {{"format", "%Message%"},
                                                      {"rotation_size", "100"},
                                                      {"max_files", "100"}})
    {
        path_log = path;
        good_path = directoryExists(path) && fileRules(path);

        if (conf_map.empty()) {
            map = default_map;
        } else {
            map = conf_map;
        }
    }

    inline void logit(std::string message, std::string tag, severity_level level)
    {
        if (good_path) {
            add_missing_tag(tag);
            logger_type lg;
            src::severity_logger< severity_level > slg;
            slg.add_attribute("TagString", attrs::constant< std::string >("initialization string"));

            std::string severity_key = getLevel(level);
            BOOST_LOG_CHANNEL_SEV (lg, tag, level) << logging::add_value(AttributeKeyWord::tag_attr, severity_key) << message;
        } else {
            cout << "Logit: " << tag << ": " << message << "\n";
        }
    }

    inline void logit(std::string message, std::string tag)
    {
        logit(message, tag, severity_level::debug);
    }

private:
    std::list<std::string> our_tags;
    std::string path_log = "";
    bool good_path = false;
    std::map<std::string, std::string> map;
    std::map<std::string, std::string> default_map = {{"format", "[%TimeStamp%][%Channel%][%Severity%][%TagString%]:  %Message%"},{"rotation_size", "100"},{"max_files", "200"}};
    typedef src::severity_channel_logger< severity_level, std::string > logger_type;

    inline std::string getLevel(int level)
    {
        std::string outLevel;

        switch (level) {
        case 0:
            outLevel = "debug";
            break;
        case 1:
            outLevel = "trace";
            break;
        case 2:
            outLevel = "normal";
            break;
        case 3:
            outLevel = "notification";
            break;
        case 4:
            outLevel = "warning";
            break;
        case 5:
            outLevel = "error";
            break;
        case 6:
            outLevel = "critical";
            break;
        default:
            outLevel = "debug";
            break;
        }

        return outLevel;
    }

    inline bool fileRules(std::string path)
    {
        const char* pzPath = path.c_str();
        int rval;
        /* Проверка права доступа. */
        rval = access(pzPath, R_OK);
        if (rval != 0) printf("%s is not readable (access denied) \n", pzPath);

        /* проверка права записи. */
        rval = access(pzPath, W_OK);
        if (rval == 0) {
            return true; // доступен для записи
        } else if (errno == EACCES) {
            cout << pzPath << " is not writable (access denied) \n";
            return false;
        } else if (errno == EROFS) {
            cout << pzPath << " is not writable (read-only filesystem) \n";
            return false;
        } else {
           cout << "Unknown error " << errno << "\n";
           return false;
        }
    }

    inline bool directoryExists(std::string path)
    {
        if (boost::filesystem::exists(path)) {
            return true;
        } else {
            cout << "Directory " << path << " does not exist \n";
            return false;
        }
    }

    inline void add_missing_tag(std::string tag)
    {
        for(const std::string &it: our_tags)
        {
            if (it == tag){
                return;
            }
        }

        boost::log::register_simple_formatter_factory< severity_level, char >("Severity");

        our_tags.push_back(tag);
        const std::string format(map["format"]);

        try {
            boost::log::add_file_log(
                        keywords::target = path_log + "/backUp",
                        keywords::target_file_name = tag + "_%N.log",
                        keywords::enable_final_rotation = false,
                        keywords::file_name = path_log + "/" + tag + ".log",
                        keywords::rotation_size = std::stoi(map["rotation_size"]) * 1024 * 1024, // rotation size, in characters// work in bytes
                        keywords::filter = (AttributeKeyWord::channel == tag),
                        keywords::format = format,
                        keywords::open_mode = (std::ios::out | std::ios::app | std::ios::ate),//std::ios_base::ate,  //::app,
                        keywords::max_files = std::stoi(map["max_files"]),
                        keywords::auto_flush = true
                        );

            logging::add_common_attributes();
        }
        catch (const std::exception& ex) {
            cout << "Error while adding log file: " << ex.what() << "\n";
        }
    }

protected:
    Logger() {};

};

#endif
