#ifndef KH_STL_TYPE_UNORDERED_MULTI_SET_H_
#define KH_STL_TYPE_UNORDERED_MULTI_SET_H_

#include "../../Utility/TypeHashBase.h"
#include "../../Utility/TypeIterator.h"
#include "../../Utility/TypeHash.h"

namespace KhSTL {


template<typename _Kty>
class tUnorderedMultiSet : public tHashBase
{
public:
	using KeyType = _Kty;

	using ValueType = _Kty;

	using Node = tHashNode<_Kty>;

	using Iterator = tHashIterator<_Kty>;

	using ConstIterator = tConstHashIterator<_Kty>;
	/// Hash map reverse iterator
	using ReverseIterator = tReverseIterator<Iterator>;
	/// Hash map const reverse iterator
	using ConstReverseIterator = tReverseIterator<ConstIterator>;
public:

	/**
	* @brief : Construct Empty
	*/
	tUnorderedMultiSet()
	{
		// Reserve the tail node
		_allocator = AllocatorInitialize((unsigned)sizeof(Node));
		_head = _tail = reserveNode();
	}
	/**
	* @brief : Construct from another hash set
	*/
	tUnorderedMultiSet(const tUnorderedMultiSet<_Kty>& set)
	{
		// Reserve the tail node + initial capacity according to the set's size
		_allocator = AllocatorInitialize((unsigned)sizeof(Node), set.Size() + 1);
		_head = _tail = reserveNode();
		*this = set;
	}
	/**
	* @brief : Move-construct from another hash set
	*/
	tUnorderedMultiSet(tUnorderedMultiSet<_Kty> && set) noexcept
	{
		Swap(set);
	}
	/**
	* @brief : Aggregate initialization constructor
	*/
	tUnorderedMultiSet(const std::initializer_list<_Kty>& list) : tUnorderedMultiSet()
	{
		for (auto it = list.begin(); it != list.end(); it++)
		{
			Insert(*it);
		}
	}
	/**
	* @brief : Destruct
	*/
	~tUnorderedMultiSet()
	{
		if (_allocator)
		{
			Clear();
			freeNode(tail());
			AllocatorUninitialize(_allocator);
			delete[] _ptrs;
		}
	}
	/**
	* @brief : Assign a hash set
	*/
	tUnorderedMultiSet& operator =(const tUnorderedMultiSet<_Kty>& rhs)
	{
		// In case of This-assignment do nothing
		if (&rhs != this)
		{
			Clear();
			Insert(rhs);
		}
		return *this;
	}
	/**
	* @brief : Move-assign a hash set
	*/
	tUnorderedMultiSet& operator =(tUnorderedMultiSet<_Kty> && rhs) noexcept
	{
		assert(&rhs != this);
		Swap(rhs);
		return *this;
	}
	/**
	* @brief : Add-assign a value
	*/
	tUnorderedMultiSet& operator +=(const _Kty& rhs)
	{
		Insert(rhs);
		return *this;
	}
	/**
	* @brief : Add-assign a hash set
	*/
	tUnorderedMultiSet& operator +=(const tUnorderedMultiSet<_Kty>& rhs)
	{
		Insert(rhs);
		return *this;
	}
	/**
	* @brief : Test for equality with another hash set
	*/
	bool operator ==(const tUnorderedMultiSet<_Kty>& rhs) const
	{
		if (rhs.Size() != Size())
			return false;

		ConstIterator it = Begin();
		while (it != End())
		{
			if (!rhs.Contains(*it))
				return false;
			++it;
		}

		return true;
	}
	/**
	* @brief : Test for inequality with another hash set
	*/
	bool operator !=(const tUnorderedMultiSet<_Kty>& rhs) const
	{
		if (rhs.Size() != Size())
			return true;

		ConstIterator it = Begin();
		while (it != End())
		{
			if (!rhs.Contains(*it))
				return true;
			++it;
		}

		return false;
	}
	/**
	* @brief : Insert a key. Return an iterator to it
	*/
	Iterator Insert(const _Kty& key,bool findExisting = false)
	{
		// If no pointers yet, allocate with minimum bucket count
		if (!_ptrs)
		{
			allocateBuckets(Size(), MIN_BUCKETS);
			reHash();
		}

		unsigned hashKey = Hash(key);

		if (findExisting)
		{
			Node* existing = findNode(key, hashKey);
			if (existing)
				return Iterator(existing);
		}

		Node* newNode = insertNode(tail(), key);
		newNode->down = ptrs()[hashKey];
		ptrs()[hashKey] = newNode;

		// Rehash if the maximum load factor has been exceeded
		if (Size() > NumBuckets() * MAX_LOAD_FACTOR)
		{
			allocateBuckets(Size(), NumBuckets() << 1);
			reHash();
		}

		return Iterator(newNode);
	}
	/**
	* @brief : Insert a key. Return an iterator and set exists
	*			flag according to whether the key already existed
	*/
	Iterator Insert(const _Kty& key, bool& exists)
	{
		unsigned oldSize = Size();
		Iterator ret = Insert(key);
		exists = (Size() == oldSize);
		return ret;
	}
	/**
	* @brief : Insert a set
	*/
	void Insert(const tUnorderedMultiSet<_Kty>& set)
	{
		ConstIterator it = set.Begin();
		ConstIterator end = set.End();
		while (it != end)
			Insert(*it++);
	}
	/**
	* @brief : Insert a key by iterator. Return iterator to the value
	*/
	Iterator Insert(const ConstIterator& it)
	{
		return Iterator(InsertNode(*it));
	}
	/**
	* @brief : Erase a key. Return true if was found
	*/
	bool Erase(const _Kty& key)
	{
		if (!_ptrs)
			return false;

		unsigned hashKey = Hash(key);

		Node* previous;
		Node* node = findNode(key, hashKey, previous);
		if (!node)
			return false;

		if (previous)
			previous->down = node->down;
		else
			ptrs()[hashKey] = node->down;

		eraseNode(node);
		return true;
	}
	/**
	* @brief : Erase a key by iterator. Return iterator to the next key
	*/
	Iterator Erase(const Iterator& it)
	{
		if (!_ptrs || !it._ptr)
			return End();

		auto* node = static_cast<Node*>(it._ptr);
		Node* next = node->Next();

		unsigned hashKey = Hash(node->data);

		Node* previous = 0;
		auto* current = static_cast<Node*>(ptrs()[hashKey]);
		while (current && current != node)
		{
			previous = current;
			current = current->Down();
		}

		assert(current == node);

		if (previous)
			previous->down = node->down;
		else
			ptrs()[hashKey] = node->down;

		eraseNode(node);
		return Iterator(next);
	}
	/**
	* @brief : Clear the set
	*/
	void Clear()
	{
		// Prevent Find() from returning anything while the map is being cleared
		resetPtrs();

		if (Size())
		{
			for (Iterator i = Begin(); i != End();)
			{
				freeNode(static_cast<Node*>(i++.ptr));
				i.ptr->prev = 0;
			}

			_head = _tail;
			setSize(0);
		}
	}
	/**
	* @brief : Sort keys. After sorting the set can be iterated in order
	*			until new elements are inserted
	*/
	void Sort()
	{
		unsigned numKeys = Size();
		if (!numKeys)
			return;

		auto** ptrs = new Node*[numKeys];
		Node* ptr = head();

		for (unsigned i = 0; i < numKeys; ++i)
		{
			ptrs[i] = ptr;
			ptr = ptr->Next();
		}

		Sort(tIterator<Node*>(ptrs), tIterator<Node*>(ptrs + numKeys), compareNodes);

		_head = ptrs[0];
		ptrs[0]->prev = 0;
		for (unsigned i = 1; i < numKeys; ++i)
		{
			ptrs[i - 1]->next = ptrs[i];
			ptrs[i]->prev = ptrs[i - 1];
		}
		ptrs[numKeys - 1]->next = _tail;
		_tail->prev = ptrs[numKeys - 1];

		delete[] ptrs;
	}
	/**
	* @brief : Rehash to a specific bucket count, which must be a
	*			power of two. Return true if successful
	*/
	bool Rehash(unsigned numBuckets)
	{
		if (numBuckets == NumBuckets())
			return true;
		if (!numBuckets || numBuckets < Size() / MAX_LOAD_FACTOR)
			return false;

		// Check for being power of two
		unsigned check = numBuckets;
		while (!(check & 1u))
			check >>= 1;
		if (check != 1)
			return false;

		allocateBuckets(Size(), numBuckets);
		Rehash();
		return true;
	}
	/**
	* @brief : Return iterator to the key, or end iterator if not found
	*/
	Iterator Find(const _Kty& key)
	{
		if (!_ptrs)
			return End();

		unsigned hashKey = Hash(key);
		Node* node = findNode(key, hashKey);
		if (node)
			return Iterator(node);
		else
			return End();
	}
	/**
	* @brief : Return const iterator to the key, or end iterator if not found
	*/
	ConstIterator Find(const _Kty& key) const
	{
		if (!_ptrs)
			return End();

		unsigned hashKey = Hash(key);
		Node* node = findNode(key, hashKey);
		if (node)
			return ConstIterator(node);
		else
			return End();
	}
	/**
	* @brief : Return whether contains a key
	*/
	bool Contains(const _Kty& key) const
	{
		if (!_ptrs)
			return false;

		unsigned hashKey = Hash(key);
		return findNode(key, hashKey) != 0;
	}
	/**
	* @brief : Return iterator to the beginning
	*/
	Iterator Begin() { return Iterator(head()); }
	/**
	* @brief : Return iterator to the beginning
	*/
	ConstIterator Begin() const { return ConstIterator(head()); }
	/**
	* @brief : Return iterator to the end
	*/
	Iterator End() { return Iterator(tail()); }
	/**
	* @brief : Return iterator to the end
	*/
	ConstIterator End() const { return ConstIterator(tail()); }
	/**
	* @brief : Return first key
	*/
	const _Kty& Front() const { return *Begin(); }
	/**
	* @brief : Return last key
	*/
	const _Kty& Back() const { return *(--End()); }

private:
	/**
	* @brief : Return the head node
	*/
	Node* head() const { return static_cast<Node*>(_head); }
	/**
	* @brief : Return the tail node
	*/
	Node* tail() const { return static_cast<Node*>(_tail); }
	/**
	* @brief : Find a node from the buckets. Do not call if
	*			the buckets have not been allocated
	*/
	Node* findNode(const _Kty& key, unsigned hashKey) const
	{
		auto* node = static_cast<Node*>(ptrs()[hashKey]);
		while (node)
		{
			if (node->data == key)
				return node;
			node = node->Down();
		}

		return 0;
	}
	/**
	* @brief : Find a node and the previous node from the buckets.
	*			Do not call if the buckets have not been allocated
	*/
	Node* findNode(const _Kty& key, unsigned hashKey, Node*& previous) const
	{
		previous = 0;

		auto* node = static_cast<Node*>(ptrs()[hashKey]);
		while (node)
		{
			if (node->data == key)
				return node;
			previous = node;
			node = node->Down();
		}

		return 0;
	}
	/**
	* @brief : Insert a node into the list. Return the new node
	*/
	Node* insertNode(Node* dest, const _Kty& key)
	{
		if (!dest)
			return 0;

		Node* newNode = reserveNode(key);
		Node* prev = dest->Prev();
		newNode->next = dest;
		newNode->prev = prev;
		if (prev)
			prev->next = newNode;
		dest->prev = newNode;

		// Reassign the head node if necessary
		if (dest == head())
			_head = newNode;

		setSize(Size() + 1);

		return newNode;
	}
	/**
	* @brief : Erase a node from the list. Return pointer to the next element,
	*				or to the end if could not erase
	*/
	Node* eraseNode(Node* node)
	{
		// The tail node can not be removed
		if (!node || node == _tail)
			return tail();

		Node* prev = node->Prev();
		Node* next = node->Next();
		if (prev)
			prev->next = next;
		next->prev = prev;

		// Reassign the head node if necessary
		if (node == head())
			_head = next;

		freeNode(node);
		setSize(Size() - 1);

		return next;
	}
	/**
	* @brief : Reserve a node
	*/
	Node* reserveNode()
	{
		auto* newNode = static_cast<Node*>(AllocatorReserve(_allocator));
		new(newNode) Node();
		return newNode;
	}
	/**
	* @brief : Reserve a node with specified key
	*/
	Node* reserveNode(const _Kty& key)
	{
		auto* newNode = static_cast<Node*>(AllocatorReserve(_allocator));
		new(newNode) Node(key);
		return newNode;
	}
	/**
	* @brief : Free a node
	*/
	void freeNode(Node* node)
	{
		(node)->~Node();
		AllocatorFree(_allocator, node);
	}
	/**
	* @brief : Rehash the buckets
	*/
	void reHash()
	{
		for (Iterator it = Begin(); it != End(); ++it)
		{
			auto* node = static_cast<Node*>(it.ptr);
			unsigned hashKey = Hash(*it);
			node->down = ptrs()[hashKey];
			ptrs()[hashKey] = node;
		}
	}
	/**
	* @brief : Compare two nodes
	*/
	static bool compareNodes(Node*& lhs, Node*& rhs) { return lhs->data < rhs->data; }
	/**
	* @brief : Compute a hash based on the key and the bucket size
	*/
	unsigned Hash(const _Kty& key) const { return MakeHash(key) & (NumBuckets() - 1); }
};
template <typename _Kty> typename tUnorderedMultiSet<_Kty>::ConstIterator begin(const tUnorderedMultiSet<_Kty>& v) { return v.Begin(); }

template <typename _Kty> typename tUnorderedMultiSet<_Kty>::ConstIterator end(const tUnorderedMultiSet<_Kty>& v) { return v.End(); }

template <typename _Kty> typename tUnorderedMultiSet<_Kty>::Iterator begin(tUnorderedMultiSet<_Kty>& v) { return v.Begin(); }

template <typename _Kty> typename tUnorderedMultiSet<_Kty>::Iterator end(tUnorderedMultiSet<_Kty>& v) { return v.End(); }

template <typename _Kty> typename tUnorderedMultiSet<_Kty>::ConstReverseIterator rbegin(const tUnorderedMultiSet<_Kty>& v) { return v.RBegin(); }

template <typename _Kty> typename tUnorderedMultiSet<_Kty>::ConstReverseIterator rend(const tUnorderedMultiSet<_Kty>& v) { return v.REnd(); }

template <typename _Kty> typename tUnorderedMultiSet<_Kty>::ReverseIterator rbegin(tUnorderedMultiSet<_Kty>& v) { return v.RBegin(); }

template <typename _Kty> typename tUnorderedMultiSet<_Kty>::ReverseIterator rend(tUnorderedMultiSet<_Kty>& v) { return v.REnd(); }

}
#endif // !KH_STL_TYPE_UNORDERED_MULTI_SET_H_