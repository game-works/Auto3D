#pragma once

#include "Allocator.h"
#include "Pair.h"
#include "Sort.h"
#include "Vector.h"

namespace Auto3D
{

/// Hash map template class.
template <class _Ty, class U> class HashMap : public HashBase
{
public:
    /// Hash map _key-value pair with const _key.
    class KeyValue
    {
    public:
        /// Default-construct.
        KeyValue() :
            first(_Ty())
        {
        }
        
        /// Construct with _key and value.
        KeyValue(const _Ty& key, const U& value) :
            first(key),
            second(value)
        {
        }
        
        /// Test for equality with another pair.
        bool operator == (const KeyValue& rhs) const { return first == rhs.first && second == rhs.second; }
        /// Test for inequality with another pair.
        bool operator != (const KeyValue& rhs) const { return !(*this == rhs); }
        
        /// Key.
        const _Ty first;
        /// Value.
        U second;

    private:
        /// Prevent copy construction.
        KeyValue(const KeyValue& rhs);
        /// Prevent assignment.
        KeyValue& operator = (const KeyValue& rhs);
    };
    
    /// Hash map node.
    struct Node : public HashNodeBase
    {
        /// Construct undefined.
        Node()
        {
        }
        
        /// Construct with _key and value.
        Node(const _Ty& key, const U& value) :
            pair(key, value)
        {
        }
        
        /// Key-value pair.
        KeyValue pair;
        
        /// Return next node.
        Node* Next() const { return static_cast<Node*>(next); }
        /// Return previous node.
        Node* Prev() const { return static_cast<Node*>(prev); }
        /// Return next node in the bucket.
        Node* Down() const { return static_cast<Node*>(down); }
    };
    
    /// Hash map node iterator.
    struct Iterator : public HashIteratorBase
    {
        /// Construct.
        Iterator()
        {
        }
        
        /// Construct with a node pointer.
        Iterator(Node* ptr) :
            HashIteratorBase(ptr)
        {
        }
        
        /// Preincrement the pointer.
        Iterator& operator ++ () { GotoNext(); return *this; }
        /// Postincrement the pointer.
        Iterator operator ++ (int) { Iterator it = *this; GotoNext(); return it; }
        /// Predecrement the pointer.
        Iterator& operator -- () { GotoPrev(); return *this; }
        /// Postdecrement the pointer.
        Iterator operator -- (int) { Iterator it = *this; GotoPrev(); return it; }
        
        /// Point to the pair.
        KeyValue* operator -> () const { return &(static_cast<Node*>(ptr))->pair; }
        /// Dereference the pair.
        KeyValue& operator * () const { return (static_cast<Node*>(ptr))->pair; }
    };
    
    /// Hash map node const iterator.
    struct ConstIterator : public HashIteratorBase
    {
        /// Construct.
        ConstIterator()
        {
        }
        
        /// Construct with a node pointer.
        ConstIterator(Node* ptr) :
            HashIteratorBase(ptr)
        {
        }
        
        /// Construct from a non-const iterator.
        ConstIterator(const Iterator& rhs) :
            HashIteratorBase(rhs.ptr)
        {
        }
        
        /// Assign from a non-const iterator.
        ConstIterator& operator = (const Iterator& rhs) { ptr = rhs.ptr; return *this; }
        /// Preincrement the pointer.
        ConstIterator& operator ++ () { GotoNext(); return *this; }
        /// Postincrement the pointer.
        ConstIterator operator ++ (int) { ConstIterator it = *this; GotoNext(); return it; }
        /// Predecrement the pointer.
        ConstIterator& operator -- () { GotoPrev(); return *this; }
        /// Postdecrement the pointer.
        ConstIterator operator -- (int) { ConstIterator it = *this; GotoPrev(); return it; }
        
        /// Point to the pair.
        const KeyValue* operator -> () const { return &(static_cast<Node*>(ptr))->pair; }
        /// Dereference the pair.
        const KeyValue& operator * () const { return (static_cast<Node*>(ptr))->pair; }
    };
    
    /// Construct empty.
    HashMap()
    {
    }
    
    /// Construct from another hash map.
    HashMap(const HashMap<_Ty, U>& map)
    {
        Initialize(map.NumBuckets(), map.Size() + 1);
        *this = map;
    }
    
    /// Destruct.
    ~HashMap()
    {
        if (ptrs && allocator)
        {
            Clear();
            FreeNode(Tail());
            AllocatorUninitialize(allocator);
        }
    }
    
    /// Assign a hash map.
    HashMap& operator = (const HashMap<_Ty, U>& rhs)
    {
        if (&rhs != this)
        {
            Clear();
            Insert(rhs);
        }
        return *this;
    }
    
    /// Add-assign a pair.
    HashMap& operator += (const Pair<_Ty, U>& rhs)
    {
        Insert(rhs);
        return *this;
    }
    
    /// Add-assign a hash map.
    HashMap& operator += (const HashMap<_Ty, U>& rhs)
    {
        Insert(rhs);
        return *this;
    }
    
    /// Test for equality with another hash map.
    bool operator == (const HashMap<_Ty, U>& rhs) const
    {
        if (rhs.Size() != Size())
            return false;
        
        for (ConstIterator it = Begin(); it != End(); ++it)
        {
            ConstIterator rhsIt = rhs.Find(it->first);
            if (rhsIt == rhs.End() || rhsIt->second != it->second)
                return false;
        }
        
        return true;
    }
    
    /// Test for inequality with another hash map.
    bool operator != (const HashMap<_Ty, U>& rhs) const { return !(*this == rhs); }

    /// Index the map. Create a new pair if _key not found.
    U& operator [] (const _Ty& key)
    {
        return InsertNode(key)->pair.second;
    }
    
    /// Insert a pair. Return an iterator to it.
    Iterator Insert(const Pair<_Ty, U>& pair)
    {
        return Iterator(InsertNode(pair.first, pair.second));
    }
    
    /// Insert a map.
    void Insert(const HashMap<_Ty, U>& map)
    {
        for (ConstIterator it = map.Begin(); it != map.End(); ++it)
            InsertNode(it->first, it->second);
    }
    
    /// Insert a pair by iterator. Return iterator to the value.
    Iterator Insert(const ConstIterator& it) { return Iterator(InsertNode(it->first, it->second)); }
    
    /// Insert a range by iterators.
    void Insert(const ConstIterator& start, const ConstIterator& end)
    {
        ConstIterator it = start;
        while (it != end)
            Insert(*it++);
    }
    
    /// Erase a pair by _key. Return true if was found.
    bool Erase(const _Ty& key)
    {
        if (!ptrs)
            return false;
        
        unsigned hashKey = Hash(key);
        
        Node* previous;
        Node* node = FindNode(key, hashKey, previous);
        if (!node)
            return false;
        
        if (previous)
            previous->down = node->down;
        else
            Ptrs()[hashKey] = node->down;
        
        EraseNode(node);
        return true;
    }
    
    /// Erase a pair by iterator. Return iterator to the next pair.
    Iterator Erase(const Iterator& it)
    {
        if (!ptrs || !it.ptr)
            return End();
        
        Node* node = static_cast<Node*>(it.ptr);
        Node* next = node->Next();
        
        unsigned hashKey = Hash(node->pair.first);
        
        Node* previous = nullptr;
        Node* current = static_cast<Node*>(Ptrs()[hashKey]);
        while (current && current != node)
        {
            previous = current;
            current = current->Down();
        }
        
        assert(current == node);
        
        if (previous)
            previous->down = node->down;
        else
            Ptrs()[hashKey] = node->down;
        
        EraseNode(node);
        return Iterator(next);
    }
    
    /// Clear the map.
    void Clear()
    {
        if (Size())
        {
            for (Iterator it = Begin(); it != End();)
                FreeNode(static_cast<Node*>(it++.ptr));

            Node* tail = Tail();
            tail->prev = nullptr;
            SetHead(tail);
            SetSize(0);
            ResetPtrs();
        }
    }
    
    /// Sort pairs. After sorting the map can be iterated in order until new elements are inserted.
    void Sort()
    {
        size_t numKeys = Size();
        if (!numKeys)
            return;
        
        Node** ptrs = new Node*[numKeys];
        Node* ptr = Head();
        
        for (size_t i = 0; i < numKeys; ++i)
        {
            ptrs[i] = ptr;
            ptr = ptr->Next();
        }
        
        Auto3D::Sort(RandomAccessIterator<Node*>(ptrs), RandomAccessIterator<Node*>(ptrs + numKeys), CompareNodes);
        
        SetHead(ptrs[0]);
        ptrs[0]->prev = nullptr;
        for (size_t i = 1; i < numKeys; ++i)
        {
            ptrs[i - 1]->next = ptrs[i];
            ptrs[i]->prev = ptrs[i - 1];
        }
        ptrs[numKeys - 1]->next = Tail();
        Tail()->prev = ptrs[numKeys - 1];
        
        delete[] ptrs;
    }
    
    /// Rehash to a specific bucket count, which must be a power of two. Return true on success.
    bool Rehash(size_t numBuckets)
    {
        if (numBuckets == NumBuckets())
            return true;
        if (!numBuckets || numBuckets < Size() / MAX_LOAD_FACTOR)
            return false;
        
        // Check for being power of two
        size_t check = numBuckets;
        while (!(check & 1))
            check >>= 1;
        if (check != 1)
            return false;
        
        AllocateBuckets(Size(), numBuckets);
        Rehash();
        return true;
    }
    
    /// Return iterator to the pair with _key, or end iterator if not found.
    Iterator Find(const _Ty& key)
    {
        if (!ptrs)
            return End();
        
        unsigned hashKey = Hash(key);
        Node* node = FindNode(key, hashKey);
        if (node)
            return Iterator(node);
        else
            return End();
    }
    
    /// Return const iterator to the pair with _key, or end iterator if not found.
    ConstIterator Find(const _Ty& key) const
    {
        if (!ptrs)
            return End();
        
        unsigned hashKey = Hash(key);
        Node* node = FindNode(key, hashKey);
        if (node)
            return ConstIterator(node);
        else
            return End();
    }
    
    /// Return whether contains a pair with _key.
    bool Contains(const _Ty& key) const
    {
        if (!ptrs)
            return false;
        
        unsigned hashKey = Hash(key);
        return FindNode(key, hashKey) != nullptr;
    }
    
    /// Return all the keys.
    Vector<_Ty> Keys() const
    {
        Vector<_Ty> result;
        result.Reserve(Size());
        for (ConstIterator it = Begin(); it != End(); ++it)
            result.Push(it->first);
        return result;
    }

    /// Return all the values.
    Vector<U> Values() const
    {
        Vector<U> result;
        result.Reserve(Size());
        for (ConstIterator it = Begin(); it != End(); ++it)
            result.Push(it->second);
        return result;
    }

    /// Return iterator to the first element. Is not the lowest _key unless the map has been sorted.
    Iterator Begin() { return Iterator(Head()); }
    /// Return const iterator to the beginning.
    ConstIterator Begin() const { return ConstIterator(Head()); }
    /// Return iterator to the end.
    Iterator End() { return Iterator(Tail()); }
    /// Return const iterator to the end.
    ConstIterator End() const { return ConstIterator(Tail()); }
    /// Return first keyvalue. Is not the lowest _key unless the map has been sorted.
    const _Ty& Front() const { return *Begin(); }
    /// Return last keyvalue.
    const _Ty& Back() const { assert(Size()); return *(--End()); }
    
private:
    /// Return head node with correct type.
    Node* Head() const { return static_cast<Node*>(HashBase::Head()); }
    /// Return tail node with correct type.
    Node* Tail() const { return static_cast<Node*>(HashBase::Tail()); }

    /// Reserve the tail node and initial buckets.
    void Initialize(size_t numBuckets, size_t numNodes)
    {
        AllocateBuckets(0, numBuckets);
        allocator = AllocatorInitialize(sizeof(Node), numNodes);
        HashNodeBase* tail = AllocateNode();
        SetHead(tail);
        SetTail(tail);
    }
    
    /// Find a node from the buckets.
    Node* FindNode(const _Ty& key, unsigned hashKey) const
    {
        if (!ptrs)
            return nullptr;

        Node* node = static_cast<Node*>(Ptrs()[hashKey]);
        while (node)
        {
            if (node->pair.first == key)
                return node;
            node = node->Down();
        }
        
        return nullptr;
    }
    
    /// Find a node and the previous node from the buckets.
    Node* FindNode(const _Ty& key, unsigned hashKey, Node*& previous) const
    {
        previous = nullptr;
        if (!ptrs)
            return nullptr;

        Node* node = static_cast<Node*>(Ptrs()[hashKey]);
        while (node)
        {
            if (node->pair.first == key)
                return node;
            previous = node;
            node = node->Down();
        }
        
        return nullptr;
    }
    
    /// Insert a _key and default value and return either the new or existing node.
    Node* InsertNode(const _Ty& key)
    {
        unsigned hashKey = Hash(key);

        // If exists, just return the node
        Node* existing = FindNode(key, hashKey);
        if (existing)
            return existing;

        Node* newNode = InsertNode(Tail(), key, U());
        newNode->down = Ptrs()[hashKey];
        Ptrs()[hashKey] = newNode;

        // Rehash if the maximum load factor has been exceeded
        if (Size() > NumBuckets() * MAX_LOAD_FACTOR)
        {
            AllocateBuckets(Size(), NumBuckets() << 1);
            Rehash();
        }

        return newNode;
    }

    /// Insert a _key and value and return either the new or existing node.
    Node* InsertNode(const _Ty& key, const U& value)
    {
        unsigned hashKey = Hash(key);
        
        // If exists, just change the value
        Node* existing = FindNode(key, hashKey);
        if (existing)
        {
            existing->pair.second = value;
            return existing;
        }
        
        Node* newNode = InsertNode(Tail(), key, value);
        newNode->down = Ptrs()[hashKey];
        Ptrs()[hashKey] = newNode;
        
        // Rehash if the maximum load factor has been exceeded
        if (Size() > NumBuckets() * MAX_LOAD_FACTOR)
        {
            AllocateBuckets(Size(), NumBuckets() << 1);
            Rehash();
        }
        
        return newNode;
    }
    
    /// Allocate and insert a node into the list. Return the new node.
    Node* InsertNode(Node* dest, const _Ty& key, const U& value)
    {
        // If no pointers yet, allocate with minimum bucket count
        if (!ptrs)
        {
            Initialize(MIN_BUCKETS, 2);
            dest = Head();
        }
        
        Node* newNode = AllocateNode(key, value);
        Node* prev = dest->Prev();
        newNode->next = dest;
        newNode->prev = prev;
        if (prev)
            prev->next = newNode;
        dest->prev = newNode;
        
        // Reassign the head node if necessary
        if (dest == Head())
            SetHead(newNode);
        
        SetSize(Size() + 1);
        
        return newNode;
    }
    
    /// Erase a node from the list. Return pointer to the next element, or to the end if could not erase.
    Node* EraseNode(Node* node)
    {
        // The tail node can not be removed
        if (!node || node == Tail())
            return Tail();
        
        Node* prev = node->Prev();
        Node* next = node->Next();
        if (prev)
            prev->next = next;
        next->prev = prev;
        
        // Reassign the head node if necessary
        if (node == Head())
            SetHead(next);
        
        FreeNode(node);
        SetSize(Size() - 1);
        
        return next;
    }
    
    /// Allocate a node with optionally specified _key and value.
    Node* AllocateNode(const _Ty& key = _Ty(), const U& value = U())
    {
        Node* newNode = static_cast<Node*>(AllocatorGet(allocator));
        new(newNode) Node(key, value);
        return newNode;
    }
    
    /// Free a node.
    void FreeNode(Node* node)
    {
        (node)->~Node();
        AllocatorFree(allocator, node);
    }
    
    /// Rehash the buckets.
    void Rehash()
    {
        for (Iterator it = Begin(); it != End(); ++it)
        {
            Node* node = static_cast<Node*>(it.ptr);
            unsigned hashKey = Hash(it->first);
            node->down = Ptrs()[hashKey];
            Ptrs()[hashKey] = node;
        }
    }
    
    /// Compare two nodes.
    static bool CompareNodes(Node*& lhs, Node*& rhs) { return lhs->pair.first < rhs->pair.first; }
    
    /// Compute a hash based on the _key and the bucket _size
    unsigned Hash(const _Ty& key) const { return MakeHash(key) & (NumBuckets() - 1); }
};

}
