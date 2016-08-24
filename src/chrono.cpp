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
#include "scraps/chrono.h"

namespace scraps {

timeval ToTimeval(const std::chrono::microseconds& value) {
    struct timeval result;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(value);
    std::chrono::microseconds microseconds = value;
    microseconds -= seconds;
    result.tv_sec = seconds.count();
    result.tv_usec = microseconds.count();

    return result;
}

} // namespace scraps
