#pragma once

#include "scraps/config.h"

#include <list>
#include <queue>
#include <cassert>

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
    void addChildToBack(T* child);
    void addChildToFront(T* child);

    /**
     * Remove children. Returns whether a child was removed.
     */
    bool removeChild(T* child);

    /**
     * Rearrange the order of this node relative to its siblings. If this node
     * doesn't have a parent, the behavior is undefined.
     */
    void sendToBack();
    void bringToFront();

    /**
     * Get a list of all children.
     */
    const std::list<T*>& children() const { return _children; }
    std::list<T*>& children()             { return _children; }

    /**
     * Get the parent.
     */
    const T* parent() const { return _parent; }
    T* parent()             { return _parent; }

    /**
     * Get the root of the tree. Will return `this` if the there is no parent.
     */
    const T* root() const;
    T* root();

    /**
     * Returns the first node that `other` and this node have in common
     * (including if one is a descendant or ancestor of the other) or null
     * otherwise.
     */
    const T* commonNode(const T* other) const;

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
    T* _parent = nullptr;
    std::list<T*> _children;

    template <typename F, typename R>
    auto _traverseRelation(Relation relation, F&& function, R&& result, bool* shouldContinue);

    auto _self() const { return static_cast<const T*>(this); }
    auto _self()       { return static_cast<T*>(this); }
};

template <typename T>
void TreeNode<T>::addChildToBack(T* child) {
    assert(child != nullptr);
    assert(child != _self());
    assert(child->TreeNode::parent() == nullptr);

    _children.push_back(child);
    child->_parent = _self();
}

template <typename T>
void TreeNode<T>::addChildToFront(T* child) {
    assert(child != nullptr);
    assert(child != _self());
    assert(child->TreeNode::parent() == nullptr);

    _children.push_front(child);
    child->_parent = _self();
}

template <typename T>
bool TreeNode<T>::removeChild(T* child) {
    assert(child != nullptr);
    auto it = std::find(_children.begin(), _children.end(), child);
    if (it == _children.end()) {
        return false;
    }
    _children.erase(it);
    child->_parent = nullptr;
    return true;
}

template <typename T>
void TreeNode<T>::sendToBack() {
    assert(parent() != nullptr);
    auto& siblings = parent()->TreeNode::children();

    auto it = std::find(siblings.begin(), siblings.end(), _self());
    assert(it != siblings.end());

    siblings.erase(it);
    siblings.push_back(_self());
}

template <typename T>
void TreeNode<T>::bringToFront() {
    assert(parent() != nullptr);
    auto& siblings = parent()->TreeNode::children();

    auto it = std::find(siblings.begin(), siblings.end(), _self());
    assert(it != siblings.end());

    siblings.erase(it);
    siblings.push_front(_self());
}

template <typename T>
const T* TreeNode<T>::root() const {
    auto current = _self();
    while (current->TreeNode::parent()) { current = current->TreeNode::parent(); }
    return current;
}

template <typename T>
T* TreeNode<T>::root() {
    auto current = _self();
    while (current->TreeNode::parent()) { current = current->TreeNode::parent(); }
    return current;
}

template <typename T>
const T* TreeNode<T>::commonNode(const T* other) const {
    for (auto current = _self(); current; current = current->TreeNode::parent()) {
        if (current == other || current->TreeNode::isAncestorOf(other)) {
            return current;
        }
    }
    return nullptr;
}

template <typename T>
bool TreeNode<T>::isDescendantOf(const T* other) const {
    if (!other) {
        return false;
    }
    for (auto current = parent(); current; current = current->TreeNode::parent()) {
        if (current == other) {
            return true;
        }
    }
    return false;
}

template <typename T>
bool TreeNode<T>::isAncestorOf(const T* other) const {
    return other && other->TreeNode::isDescendantOf(_self());
}

template <typename T>
bool TreeNode<T>::hasRelation(TreeNode::Relation relation, const T* other) const {
    if (!other) {
        return false;
    }
    switch (relation) {
        case Relation::kCommonRoot: return root() && root() == other->TreeNode::root();
        case Relation::kDescendant: return isDescendantOf(other);
        case Relation::kAncestor:   return isAncestorOf(other);
        case Relation::kSibling:    return other != _self() && parent() && parent() == other->TreeNode::parent();
        case Relation::kSelf:       return other == _self();
        default:                    assert(false); return false;
    }
}

template <typename T>
template <typename F, typename R>
auto TreeNode<T>::traverseRelation(Relation relation, F&& function, R init, bool* shouldContinue) {
    if (shouldContinue == nullptr) {
        auto b = true;
        return _traverseRelation(relation, std::forward<F>(function), std::move(init), &b);
    }
    if (*shouldContinue) {
        return _traverseRelation(relation, std::forward<F>(function), std::move(init), shouldContinue);
    }
    return init;
}

template <typename T>
template <typename F, typename R>
auto TreeNode<T>::_traverseRelation(Relation relation, F&& function, R&& result, bool* shouldContinue) {
    if (!*shouldContinue) {
        return 0;
    }

    R& ret = result;

    switch (relation) {
        case Relation::kSelf: {
            ret += function(_self(), shouldContinue);
            break;
        }
        case Relation::kSibling: {
            if (!parent()) {
                break;
            }
            for (auto& sibling : parent()->TreeNode::children()) {
                if (sibling == _self()) { continue; }
                ret += function(sibling, shouldContinue);
                if (shouldContinue == nullptr) {
                    break;
                }
            }
            break;
        }
        case Relation::kDescendant: {
            std::deque<T*> q{_self()};
            while (shouldContinue && !q.empty()) {
                auto parent = q.front();
                q.pop_front();
                for (auto& descendant : parent->TreeNode::children()) {
                    ret += function(descendant, shouldContinue);
                    if (shouldContinue == nullptr) {
                        break;
                    }
                    q.push_back(descendant);
                }
            }
            break;
        }
        case Relation::kAncestor: {
            for (auto ancestor = parent(); ancestor; ancestor = ancestor->TreeNode::parent()) {
                ret += function(ancestor, shouldContinue);
                if (shouldContinue == nullptr) {
                    break;
                }
            }
            break;
        }
        case Relation::kCommonRoot: {
            if (root()) {
                root()->TreeNode::_traverseRelation(Relation::kSelf, function, ret, shouldContinue);
                if (shouldContinue == nullptr) {
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
