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

#include <scraps/log/AndroidLogger.h>
#include <scraps/log/AsyncLogger.h>
#include <scraps/log/DogStatsDLogger.h>
#include <scraps/log/FileLogger.h>
#include <scraps/log/FilterLogger.h>
#include <scraps/log/LogCounter.h>
#include <scraps/log/LoggerLogger.h>
#include <scraps/log/RateLimitedLogger.h>
#include <scraps/log/StandardLogger.h>

namespace scraps {

#if SCRAPS_ANDROID
using AndroidLogger     = log::AndroidLogger;
#endif
using AsyncLogger       = log::AsyncLogger;
using DogStatsDLogger   = log::DogStatsDLogger;
using FileLogger        = log::FileLogger;
using FilterLogger      = log::FilterLogger;
using LogCounter        = log::LogCounter;
using LoggerLogger      = log::LoggerLogger;
using RateLimitedLogger = log::RateLimitedLogger;
using StandardLogger    = log::StandardLogger;

} // namespace scraps
