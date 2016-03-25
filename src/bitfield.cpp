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

bool BitIterator::operator==(const BitIterator& other) {
    return target == other.target && len == other.len && character == other.character && mask == other.mask;
}

bool BitIterator::operator!=(const BitIterator& other) { return !(*this == other); }

std::vector<bool> BitfieldDecode(const void* encoded, size_t len) {
    auto begin = BitIterator::Begin(encoded, len);
    auto end   = BitIterator::End(encoded, len);

    std::vector<bool> ret;

    if (begin == end) {
        return ret;
    }

    auto it           = begin;
    bool countedValue = *(it++);

    while (it != end) {
        size_t count = EliasOmegaDecode<size_t>(it, end, &it);
        if (!count) {
            return ret;
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
