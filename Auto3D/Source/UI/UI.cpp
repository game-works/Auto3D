#include "UI.h"
#include "Canvas.h"
#include "Text.h"
#include "Button.h"
#include "Slider.h"
#include "ColorEdit.h"
#include "CheckBox.h"

#include <imgui.h>

#include "../Adapter/imgui_impl_sdl.h"
#include "../Adapter/imgui_impl_opengl3.h"

#include "../Graphics/Graphics.h"

#include "../Debug/Log.h"

namespace Auto3D
{

UI::UI() :
	_window(nullptr)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsYellow();

	RegisterSubsystem(this);
}

UI::~UI()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	RemoveSubsystem(this);
}

#ifdef AUTO_OPENGL
bool UI::SetMode(Window* window, GLContext* context)
#else
bool UI::SetMode(SDL_Window* window)
#endif
{
	if (!window)
		return false;

	_window = window;
	const char* glslVersion = Subsystem<Graphics>()->GetGraphicsGLSLVersion().CString();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window->Handle(), context->Context());
	ImGui_ImplOpenGL3_Init(glslVersion);
	return true;
}

bool UI::BeginUI()
{
	if (!_window)
	{
		ErrorString("Fail update new frame from ui,may bo window not create.");
		return false;
	}
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(_window->Handle());
	ImGui::NewFrame();
	return true;
}

void UI::Present()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::Render(Canvas* canvas)
{
	if (!canvas)
	{
		ErrorString("Fail render canvas,maybe canvas fail create");
		return;
	}
		
	const char* title = canvas->GetTitle().CString();
	
	if (canvas->GetCloseButtonEnable())
	{
		bool state;
		ImGui::Begin(title, &state);
		canvas->SetCloseState(state);
	}	
	else
		ImGui::Begin(title);

	const Vector<SharedPtr<UINode> >& nodes = canvas->Children();
	for (auto it = nodes.Begin(); it != nodes.End(); ++it)
	{
		UINode* node = *it;
		if(node->IsEnabled())
			node->DefineNode();
	}

	ImGui::End();
}

void UI::ProcessEvent(const SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

void RegisterUILibrary()
{
	static bool registered = false;
	if (registered)
		return;
	registered = true;

	Canvas::RegisterObject();
	Text::RegisterObject();
	Button::RegisterObject();
	Slider::RegisterObject();
	ColorEdit::RegisterObject();
	CheckBox::RegisterObject();
}

}