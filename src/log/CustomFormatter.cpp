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
#include <scraps/log/CustomFormatter.h>

#include <iostream>

namespace scraps::log {

CustomFormatter::Format::Format() {}

CustomFormatter::Format::Format(std::string format)
    : _format{std::move(format)}
{
}

CustomFormatter::Format::Format(std::unordered_map<std::string, AppendElement> mapping)
    : _mapping{std::move(mapping)}
{
}

CustomFormatter::Format::Format(std::string format, std::unordered_map<std::string, AppendElement> mapping)
    : _format{std::move(format)}
    , _mapping{std::move(mapping)}
{}

std::vector<CustomFormatter::AppendElement> CustomFormatter::Format::functions() {
    std::vector<AppendElement> functions;

    std::string text;
    for (std::size_t pos = 0; ; ) {
        auto openBracket = _format.find('{', pos);
        if (openBracket == _format.npos) {
            text += _format.substr(pos); // suffix
            functions.emplace_back([text](std::string& buffer, const Message&) {
                buffer += text;
            });
            break;
        }

        // check for escaping "\\{"
        if (openBracket > 0 && _format[openBracket-1] == '\\') {
            text += _format.substr(pos, openBracket-pos-1);
            text += '{';
            pos = openBracket + 1;
            continue;
        }

        // append anything between elements
        text += _format.substr(pos, openBracket-pos);
        functions.emplace_back([text](std::string& buffer, const Message&) {
            buffer += text;
        });
        text.clear();

        auto closeBracket = _format.find('}', openBracket+1);
        if (closeBracket == std::string::npos) { // missing close bracket
            auto suffix = _format.substr(openBracket);
            functions.emplace_back([suffix](std::string& buffer, const Message&) {
                buffer += suffix;
            });
            break;
        }
        auto tag = _format.substr(openBracket+1, closeBracket-openBracket-1);
        std::string attributes;
        auto attributesStart = tag.find(':');
        if (attributesStart != tag.npos) {
            attributes = tag.substr(attributesStart+1);
            tag = tag.substr(0, attributesStart);
        }
        auto appendFunction = _mapping.find(tag);
        if (appendFunction != _mapping.end()) {
            if (!attributes.empty()) {
                auto function = FromAttributes(tag, attributes);
                if (function) {
                    functions.emplace_back(*function);
                } else {
                    std::cerr << "unknown attributes given for " << tag << std::endl;
                    functions.emplace_back(appendFunction->second);
                }
            } else {
                functions.emplace_back(appendFunction->second);
            }
        } else {
            std::cerr << "could not find " << tag << " mapping in custom format " << _format << std::endl;
        }

        pos = closeBracket + 1;
    }

    return functions;
}

CustomFormatter::Format& CustomFormatter::Format::set(const std::string& tag, AppendElement function) {
    _mapping[tag] = function;
    return *this;
}

CustomFormatter::Format& CustomFormatter::Format::set(const std::string& tag, const std::string& attributes) {
    auto function = FromAttributes(tag, attributes);
    if (function) {
        _mapping[tag] = *function;
    } else {
        std::cerr << "unknown attributes given for " << tag << std::endl;
    }
    return *this;
}

stdts::optional<CustomFormatter::AppendElement> CustomFormatter::Format::FromAttributes(const std::string& tag, const std::string& attributes) {
    if (tag == "time") {
        return stdts::optional<CustomFormatter::AppendElement>{AppendTime{attributes}};
    } else if (tag == "level") {
        return stdts::optional<CustomFormatter::AppendElement>{AppendLevel{attributes}};
    } else {
        return stdts::optional<CustomFormatter::AppendElement>{};
    }
}

std::unordered_map <std::string, CustomFormatter::AppendElement> CustomFormatter::Format::_DefaultMapping() {
    return {
        {"time",  AppendTime{}},
        {"level", AppendLevel{}},
        {"file",  AppendFile},
        {"line",  AppendLine},
        {"text",  AppendText},
        {"color", AppendStartColor{}},
        {"/color", AppendEndColor{}}
    };
}

CustomFormatter::CustomFormatter(std::vector<AppendElement> functions)
    : _functions{std::move(functions)}
{
}

CustomFormatter::CustomFormatter(std::string format)
    : CustomFormatter{Format{std::move(format)}.functions()}
{
}

std::string CustomFormatter::format(const Message& message) const {
    std::string buffer;
    // reserve a reasonable approximation of the buffer size
    buffer.reserve(sizeof("[0000-00-00 00:00:00.000]")+10+strlen(message.file)+10+message.text.size());

    for (auto& function : _functions) {
        function(buffer, message);
    }

    return buffer;
}

void CustomFormatter::AppendFile(std::string& buffer, const Message& message) {
    buffer += message.file;
}

void CustomFormatter::AppendLine(std::string& buffer, const Message& message) {
    buffer += std::to_string(message.line);
}

void CustomFormatter::AppendText(std::string& buffer, const Message& message) {
    buffer += message.text;
}

void CustomFormatter::AppendTime::operator()(std::string& buffer, const Message& message) const {
    auto d = message.time.time_since_epoch();
    auto timet = std::chrono::system_clock::to_time_t(message.time);
    auto size = buffer.size();
    buffer.resize(buffer.size()+maxFormatSize);
    auto bytes = strftime(&buffer[0]+size, maxFormatSize, format.c_str(), gmtime(&timet));
    buffer.erase(buffer.begin() + (size + bytes), buffer.end());
    if (subsecondPrecision != kSecond && subsecondSeparator) {
        buffer += subsecondSeparator;
    }

    switch (subsecondPrecision) {
    case kSecond:
        break;
    case kMillisecond: {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(d - std::chrono::duration_cast<std::chrono::seconds>(d));
        buffer += Formatf("%03d", milliseconds.count());
        break;
    }
    case kMicrosecond: {
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(d - std::chrono::duration_cast<std::chrono::seconds>(d));
        buffer += Formatf("%06d", microseconds.count());
        break;
    }
    }
}

CustomFormatter::AppendLevel::AppendLevel() {}

CustomFormatter::AppendLevel::AppendLevel(std::string debug, std::string info, std::string warning, std::string error)
    : labels({debug, info, warning, error})
{
}

CustomFormatter::AppendLevel::AppendLevel(const std::string& attributes) {
    std::size_t start = 0;
    auto end = attributes.find(',');
    for (auto& label : labels) {
        if (start >= attributes.size()) {
            break;
        }
        label = attributes.substr(start, end == std::string::npos ? end : end - start);
        start = end + 1;
        end = attributes.find(',', start);
    }
}

void CustomFormatter::AppendLevel::operator()(std::string& buffer, const Message& message) const {
    switch (message.level) {
    case Level::kDebug:
        buffer += labels[0];
        break;
    case Level::kInfo:
        buffer += labels[1];
        break;
    case Level::kWarning:
        buffer += labels[2];
        break;
    case Level::kError:
        buffer += labels[3];
        break;
    }
}

uint8_t CustomFormatter::Colors::index(Level level) const {
    switch (level) {
    case LogLevel::kDebug:
        return 0;
    case LogLevel::kInfo:
        return 1;
    case LogLevel::kWarning:
        return 2;
    case LogLevel::kError:
        return 3;
    }
}

void CustomFormatter::AppendStartColor::operator()(std::string& buffer, const Message& message) const {
    auto index = colors.index(message.level);
    if (colors.background[index] != Color::kDefault) {
        buffer += Formatf("\033[%dm", (40 + colors.background[index]));
    }
    if (colors.foreground[index] != Color::kDefault) {
        buffer += Formatf("\033[%dm", (30 + colors.foreground[index]));
    }
}

void CustomFormatter::AppendEndColor::operator()(std::string& buffer, const Message& message) const {
    auto index = colors.index(message.level);
    if (colors.background[index] != Color::kDefault) {
        buffer += Formatf("\033[%dm", (40 + Color::kDefault));
    }
    if (colors.foreground[index] != Color::kDefault) {
        buffer += Formatf("\033[%dm", (30 + Color::kDefault));
    }
}

} // namespace scraps::log
