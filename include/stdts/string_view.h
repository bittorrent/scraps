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

#if __has_include(<string_view>)
    #include <string_view>
    namespace stdts {
        using std::basic_string_view;
        using std::string_view;
        using std::u16string_view;
        using std::u32string_view;
        using std::wstring_view;
    } // namespace stdts
#elif __has_include(<experimental/string_view>)
    #include <experimental/string_view>
    namespace stdts {
        using std::experimental::basic_string_view;
        using std::experimental::string_view;
        using std::experimental::u16string_view;
        using std::experimental::u32string_view;
        using std::experimental::wstring_view;
    } // namespace stdts
#else
    #include <core/string_view.hpp>
    namespace stdts {
        using core::basic_string_view;
        using core::string_view;
        using core::u16string_view;
        using core::u32string_view;
        using core::wstring_view;
    } // namespace stdts
#endif
