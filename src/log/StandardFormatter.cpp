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
#include <scraps/log/StandardFormatter.h>

namespace scraps::log {

std::string StandardFormatter::format(const Message& message) const {
    char buffer[sizeof("0000-00-00 00:00:00")] = {};
    auto d = message.time.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(d - std::chrono::duration_cast<std::chrono::seconds>(d));
    auto timet = std::chrono::system_clock::to_time_t(message.time);
    strftime(buffer, sizeof(buffer), "%F %T", gmtime(&timet));

    return Formatf(
        "[%s.%03d] %s %s:%u %s",
        buffer,
        milliseconds.count(),
        LevelString(message.level),
        message.file,
        message.line,
        message.text);
}

} // namespace scraps::log
