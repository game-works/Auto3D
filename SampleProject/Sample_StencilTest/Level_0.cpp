#include "Level_0.h"
#include "Node.h"
#include "Light.h"
#include "MeshRenderer.h"
#include "../FreeCamera.h"
#include "ResourceSystem.h"

void Level_0::Start()
{
	auto cube = GetSubSystem<ResourceSystem>()->GetResource<Mesh>("object/base/Cube.3DS");

	GameNode lightObj = CreateNode();
	lightObj->SetPosition(2.0f, 5.0f, 0.0f);
	auto light = lightObj->CreateComponent<Light>();
	light->SetType(LightType::Directional);
	/////////////////////////////////////////////////////////////////////////////////////////////
	GameNode camObj = CreateNode();
	SharedPtr<FreeCamera> freeCamera = MakeShared<FreeCamera>(_ambient);
	camObj->AddComponent(freeCamera);
	freeCamera->cameraNode->SetPosition(0.0f, 0.0f, 10.0f);
	/////////////////////////////////////////////////////////////////////////////////////////////
	GameNode obj1 = CreateNode();
	auto plane = obj1->CreateComponent<MeshRenderer>();
	plane->SetMesh(cube);
	plane->GetMaterial()->color.Set(0.5f, 0.5f, 0.5f);
	obj1->SetScale(10.0f, 0.1f, 10.0f);
	/////////////////////////////////////////////////////////////////////////////////////////////
	GameNode obj2 = CreateNode();
	obj2->SetPosition(1.0f, 0.5f, 3.0f);
	auto box1 = obj2->CreateComponent<MeshRenderer>();
	box1->SetMesh(cube);
	box1->EnableStencil(true);
	box1->StencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	box1->StencilFunc(GL_ALWAYS, 1, 0xFF);
	box1->StencilMask(0xFF);
	box1->GetMaterial()->color.Set(1.0f, 0.0f, 0.5f);
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	GameNode obj3 = CreateNode();
	obj3->SetPosition(1.0f, 0.5f, 3.0f);
	auto box2 = obj3->CreateComponent<MeshRenderer>();
	box2->EnableStencil(true);
	box2->StencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	box2->StencilFunc(GL_NOTEQUAL, 1, 0xFF);
	box2->StencilMask(0x00);
	obj3->SetScale(1.1f, 1.1f, 1.1f);

}

void Level_0::Update()
{
}

