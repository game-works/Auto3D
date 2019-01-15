#pragma once
#include "GameManager.h"
#include "Prefab.h"
namespace Auto3D {
class PreLoadManager : public GlobalGameManager
{
	REGISTER_OBJECT_CLASS(PreLoadManager, GlobalGameManager)

public:
	explicit PreLoadManager(SharedPtr<Ambient> ambient);
	void AddPrefab(Prefab* prefab);
	void RemovePrefab(Prefab* prefab);
	LIST<Prefab*> GetPrefabs() { return _prefabs; }
private:
	LIST<Prefab*> _prefabs;
};
}
