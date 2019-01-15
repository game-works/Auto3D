#include "WorkSapce.h"
#include "Config.h"
#include "ResourceSystem.h"
#include "FileSystem.h"
#include "level_0.h"

AUTO_APPLICATION_MAIN(WorkSapce)

void WorkSapce::Init()
{
	STRING ResourceDir = system_content_dictionary;
	GetSubSystem<ResourceSystem>()->AddResourceDir(ResourceDir);
	GetSubSystem<Scene>()->RegisterScene(MakeShared<Level_0>(_ambient, 0));
}
void WorkSapce::Start()
{
}
void WorkSapce::Stop()
{
}