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

#if !SCRAPS_DISABLE_IGNORE_WARNINGS && defined(__clang__)
    #define SCRAPS_IGNORE_WARNINGS_PUSH                                   \
        _Pragma("clang diagnostic push")                                  \
        _Pragma("clang diagnostic ignored \"-Wsign-compare\"")            \
        _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")         \
        _Pragma("clang diagnostic ignored \"-Wunused-local-typedef\"")    \
        _Pragma("clang diagnostic ignored \"-Wconversion\"")              \
        _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"") \

    #define SCRAPS_IGNORE_WARNINGS_POP                                    \
        _Pragma("clang diagnostic pop")

#else
    #define SCRAPS_IGNORE_WARNINGS_PUSH
    #define SCRAPS_IGNORE_WARNINGS_POP
#endif
