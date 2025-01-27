/*
    Copyright 2019 natinusala
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include <fmt/core.h>

#include <borealis/core/event.hpp>
#include <chrono>
#include <string>

namespace brls
{

enum class LogLevel
{
    LOG_ERROR = 0,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    LOG_VERBOSE
};

class Logger
{
  public:
    static void setLogLevel(LogLevel logLevel);

    template <typename... Args>
    inline static void log(LogLevel logLevel, std::string prefix, std::string color, fmt::format_string<Args...> format, Args&&... args)
    {
        if (Logger::logLevel < logLevel)
            return;

        auto now    = std::chrono::system_clock::now();
        uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
            - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
        time_t tt        = std::chrono::system_clock::to_time_t(now);
        auto time_tm     = localtime(&tt);
        char strTime[13] = { 0 };
        sprintf(strTime, "%02d:%02d:%02d.%03d", time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec, (int)ms);

        try
        {
            fmt::print("{}\033{}[{}]\033[0m ", strTime, color, prefix);
            fmt::print(format, std::forward<Args>(args)...);
            fmt::print("\n");

            std::string log = fmt::format(format, std::forward<Args>(args)...);
            logEvent.fire(log);
        }
        catch (const std::exception& e)
        {
            // will be printed after the first fmt::print (so after the log tag)
            printf("! Invalid log format string: \"%s\": %s\n", fmt::basic_string_view<char>(format).data(), e.what());
        }

#ifdef __MINGW32__
        fflush(0);
#endif
    }

    template <typename... Args>
    inline static void error(fmt::format_string<Args...> format, Args&&... args)
    {
        Logger::log(LogLevel::LOG_ERROR, "ERROR", "[0;31m", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void warning(fmt::format_string<Args...> format, Args&&... args)
    {
        Logger::log(LogLevel::LOG_WARNING, "WARNING", "[0;33m", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void info(fmt::format_string<Args...> format, Args&&... args)
    {
        Logger::log(LogLevel::LOG_INFO, "INFO", "[0;34m", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void debug(fmt::format_string<Args...> format, Args&&... args)
    {
        Logger::log(LogLevel::LOG_DEBUG, "DEBUG", "[0;32m", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void verbose(fmt::format_string<Args...> format, Args&&... args)
    {
        Logger::log(LogLevel::LOG_VERBOSE, "VERBOSE", "[0;37m", format, std::forward<Args>(args)...);
    }

    static Event<std::string>* getLogEvent()
    {
        return &logEvent;
    }

  private:
    inline static LogLevel logLevel = LogLevel::LOG_INFO;
    inline static Event<std::string> logEvent;
};

} // namespace brls
