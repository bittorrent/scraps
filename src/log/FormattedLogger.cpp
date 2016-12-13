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
#include <scraps/log/FormattedLogger.h>

#include <scraps/log/StandardFormatter.h>

namespace scraps::log {

FormattedLogger::FormattedLogger()
    : FormattedLogger{std::make_shared<StandardFormatter>()}
{
}

FormattedLogger::FormattedLogger(std::shared_ptr<FormatterInterface> formatter)
    : _formatter{formatter ? std::move(formatter) : std::make_shared<StandardFormatter>()}
{
}

void FormattedLogger::log(Message message) {
    auto formattedMessage = _formatter->format(message);
    write(message.level, formattedMessage);
}

} // namespace scraps::log
