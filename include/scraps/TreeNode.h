#pragma once

#include "scraps/config.h"

#include "scraps/Reverse.h"

#include <gsl.h>

#include <list>
#include <queue>

namespace scraps {

/**
 * Intrusive container for maintaining a tree of homogenous objects. T must
 * derive from TreeNode<T> and if the inheritance is private, TreeNode<T> must
 * be a friend of the derived.
 */
template <typename T>
class TreeNode {
public:
    /**
     * Add children.
     */
    void addChildToBack(gsl::not_null<T*> child);
    void addChildToFront(gsl::not_null<T*> child);

    /**
     * Remove children. Returns the number of children removed.
     */
    size_t removeChild(gsl::not_null<T*> child);

    /**
     * Rearrange the order of this node relative to its siblings. If this node
     * doesn't have a parent, the behavior is undefined.
     */
    void sendToBack();
    void bringToFront();

    /**
     * Get a list of all children.
     */
    inline auto& children() const { return _children; }
    inline auto& children()       { return _children; }

    /**
     * Get the parent.
     */
    inline T* parent() const { return _parent; }
    inline T* parent()       { return _parent; }

    /**
     * Get the root of the tree. Will return `this` if the there is no parent;
     */
    const T* root() const;
    T* root();

    /**
     * Returns the first node that `other` and this node have in common
     * (including if one is a descendant or ancestor of the other) or null
     * otherwise.
     */
    const T* commonNode(gsl::not_null<const T*> other) const;

    /**
     * Determine if a node is a descendant or ancestor of this node.
     */
    bool isAncestorOf(const T* other) const;
    bool isDescendantOf(const T* other) const;

    enum class Relation {
        kCommonRoot,
        kAncestor,
        kDescendant,
        kSibling,
        kSelf,
    };

    /**
     * Determine if a relationship exists between two nodes.
     */
    bool hasRelation(Relation relation, const T* other) const;

    /**
     * Apply function to each node in the specified relation and accumulate the
     * result.
     *
     * F should be callable in the form F(T* node, bool* shouldContinue)
     * where `node` is a pointer to the node and `shouldContinue` is a pointer
     * to a bool that will be checked before every call. If shouldContinue is
     * null, a local bool will be used instead. This allows multiple
     * traversRelation calls to be chained while maintaining that state.
     *
     * Traversal order:
     *   kCommonRoot: breadth-first front to back starting at root.
     *   kAncestor:   bottom to top starting at this node.
     *   kDescendant: breadth-first front to back.
     *   kSibling:    front to back starting with this node's parent's children.
     *   kSelf:       self.
     */
    template <typename F, typename R>
    auto traverseRelation(Relation relation, F&& function, R init, bool* shouldContinue = nullptr);

private:
    std::list<T*>  _children;
    T* _parent = nullptr;

    template <typename F, typename R>
    auto _traverseRelation(Relation relation, F&& function, R&& result, gsl::not_null<bool*> shouldContinue);
};

template <typename T>
void TreeNode<T>::addChildToBack(gsl::not_null<T*> child) {
    SCRAPS_ASSERT(child != static_cast<T*>(this));
    SCRAPS_ASSERT(child->parent() == nullptr);

    _children.push_back(child);
    child->_parent = static_cast<T*>(this);
}

template <typename T>
void TreeNode<T>::addChildToFront(gsl::not_null<T*> child) {
    SCRAPS_ASSERT(child != static_cast<T*>(this));
    SCRAPS_ASSERT(child->parent() == nullptr);

    _children.push_front(child);
    child->_parent = static_cast<T*>(this);
}

template <typename T>
size_t TreeNode<T>::removeChild(gsl::not_null<T*> child) {
    auto it = std::find(_children.begin(), _children.end(), child);
    if (it == _children.end()) {
        return 0;
    }
    _children.erase(it);
    child->_parent = nullptr;
    return 1;
}

template <typename T>
void TreeNode<T>::sendToBack() {
    auto& siblings = parent()->TreeNode::children();
    for (auto it = siblings.begin(); it != siblings.end(); ++it) {
        if (*it == this) {
            siblings.erase(it);
            siblings.push_back(static_cast<T*>(this));
            return;
        }
    }
}

template <typename T>
void TreeNode<T>::bringToFront() {
    auto& siblings = parent()->TreeNode::children();
    for (auto it = siblings.begin(); it != siblings.end(); ++it) {
        if (*it == this) {
            siblings.erase(it);
            siblings.push_front(static_cast<T*>(this));
            return;
        }
    }
}

template <typename T>
const T* TreeNode<T>::root() const {
    const T* current = static_cast<const T*>(this);
    const T* currentParent = parent();
    while (currentParent) {
        current = currentParent;
        currentParent = current->parent();
    }
    return current;
}

template <typename T>
T* TreeNode<T>::root() {
    T* current = static_cast<T*>(this);
    T* currentParent = parent();
    while (currentParent) {
        current = currentParent;
        currentParent = current->parent();
    }
    return current;
}

template <typename T>
const T* TreeNode<T>::commonNode(gsl::not_null<const T*> other) const {
    const T* current = static_cast<const T*>(this);
    while (current) {
        if (current == other || current->isAncestorOf(other)) {
            return current;
        }
        current = current->parent();
    }
    return nullptr;
}

template <typename T>
bool TreeNode<T>::isDescendantOf(const T* other) const {
    if (!other) {
        return false;
    }
    const T* current = parent();
    while (current) {
        if (current == other) {
            return true;
        }
        current = current->parent();
    }
    return false;
}

template <typename T>
bool TreeNode<T>::isAncestorOf(const T* other) const {
    return other && other->isDescendantOf(static_cast<const T*>(this));
}

template <typename T>
bool TreeNode<T>::hasRelation(TreeNode::Relation relation, const T* other) const {
    if (!other) {
        return false;
    }
    switch (relation) {
        case Relation::kCommonRoot:
            return root() && root() == other->root();
        case Relation::kDescendant:
            return isDescendantOf(other);
        case Relation::kAncestor:
            return isAncestorOf(other);
        case Relation::kSibling:
            return other != static_cast<const T*>(this) && parent() && parent() == other->parent();
        case Relation::kSelf:
            return other == static_cast<const T*>(this);
    }
}

template <typename T>
template <typename F, typename R>
auto TreeNode<T>::traverseRelation(Relation relation, F&& function, R init, bool* shouldContinue) {
    if (!shouldContinue) {
        bool b = true;
        return _traverseRelation(relation, std::forward<F>(function), std::move(init), &b);
    }
    if (*shouldContinue) {
        return _traverseRelation(relation, std::forward<F>(function), std::move(init), shouldContinue);
    }
    return init;
}

template <typename T>
template <typename F, typename R>
auto TreeNode<T>::_traverseRelation(Relation relation, F&& function, R&& result, gsl::not_null<bool*> shouldContinue) {
    if (!*shouldContinue) {
        return 0;
    }

    R& ret = result;

    switch (relation) {
        case Relation::kSelf: {
            ret += function(static_cast<T*>(this), shouldContinue);
            break;
        }
        case Relation::kSibling: {
            if (!parent()) {
                break;
            }
            for (auto& child : parent()->TreeNode::children()) {
                if (child == static_cast<T*>(this)) { continue; }
                ret += function(child, shouldContinue);
                if (!shouldContinue) {
                    break;
                }
            }
            break;
        }
        case Relation::kDescendant: {
            std::deque<T*> q{static_cast<T*>(this)};
            while (shouldContinue && !q.empty()) {
                auto parent = q.front();
                q.pop_front();
                for (auto& child : parent->children()) {
                    ret += function(child, shouldContinue);
                    if (!shouldContinue) {
                        break;
                    }
                    q.push_back(child);
                }
            }
            break;
        }
        case Relation::kAncestor: {
            auto* child = parent();
            while (child) {
                ret += function(child, shouldContinue);
                if (!shouldContinue) {
                    break;
                }
                child = child->parent();
            }
            break;
        }
        case Relation::kCommonRoot: {
            if (root()) {
                root()->TreeNode::_traverseRelation(Relation::kSelf, function, ret, shouldContinue);
                if (!shouldContinue) {
                    break;
                }
                root()->TreeNode::_traverseRelation(Relation::kDescendant, function, ret, shouldContinue);
            }
            break;
        }
    }
    return ret;
}

} // namespace scraps
