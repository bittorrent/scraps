#pragma once

#include "scraps/config.h"

#include <list>

namespace scraps {

/**
* Creates a spinnable wheel of elements. This class is not thread-safe.
*/
template <typename T>
class Wheel {
public:
    Wheel() { _selection = _contents.begin(); }

    /**
    * Inserts an element to the wheel. The element is inserted so that it will be selected
    * last if the wheel is spun forward one element at a time.
    *
    * @param element the element to be inserted
    */
    void insert(const T& element) {
        if (_contents.empty()) {
            _contents.push_back(element);
            _selection = _contents.begin();
        } else {
            _contents.insert(_selection, element);
        }
    }

    /**
    * Removes the given element. If the element is currently selected, the selection moves to the next element.
    * If the element exists more than once in the wheel, only one is removed and which one is undefined.
    *
    * @return true if an element was removed
    */
    bool remove(const T& element) {
        for (auto it = _contents.begin(); it != _contents.end(); ++it) {
            if (*it == element) {
                if (it == _selection) {
                    if ((_selection = _contents.erase(it)) == _contents.end()) {
                        _selection = _contents.begin();
                    }
                } else {
                    _contents.erase(it);
                }
                return true;
            }
        }
        return false;
    }

    /**
    * Removes the selected element. The selection moves to the next element.
    */
    void removeSelection() {
        if ((_selection = _contents.erase(_selection)) == _contents.end()) {
            _selection = _contents.begin();
        }
    }

    /**
    * Removes all elements from the wheel.
    */
    void clear() {
        _contents.clear();
        _selection = _contents.begin();
    }

    /**
    * Returns the currently selected element.
    */
    T& selection() { return *_selection; }

    /**
    * Spins the wheel forward or backwards `count` elements.
    */
    void spin(ssize_t count = 1) {
        if (_contents.empty()) {
            return;
        }

        if (count > 0) {
            for (ssize_t i = count % _contents.size(); i > 0; --i) {
                if (++_selection == _contents.end()) {
                    _selection = _contents.begin();
                }
            }
        } else if (count < 0) {
            for (ssize_t i = -count % _contents.size(); i > 0; --i) {
                if (_selection == _contents.begin()) {
                    _selection = _contents.end();
                }
                --_selection;
            }
        }
    }

    /**
    * Indicates whether or not the wheel is empty.
    *
    * @return true if the wheel is empty
    */
    bool empty() { return _contents.empty(); }

    /**
    * Returns the size of the wheel.
    *
    * @return the number of elements in the wheel
    */
    size_t size() { return _contents.size(); }

private:
    std::list<T> _contents;
    typename std::list<T>::iterator _selection;
};

} // namespace scraps
