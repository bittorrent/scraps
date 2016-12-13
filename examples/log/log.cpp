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

/**
* e.g.:
*   ./scraps-log "[{time:%F %T}] {level:DEBUG,INFO,WARNING,ERROR} {file}:{line} {text}"
*   ./scraps-log "{level} {file}:{line} {text}"
*   ./scraps-log "{time:%T} {level:E,I,W,D} {file}:{line} {text}"
*   ./scraps-log "{time:%T} {color}{level:E,I,W,D}{/color} {file}:{line} {text}"
*   ./scraps-log "{color}{level}{/color} {file}:{line} {color}{text}{/color}"
*   ./scraps-log "{level:📘,📗,📒,📕} {file}:{line} {color}{text}{/color}"
*/

#include <scraps/logging.h>

#include <scraps/log/StandardLogger.h>
#include <scraps/log/CustomFormatter.h>

int main(int argc, const char* argv[]) {
    std::string format;
    if (argc > 1) {
        format = argv[1];
    }

    auto formatter = !format.empty() ? std::make_shared<scraps::log::CustomFormatter>(format) : nullptr;
    auto logger = formatter ?
        std::make_shared<scraps::log::StandardLogger>(formatter) :
        std::make_shared<scraps::log::StandardLogger>()
    ;

    scraps::log::SetLogger(logger);
    scraps::log::SetLogLevel(scraps::log::Level::kDebug);

    SCRAPS_LOG_INFO("Uh, everything's under control.");
    SCRAPS_LOG_INFO("Situation normal.");
    SCRAPS_LOG_WARNING("What happened?");
    SCRAPS_LOG_DEBUG("Uh, we had a slight weapons malfunction, but uh... everything's perfectly alright now.");
    SCRAPS_LOG_DEBUG("We're fine.");
    SCRAPS_LOG_DEBUG("We're all fine here now, thank you.");
    SCRAPS_LOG_DEBUG("How are you?");
    SCRAPS_LOG_WARNING("We're sending a squad up.");
    SCRAPS_LOG_INFO("Uh, uh... negative, negative.");
    SCRAPS_LOG_INFO("We had a reactor leak here now.");
    SCRAPS_LOG_INFO("Give us a few minutes to lock it down.");
    SCRAPS_LOG_INFO("Large leak, very dangerous.");
    SCRAPS_LOG_WARNING("Who is this?");
    SCRAPS_LOG_WARNING("What's your operating number?");
    SCRAPS_LOG_INFO("Uh..");
    SCRAPS_LOG_INFO("Boring conversation anyway.");
    SCRAPS_LOG_ERROR("Luke, we're gonna have company!");

    return 0;
}
