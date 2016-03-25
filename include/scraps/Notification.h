#pragma once

#include "scraps/config.h"

#include <tuple>

namespace scraps {

class AbstractNotification {
protected:
    virtual ~AbstractNotification() = default;
};

namespace detail {
/**
 * Helper class that is used to help tell when a type is an
 * ArgumentNotification type.
 */
class ArgumentNotificationBase : public AbstractNotification {};

} // namespace detail

/**
 * Notification that takes any given numer of parameters as that are passed
 * to an observer.
 */
template <typename... Args>
class ArgumentNotification : public detail::ArgumentNotificationBase {
public:
    using Tuple = std::tuple<Args...>;

    ArgumentNotification(Args... args) : data{std::forward<Args>(args)...} {}

    ArgumentNotification(const Tuple& data) : data{data} {}
    ArgumentNotification(Tuple&& data) : data{std::move(data)} {}

    ArgumentNotification& operator=(const Tuple& data) {
        this->data = data;
        return *this;
    }

    ArgumentNotification& operator=(Tuple&& data) {
        this->data = std::move(data);
        return *this;
    }

    virtual ~ArgumentNotification() = default;

    Tuple data;
};

} // namespace scraps
