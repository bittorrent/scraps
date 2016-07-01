#pragma once

#include "scraps/config.h"

#include <iostream>

class NullStreambuf : public std::streambuf {
public:
    virtual int overflow(int c) override { return c; }
};

class NullOStream : private NullStreambuf, public std::ostream {
public:
    NullOStream() : std::ostream(this) {}
private:
    NullStreambuf _buf;
};
