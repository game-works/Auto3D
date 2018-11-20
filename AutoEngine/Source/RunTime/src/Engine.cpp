#include "Engine.h"
#include "../../EngineSetting/Optimize.h"

#include "Input.h"
#include "Renderer.h"
#include "Graphics.h"
#include "Audio.h"
#include "BaseSpace.h"
#include "Time.h"
#include "Script.h"
#include "Resource.h"
#include "Scene.h"
#include "IO.h"
#include "UI.h"
#include "Behavior.h"

namespace Auto3D {

Engine::Engine(Ambient* ambient)
	:Super(ambient)
{
	_ambient->RegisterSubSystem(new Renderer(_ambient));
	_ambient->RegisterSubSystem(new Graphics(_ambient));
	_ambient->RegisterSubSystem(new BaseSpace(_ambient));
	_ambient->RegisterSubSystem(new Time(_ambient));
	_ambient->RegisterSubSystem(new Input(_ambient));
	_ambient->RegisterSubSystem(new Script(_ambient));
	_ambient->RegisterSubSystem(new Resource(_ambient));
	_ambient->RegisterSubSystem(new Scene(_ambient));
	_ambient->RegisterSubSystem(new IO(_ambient));
	_ambient->RegisterSubSystem(new Audio(_ambient));
	_ambient->RegisterSubSystem(new UI(_ambient));
	_ambient->RegisterSubSystem(new Behavior(_ambient));
}

Engine::~Engine()
{
}

void Engine::Init()
{
	GetSubSystem<Audio>()->Init();
	GetSubSystem<BaseSpace>()->Awake();
	GetSubSystem<Behavior>()->Awake();

	GetSubSystem<Renderer>()->Init();
	GetSubSystem<BaseSpace>()->Start();
	GetSubSystem<Behavior>()->Start();

	GetSubSystem<Renderer>()->ReadyToRender();
	
}
void Engine::RunFrame()
{
	update();
	render();
	frameFinish();
}
void Engine::Exit() 
{
	GetSubSystem<Graphics>()->DestoryWindow();
}
void Engine::render()
{
	auto* graphics = GetSubSystem<Graphics>();
	if (!graphics->BeginFrame())
		return;
	GetSubSystem<Renderer>()->Render();
	graphics->EndFrame();
}
void Engine::update()
{
	auto* input = GetSubSystem<Input>();
	GetSubSystem<Time>()->Update();
	input->Update();
	GetSubSystem<BaseSpace>()->Update();
	GetSubSystem<Behavior>()->Update();
	if (input->GetKeyDown(KEY_ESCAPE))
		_isExiting = true;
}
void Engine::frameFinish()
{
	GetSubSystem<BaseSpace>()->Finish();
	GetSubSystem<Behavior>()->Finish();
	GetSubSystem<Input>()->EndFrame();
}


}