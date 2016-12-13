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
#include <scraps/log/DogStatsDLogger.h>

namespace scraps::log {

DogStatsDLogger::DogStatsDLogger(net::Endpoint endpoint)
    : _endpoint{std::move(endpoint)}
    , _socket{std::make_unique<net::UDPSocket>(net::UDPSocket::Protocol::kIPv4)}
{}

void DogStatsDLogger::log(Message message) {
    std::lock_guard<std::mutex> lock{_mutex};

    auto text = Format("{} ({}:{})", message.text, message.file, message.line);
    auto data = Format("_e{{{},{}}}:{}|{}|t:{}\n", message.text.size(), text.size(), message.text, text,
        message.level == LogLevel::kError ? "error" : (message.level == LogLevel::kWarning ? "warning" : "info")
    );

    _socket->send(_endpoint, data.data(), data.size());
}

} // namespace scraps::log
