#pragma once
#include "GameManager.h"
#include "Singleton.h"
#include "Prefab.h"
namespace Auto3D {
class PreLoadManager : public GlobalGameManager
{
	REGISTER_DERIVED_CLASS(PreLoadManager, GlobalGameManager);
	DECLARE_OBJECT_SERIALIZE(PreLoadManager);
	using PreContainer = LIST<Prefab*>;
public:
	explicit PreLoadManager(Ambient* ambient);
	void AddPrefab(Prefab* prefab);
	void RemovePrefab(Prefab* prefab);
	PreContainer GetPrefabs() { return _prefabs; }
private:
	PreContainer _prefabs;
};
}
