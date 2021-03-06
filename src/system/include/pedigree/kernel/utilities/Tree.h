/*
 * Copyright (c) 2008-2014, Pedigree Developers
 *
 * Please see the CONTRIB file in the root of the source tree for a full
 * list of contributors.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef KERNEL_UTILITIES_TREE_H
#define KERNEL_UTILITIES_TREE_H

#include "pedigree/kernel/compiler.h"
#include "pedigree/kernel/processor/types.h"
#include "pedigree/kernel/utilities/Iterator.h"

/** @addtogroup kernelutilities
 * @{ */

/** Dictionary class, aka Map. This is implemented as an AVL self-balancing
 * binary search tree. \brief A key/value dictionary. */
template <class K, class E>
class EXPORTED_PUBLIC Tree
{
  private:
    /** Tree node. */
    struct Node
    {
        K key;
        E element;
        struct Node *leftChild = nullptr;
        struct Node *rightChild = nullptr;
        struct Node *parent = nullptr;
        size_t height = 0;
    };

  public:
    /// \todo This will actually mean for each Tree you can only use one
    /// iterator at a time, which may
    ///       not be effective depending on how this is used.
    class IteratorNode
    {
      public:
        IteratorNode() : value(0), pNode(0), pPreviousNode(0)
        {
        }
        IteratorNode(Node *node, Node *prev, size_t n)
            : value(node), pNode(node), pPreviousNode(prev)
        {
            // skip the root node, get to the lowest node in the tree
            if (n > 1)
                traverseNext();
            value = pNode;
        }

        IteratorNode *next()
        {
            traverseNext();

            value = pNode;

            return this;
        }
        IteratorNode *previous()
        {
            return 0;
        }

        void reset(Node *node, Node *prev, size_t n)
        {
            value = pNode = node;
            pPreviousNode = prev;
            if (n > 1)
                traverseNext();
            value = pNode;
        }

        Node *value;

      private:
        Node *pNode;
        Node *pPreviousNode;

        void traverseNext()
        {
            if (pNode == 0)
                return;

            if ((pPreviousNode == pNode->parent) && pNode->leftChild)
            {
                pPreviousNode = pNode;
                pNode = pNode->leftChild;
                traverseNext();
            }
            else if (
                (((pNode->leftChild) && (pPreviousNode == pNode->leftChild)) ||
                 ((!pNode->leftChild) && (pPreviousNode != pNode))) &&
                (pPreviousNode != pNode->rightChild))
            {
                pPreviousNode = pNode;
            }
            else if ((pPreviousNode == pNode) && pNode->rightChild)
            {
                pPreviousNode = pNode;
                pNode = pNode->rightChild;
                traverseNext();
            }
            else
            {
                pPreviousNode = pNode;
                pNode = pNode->parent;
                traverseNext();
            }
        }
    };

    // typedef void**        Iterator;

    typedef ::TreeIterator<
        E, IteratorNode, &IteratorNode::previous, &IteratorNode::next, K>
        Iterator;
    /** Constant random-access iterator for the Tree */
    typedef E const *ConstIterator;

    /** The default constructor, does nothing */
    Tree() : root(0), nItems(0), m_Begin(0)
    {
    }

    /** The copy-constructor
     *\param[in] x the reference object to copy */
    Tree(const Tree &x) : root(0), nItems(0), m_Begin(0)
    {
        copyFrom(x);
    }

    /** The destructor, deallocates memory */
    ~Tree()
    {
        clear();
        delete m_Begin;
    }

    /** The assignment operator
     *\param[in] x the object that should be copied */
    Tree &operator=(const Tree &x)
    {
        copyFrom(x);

        return *this;
    }

    /** Get the number of elements in the Tree
     *\return the number of elements in the Tree */
    size_t count() const
    {
        return nItems;
    }

    /** Add an element to the Tree.
     *\param[in] key the key
     *\param[in] value the element */
    void insert(const K &key, const E &value)
    {
        Node *insertionNode = createInsertionNode(key);
        insertionNode->element = value;
        ++nItems;
    }

    /** Move an element into the Tree.
     *\param[in] key the key
     *\param[in] value the element */
    void insert(const K &key, E &&value)
    {
        Node *insertionNode = createInsertionNode(key);
        insertionNode->element = pedigree_std::move(value);
        ++nItems;
    }

    /** Attempts to find an element with the given key.
     *\return the element found, or NULL if not found. */
    E lookup(const K &key) const
    {
        Node *n = root;
        while (n != 0)
        {
            if (n->key == key)
                return n->element;
            else if (n->key > key)
                n = n->leftChild;
            else
                n = n->rightChild;
        }
        return 0;
    }

    /** Attempts to find an element with the given key.
     *\return a reference to the element found. */
    const E &lookupRef(const K &key, const E &failed=E()) const
    {
        Node *n = root;
        while (n != 0)
        {
            if (n->key == key)
                return n->element;
            else if (n->key > key)
                n = n->leftChild;
            else
                n = n->rightChild;
        }
        return failed;
    }

    /** Reports whether a given key exists in the tree.
     *\return true if the key exists, false otherwise. */
    bool contains(const K &key) const
    {
        Node *n = root;
        while (n != 0)
        {
            if (n->key == key)
                return true;
            else if (n->key > key)
                n = n->leftChild;
            else
                n = n->rightChild;
        }
        return false;
    }

    /** Attempts to remove an element with the given key. */
    void remove(const K &key)
    {
        Node *n = root;
        while (n != 0)
        {
            if (n->key == key)
                break;
            else if (n->key > key)
                n = n->leftChild;
            else
                n = n->rightChild;
        }

        Node *orign = n;
        if (n == 0)
            return;

        while (n->leftChild || n->rightChild)  // While n is not a leaf.
        {
            size_t hl = height(n->leftChild);
            size_t hr = height(n->rightChild);
            if (hl == 0)
                rotateLeft(n);  // N is now a leaf.
            else if (hr == 0)
                rotateRight(n);  // N is now a leaf.
            else if (hl <= hr)
            {
                rotateRight(n);
                rotateLeft(n);  // These are NOT inverse operations -
                                // rotateRight changes n's position.
            }
            else
            {
                rotateLeft(n);
                rotateRight(n);
            }
        }

        // N is now a leaf, so can be easily pruned.
        if (n->parent == 0)
            root = 0;
        else
        {
            if (n->parent->leftChild == n)
                n->parent->leftChild = 0;
            else
                n->parent->rightChild = 0;
        }

        // Work our way up the path, balancing.
        while (n)
        {
            int b = balanceFactor(n);
            if ((b < -1) || (b > 1))
                rebalanceNode(n);
            n = n->parent;
        }

        delete orign;
        nItems--;
    }

    /** Clear the Vector */
    void clear()
    {
        traverseNode_Remove(root);
        root = 0;
        nItems = 0;

        delete m_Begin;
        m_Begin = 0;
    }

    /** Erase one Element */
    void erase(Iterator iter)
    {
        // Remove the key from the tree.
        remove(iter.key());

        // Passed iterator is now invalid.
    }

    /** Get an iterator pointing to the beginning of the Vector
     *\return iterator pointing to the beginning of the Vector */
    Iterator begin()
    {
        // If there is no node already, create a new one
        if (!m_Begin)
            m_Begin = new IteratorNode(root, 0, nItems);

        // Reset the iterator node
        else
            m_Begin->reset(root, 0, nItems);
        // m_Begin = new (static_cast<void*>(m_Begin)) IteratorNode(root, 0,
        // nItems);

        return Iterator(m_Begin);
    }
    /** Get a constant iterator pointing to the beginning of the Vector
     *\return constant iterator pointing to the beginning of the Vector */
    ConstIterator begin() const
    {
        return 0;
    }
    /** Get an iterator pointing to the last element + 1
     *\return iterator pointing to the last element + 1 */
    Iterator end()
    {
        return Iterator(0);
    }
    /** Get a constant iterator pointing to the last element + 1
     *\return constant iterator pointing to the last element + 1 */
    ConstIterator end() const
    {
        return 0;
    }

  private:
    void copyFrom(const Tree &other)
    {
        clear();
        // Traverse the tree, adding everything encountered.
        traverseNode_Insert(other.root);

        if (m_Begin)
            delete m_Begin;
        m_Begin = new IteratorNode(root, 0, nItems);
    }

    void rotateLeft(Node *n)
    {
        // See Cormen,Lieserson,Rivest&Stein  pp-> 278 for pseudocode.
        Node *y = n->rightChild;  // Set Y.

        n->rightChild =
            y->leftChild;  // Turn Y's left subtree into N's right subtree.
        if (y->leftChild != 0)
            y->leftChild->parent = n;

        y->parent = n->parent;  // Link Y's parent to N's parent.
        if (n->parent == 0)
            root = y;
        else if (n == n->parent->leftChild)
            n->parent->leftChild = y;
        else
            n->parent->rightChild = y;
        y->leftChild = n;
        n->parent = y;
    }

    void rotateRight(Node *n)
    {
        Node *y = n->leftChild;

        n->leftChild = y->rightChild;
        if (y->rightChild != 0)
            y->rightChild->parent = n;

        y->parent = n->parent;
        if (n->parent == 0)
            root = y;
        else if (n == n->parent->leftChild)
            n->parent->leftChild = y;
        else
            n->parent->rightChild = y;

        y->rightChild = n;
        n->parent = y;
    }

    size_t height(Node *n)
    {
        // Assumes: n's children's heights are up to date. Will always be true
        // if balanceFactor
        //          is called in a bottom-up fashion.
        if (n == 0)
            return 0;

        size_t tempL = 0;
        size_t tempR = 0;

        if (n->leftChild != 0)
            tempL = n->leftChild->height;
        if (n->rightChild != 0)
            tempR = n->rightChild->height;

        tempL++;  // Account for the height increase stepping up to us, its
                  // parent.
        tempR++;

        if (tempL > tempR)  // If one is actually bigger than the other, return
                            // that, else return the other.
        {
            n->height = tempL;
            return tempL;
        }
        else
        {
            n->height = tempR;
            return tempR;
        }
    }

    int balanceFactor(Node *n)
    {
        return static_cast<int>(height(n->rightChild)) -
               static_cast<int>(height(n->leftChild));
    }

    void rebalanceNode(Node *n)
    {
        // This way of choosing which rotation to do took me AGES to find...
        // See
        // http://www.cmcrossroads.com/bradapp/ftp/src/libs/C++/AvlTrees.html
        int balance = balanceFactor(n);
        if (balance < -1)  // If it's left imbalanced, we need a right rotation.
        {
            if (balanceFactor(n->leftChild) >
                0)  // If its left child is right heavy...
            {
                // We need a RL rotation - left rotate n's left child, then
                // right rotate N.
                rotateLeft(n->leftChild);
                rotateRight(n);
            }
            else
            {
                // RR rotation will do.
                rotateRight(n);
            }
        }
        else if (balance > 1)
        {
            if (balanceFactor(n->rightChild) <
                0)  // If its right child is left heavy...
            {
                // We need a LR rotation; Right rotate N's right child, then
                // left rotate N.
                rotateRight(n->rightChild);
                rotateLeft(n);
            }
            else
            {
                // LL rotation.
                rotateLeft(n);
            }
        }
    }

    void traverseNode_Insert(Node *n)
    {
        if (!n)
            return;
        insert(n->key, n->element);
        traverseNode_Insert(n->leftChild);
        traverseNode_Insert(n->rightChild);
    }

    void traverseNode_Remove(Node *n)
    {
        if (!n)
            return;

        Node *left = n->leftChild;
        Node *right = n->rightChild;
        n->leftChild = nullptr;
        n->rightChild = nullptr;

        traverseNode_Remove(left);
        traverseNode_Remove(right);
        delete n;
    }

    Node *createInsertionNode(const K &key)
    {
        Node *insertionNode = nullptr;

        if (root == 0)
        {
            insertionNode = new Node;
            insertionNode->key = key;

            root = insertionNode;  // We are the root node.

            if (m_Begin)
            {
                delete m_Begin;
            }
            m_Begin = new IteratorNode(root, 0, nItems);
        }
        else
        {
            // Traverse the tree.
            Node *currentNode = root;

            bool inserted = false;
            while (!inserted)
            {
                if (key > currentNode->key)
                {
                    if (currentNode->rightChild ==
                        0)  // We have found our insert point.
                    {
                        insertionNode = new Node;
                        insertionNode->key = key;
                        insertionNode->parent = currentNode;
                        currentNode->rightChild = insertionNode;
                        inserted = true;
                    }
                    else
                    {
                        currentNode = currentNode->rightChild;
                    }
                }
                else if (key == currentNode->key)
                {
                    // overwrite existing value
                    insertionNode = currentNode;
                    inserted = true;
                }
                else
                {
                    if (currentNode->leftChild ==
                        0)  // We have found our insert point.
                    {
                        insertionNode = new Node;
                        insertionNode->key = key;
                        insertionNode->parent = currentNode;
                        currentNode->leftChild = insertionNode;
                        inserted = true;
                    }
                    else
                    {
                        currentNode = currentNode->leftChild;
                    }
                }
            }

            // The value has been inserted, but has that messed up the balance
            // of the tree?
            while (currentNode)
            {
                int b = balanceFactor(currentNode);
                if ((b < -1) || (b > 1))
                {
                    rebalanceNode(currentNode);
                }
                currentNode = currentNode->parent;
            }
        }

        return insertionNode;
    }

    Node *root;
    size_t nItems;

    IteratorNode *m_Begin;
};

// External specializations.
extern template class Tree<void *, void *>;      // IWYU pragma: keep
extern template class Tree<int8_t, void *>;      // IWYU pragma: keep
extern template class Tree<int16_t, void *>;     // IWYU pragma: keep
extern template class Tree<int32_t, void *>;     // IWYU pragma: keep
extern template class Tree<int64_t, void *>;     // IWYU pragma: keep
extern template class Tree<uint8_t, void *>;     // IWYU pragma: keep
extern template class Tree<uint16_t, void *>;    // IWYU pragma: keep
extern template class Tree<uint32_t, void *>;    // IWYU pragma: keep
extern template class Tree<uint64_t, void *>;    // IWYU pragma: keep
extern template class Tree<int8_t, int8_t>;      // IWYU pragma: keep
extern template class Tree<int16_t, int16_t>;    // IWYU pragma: keep
extern template class Tree<int32_t, int32_t>;    // IWYU pragma: keep
extern template class Tree<int64_t, int64_t>;    // IWYU pragma: keep
extern template class Tree<uint8_t, uint8_t>;    // IWYU pragma: keep
extern template class Tree<uint16_t, uint16_t>;  // IWYU pragma: keep
extern template class Tree<uint32_t, uint32_t>;  // IWYU pragma: keep
extern template class Tree<uint64_t, uint64_t>;  // IWYU pragma: keep

/** @} */

#endif
