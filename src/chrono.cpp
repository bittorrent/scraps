#include "scraps/chrono.h"

namespace scraps {

timeval ToTimeval(const std::chrono::microseconds& value) {
    struct timeval result;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(value);
    std::chrono::microseconds microseconds = value;
    microseconds -= seconds;
    result.tv_sec = seconds.count();
    result.tv_usec = microseconds.count();

    return result;
}

} // namespace scraps
