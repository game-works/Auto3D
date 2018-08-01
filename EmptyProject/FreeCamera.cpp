#include "FreeCamera.h"
#include "BaseSpace.h"
#include "Time.h"
#include "GameObjectManager.h"
#include "Camera.h"
#include "Light.h"
#include "Input.h"


void FreeCamera::processInput()
{

	if (GetSubSystem<Input>()->GetKeyPress(SDLK_w))
		freeCamera->ProcessKeyboard(FORWARD, GetSubSystem<Time>()->GetDeltaTime());
	if (GetSubSystem<Input>()->GetKeyPress(SDLK_s))
		freeCamera->ProcessKeyboard(BACKWARD, GetSubSystem<Time>()->GetDeltaTime());
	if (GetSubSystem<Input>()->GetKeyPress(SDLK_a))
		freeCamera->ProcessKeyboard(LEFT, GetSubSystem<Time>()->GetDeltaTime());
	if (GetSubSystem<Input>()->GetKeyPress(SDLK_d))
		freeCamera->ProcessKeyboard(RIGHT, GetSubSystem<Time>()->GetDeltaTime());
	if (GetSubSystem<Input>()->IsMouseMove())
	{
		freeCamera->ProcessMouseMovement(GetSubSystem<Input>()->GetMouseMove().x, GetSubSystem<Input>()->GetMouseMove().y);
	}
	freeCamera->ProcessMouseScroll(GetSubSystem<Input>()->GetMouseWheelOffset());
}


FreeCamera::FreeCamera(Ambient* ambient)
	:ScriptComponent(ambient)
{
}
FreeCamera::~FreeCamera()
{
}

void FreeCamera::Start()
{
	freeCamera = new Camera(_ambient);
	freeCamera->SetFar(1000.0f);
	freeCameraObject = new GameObject(_ambient);
	freeCameraObject->GetComponent(Transform).SetPosition(0.0f, 0.0f, 3.0f);

	freeCameraObject->AddComponent(freeCamera);

	GetSubSystem<Input>()->ShowCursor(false);
	GetSubSystem<Input>()->LockCursorInCenter();

}
void FreeCamera::Update()
{
	processInput();

}
