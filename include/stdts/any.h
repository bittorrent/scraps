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

// cppcheck-suppress preprocessorErrorDirective
#if __has_include(<any>)
    #include <any>
    namespace stdts {
        using std::any;
        using std::any_cast;
        using std::bad_any_cast;
        using std::swap;
    } // namespace stdts
// cppcheck-suppress preprocessorErrorDirective
#elif __has_include(<experimental/any>)
    #include <experimental/any>
    namespace stdts {
        using std::experimental::any;
        using std::experimental::any_cast;
        using std::experimental::bad_any_cast;
        using std::experimental::swap;
    } // namespace stdts
#else
    #include <core/any.hpp>
    namespace stdts {
        using core::any;
        using core::any_cast;
        using core::bad_any_cast;
        using core::swap;
    } // namespace stdts
#endif
