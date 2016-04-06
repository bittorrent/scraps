#include "gtest/gtest.h"

#include "scraps/TreeNode.h"

#include <list>

using namespace scraps;

struct TestNode : public TreeNode<TestNode> {};

TEST(TreeNode, construction) {
    TestNode t;

    EXPECT_EQ(t.parent(), nullptr);
    EXPECT_TRUE(t.children().empty());
}

TEST(TreeNode, addChildToBack) {
    TestNode root;
    TestNode c1, c2, c3;

    EXPECT_EQ(root.children(), (std::list<TestNode*>{}));

    root.addChildToBack(&c1);
    EXPECT_EQ(root.children(), (std::list<TestNode*>{&c1}));

    root.addChildToBack(&c2);
    root.addChildToBack(&c3);
    EXPECT_EQ(root.children(), (std::list<TestNode*>{&c1, &c2, &c3}));

    for (auto& c : root.children()) {
        EXPECT_EQ(c->parent(), &root);
    }
}

TEST(TreeNode, addChildToFront) {
    TestNode root;
    TestNode c1, c2, c3, c4;

    EXPECT_EQ(root.children(), (std::list<TestNode*>{}));

    root.addChildToFront(&c1);
    EXPECT_EQ(root.children(), (std::list<TestNode*>{&c1}));

    root.addChildToFront(&c2);
    EXPECT_EQ(root.children(), (std::list<TestNode*>{&c2, &c1}));

    root.addChildToFront(&c3);
    EXPECT_EQ(root.children(), (std::list<TestNode*>{&c3, &c2, &c1}));

    root.addChildToFront(&c4);
    EXPECT_EQ(root.children(), (std::list<TestNode*>{&c4, &c3, &c2, &c1}));

    for (auto& c : root.children()) {
        EXPECT_EQ(c->parent(), &root);
    }
}

TEST(TreeNode, removeChild) {
    TestNode root;
    TestNode c1, c2, c3;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    root.addChildToBack(&c3);
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c1, &c2, &c3}));

    EXPECT_EQ(root.removeChild(&c1), 1);
    EXPECT_EQ(root.children(), (std::list<TestNode*>{&c2, &c3}));

    EXPECT_EQ(c1.parent(), nullptr);
}

TEST(TreeNode, removeChildOnlyRemovesValidChildren) {
    TestNode root1, root2;
    TestNode c1, c2;

    root1.addChildToBack(&c1);
    root2.addChildToBack(&c2);

    EXPECT_EQ(root1.removeChild(&c2), 0);

    EXPECT_EQ(root1.children(), (std::list<TestNode*>{&c1}));
    EXPECT_EQ(root2.children(), (std::list<TestNode*>{&c2}));

    EXPECT_EQ(root1.children().front()->parent(), &root1);
    EXPECT_EQ(root2.children().front()->parent(), &root2);
}

TEST(TreeNode, bringToFront) {
    TestNode root;
    TestNode c1, c2, c3;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    root.addChildToBack(&c3);
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c1, &c2, &c3}));

    c1.bringToFront();
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c1, &c2, &c3}));

    c2.bringToFront();
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c2, &c1, &c3}));

    c3.bringToFront();
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c3, &c2, &c1}));
}

TEST(TreeNode, sendToBack) {
    TestNode root;
    TestNode c1, c2, c3;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    root.addChildToBack(&c3);
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c1, &c2, &c3}));

    c1.sendToBack();
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c2, &c3, &c1}));

    c2.sendToBack();
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c3, &c1, &c2}));

    c3.sendToBack();
    ASSERT_EQ(root.children(), (std::list<TestNode*>{&c1, &c2, &c3}));
}

TEST(TreeNode, parent) {
    TestNode root;
    TestNode c1, c2;

    root.addChildToBack(&c1);
    c1.addChildToBack(&c2);

    EXPECT_EQ(c1.parent(), &root);
    EXPECT_EQ(c2.parent(), &c1);
}

TEST(TreeNode, root) {
    TestNode root;
    TestNode c1, c2;

    root.addChildToBack(&c1);
    c1.addChildToBack(&c2);

    EXPECT_EQ(root.root(), &root);
    EXPECT_EQ(c1.root(), &root);
    EXPECT_EQ(c2.root(), &root);
}

TEST(TreeNode, isDescendantOf) {
    TestNode root;
    TestNode c1, c2;

    root.addChildToBack(&c1);
    c1.addChildToBack(&c2);

    EXPECT_TRUE(c2.isDescendantOf(&root));
    EXPECT_TRUE(c2.isDescendantOf(&c1));

    EXPECT_TRUE(c1.isDescendantOf(&root));
    EXPECT_FALSE(c1.isDescendantOf(&c2));

    EXPECT_FALSE(root.isDescendantOf(&c1));
    EXPECT_FALSE(root.isDescendantOf(&c2));
}


TEST(TreeNode, isAnscestorOf) {
    TestNode root;
    TestNode c1, c2;

    root.addChildToBack(&c1);
    c1.addChildToBack(&c2);

    EXPECT_FALSE(c2.isAnscestorOf(&root));
    EXPECT_FALSE(c2.isAnscestorOf(&c1));

    EXPECT_FALSE(c1.isAnscestorOf(&root));
    EXPECT_TRUE(c1.isAnscestorOf(&c2));

    EXPECT_TRUE(root.isAnscestorOf(&c1));
    EXPECT_TRUE(root.isAnscestorOf(&c2));
}

TEST(TreeNode, commonNode) {
    TestNode root;
    TestNode c1, c2, c3;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    c2.addChildToBack(&c3);

    EXPECT_EQ(root.commonNode(&root), &root);
    EXPECT_EQ(root.commonNode(&c1), &root);
    EXPECT_EQ(root.commonNode(&c2), &root);
    EXPECT_EQ(root.commonNode(&c3), &root);

    EXPECT_EQ(c1.commonNode(&root), &root);
    EXPECT_EQ(c1.commonNode(&c1), &c1);
    EXPECT_EQ(c1.commonNode(&c2), &root);
    EXPECT_EQ(c1.commonNode(&c3), &root);

    EXPECT_EQ(c2.commonNode(&root), &root);
    EXPECT_EQ(c2.commonNode(&c1), &root);
    EXPECT_EQ(c2.commonNode(&c2), &c2);
    EXPECT_EQ(c2.commonNode(&c3), &c2);

    EXPECT_EQ(c3.commonNode(&root), &root);
    EXPECT_EQ(c3.commonNode(&c1), &root);
    EXPECT_EQ(c3.commonNode(&c2), &c2);
    EXPECT_EQ(c3.commonNode(&c3), &c3);
}

TEST(TreeNode, relation) {
    TestNode root;
    TestNode c1, c2, c3;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    c2.addChildToBack(&c3);

    // root
    EXPECT_TRUE(root.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &root));
    EXPECT_TRUE(root.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c1));
    EXPECT_TRUE(root.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c2));
    EXPECT_TRUE(root.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c3));

    EXPECT_TRUE(root.hasRelation(TreeNode<TestNode>::Relation::kSelf, &root));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c1));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c2));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c3));

    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &root));
    EXPECT_TRUE(root.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c1));
    EXPECT_TRUE(root.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c2));
    EXPECT_TRUE(root.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c3));

    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &root));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c1));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c2));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c3));

    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kSibling, &root));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c1));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c2));
    EXPECT_FALSE(root.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c3));

    // c1
    EXPECT_TRUE(c1.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &root));
    EXPECT_TRUE(c1.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c1));
    EXPECT_TRUE(c1.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c2));
    EXPECT_TRUE(c1.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c3));

    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kSelf, &root));
    EXPECT_TRUE(c1.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c1));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c2));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c3));

    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &root));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c1));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c2));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c3));

    EXPECT_TRUE(c1.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &root));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c1));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c2));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c3));

    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kSibling, &root));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c1));
    EXPECT_TRUE(c1.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c2));
    EXPECT_FALSE(c1.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c3));

    // c2
    EXPECT_TRUE(c2.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &root));
    EXPECT_TRUE(c2.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c1));
    EXPECT_TRUE(c2.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c2));
    EXPECT_TRUE(c2.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c3));

    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kSelf, &root));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c1));
    EXPECT_TRUE(c2.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c2));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c3));

    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &root));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c1));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c2));
    EXPECT_TRUE(c2.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c3));

    EXPECT_TRUE(c2.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &root));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c1));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c2));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c3));

    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kSibling, &root));
    EXPECT_TRUE(c2.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c1));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c2));
    EXPECT_FALSE(c2.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c3));

    // c3
    EXPECT_TRUE(c3.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &root));
    EXPECT_TRUE(c3.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c1));
    EXPECT_TRUE(c3.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c2));
    EXPECT_TRUE(c3.hasRelation(TreeNode<TestNode>::Relation::kCommonRoot, &c3));

    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kSelf, &root));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c1));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c2));
    EXPECT_TRUE(c3.hasRelation(TreeNode<TestNode>::Relation::kSelf, &c3));

    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &root));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c1));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c2));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kAncestor, &c3));

    EXPECT_TRUE(c3.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &root));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c1));
    EXPECT_TRUE(c3.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c2));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kDescendant, &c3));

    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kSibling, &root));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c1));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c2));
    EXPECT_FALSE(c3.hasRelation(TreeNode<TestNode>::Relation::kSibling, &c3));
}

struct TraversalNode : TreeNode<TraversalNode> {
    size_t traversalOrder = 0;
};

TEST(TreeNode, traverseRelationCommonRoot) {
    TraversalNode root;
    TraversalNode c1, c2, c11, c12, c21, c22;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    c1.addChildToBack(&c11);
    c1.addChildToBack(&c12);
    c2.addChildToBack(&c21);
    c2.addChildToBack(&c22);

    size_t order = 0;

    auto numAffected = c1.traverseRelation(TreeNode<TraversalNode>::Relation::kCommonRoot, [&](auto* e, bool* shouldContinue){
        e->traversalOrder = ++order;
        return 1;
    }, 0);

    EXPECT_EQ(numAffected, 7);
    EXPECT_EQ(root.traversalOrder, 1);
    EXPECT_EQ(c1.traversalOrder, 2);
    EXPECT_EQ(c2.traversalOrder, 3);
    EXPECT_EQ(c11.traversalOrder, 4);
    EXPECT_EQ(c12.traversalOrder, 5);
    EXPECT_EQ(c21.traversalOrder, 6);
    EXPECT_EQ(c22.traversalOrder, 7);
}

TEST(TreeNode, traverseRelationAncestor) {
    TraversalNode root;
    TraversalNode c1, c2, c11, c12, c21, c22;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    c1.addChildToBack(&c11);
    c1.addChildToBack(&c12);
    c2.addChildToBack(&c21);
    c2.addChildToBack(&c22);

    size_t order = 0;

    auto numAffected = c1.traverseRelation(TreeNode<TraversalNode>::Relation::kAncestor, [&](auto* e, bool* shouldContinue){
        e->traversalOrder = ++order;
        return 1;
    }, 0);

    EXPECT_EQ(numAffected, 1);
    EXPECT_EQ(root.traversalOrder, 1);
    EXPECT_EQ(c1.traversalOrder, 0);
    EXPECT_EQ(c2.traversalOrder, 0);
    EXPECT_EQ(c11.traversalOrder, 0);
    EXPECT_EQ(c12.traversalOrder, 0);
    EXPECT_EQ(c21.traversalOrder, 0);
    EXPECT_EQ(c22.traversalOrder, 0);
}

TEST(TreeNode, traverseRelationDescendant) {
    TraversalNode root;
    TraversalNode c1, c2, c11, c12, c21, c22;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    c1.addChildToBack(&c11);
    c1.addChildToBack(&c12);
    c2.addChildToBack(&c21);
    c2.addChildToBack(&c22);

    size_t order = 0;

    auto numAffected = c1.traverseRelation(TreeNode<TraversalNode>::Relation::kDescendant, [&](auto* e, bool* shouldContinue){
        e->traversalOrder = ++order;
        return 1;
    }, 0);

    EXPECT_EQ(numAffected, 2);
    EXPECT_EQ(root.traversalOrder, 0);
    EXPECT_EQ(c1.traversalOrder, 0);
    EXPECT_EQ(c2.traversalOrder, 0);
    EXPECT_EQ(c11.traversalOrder, 1);
    EXPECT_EQ(c12.traversalOrder, 2);
    EXPECT_EQ(c21.traversalOrder, 0);
    EXPECT_EQ(c22.traversalOrder, 0);
}

TEST(TreeNode, traverseRelationSibling) {
    TraversalNode root;
    TraversalNode c1, c2, c11, c12, c21, c22;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    c1.addChildToBack(&c11);
    c1.addChildToBack(&c12);
    c2.addChildToBack(&c21);
    c2.addChildToBack(&c22);

    size_t order = 0;

    auto numAffected = c1.traverseRelation(TreeNode<TraversalNode>::Relation::kSibling, [&](auto* e, bool* shouldContinue){
        e->traversalOrder = ++order;
        return 1;
    }, 0);

    EXPECT_EQ(numAffected, 1);
    EXPECT_EQ(root.traversalOrder, 0);
    EXPECT_EQ(c1.traversalOrder, 0);
    EXPECT_EQ(c2.traversalOrder, 1);
    EXPECT_EQ(c11.traversalOrder, 0);
    EXPECT_EQ(c12.traversalOrder, 0);
    EXPECT_EQ(c21.traversalOrder, 0);
    EXPECT_EQ(c22.traversalOrder, 0);
}

TEST(TreeNode, traverseRelationSelf) {
    TraversalNode root;
    TraversalNode c1, c2, c11, c12, c21, c22;

    root.addChildToBack(&c1);
    root.addChildToBack(&c2);
    c1.addChildToBack(&c11);
    c1.addChildToBack(&c12);
    c2.addChildToBack(&c21);
    c2.addChildToBack(&c22);

    size_t order = 0;

    auto numAffected = c1.traverseRelation(TreeNode<TraversalNode>::Relation::kSelf, [&](auto* e, bool* shouldContinue){
        e->traversalOrder = ++order;
        return 1;
    }, 0);

    EXPECT_EQ(numAffected, 1);
    EXPECT_EQ(root.traversalOrder, 0);
    EXPECT_EQ(c1.traversalOrder, 1);
    EXPECT_EQ(c2.traversalOrder, 0);
    EXPECT_EQ(c11.traversalOrder, 0);
    EXPECT_EQ(c12.traversalOrder, 0);
    EXPECT_EQ(c21.traversalOrder, 0);
    EXPECT_EQ(c22.traversalOrder, 0);
}
