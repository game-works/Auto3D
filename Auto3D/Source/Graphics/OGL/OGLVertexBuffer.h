#pragma once

#include "../../Base/AutoPtr.h"
#include "../../Base/Vector.h"
#include "../GPUObject.h"
#include "../GraphicsDefs.h"

namespace Auto3D
{

/// GPU buffer for vertex data.
class AUTO_API VertexBuffer : public RefCounted, public GPUObject
{
public:
    /// Construct.
    VertexBuffer();
    /// Destruct.
    ~VertexBuffer();

    /// Release the vertex buffer and CPU shadow data.
    void Release() override;
    /// Recreate the GPU resource after data loss.
    void Recreate() override;

    /// Define buffer. Immutable buffers must specify initial data here. Return true on success.
    bool Define(ResourceUsage usage, size_t numVertices, const Vector<VertexElement>& elements, bool useShadowData, const void* data = nullptr);
    /// Define buffer. Immutable buffers must specify initial data here. Return true on success.
    bool Define(ResourceUsage usage, size_t numVertices, size_t numElements, const VertexElement* elements, bool useShadowData, const void* data = nullptr);
    /// Redefine buffer data either completely or partially. Not supported for immutable buffers. Return true on success.
    bool SetData(size_t firstVertex, size_t numVertices, const void* data);

    /// Return CPU-side shadow data if exists.
    unsigned char* ShadowData() const { return _shadowData.Get(); }
    /// Return number of vertices.
    size_t NumVertices() const { return _numVertices; }
    /// Return number of vertex elements.
    size_t NumElements() const { return _elements.Size(); }
    /// Return vertex elements.
    const Vector<VertexElement>& Elements() const { return _elements; }
    /// Return _size of vertex in bytes.
    size_t VertexSize() const { return _vertexSize; }
    /// Return vertex declaration hash code.
    unsigned ElementHash() const { return _elementHash; }
    /// Return resource usage type.
    ResourceUsage Usage() const { return _usage; }
    /// Return whether is dynamic.
    bool IsDynamic() const { return _usage == USAGE_DYNAMIC; }
    /// Return whether is immutable.
    bool IsImmutable() const { return _usage == USAGE_IMMUTABLE; }

    /// Return the OpenGL buffer identifier. Used internally and should not be called by portable application code.
    unsigned GLBuffer() const { return _buffer; }

    /// Compute the hash code of one vertex element by index and semantic.
    static unsigned ElementHash(size_t index, ElementSemantic semantic) { return (semantic + 1) << (index * 3); }

    /// Vertex element D3D11 format by element type.
    static const unsigned elementFormats[];
    /// Vertex element semantic names.
    static const char* elementSemanticNames[];

private:
    /// Create the GPU-side vertex buffer. Return true on success.
    bool Create(const void* data);

    /// OpenGL buffer object identifier.
    unsigned _buffer;
    /// CPU-side shadow data.
    AutoArrayPtr<unsigned char> _shadowData;
    /// Number of vertices.
    size_t _numVertices;
    /// Size of vertex in bytes.
    size_t _vertexSize;
    /// Vertex elements.
    Vector<VertexElement> _elements;
    /// Vertex element hash code.
    unsigned _elementHash;
    /// Resource usage type.
    ResourceUsage _usage;
};

}