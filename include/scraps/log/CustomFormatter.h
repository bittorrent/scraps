/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <scraps/config.h>

#include <scraps/log/FormatterInterface.h>

#include <stdts/optional.h>

#include <array>
#include <unordered_map>

namespace scraps::log {

/**
* CustomFormatter class for easily customizing the format of a log message.
*/
class CustomFormatter : public FormatterInterface {
public:
    using AppendElement = std::function<void(std::string&, const Message&)>;

    /**
    *   Helper class to construct a list of append functions.
    *
    *   e.g.:
    *       // [0000-00-00 00:00:00.000] INFO file.c:1 text
    *       Format standardFormat{"[{time:%F %T}] {level:DEBUG,INFO,WARNING,ERROR} {file}:{line} {text}"};
    *
    *       // [0000-00-00 00:00:00.000] INFO file.c text
    *       Format noLineNumber{"[{time}] {level} {file} {text}"};
    *
    *       // INFO file.c:1 text
    *       Format noTime{"{level} {file}:{line} {text}"};
    *
    *       // [0000-00-00 00:00:00.000] I file.c:1 text
    *       auto customLevel = Format{}.set("level", "D,I,W,E");
    *
    *       // 00:00:00.000 I text
    *       Format simpleFormat{"{time:%T} {level:D,I,W,E} {text}"};
    */
    struct Format {
        Format();
        Format(std::string format);
        Format(std::unordered_map<std::string, AppendElement> mapping);
        Format(std::string format, std::unordered_map<std::string, AppendElement> mapping);

        /**
        * Convenience function to override a function.
        */
        Format& set(const std::string& tag, AppendElement function);

        /**
        * Convenience function to override a function if it supports attributes.
        */
        Format& set(const std::string& tag, const std::string& attributes);

        /**
        * Factory for creating AppendElement with attributes.
        */
        static stdts::optional<AppendElement> FromAttributes(const std::string& tag, const std::string& attributes);

        /**
        * Returns a list of append functions by combining the format string
        * and the corresponding mapping.
        */
        std::vector<AppendElement> functions();

    private:
        static std::unordered_map <std::string, AppendElement> _DefaultMapping();

        std::string                                    _format  = "[{time}] {level} {file}:{line} {text}";
        std::unordered_map<std::string, AppendElement> _mapping = _DefaultMapping();
    };

    CustomFormatter(std::vector<AppendElement> functions = Format().functions());
    CustomFormatter(std::string format);

    /**
    * Returns a formatted string ready for logging.
    */
    virtual std::string format(const Message& message) const override;

    /**
    * Default function to append a file.
    */
    static void AppendFile(std::string& buffer, const Message& message);

    /**
    * Default function to append a line number.
    */
    static void AppendLine(std::string& buffer, const Message& message);

    /**
    * Default function to append the log text.
    */
    static void AppendText(std::string& buffer, const Message& message);

    struct AppendTime {
        enum SubsecondPrecision : uint8_t {
            kSecond,
            kMillisecond,
            kMicrosecond,
        };

        std::string format = "%F %T";
        std::size_t maxFormatSize = sizeof("0000-00-00 00:00:00");
        SubsecondPrecision subsecondPrecision = SubsecondPrecision::kMillisecond;
        char subsecondSeparator = '.';

        void operator()(std::string& buffer, const Message& message) const;
    };

    struct AppendLevel {
        std::array<std::string, 4> labels = {"DEBUG", "INFO", "WARNING", "ERROR"};

        AppendLevel();
        AppendLevel(std::string debug, std::string info, std::string warning, std::string error);
        AppendLevel(const std::string& attributes);

        void operator()(std::string& buffer, const Message& message) const;
    };

    enum Color : uint8_t {
        kDefault = 9,
        kBlack   = 0,
        kRed     = 1,
        kGreen   = 2,
        kYellow  = 3,
        kBlue    = 4,
        kMagenta = 5,
        kCyan    = 6,
        kWhite   = 7,
    };

    struct Colors {
        std::array<Color, 4> foreground = {Color::kCyan, Color::kDefault, Color::kYellow, Color::kRed};
        std::array<Color, 4> background = {Color::kDefault, Color::kDefault, Color::kDefault, Color::kDefault};

        uint8_t index(Level level) const;
    };

    struct AppendStartColor {
        Colors colors;

        void operator()(std::string& buffer, const Message& message) const;
    };

    struct AppendEndColor {
        Colors colors;

        void operator()(std::string& buffer, const Message& message) const;
    };

private:
    const std::vector<AppendElement> _functions;
};

} // namespace scraps::log
