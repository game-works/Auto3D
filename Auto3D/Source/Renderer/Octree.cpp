#include "../Debug/Log.h"
#include "../Math/Ray.h"
#include "Octree.h"

#include <cassert>

#include "../Debug/DebugNew.h"

namespace Auto3D
{

static const float DEFAULT_OCTREE_SIZE = 1000.0f;
static const int DEFAULT_OCTREE_LEVELS = 8;
static const int MAX_OCTREE_LEVELS = 256;

bool CompareRaycastResults(const RaycastResult& lhs, const RaycastResult& rhs)
{
    return lhs._distance < rhs._distance;
}

bool CompareNodeDistances(const Pair<OctreeNode*, float>& lhs, const Pair<OctreeNode*, float>& rhs)
{
    return lhs._second < rhs._second;
}

Octant::Octant() :
    _parent(nullptr),
    _numNodes(0)
{
    for (size_t i = 0; i < NUM_OCTANTS; ++i)
        _children[i] = nullptr;
}

void Octant::Initialize(Octant* parent, const BoundingBoxF& boundingBox, int level)
{
    _worldBoundingBox = boundingBox;
    _center = _worldBoundingBox.Center();
    _halfSize = _worldBoundingBox.HalfSize();
    _cullingBox = BoundingBoxF(_worldBoundingBox._min - _halfSize, _worldBoundingBox._max + _halfSize);
    _level = level;
    _parent = parent;
}

bool Octant::FitBoundingBox(const BoundingBoxF& box, const Vector3F& boxSize) const
{
    // If max split level, _size always OK, otherwise check that box is at least half _size of octant
    if (_level <= 1 || boxSize._x >= _halfSize._x || boxSize._y >= _halfSize._y || boxSize._z >= _halfSize._z)
        return true;
    // Also check if the box can not fit inside a child octant's culling box, in that case _size OK (must insert here)
    else
    {
        if (box._min._x <= _worldBoundingBox._min._x - 0.5f * _halfSize._x || box._min._y <= _worldBoundingBox._min._y - 0.5f * _halfSize._y ||
            box._min._z <= _worldBoundingBox._min._z - 0.5f * _halfSize._z || box._max._x >= _worldBoundingBox._max._x + 0.5f * _halfSize._x ||
            box._max._y >= _worldBoundingBox._max._y + 0.5f * _halfSize._y || box._max._z >= _worldBoundingBox._max._z + 0.5f * _halfSize._z)
            return true;
    }

    // Bounding box too small, should create a child octant
    return false;
}

Octree::Octree()
{
    _root.Initialize(nullptr, BoundingBoxF(-DEFAULT_OCTREE_SIZE, DEFAULT_OCTREE_SIZE), DEFAULT_OCTREE_LEVELS);
}

Octree::~Octree()
{
    DeleteChildOctants(&_root, true);
}

void Octree::RegisterObject()
{
    RegisterFactory<Octree>();
    CopyBaseAttributes<Octree, Node>();
    RegisterRefAttribute("boundingBox", &Octree::BoundingBoxAttr, &Octree::SetBoundingBoxAttr);
    RegisterAttribute("numLevels", &Octree::NumLevelsAttr, &Octree::SetNumLevelsAttr);
}

void Octree::Update()
{
    PROFILE(UpdateOctree);

    for (auto it = _updateQueue.Begin(); it != _updateQueue.End(); ++it)
    {
        OctreeNode* node = *it;
        // If node was removed before update could happen, a null pointer will be in its place
        if (node)
        {
            node->SetFlag(NF_OCTREE_UPDATE_QUEUED, false);

            // Do nothing if still fits the current octant
            const BoundingBoxF& box = node->WorldBoundingBox();
            Vector3F boxSize = box.Size();
            Octant* oldOctant = node->_octant;

		
            if (oldOctant && oldOctant->_cullingBox.IsInside(box) == INSIDE && oldOctant->FitBoundingBox(box, boxSize))
                continue;

            // Begin reinsert process. Start from root and check what level child needs to be used
            Octant* newOctant = &_root;
            Vector3F boxCenter = box.Center();

            for (;;)
            {
                bool insertHere;
                // If node does not fit fully inside root octant, must remain in it
                if (newOctant == &_root)
                    insertHere = newOctant->_cullingBox.IsInside(box) != INSIDE || newOctant->FitBoundingBox(box, boxSize);
                else
                    insertHere = newOctant->FitBoundingBox(box, boxSize);

                if (insertHere)
                {
                    if (newOctant != oldOctant)
                    {
                        // Add first, then remove, because node count going to zero deletes the octree branch in question
                        AddNode(node, newOctant);
                        if (oldOctant)
                            RemoveNode(node, oldOctant);
                    }
                    break;
                }
                else
                    newOctant = CreateChildOctant(newOctant, newOctant->ChildIndex(boxCenter));
            }
        }
    }

    _updateQueue.Clear();
}

void Octree::Resize(const BoundingBoxF& boundingBox, int numLevels)
{
    PROFILE(ResizeOctree);

    // Collect nodes to the root and delete all child octants
    _updateQueue.Clear();
    CollectNodes(_updateQueue, &_root);
    DeleteChildOctants(&_root, false);
    _allocator.Reset();
    _root.Initialize(nullptr, boundingBox, Clamp(numLevels, 1, MAX_OCTREE_LEVELS));

    // Reinsert all nodes (recreates new child octants as necessary)
    Update();
}

void Octree::RemoveNode(OctreeNode* node)
{
    assert(node);
    RemoveNode(node, node->_octant);
    if (node->TestFlag(NF_OCTREE_UPDATE_QUEUED))
        CancelUpdate(node);
    node->_octant = nullptr;
}

void Octree::QueueUpdate(OctreeNode* node)
{
    assert(node);
    _updateQueue.Push(node);
    node->SetFlag(NF_OCTREE_UPDATE_QUEUED, true);
}

void Octree::CancelUpdate(OctreeNode* node)
{
    assert(node);
    auto it = _updateQueue.Find(node);
    if (it != _updateQueue.End())
        *it = nullptr;
    node->SetFlag(NF_OCTREE_UPDATE_QUEUED, false);
}

void Octree::Raycast(Vector<RaycastResult>& result, const Ray& ray, unsigned short nodeFlags, float maxDistance, unsigned layerMask)
{
    PROFILE(OctreeRaycast);

    result.Clear();
    CollectNodes(result, &_root, ray, nodeFlags, maxDistance, layerMask);
    Sort(result.Begin(), result.End(), CompareRaycastResults);
}

RaycastResult Octree::RaycastSingle(const Ray& ray, unsigned short nodeFlags, float maxDistance, unsigned layerMask)
{
    PROFILE(OctreeRaycastSingle);

    // Get first the potential hits
    _initialRes.Clear();
    CollectNodes(_initialRes, &_root, ray, nodeFlags, maxDistance, layerMask);
    Sort(_initialRes.Begin(), _initialRes.End(), CompareNodeDistances);

    // Then perform actual per-node ray tests and early-out when possible
    _finalRes.Clear();
    float closestHit = M_INFINITY;
    for (auto it = _initialRes.Begin(); it != _initialRes.End(); ++it)
    {
        if (it->_second < Min(closestHit, maxDistance))
        {
            size_t oldSize = _finalRes.Size();
            it->_first->OnRaycast(_finalRes, ray, maxDistance);
            if (_finalRes.Size() > oldSize)
                closestHit = Min(closestHit, _finalRes.Back()._distance);
        }
        else
            break;
    }

    if (_finalRes.Size())
    {
        Sort(_finalRes.Begin(), _finalRes.End(), CompareRaycastResults);
        return _finalRes.Front();
    }
    else
    {
        RaycastResult emptyRes;
        emptyRes._position = emptyRes._normal = Vector3F::ZERO;
        emptyRes._distance = M_INFINITY;
        emptyRes._node = nullptr;
        emptyRes._subObject = 0;
        return emptyRes;
    }
}

void Octree::SetBoundingBoxAttr(const BoundingBoxF& boundingBox)
{
    _root._worldBoundingBox = boundingBox;
}

const BoundingBoxF& Octree::BoundingBoxAttr() const
{
    return _root._worldBoundingBox;
}

void Octree::SetNumLevelsAttr(int numLevels)
{
    /// Setting the number of level (last attribute) triggers octree resize when deserializing
    Resize(_root._worldBoundingBox, numLevels);
}

int Octree::NumLevelsAttr() const
{
    return _root._level;
}

void Octree::AddNode(OctreeNode* node, Octant* octant)
{
    octant->_nodes.Push(node);
    node->_octant = octant;

    // Increment the node count in the whole parent branch
    while (octant)
    {
        ++octant->_numNodes;
        octant = octant->_parent;
    }
}

void Octree::RemoveNode(OctreeNode* node, Octant* octant)
{
    // Do not set the node's octant pointer to zero, as the node may already be added into another octant
    octant->_nodes.Remove(node);
    
    // Decrement the node count in the whole parent branch and erase empty octants as necessary
    while (octant)
    {
        --octant->_numNodes;
        Octant* next = octant->_parent;
        if (!octant->_numNodes && next)
            DeleteChildOctant(next, next->ChildIndex(octant->_center));
        octant = next;
    }
}

Octant* Octree::CreateChildOctant(Octant* octant, size_t index)
{
    if (octant->_children[index])
        return octant->_children[index];

    Vector3F newMin = octant->_worldBoundingBox._min;
    Vector3F newMax = octant->_worldBoundingBox._max;
    const Vector3F& oldCenter = octant->_center;

    if (index & 1)
        newMin._x = oldCenter._x;
    else
        newMax._x = oldCenter._x;

    if (index & 2)
        newMin._y = oldCenter._y;
    else
        newMax._y = oldCenter._y;

    if (index & 4)
        newMin._z = oldCenter._z;
    else
        newMax._z = oldCenter._z;

    Octant* child = _allocator.Allocate();
    child->Initialize(octant, BoundingBoxF(newMin, newMax), octant->_level - 1);
    octant->_children[index] = child;

    return child;
}

void Octree::DeleteChildOctant(Octant* octant, size_t index)
{
    _allocator.Free(octant->_children[index]);
    octant->_children[index] = nullptr;
}

void Octree::DeleteChildOctants(Octant* octant, bool deletingOctree)
{
    for (auto it = octant->_nodes.Begin(); it != octant->_nodes.End(); ++it)
    {
        OctreeNode* node = *it;
        node->_octant = nullptr;
        node->SetFlag(NF_OCTREE_UPDATE_QUEUED, false);
        if (deletingOctree)
            node->_octree = nullptr;
    }
    octant->_nodes.Clear();
    octant->_numNodes = 0;

    for (size_t i = 0; i < NUM_OCTANTS; ++i)
    {
        if (octant->_children[i])
        {
            DeleteChildOctants(octant->_children[i], deletingOctree);
            octant->_children[i] = nullptr;
        }
    }

    if (octant != &_root)
        _allocator.Free(octant);
}

void Octree::CollectNodes(Vector<OctreeNode*>& result, const Octant* octant) const
{
    result.Push(octant->_nodes);

    for (size_t i = 0; i < NUM_OCTANTS; ++i)
    {
        if (octant->_children[i])
            CollectNodes(result, octant->_children[i]);
    }
}

void Octree::CollectNodes(Vector<OctreeNode*>& result, const Octant* octant, unsigned short nodeFlags, unsigned layerMask) const
{
    const Vector<OctreeNode*>& octantNodes = octant->_nodes;
    for (auto it = octantNodes.Begin(); it != octantNodes.End(); ++it)
    {
        OctreeNode* node = *it;
        if ((node->Flags() & nodeFlags) == nodeFlags && (node->GetLayerMask() & layerMask))
            result.Push(node);
    }

    for (size_t i = 0; i < NUM_OCTANTS; ++i)
    {
        if (octant->_children[i])
            CollectNodes(result, octant->_children[i], nodeFlags, layerMask);
    }
}

void Octree::CollectNodes(Vector<RaycastResult>& result, const Octant* octant, const Ray& ray, unsigned short nodeFlags, 
    float maxDistance, unsigned layerMask) const
{
    float octantDist = ray.HitDistance(octant->_cullingBox);
    if (octantDist >= maxDistance)
        return;

    const Vector<OctreeNode*>& octantNodes = octant->_nodes;
    for (auto it = octantNodes.Begin(); it != octantNodes.End(); ++it)
    {
        OctreeNode* node = *it;
        if ((node->Flags() & nodeFlags) == nodeFlags && (node->GetLayerMask() & layerMask))
            node->OnRaycast(result, ray, maxDistance);
    }

    for (size_t i = 0; i < NUM_OCTANTS; ++i)
    {
        if (octant->_children[i])
            CollectNodes(result, octant->_children[i], ray, nodeFlags, maxDistance, layerMask);
    }
}

void Octree::CollectNodes(Vector<Pair<OctreeNode*, float> >& result, const Octant* octant, const Ray& ray, unsigned short nodeFlags,
    float maxDistance, unsigned layerMask) const
{
    float octantDist = ray.HitDistance(octant->_cullingBox);
    if (octantDist >= maxDistance)
        return;

    const Vector<OctreeNode*>& octantNodes = octant->_nodes;
    for (auto it = octantNodes.Begin(); it != octantNodes.End(); ++it)
    {
        OctreeNode* node = *it;
        if ((node->Flags() & nodeFlags) == nodeFlags && (node->GetLayerMask() & layerMask))
        {
            float distance = ray.HitDistance(node->WorldBoundingBox());
            if (distance < maxDistance)
                result.Push(MakePair(node, distance));
        }
    }

    for (size_t i = 0; i < NUM_OCTANTS; ++i)
    {
        if (octant->_children[i])
            CollectNodes(result, octant->_children[i], ray, nodeFlags, maxDistance, layerMask);
    }
}

}
