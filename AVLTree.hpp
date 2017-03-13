/*
Copyright (c) 2017 Patrick Rutkowski. All Rights Reserved.

This file may not be copied nor distributed without the express permission of
the author.

This file is distributed on an "AS IS" basis WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED. THE AUTHOR HEREBY DISCLAIMS ALL SUCH WARRANTIES,
INCLUDING WITHOUT LIMITATION ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
*/

#ifndef XKIT_AVLTREE_HPP
#define XKIT_AVLTREE_HPP

#include <XKit/Util.hpp>
#include <XKit/CUtil.hpp>
#include <utility>
#include <stddef.h>

#define Template template<class KT, class VT>
#define TA KT,VT

namespace XK
{

template<class KT, class VT = int>
class AVLTree
{

public:
class Node
{
friend class AVLTree;

private:
    int mBalance;
    Node* mParent;
    Node* mLeft;
    Node* mRight;

private:
    Node* subtreeSuccessor();
    Node* subtreePredecessor();

    void adopt(Node* left, Node* right);
    void adoptLeft(Node* newChild);
    void adoptRight(Node* newChild);

    template<class VU>
    Node(const typename AVLTree<KT,VU>::Node& node);

    size_t depth() const;
    size_t subtreeSize() const;
    bool validate() const;

public:
    const KT key;
    VT value;

public:
    Node(KT key, VT value = VT());

    Node* next();
    Node* prev();

    const Node* next() const;
    const Node* prev() const;
};

public:
class Context
{
friend class AVLTree;
private:
    Node* parent;
    int dir;
};

public:
class Iterator
{
private:
    Node* mNode;

public:
    Iterator(Node* node);

    bool operator!=(const Iterator& ite);
    Iterator& operator++();
    Node& operator*();
};

public:
class ConstIterator
{
private:
    const Node* mNode;

public:
    ConstIterator(const Node* node);

    bool operator!=(const ConstIterator& ite);
    ConstIterator& operator++();
    const Node& operator*();
};

private:
    size_t mSize;
    Node* mRoot;

    Node** downPointer(Node* node);
    void resetDownPointer(Node* orig, Node* node);

    void leftLeft(Node* node);
    void rightRight(Node* node);
    void leftRight(Node* node);
    void rightLeft(Node* node);

    void balanceNode(Node* node);

private:
    const Node* getFirst() const;
    const Node* getLast() const;

    template<class VU>
    AVLTree&
    copyAssign(const AVLTree<KT,VU>& avl);

public:
    AVLTree();

    AVLTree(AVLTree&& avl);

    template<class VU>
    AVLTree(const AVLTree<KT,VU>& avl);

    AVLTree(const AVLTree& avl);

    ~AVLTree();

    AVLTree&
    operator=(AVLTree&& avl);

    template<class VU>
    AVLTree&
    operator=(const AVLTree<KT,VU>& avl);

    AVLTree&
    operator=(const AVLTree& avl);

    VT& operator[](KT key);

    size_t size() const { return mSize; }

    Node* find(ConstRef<KT> key, Context* ctx = nullptr);
    const Node* find(ConstRef<KT> key, Context* ctx = nullptr) const;

    bool exists(ConstRef<KT> key, Context* ctx = nullptr);

    Node* first();
    Node* last();

    const Node* first() const;
    const Node* last() const;

    Node* set(KT key, VT value, const Context* ctx = nullptr);

    Node* remove(Node* node);

    Node* insert(Node* node, const Context& ctx);
    Node* insert(KT key, const Context* ctx = nullptr);

    void erase(Node* node);
    bool erase(ConstRef<KT> key);

    void clear();

    Iterator begin();
    ConstIterator begin() const;
    
    Iterator end();
    ConstIterator end() const;

    bool validate() const;
};

Template
AVLTree<TA>::Iterator::Iterator(Node* node)
{
    mNode = node;
}

Template
typename
AVLTree<TA>::Iterator&
AVLTree<TA>::Iterator::operator++()
{
    if( ! mNode )
        return *this;
        
    return *this = mNode->next();
}

Template
bool
AVLTree<TA>::Iterator::operator!=(const Iterator& ite)
{
    return mNode != ite.mNode;
}

Template
typename
AVLTree<TA>::Node&
AVLTree<TA>::Iterator::operator*()
{
    return *mNode;
}

Template
AVLTree<TA>::ConstIterator::ConstIterator(const Node* node)
{
    mNode = node;
}

Template
typename
AVLTree<TA>::ConstIterator&
AVLTree<TA>::ConstIterator::operator++()
{
    if( ! mNode )
        return *this;

    return *this = mNode->next();
}

Template
bool
AVLTree<TA>::ConstIterator::operator!=(const ConstIterator& ite)
{
    return mNode != ite.mNode;
}

Template
const
typename
AVLTree<TA>::Node&
AVLTree<TA>::ConstIterator::operator*()
{
    return *mNode;
}

// Return the leftmost node in the right subtree. In the linear ordering of
// elements this is the next element.

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::Node::subtreeSuccessor()
{
    Node* iter = this;
    iter = iter->mRight;
    while(iter && iter->mLeft)
        iter = iter->mLeft;
    return iter;
}

// Return the rightmost node in the left subtree. In the linear ordering of
// elements this is the previous element.

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::Node::subtreePredecessor()
{
    Node* iter = this;
    iter = iter->mLeft;
    while(iter && iter->mRight)
        iter = iter->mRight;
    return iter;
}

Template
template<class VU>
AVLTree<TA>::Node::Node(const typename AVLTree<KT,VU>::Node& n) :
Node(std::move(n.key), std::move(n.value))
{
}

Template
AVLTree<TA>::Node::Node(KT k, VT v) :
key(std::move(k)),
value(std::move(v))
{
    mBalance = 0;
    mParent = nullptr;
    mLeft = nullptr;
    mRight = nullptr;
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::Node::next()
{
    if( mRight )
        return subtreeSuccessor();

    Node* iter = this;
    while(iter->mParent)
    {
        int dir = iter->mParent->mLeft == iter ? -1 : +1;

        // If dir is -1 then iter is the lesser child of the parent and thus
        // the parent has a greater value. Return the parent.

        if( dir == -1 )
            return iter->mParent;

        iter = iter->mParent;
    }

    return nullptr;
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::Node::prev()
{
    if( mLeft )
        return subtreePredecessor();

    Node* iter = this;
    while(iter->mParent)
    {
        // Set dir to -1 if iter is the left child of its parent,
        // or to +1 if iter is the right child of its parent.

        int dir = iter->mParent->mLeft == iter ? -1 : +1;

        // If dir is +1 then we are the lesser child of the parent and thus
        // the parent has a greater value. Return the parent.

        if( dir == +1 )
            return iter->mParent;
        
        iter = iter->mParent;
    }

    return nullptr;
}

Template
const
typename
AVLTree<TA>::Node*
AVLTree<TA>::Node::next()
const
{
    return const_cast<AVLTree::Node*>(this)->next();
}

Template
const
typename
AVLTree<TA>::Node*
AVLTree<TA>::Node::prev()
const
{
    return const_cast<AVLTree::Node*>(this)->prev();
}

Template
void
AVLTree<TA>::Node::adopt(Node* left, Node* right)
{
    adoptLeft(left);
    adoptRight(right);
}

Template
void
AVLTree<TA>::Node::adoptLeft(Node* newChild)
{
    mLeft = newChild;
    if( newChild )
        newChild->mParent = this;
}

Template
void
AVLTree<TA>::Node::adoptRight(Node* newChild)
{
    mRight = newChild;
    if( newChild )
        newChild->mParent = this;
}

Template
size_t
AVLTree<TA>::Node::depth()
const
{
    int dir = 0;
    const Node* iter = this;
    size_t dp = 0;
    size_t maxDP = 0;

    while(true)
    {
        if( dir == 0 )
        {
            dp++;

            if( dp > maxDP )
                maxDP = dp;

            if( iter->mLeft )
                iter = iter->mLeft;
            else if( iter->mRight )
                iter = iter->mRight;
            else
                goto GOUP;
        }
        else if( dir == -1 )
        {
            dp--;

            if( iter->mRight )
            {
                dir = 0;
                iter = iter->mRight;
            }
            else
            {
                goto GOUP;
            }
        }
        else if( dir == +1 )
        {
            dp--;
            goto GOUP;
        }

        continue;

        GOUP:

        if( iter == this )
            break;

        dir = iter->mParent->mLeft == iter ? -1 : +1;
        iter = iter->mParent;
    }

    return maxDP;
}

Template
size_t
AVLTree<TA>::Node::subtreeSize()
const
{
    int dir = 0;
    const Node* iter = this;
    size_t count = 0;

    while(true)
    {
        if( dir == 0 )
        {
            count++;

            if( iter->mLeft )
                iter = iter->mLeft;
            else if( iter->mRight )
                iter = iter->mRight;
            else
                goto GOUP;
        }
        else if( dir == -1 )
        {
            if( iter->mRight )
            {
                dir = 0;
                iter = iter->mRight;
            }
            else
            {
                goto GOUP;
            }
        }
        else if( dir == +1 )
        {
            goto GOUP;
        }

        continue;

        GOUP:

        if( iter == this )
            break;

        dir = iter->mParent->mLeft == iter ? -1 : +1;
        iter = iter->mParent;
    }

    return count;
}

Template
bool
AVLTree<TA>::Node::validate()
const
{
    if( mLeft && mLeft->key >= key )
        return false;

    if( mRight && mRight->key <= key )
        return false;

    if( mBalance != -1 && mBalance != 0 && mBalance != +1 )
        return false;

    size_t leftDepth = mLeft ? mLeft->depth() : 0;
    size_t rightDepth = mRight ? mRight->depth() : 0;

    size_t balance = rightDepth - leftDepth;

    if( mBalance != balance )
        return false;

    if( mLeft && ! mLeft->validate() )
        return false;

    if( mRight && ! mRight->validate() )
        return false;

    return true;
}

Template
typename
AVLTree<TA>::Iterator
AVLTree<TA>::begin()
{
    return Iterator(first());
}

Template
typename
AVLTree<TA>::ConstIterator
AVLTree<TA>::begin()
const
{
    return ConstIterator(first());
}

Template
typename
AVLTree<TA>::Iterator
AVLTree<TA>::end()
{
    return Iterator(nullptr);
}

Template
typename
AVLTree<TA>::ConstIterator
AVLTree<TA>::end()
const
{
    return ConstIterator(nullptr);
}

Template
AVLTree<TA>::AVLTree()
{
    mSize = 0;
    mRoot = nullptr;
}

Template
AVLTree<TA>::AVLTree(AVLTree&& avl)
{
    *this = std::move(avl);
}

Template
template<class VU>
AVLTree<TA>::AVLTree(const AVLTree<KT,VU>& avl) :
AVLTree()
{
    *this = avl;
}

Template
AVLTree<TA>::AVLTree(const AVLTree& avl) :
AVLTree()
{
    *this = avl;
}

Template
AVLTree<TA>::~AVLTree()
{
    clear();
}

Template
AVLTree<TA>&
AVLTree<TA>::operator=(AVLTree&& avl)
{
    mSize = avl.mSize;
    mRoot = avl.mRoot;
    avl.mSize = 0;
    avl.mRoot = nullptr;
    return *this;
}

Template
template<class VU>
AVLTree<TA>&
AVLTree<TA>::copyAssign(const AVLTree<KT,VU>& avl)
{
    clear();

    mSize = avl.mSize;

    if( avl.mSize == 0 )
    {
        mRoot = nullptr;
        return *this;
    }

    mRoot = new Node(*avl.mRoot);

    Node* iter = avl.mRoot;
    Node* iter2 = mRoot;

    int dir = 0;
    while(true)
    {
        if( dir == 0 )
        {
            if( iter->mLeft )
            {
                iter2->adoptLeft(new Node(*iter->mLeft));
                iter2->mLeft->mBalance = iter->mLeft->mBalance;
            }

            if( iter->mRight )
            {
                iter2->adoptRight(new Node(*iter->mRight));
                iter2->mRight->mBalance = iter->mRight->mBalance;
            }

            if( iter->mLeft )
            {
                iter = iter->mLeft;
                iter2 = iter2->mLeft;
            }
            else if( iter->mRight )
            {
                iter = iter->mRight;
                iter2 = iter2->mRight;
            }
            else
            {
                goto GOUP;
            }
        }
        else if( dir == -1 )
        {
            if( iter->mRight )
            {
                dir = 0;
                iter = iter->mRight;
                iter2 = iter2->mRight;
            }
            else
            {
                goto GOUP;
            }
        }
        else if( dir == 1 )
        {
            goto GOUP;
        }

        continue;

        GOUP:

        if( iter == avl.mRoot )
            break;

        dir = iter->mParent->mLeft == iter ? -1 : +1;
        iter = iter->mParent;
        iter2 = iter2->mParent;
    }

    return *this;
}

Template
template<class VU>
AVLTree<TA>&
AVLTree<TA>::operator=(const AVLTree<KT,VU>& avl)
{
    return copyAssign(avl);
}

Template
AVLTree<TA>&
AVLTree<TA>::operator=(const AVLTree& avl)
{
    return copyAssign(avl);
}

// The downPointer() function gives access to a node's parent connection so
// that code can easily re-parent a node without worrying about where the
// parent connection lives (it might live in parent->left, parent->right, or
// even if tree->root).

Template
typename
AVLTree<TA>::Node**
AVLTree<TA>::downPointer(Node* node)
{
    if( node->mParent == nullptr )
        return &mRoot;
    else if( node->mParent->mLeft == node )
        return &node->mParent->mLeft;
    else
        return &node->mParent->mRight;
}

Template
void
AVLTree<TA>::resetDownPointer(Node* orig, Node* node)
{
    *downPointer(orig) = node;
    node->mParent = orig->mParent;
}

Template
void
AVLTree<TA>::leftLeft(Node* node)
{
    Node* c = node;
    Node* b = c->mLeft;
    Node* a = b->mLeft;

    Node* br = b->mRight;

    resetDownPointer(c, b);

    b->adopt(a, c);
    c->adoptLeft(br);

    c->mBalance = -b->mBalance - 1;
    b->mBalance = +b->mBalance + 1;
}

Template
void
AVLTree<TA>::rightRight(Node* node)
{
    Node* a = node;
    Node* b = a->mRight;
    Node* c = b->mRight;

    Node* bl = b->mLeft;

    resetDownPointer(a, b);

    b->adopt(a, c);
    a->adoptRight(bl);

    a->mBalance = -b->mBalance + 1;
    b->mBalance = +b->mBalance - 1;
}

Template
void
AVLTree<TA>::leftRight(Node* node)
{
    Node* c = node;
    Node* a = c->mLeft;
    Node* b = a->mRight;

    Node* bl = b->mLeft;
    Node* br = b->mRight;

    resetDownPointer(c, b);

    b->adopt(a, c);
    a->adoptRight(bl);
    c->adoptLeft(br);

    a->mBalance = b->mBalance == -1 ? 0
                : b->mBalance ==  0 ? 0
                : b->mBalance == +1 ? -1 : 0;

    c->mBalance = b->mBalance == -1 ? 1
                : b->mBalance ==  0 ? 0
                : b->mBalance == +1 ? 0 : 0;

    b->mBalance = 0;
}

Template
void
AVLTree<TA>::rightLeft(Node* node)
{
    Node* a = node;
    Node* c = a->mRight;
    Node* b = c->mLeft;

    Node* bl = b->mLeft;
    Node* br = b->mRight;

    resetDownPointer(a, b);

    b->adopt(a, c);
    a->adoptRight(bl);
    c->adoptLeft(br);

    a->mBalance = b->mBalance == -1 ? 0
                : b->mBalance ==  0 ? 0
                : b->mBalance == +1 ? -1 : 0;

    c->mBalance = b->mBalance == -1 ? 1
                : b->mBalance ==  0 ? 0
                : b->mBalance == +1 ? 0 : 0;

    b->mBalance = 0;
}

Template
void
AVLTree<TA>::balanceNode(Node* node)
{
    if( node->mBalance == -2 )
    {
        if( node->mLeft->mBalance == -1 ||
            node->mLeft->mBalance == 0 )
        {
            leftLeft(node);
        }
        else if( node->mLeft->mBalance == +1 )
        {
            leftRight(node);
        }
    }
    else if( node->mBalance == +2 )
    {
        if( node->mRight->mBalance == +1 ||
            node->mRight->mBalance == 0 )
        {
            rightRight(node);
        }
        else if( node->mRight->mBalance == -1 )
        {
            rightLeft(node);
        }
    }
}

Template
VT&
AVLTree<TA>::operator[](KT key)
{
    Context ctx;
    Node* node = find(key, &ctx);

    if( node == nullptr )
        node = set(std::move(key), VT(), &ctx);

    return node->value;
}

Template
const
typename
AVLTree<TA>::Node*
AVLTree<TA>::find(
    ConstRef<KT> key,
    Context* ctx)
const
{
    Node* iter = mRoot;
    int dir = 0;

    while( iter )
    {
        dir = XK::Order(key, iter->key);

        if( dir < 0 )
        {
            if( iter->mLeft )
                iter = iter->mLeft;
            else
                break;
        }
        else if( dir > 0 )
        {
            if( iter->mRight )
                iter = iter->mRight;
            else
                break;
        }
        else
        {
            return iter;
        }
    }

    if( ctx )
    {
        ctx->parent = iter;
        ctx->dir = dir;
    }

    return nullptr;
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::find(
    ConstRef<KT> key,
    Context* ctx)
{
    return const_cast<Node*>(
        const_cast<const AVLTree<TA>*>(
            this)->find(key, ctx));
}

Template
bool
AVLTree<TA>::exists(
    ConstRef<KT> key,
    Context* ctx)
{
    return (bool)find(key, ctx);
}

Template
const
typename
AVLTree<TA>::Node*
AVLTree<TA>::getFirst()
const
{
    Node* iter = mRoot;
    while(iter && iter->mLeft)
        iter = iter->mLeft;
    return iter;
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::first()
{
    return (AVLTree<TA>::Node*)getFirst();
}

Template
const
typename
AVLTree<TA>::Node*
AVLTree<TA>::first()
const
{
    return getFirst();
}

Template
const
typename
AVLTree<TA>::Node*
AVLTree<TA>::getLast()
const
{
    Node* iter = mRoot;
    while(iter && iter->mRight)
        iter = iter->mRight;
    return iter;
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::last()
{
    return getLast();
}

Template
const
typename
AVLTree<TA>::Node*
AVLTree<TA>::last()
const
{
    return (Node*)getLast();
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::insert(
    Node* node,
    const Context& ctx)
{
    Node* parent = ctx.parent;
    int dir = ctx.dir;

    Node* iter;

    mSize++;

    node->mBalance = 0;
    node->mLeft    = nullptr;
    node->mRight   = nullptr;
    node->mParent  = parent;

    if( dir == 0 )
    {
        this->mRoot = node;
        return node;
    }

    if( dir <= -1 )
        parent->mLeft = node;
    else if( dir >= +1 )
        parent->mRight = node;

    iter = parent;
    while(true)
    {
        iter->mBalance += dir;

        if( iter->mBalance == 0 )
            break;

        if( iter->mBalance == +2 || iter->mBalance == -2 )
        {
            balanceNode(iter);
            break;
        }

        if( iter->mParent == nullptr )
            break;

        if( iter->mParent->mLeft == iter )
            dir = -1;
        else
            dir = +1;

        iter = iter->mParent;
    }

    return node;
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::remove(Node* node)
{
    // Setting dir and iterRef to 0 and NULL quiets a GCC warning that
    // complains of them being used without being initialized, which is never
    // actually the case.

    Node* replacement;
    Node** ref;
    Node* iter;
    Node** iterRef = nullptr;
    Node* iterChild;
    int dir = 0;

    if( node == nullptr )
        return node;

    this->mSize--;

    // If both SubtreePredecessor and SubtreeSuccessor return NULL then
    // replacement will end up being set to NULL. This is intentional.

    replacement = node->subtreePredecessor();
    if( replacement == nullptr )
        replacement = node->subtreeSuccessor();

    if( replacement )
    {
        iter = replacement->mParent;
        iterChild = replacement;
    }
    else
    {
        iter = node->mParent;
        iterChild = node;
    }

    if( iter )
    {
        dir = (iter->mLeft == iterChild) ? -1 : +1;
        iterRef = downPointer(iter);
    }

    if( replacement )
    {
        ref = downPointer(replacement);

        if( replacement->mLeft )
            *ref = replacement->mLeft;
        else if( replacement->mRight )
            *ref = replacement->mRight;
        else
            *ref = nullptr;

        if( *ref )
            (*ref)->mParent = replacement->mParent;

        replacement->mParent  = node->mParent;
        replacement->mLeft    = node->mLeft;
        replacement->mRight   = node->mRight;
        replacement->mBalance = node->mBalance;

        if( replacement->mLeft )
            replacement->mLeft->mParent = replacement;

        if( replacement->mRight )
            replacement->mRight->mParent = replacement;
    }

    *downPointer(node) = replacement;

    if( iter )
        iter = *iterRef;

    while( iter )
    {
        Node* next = iter->mParent;

        iter->mBalance -= dir;
        if( iter->mBalance == +2 || iter->mBalance == -2 )
        {
            iterRef = downPointer(iter);
            balanceNode(iter);
            iter = *iterRef;
        }

        if( iter->mBalance == -1 || iter->mBalance == +1 )
            break;

        if( next )
            dir = (next->mLeft == iter) ? -1 : +1;
        iter = next;
    }

    return node;
}

Template
void
AVLTree<TA>::erase(Node* node)
{
    remove(node);
    delete node;
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::set(
    KT key,
    VT value,
    const Context* ctx)
{
        Context localCTX;

    if( ctx == nullptr )
    {
        Node* node = find(key, &localCTX);

        if( node )
        {
            node->value = std::move(value);
            return node;
        }

        ctx = &localCTX;
    }

    auto node =
        new typename XK::AVLTree<TA>::Node(
            std::move(key),
            std::move(value));

    return insert(node, *ctx);
}

Template
typename
AVLTree<TA>::Node*
AVLTree<TA>::insert(
    KT key,
    const Context* ctx)
{
    Context localCTX;

    if( ctx == nullptr )
    {
        Node* node = find(key, &localCTX);
        if( node )
            return node;

        ctx = &localCTX;
    }

    auto node = new typename AVLTree<TA>::Node(std::move(key));

    insert(node, *ctx);
    return node;
}

Template
bool
AVLTree<TA>::erase(ConstRef<KT> key)
{
    Node* node = find(key, nullptr);
    if( node == nullptr )
        return false;
    erase(node);
    return true;
}

Template
void
AVLTree<TA>::clear()
{
    if( mSize == 0 )
        return;

    // Walk the tree and delete nodes as we go. The variable "dir" stores the
    // state of the walk:
    //
    // dir = +0: We have just walked down into a node from its parent.
    // dir = -1: We have just walked up into a node from its left child
    // dir = +1: We have just walked up into a node from its right child

    Node* iter = mRoot;
    int dir = 0;
    while(true)
    {
        if( dir == 0 )
        {
            if( iter->mLeft )
                iter = iter->mLeft;
            else if( iter->mRight )
                iter = iter->mRight;
            else
                goto FREENODE;
        }
        else if( dir == -1 )
        {
            if( iter->mRight )
            {
                dir = 0;
                iter = iter->mRight;
            }
            else
            {
                goto FREENODE;
            }
        }
        else if( dir == 1 )
        {
            goto FREENODE;
        }

        continue;

        FREENODE:

        if( iter == mRoot )
        {
            delete iter;
            break;
        }

        dir = iter->mParent->mLeft == iter ? -1 : +1;
        Node* next = iter->mParent;
        delete iter;
        iter = next;
    }

    mSize = 0;
    mRoot = nullptr;
}

Template
bool
AVLTree<TA>::validate()
const
{
    if( mRoot == nullptr )
        return mSize == 0;

    if( ! mRoot->validate() )
        return false;

    if( mSize != mRoot->subtreeSize() )
        return false;

    return true;
}

}

#undef Template
#undef TA

#endif /* XKIT_AVLTREE_HPP */
