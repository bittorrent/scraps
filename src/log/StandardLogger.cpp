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
#include <scraps/log/StandardLogger.h>

namespace scraps::log {

StandardLogger::StandardLogger(std::shared_ptr<FormatterInterface> formatter)
    : FormattedLogger{std::move(formatter)}
{
}

void StandardLogger::write(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock{_mutex};
#ifdef __ANDROID__
    auto f = stdout; // only stdout for android
#else
    auto f = level <= LogLevel::kInfo ? stdout : stderr;
#endif

    fprintf(f, "%s\n", message.c_str());
    fflush(f);
}

} // namespace scraps::log
