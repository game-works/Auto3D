#include "Allocator.h"
#include "Vector.h"

#include "../Debug/DebugNew.h"

namespace Auto3D
{

VectorBase::VectorBase() :
    buffer(nullptr)
{
}

void VectorBase::Swap(VectorBase& vector)
{
    Auto3D::Swap(buffer, vector.buffer);
}

unsigned char* VectorBase::AllocateBuffer(size_t size)
{
    // Include space for size and capacity
    return new unsigned char[size + 2 * sizeof(size_t)];
}

template<> void Swap<VectorBase>(VectorBase& first, VectorBase& second)
{
    first.Swap(second);
}

}
