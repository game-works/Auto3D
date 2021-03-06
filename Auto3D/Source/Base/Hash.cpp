#include "Hash.h"
#include "Swap.h"

#include <cassert>

#include "../Debug/DebugNew.h"

namespace Auto3D
{

void HashBase::Swap(HashBase& hash)
{
    Auto3D::Swap(_ptrs, hash._ptrs);
    Auto3D::Swap(_allocator, hash._allocator);
}

void HashBase::AllocateBuckets(size_t _size, size_t numBuckets)
{
    assert(numBuckets >= MIN_BUCKETS);

    // Remember old head & tail pointers
    HashNodeBase* head = Head();
    HashNodeBase* tail = Tail();
    delete[] _ptrs;

    HashNodeBase** newPtrs = new HashNodeBase*[numBuckets + 4];
    size_t* _data = reinterpret_cast<size_t*>(newPtrs);
    _data[0] = _size;
    _data[1] = numBuckets;
    newPtrs[2] = head;
    newPtrs[3] = tail;
    _ptrs = newPtrs;
    
    ResetPtrs();
}

void HashBase::ResetPtrs()
{
    if (_ptrs)
    {
        size_t numBuckets = NumBuckets();
        HashNodeBase** _data = Ptrs();
        for (size_t i = 0; i < numBuckets; ++i)
            _data[i] = nullptr;
    }
}

template<> void Swap<HashBase>(HashBase& first, HashBase& second)
{
    first.Swap(second);
}

}
