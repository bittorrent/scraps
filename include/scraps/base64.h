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

#include <stdts/string_view.h>

namespace scraps {

/**
* Returns a string decoded from base64
*
* @param a pointer to the data to be decoded
* @param the length of the data to be decoded
* @return the decoded data stored in a std::string
*
*/
std::string Base64Decode(stdts::string_view data);

/**
* Returns a string encoded as base64
*
* @param a pointer to the data to be encoded
* @param the length of the data to be encoded
* @return the encoded data stored in a std::string
*
*/
std::string Base64Encode(stdts::string_view data);

} // namespace scraps
