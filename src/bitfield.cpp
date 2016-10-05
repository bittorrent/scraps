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
#include "scraps/bitfield.h"

namespace scraps {

BitIterator BitIterator::Begin(const void* target, size_t len) {
    BitIterator ret;
    ret.target    = (const unsigned char*)target;
    ret.len       = len;
    ret.character = 0;
    ret.mask      = 0x80;
    ret.value     = ret.target[0] & ret.mask;
    return ret;
}

BitIterator BitIterator::End(const void* target, size_t len) {
    BitIterator ret;
    ret.target    = (const unsigned char*)target;
    ret.len       = len;
    ret.character = len;
    ret.mask      = 0x80;
    ret.value     = false;
    return ret;
}

const bool& BitIterator::operator*() const { return value; }

BitIterator BitIterator::operator++() {
    if (character >= len) {
        return *this;
    } // already at the end

    mask >>= 1;
    if (!mask) {
        ++character;
        mask = 0x80;
    }

    if (character >= len) {
        // reached the end
        return *this;
    }

    value = target[character] & mask;

    return *this;
}

BitIterator BitIterator::operator++(int) {
    BitIterator ret = *this;
    ++(*this);
    return ret;
}

BitIterator BitIterator::operator--() {
    if (mask == 0x80) {
        if (!character) {
            // already at the beginning
            return *this;
        }
        mask = 1;
        --character;
    } else {
        mask <<= 1;
    }

    value = target[character] & mask;

    return *this;
}

BitIterator BitIterator::operator--(int) {
    BitIterator ret = *this;
    --(*this);
    return ret;
}

bool BitIterator::operator==(const BitIterator& other) {
    return target == other.target && len == other.len && character == other.character && mask == other.mask;
}

bool BitIterator::operator!=(const BitIterator& other) { return !(*this == other); }

std::vector<bool> BitfieldDecode(const void* encoded, size_t len, size_t maxLength) {
    auto begin = BitIterator::Begin(encoded, len);
    auto end   = BitIterator::End(encoded, len);

    // BitfieldEncode pads with 1's, so start by stripping them
    auto one = end;
    while (one != begin && *--one) {
        --end;
    }

    if (begin == end) {
        return {};
    }

    auto it           = begin;
    bool countedValue = *(it++);

    std::vector<bool> ret;

    while (it != end) {
        size_t count = EliasOmegaDecode<size_t>(it, end, &it);
        if (!count || count > std::min(ret.max_size(), maxLength) - ret.size()) {
            return {};
        }
        ret.reserve(ret.size() + count);
        for (size_t i = 0; i < count; ++i) {
            ret.push_back(countedValue);
        }
        countedValue = !countedValue;
    }

    return ret;
}

} // namespace scraps
