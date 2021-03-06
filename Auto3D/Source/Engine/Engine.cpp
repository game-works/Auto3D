#include "Engine.h"

#include "../Window/Window.h"
#include "../Thread/Thread.h"

#include "../Audio/Audio.h"
#include "../Resource/ResourceCache.h"
#include "../Graphics/Graphics.h"
#include "../Renderer/Renderer.h"
#include "../Window/Input.h"
#include "../Debug/Log.h"
#include "../Debug/Profiler.h"
#include "../Time/Time.h"
#include "../RegisteredBox/RegisteredBox.h"
#include "../Script/Script.h"
#include "../Physics/Physics.h"
#include "../IO/FileSystem.h"
#include "../UI/UI.h"
#include "../Auto2D/Renderer2D.h"
#include "../UI/Canvas.h"
#include "../Scene/Scene.h"
#include "../Auto2D/Scene2D.h"
#include "../Base/ProcessUtils.h"
#include "../Debug/DebugNew.h"

namespace Auto3D
{

Engine::Engine():
	_exiting(false),
	_initialized(false),
	_timeStep(0.0f),
	_timeStepSmoothing(2),
	_minFps(10),
#if defined(IOS) || defined(TVOS) || defined(__ANDROID__) || defined(__arm__) || defined(__aarch64__)
	_maxFps(60),
	_maxInactiveFps(10),
	_pauseMinimized(true),
#else
	_maxFps(200),
	_maxInactiveFps(60),
	_pauseMinimized(false),
#endif
	_autoExit(true)
{
	RegisterGraphicsLibrary();
	RegisterResourceLibrary();
	RegisterRendererLibrary();
	RegisterRenderer2DLibrary();
	RegisterAudioLibrary();
	RegisterUILibrary();

	_cache = new ResourceCache();
	_cache->AddResourceDir(ExecutableDir() + "Data");

	_log = new Log();
	_input = new Input();
	_profiler = new Profiler();
	_graphics = new Graphics();
	_renderer = new Renderer();
	_time = new Time();
	_registeredBox = new RegisteredBox();
	_script = new Script();
	_renderer2d = new Renderer2D();
	_physics = new Physics();
	_fileSystem = new FileSystem();
	_ui = new UI();
}

Engine::~Engine()
{
}

bool Engine::Init()
{
	if (_initialized)
		return true;
	PROFILE(EngineInit);

	// Set random seeds based on time
	Time::RealTime& realTime = _time->GetRealTime();

	SetRandomSeed(((unsigned)(realTime._day & 0xff) << 24) |
		((unsigned)(realTime._hour & 0xff) << 16) |
		((unsigned)(realTime._minute & 0xff) << 8) |
		((unsigned)(realTime._second & 0xff)));

	if (!_graphics->SetMode(RectI(0, 0, 1024, 768), 4, false, true))
	{
		ErrorString("Failed to create a gutter.");
		return false;
	}
	// Set default Logo
	_graphics->RenderWindow()->SetIcon(_cache->LoadResource<Image>("NewLogo.png"));

	if (!_graphics->RenderWindow())
		return false;

#ifdef AUTO_OPENGL
	if (!_ui->SetMode(_graphics->RenderWindow(), _graphics->RenderContext()))
#else
	if (!_ui->SetMode(_graphics->RenderWindow()))
#endif
	{
		ErrorString("Failed to create a ui.");
		return false;
	}

	// Init FPU state of main thread
	InitFPU();

	_frameTimer.Reset();

	_initialized = true;

	return true;
}
void Engine::Exit()
{
	if (_autoExit)
	{
		DoExit();
	}
}

void Engine::Render()
{
	// Check renderer render Prepare
	if (!_graphics || !_renderer || !_renderer2d)
	{
		ErrorString("Fail to render,graphics or renderer missing!");
		return;
	}
	// Render scene
	for (auto it = _registeredBox->GetScenes().Begin(); it != _registeredBox->GetScenes().End(); it++)
	{
		Scene* scene = *it;
		Vector<Camera*>& cameras = scene->GetAllCamera();
		for (auto cameraIt = cameras.Begin(); cameraIt != cameras.End(); ++cameraIt)
		{
			Camera* camera = *cameraIt;
			_renderer->Render(scene, camera);
			// Update camera aspect ratio based on window size
			camera->SetAspectRatio((float)Subsystem<Graphics>()->GetWidth() / (float)Subsystem<Graphics>()->GetHeight());
		}
	}	

	// Render Renderer2D
	for (auto it = _registeredBox->GetScene2D().Begin(); it != _registeredBox->GetScene2D().End(); it++)
	{
		Scene2D* scene = *it;
		Vector<Camera2D*>& cameras = scene->GetAllCamera();
		for (auto cameraIt = cameras.Begin(); cameraIt != cameras.End(); ++cameraIt)
		{
			Camera2D* camera = *cameraIt;
			_renderer2d->Render(scene, camera);
		}
	}

	_ui->BeginUI();

	// Render UI
	for (auto it = _registeredBox->GetCanvas().Begin(); it != _registeredBox->GetCanvas().End(); it++)
	{
		if ((*it)->IsEnabled())
			_ui->Render(*it);
	}
	//Present ui and graphics
	_ui->Present();
	_graphics->Present();
}


bool Engine::Update()
{
	_profiler->BeginFrame();
	_time->Update();
	_input->Update();
	//If the window is minimized do not render
	if (_graphics->RenderWindow()->IsMinimized())
		return false;
	// If the window is not initialized successfully or shutdown engine is shutdown
	if (!_graphics->IsInitialized() || _graphics->RenderWindow()->IsClose())
	{
		ShutDownEngine();
		return false;
	}
	if(Subsystem<Audio>())
		Subsystem<Audio>()->Update();

	
	return true;
}
void Engine::FrameFinish()
{
	ApplyFrameLimit();
	_profiler->EndFrame();
}

void Engine::SetMinFps(int fps)
{
	_minFps = (unsigned)Max(fps, 0);
}

void Engine::SetMaxFps(int fps)
{
	_maxFps = (unsigned)Max(fps, 0);
}

void Engine::SetMaxInactiveFps(int fps)
{
	_maxInactiveFps = (unsigned)Max(fps, 0);
}

void Engine::SetTimeStepSmoothing(int frames)
{
	_timeStepSmoothing = (unsigned)Clamp(frames, 1, 20);
}

void Engine::SetPauseMinimized(bool enable)
{
	_pauseMinimized = enable;
}

void Engine::SetNextTimeStep(float seconds)
{
	_timeStep = Max(seconds, 0.0f);
}

void Engine::SetAutoExit(bool enable)
{
	// On mobile platforms exit is mandatory if requested by the platform itself and should not be attempted to be disabled
#if defined(__ANDROID__) || defined(IOS) || defined(TVOS)
	enable = true;
#endif
	_autoExit = enable;
}

void Engine::ApplyFrameLimit()
{
	if (!_initialized)
		return;

	unsigned maxFps = _maxFps;
	auto* input = Subsystem<Input>();
	if (input)
		maxFps = Min(_maxInactiveFps, maxFps);

	long long elapsed = 0;

#ifndef __EMSCRIPTEN__
	// Perform waiting loop if maximum FPS set
#if !defined(IOS) && !defined(TVOS)
	if (maxFps)
#else
	// If on iOS/tvOS and target framerate is 60 or above, just let the animation callback handle frame timing
	// instead of waiting ourselves
	if (maxFps < 60)
#endif
	{
		PROFILE(ApplyFrameLimit);

		long long targetMax = 1000000LL / maxFps;

		for (;;)
		{
			elapsed = _frameTimer.ElapsedUSec(false);
			if (elapsed >= targetMax)
				break;

			// Sleep if 1 ms or more off the frame limiting goal
			if (targetMax - elapsed >= 1000LL)
			{
				auto sleepTime = (unsigned)((targetMax - elapsed) / 1000LL);
				Thread::Sleep(sleepTime);
			}
		}
	}
#endif

	elapsed = _frameTimer.ElapsedUSec(true);

	// If FPS lower than minimum, clamp elapsed time
	if (_minFps)
	{
		long long targetMin = 1000000LL / _minFps;
		if (elapsed > targetMin)
			elapsed = targetMin;
	}

	// Perform timestep smoothing
	_timeStep = 0.0f;
	_lastTimeSteps.Push(elapsed / 1000000.0f);
	if (_lastTimeSteps.Size() > _timeStepSmoothing)
	{
		// If the smoothing configuration was changed, ensure correct amount of samples
		_lastTimeSteps.Erase(0, _lastTimeSteps.Size() - _timeStepSmoothing);
		for (unsigned i = 0; i < _lastTimeSteps.Size(); ++i)
			_timeStep += _lastTimeSteps[i];
		_timeStep /= _lastTimeSteps.Size();
	}
	else
		_timeStep = _lastTimeSteps.Back();
}

void Engine::DoExit()
{
	auto* graphics = Subsystem<Graphics>();
	if (graphics)
		graphics->Close();

	_exiting = true;
}

}
