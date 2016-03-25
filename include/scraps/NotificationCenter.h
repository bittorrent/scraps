#pragma once

#include "scraps/config.h"

#include "scraps/Observer.h"

#include <unordered_map>

namespace scraps {

template <typename Function>
struct FunctionArgumentTypeSolver {
    using type = void;
};

template <typename Class, typename Return, typename T>
struct FunctionArgumentTypeSolver<Return(Class::*)(T) const> {
    using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template <typename Function>
struct FunctionArgumentType {
    using type = typename FunctionArgumentTypeSolver<decltype(&Function::operator())>::type;
};

/**
 * Thread-safe.
 *
 * Inspired by NSNotificationCenter.
 */
class NotificationCenter {
public:
    struct Subscriber {
        template <typename NotificationType = void, typename Function>
        Subscriber& add(Function&& func);

    private:
        friend class NotificationCenter;
        Subscriber(NotificationCenter* self, ObserverList* observers) : self{self}, observers{observers} {}

        NotificationCenter* self;
        ObserverList* observers;
    };

    NotificationCenter() = default;

    /**
     * Creates a new observer, adds it to the NotificationCenter, and then
     * returns the observer.
     *
     * The notification center keeps a weak reference to the observer, so
     * a copy of it MUST be preserved.
     */
    template <typename NotificationType>
    typename std::enable_if<!std::is_base_of<detail::ArgumentNotificationBase, NotificationType>::value,
        std::shared_ptr<ApatheticObserver<NotificationType>>
    >::type addObserver(std::function<void()>&& func);

    template <typename NotificationType, typename Function>
    typename std::enable_if<std::is_same<NotificationType, void>::value,
        std::shared_ptr<NotificationObserver<typename FunctionArgumentType<Function>::type>>
    >::type addObserver(Function&& func);

    /**
     * Creates a new observer, adds it to the NotificationCenter, and then
     * returns the observer.
     *
     * The notification center keeps a weak reference to the observer, so
     * a copy of it MUST be preserved.
     */
    template <typename NotificationType, typename Function>
    typename std::enable_if<std::is_base_of<detail::ArgumentNotificationBase, NotificationType>::value,
        std::shared_ptr<ArgumentObserver<NotificationType, Function>>
    >::type addObserver(Function&& func);

    /**
     * Convenience method for initializing multiple observers.
     */
    Subscriber addObservers(ObserverList& observers);

    /**
     * Returns true if an observer exists for the given notification.
     */
    template <typename NotificationType>
    bool hasObserverForNotification() const;

    /**
     * Posts the given notification to the registered obsever(s);
     */
    void postNotification(const std::shared_ptr<AbstractNotification>& notification) const;

    /**
     * Creates a notification and posts it to the registered obsever(s).
     *
     * Returns the new notification.
     */
    template <typename NotificationType, typename... Args>
    std::shared_ptr<NotificationType> postNotification(Args&&... args) const;

    /**
     * Returns true if there are no observers.
     */
    bool empty() const;

    /**
     * Returns a count of the number of observers.
     */
    std::size_t size() const;

private:
    NotificationCenter(const NotificationCenter&) = delete;
    NotificationCenter& operator=(const NotificationCenter&) = delete;
    NotificationCenter(NotificationCenter&&) = delete;
    NotificationCenter& operator=(NotificationCenter&&) = delete;

    using Multimap = std::unordered_multimap<size_t, std::weak_ptr<AbstractObserver>>;
    struct Deleter {
        Deleter(NotificationCenter* self) : self{self} {}

        void operator()(AbstractObserver* observer) {
            self->_removeObserver(it);
            delete observer;
        }

        NotificationCenter* self;
        Multimap::iterator it;
    };

    void _addObserver(const std::shared_ptr<AbstractObserver>& observer);
    void _removeObserver(Multimap::iterator it);

    bool _hasObserverForNotification(size_t notificationHashCode) const;

    mutable std::mutex _mutex;
    Multimap _observers;
};

template <typename NotificationType, typename... Args>
std::shared_ptr<NotificationType> NotificationCenter::postNotification(Args&&... args) const {
    auto notification = std::make_shared<NotificationType>(std::forward<Args>(args)...);
    postNotification(notification);
    return notification;
}

template <typename NotificationType>
typename std::enable_if<!std::is_base_of<detail::ArgumentNotificationBase, NotificationType>::value,
        std::shared_ptr<ApatheticObserver<NotificationType>>
>::type NotificationCenter::addObserver(std::function<void()>&& func) {
    using ObserverType = ApatheticObserver<NotificationType>;
    auto rawObserver = new ObserverType{std::move(func)};
    std::shared_ptr<ObserverType> observer{rawObserver, Deleter{this}};
    _addObserver(observer);
    return observer;
}

template <typename NotificationType, typename Function>
typename std::enable_if<std::is_same<NotificationType, void>::value,
        std::shared_ptr<NotificationObserver<typename FunctionArgumentType<Function>::type>>
>::type NotificationCenter::addObserver(Function&& func) {
    using ObserverType = NotificationObserver<typename FunctionArgumentType<Function>::type>;
    auto rawObserver = new ObserverType{std::move(func)};
    std::shared_ptr<ObserverType> observer{rawObserver, Deleter{this}};
    _addObserver(observer);
    return observer;
}

template <typename NotificationType, typename Function>
typename std::enable_if<std::is_base_of<detail::ArgumentNotificationBase, NotificationType>::value,
    std::shared_ptr<ArgumentObserver<NotificationType, Function>>
>::type NotificationCenter::addObserver(Function&& func) {
    using ObserverType = ArgumentObserver<NotificationType, Function>;
    auto rawObserver = new ObserverType{std::move(func)};
    std::shared_ptr<ObserverType> observer{rawObserver, Deleter{this}};
    _addObserver(observer);
    return observer;
}

inline NotificationCenter::Subscriber NotificationCenter::addObservers(ObserverList& observers) {
    return Subscriber{this, &observers};
}

template <typename NotificationType, typename Function>
NotificationCenter::Subscriber& NotificationCenter::Subscriber::add(Function&& func) {
    observers->emplace_back(self->addObserver<NotificationType>(std::move(func)));
    return *this;
}

template <typename NotificationType>
bool NotificationCenter::hasObserverForNotification() const {
    return _hasObserverForNotification(typeid(NotificationType).hash_code());
}

} // namespace scraps
