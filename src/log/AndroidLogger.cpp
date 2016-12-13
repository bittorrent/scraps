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
#include <scraps/log/AndroidLogger.h>

#if SCRAPS_ANDROID
#include <android/log.h>

namespace scraps::log {

AndroidLogger::AndroidLogger(std::shared_ptr<FormatterInterface> formatter)
    : FormattedLogger{std::move(formatter)}
{
}

void AndroidLogger::write(LogLevel level, const std::string& message) {
    __android_log_print(level == LogLevel::kInfo ? ANDROID_LOG_INFO : ANDROID_LOG_ERROR, "live", "%s", message.c_str());
}

} // namespace scraps::log

#endif // SCRAPS_ANDROID
