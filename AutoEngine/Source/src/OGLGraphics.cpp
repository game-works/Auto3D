#include "Graphics.h"
#if AUTO_OPENGL
#include "AutoOGL.h"
#include "AutoImage.h"
#include "ResourceSystem.h"
#include "Image.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "Image.h"
#include "ConstantBuffer.h"

#include "NewDef.h"

namespace Auto3D {

static const unsigned glCmpFunc[] =
{
	GL_ALWAYS,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_LESS,
	GL_LEQUAL,
	GL_GREATER,
	GL_GEQUAL
};

static const unsigned glChangeMode[] =
{
	GL_STATIC_DRAW,
	GL_DYNAMIC_DRAW,
	GL_STREAM_DRAW
};

static const unsigned glBufferMode[] =
{
	GL_ARRAY_BUFFER ,
	GL_ELEMENT_ARRAY_BUFFER
};

static const unsigned glElementTypes[] =
{
	GL_INT,
	GL_FLOAT,
	GL_UNSIGNED_BYTE
};
static const unsigned glElementComponents[] =
{
	1,
	1,
	2,
	3,
	4,
	4,
	4
};

static const unsigned glSrcBlend[] =
{
	GL_ONE,
	GL_ONE,
	GL_DST_COLOR,
	GL_SRC_ALPHA,
	GL_SRC_ALPHA,
	GL_ONE,
	GL_ONE_MINUS_DST_ALPHA,
	GL_ONE,
	GL_SRC_ALPHA
};

static const unsigned glDestBlend[] =
{
	GL_ZERO,
	GL_ONE,
	GL_ZERO,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_ONE,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE,
	GL_ONE
};

static const unsigned glBlendOp[] =
{
	GL_FUNC_ADD,
	GL_FUNC_ADD,
	GL_FUNC_ADD,
	GL_FUNC_ADD,
	GL_FUNC_ADD,
	GL_FUNC_ADD,
	GL_FUNC_ADD,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT
};

static const unsigned glStencilOps[] =
{
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR_WRAP,
	GL_DECR_WRAP
};

void APIENTRY glDebugOutput(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	void *userParam)
{
	// ignore non-significant error/warning codes
	if (
		id == 131169 || id == 131185 || id == 131218 || id == 131204 || id || // driver-specific non-significant error codes
		id == 2000 || id == 2001 || id == 2265 // shader compilation error codes; ignore as already managed from shader object
		)
	{
		return;
	}

	std::string logMessage = "Debug output: (" + std::to_string(id) + "): " + message + "\n";

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             logMessage += "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   logMessage += "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: logMessage += "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     logMessage += "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     logMessage += "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           logMessage += "Source: Other"; break;
	} logMessage += "\n";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               logMessage += "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: logMessage += "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  logMessage += "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         logMessage += "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         logMessage += "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              logMessage += "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          logMessage += "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           logMessage += "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               logMessage += "Type: Other"; break;
	} logMessage += "\n";

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         logMessage += "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       logMessage += "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          logMessage += "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: logMessage += "Severity: notification"; break;
	} logMessage += "\n";
	logMessage += "\n";

	// only log a message a maximum of 3 times (as it'll keep spamming the message queue with
	// the same error message)
	static unsigned int msgCount = 0;
	if (msgCount++ < 3)
	{
		WarningString(logMessage);
	}
}

static void GetGLPrimitiveType(unsigned elementCount, PrimitiveType type, unsigned& primitiveCount, GLenum& glPrimitiveType)
{
	switch (type)
	{
	case PrimitiveType::TringleList:
		primitiveCount = elementCount / 3;
		glPrimitiveType = GL_TRIANGLES;
		break;

	case PrimitiveType::LineList:
		primitiveCount = elementCount / 2;
		glPrimitiveType = GL_LINES;
		break;

	case PrimitiveType::PointList:
		primitiveCount = elementCount;
		glPrimitiveType = GL_POINTS;
		break;

	case PrimitiveType::TriangleStrip:
		primitiveCount = elementCount - 2;
		glPrimitiveType = GL_TRIANGLE_STRIP;
		break;

	case PrimitiveType::LineStrip:
		primitiveCount = elementCount - 1;
		glPrimitiveType = GL_LINE_STRIP;
		break;

	case PrimitiveType::TiangleFan:
		primitiveCount = elementCount - 2;
		glPrimitiveType = GL_TRIANGLE_FAN;
		break;
	}
}

/// OpenGL framebuffer.
class Framebuffer
{
public:
	/// Construct.
	Framebuffer() :
		_depthStencil(nullptr),
		_drawBuffers(0),
		_firstUse(true)
	{
		glGenFramebuffers(1, &_buffer);
		for (size_t i = 0; i < MAX_RENDERTARGETS; ++i)
			_renderTargets[i] = nullptr;
	}

	/// Destruct.
	~Framebuffer()
	{
		glDeleteFramebuffers(1, &_buffer);
	}

	/// OpenGL FBO handle.
	unsigned _buffer;
	/// Color rendertargets bound to this FBO.
	SharedPtr<Texture> _renderTargets[MAX_RENDERTARGETS];
	/// Depth-stencil texture bound to this FBO.
	SharedPtr<Texture> _depthStencil;
	/// Enabled draw buffers.
	unsigned _drawBuffers;
	/// First use flag; used for setting up readbuffers.
	bool _firstUse;
};

Graphics::Graphics(SharedPtr<Ambient> ambient)
	:Super(ambient)
	, _window(nullptr)
	, _backbufferSize(Vector2F::ZERO)
	, _renderTargetSize(Vector2F::ZERO)
	, _attributesBySemantic((int)ElementSemantic::Count)
	, _multisample(1)
	, _vsync(false)
#if _OPENGL_4_6_
	, _apiName("OpenGL 4.6")
#elif _OPENGL_4_PLUS_
	, _apiName("OpenGL 4.3")
#elif _OPENGL_3_PLUS_
	, _apiName("OpenGL 3.3")
#elif _DIRECT_3D_12
	, _apiName("Direct3D 12")
#else
	, _apiName("UnKnow")
#endif
{
	ResetCachedState();
}

void Graphics::RegisterDebug()
{
#ifdef _DEBUG
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif
}

bool Graphics::CreateSDLWindow()
{
	_icon = GetSubSystem<ResourceSystem>()->GetResource<Image>("texture/logo.png");

	stbi_set_flip_vertically_on_load(true);

#pragma region CreateWindow

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		ErrorString("Couldn't initialize SDL");
	atexit(SDL_Quit);
	SDL_GL_LoadLibrary(NULL);

#if _OPENGL_4_6_
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif // _OPENGL_4_6_

#if  _OPENGL_4_PLUS_ 
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif //  _OPENGL_4_PLUS_

#if _OPENGL_3_PLUS_ 
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif // _OPENGL_3_PLUS_

	// Also request a depth buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// MSAA sample point
	//CreateSamplePoint(_numSample);

	// Create the window
	unsigned flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	if (_isFullScreen)
		flags |= SDL_WINDOW_FULLSCREEN;
	if (_isBorderless)
		flags |= SDL_WINDOW_BORDERLESS;
	if (_isResizable)
		flags |= SDL_WINDOW_RESIZABLE;
	if (_isHighDPI)
		flags |= SDL_WINDOW_ALLOW_HIGHDPI;

	// The position size will be reset later
	_window = SDL_CreateWindow(
		_titleName.CStr(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		0, 0, flags
	);

	if (_window == NULL)
		ErrorString("Couldn't set video mode");

	// Init window position
	SDL_Rect rect;
	SDL_GetDisplayBounds(0, &rect);
	if (_isFullScreen)
	{
		_windowRect._right = rect.w;
		_windowRect._top = rect.h;
		_windowRect._left = 0;
		_windowRect._bottom = 0;

	}
	else
	{
		if (_isCenter)
		{
			int w = _windowRect.Width();
			int h = _windowRect.Height();
			_windowRect._left = (rect.w - w) / 2;
			_windowRect._bottom = (rect.h - h) / 2;
			_windowRect._right = (rect.w + w) / 2;
			_windowRect._top = (rect.h + h) / 2;
		}
		SDL_SetWindowSize(_window, _windowRect.Width(), _windowRect.Height());
		SDL_SetWindowPosition(_window, _windowRect._left, _windowRect._bottom);

	}

	// Lock cursor in window
	if (_isGrab)
		SDL_SetWindowGrab(_window, SDL_TRUE);
	else
		SDL_SetWindowGrab(_window, SDL_FALSE);


#pragma endregion

#pragma region CreateDevice
	_glContext = SDL_GL_CreateContext(_window);
	if (_glContext == NULL)
		ErrorString("Failed to create OpenGL context");
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		AssertString(-1, "Failed to initialize GLAD from Engine");
	}
	else
	{
		// Set driver name
		_driverName = STRING((char*)glGetString(GL_VENDOR)) + STRING((char*)glGetString(GL_RENDERER));
	}

#pragma endregion

	// Create Icon
#pragma  region CreateIcon
	SDL_Surface* surface;
	surface = SetIcon();
	SDL_SetWindowIcon(_window, surface);
	SDL_FreeSurface(surface);

	return true;
}


bool Graphics::SetMode()
{
	_multisample = Clamp(_multisample, 1, 16);
	if (!IsInitialized())
	{
		if (!CreateSDLWindow())
			return false;
		if (!createContext(_multisample))
			return false;
		// Register graphics API debug
#if AUTO_DEBUG
		RegisterDebug();
#endif
	}
	ResetRenderTargets();
	ResetViewport();

#pragma endregion
	if (!_window)
		return false;
	
	return true;
}

void Graphics::Init()
{
	SetMode();
}

void Graphics::Close()
{
	_shaderPrograms.clear();
	_framebuffers.clear();

	// Release all GPU objects
	for (auto it = _gpuObjects.begin(); it != _gpuObjects.end(); ++it)
	{
		GPUObject* object = *it;
		object->Release();
	}

	//context.Reset();

	//window->Close();
	//ResetState();
}
void Graphics::DestoryWindow()
{

	SDL_GL_DeleteContext(_glContext);
	_glContext = nullptr;

	SDL_DestroyWindow(_window);
	_window = nullptr;
	SDL_Quit();
}

void Graphics::ReleaseAPI()
{
	glDeleteVertexArrays(1, &_vertexArrayObject);
}


void Graphics::ResetCachedState()
{
	for (size_t i =0; i<MAX_VERTEX_STREAMS; i++)
		_vertexBuffers[i].reset();

	_enabledVertexAttributes = 0;
	_usedVertexAttributes = 0;
	_instancingVertexAttributes = 0;

	for (size_t i = 0; i < (int)ShaderStage::Count; ++i)
	{
		for (size_t j = 0; j < MAX_CONSTANT_BUFFERS; ++j)
			_constantBuffers[i][j] = nullptr;
	}

	for (unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
	{
		_textures[i].reset();
		_textureTargets[i] = 0;
	}

	_indexBuffer = nullptr;
	_vertexShader = nullptr;
	_pixelShader = nullptr;
	_shaderProgram = nullptr;
	_framebuffer = nullptr;
	_vertexAttributesDirty = false;
	_vertexBuffersDirty = false;
	_blendStateDirty = false;
	_depthStateDirty = false;
	_rasterizerStateDirty = false;
	_framebufferDirty = false;
	_activeTexture = 0;
	_boundVBO = 0;
	_boundUBO = 0;

	_renderState.depthWrite = false;
	_renderState.depthFunc = CompareFunc::Always;
	_renderState.depthBias = 0;
	_renderState.slopeScaledDepthBias = 0;
	_renderState.depthClip = true;
	_renderState.colorWriteMask = COLORMASK_ALL;
	_renderState.alphaToCoverage = false;
	_renderState.blendMode.blendEnable = false;
	_renderState.blendMode.srcBlend = BlendFactor::Count;
	_renderState.blendMode.destBlend = BlendFactor::Count;
	_renderState.blendMode.blendOp = BlendOp::Count;
	_renderState.blendMode.srcBlendAlpha = BlendFactor::Count;
	_renderState.blendMode.destBlendAlpha = BlendFactor::Count;
	_renderState.blendMode.blendOpAlpha = BlendOp::Count;
	_renderState.fillMode = FillMode::Solid;
	_renderState.cullMode = CullMode::None;
	_renderState.scissorEnable = false;
	_renderState.scissorRect = RectInt(0, 0, 0, 0);
	_renderState.stencilEnable = false;
	_renderState.stencilRef = 0;
	_renderState.stencilTest.stencilReadMask = 0xff;
	_renderState.stencilTest.stencilWriteMask = 0xff;
	_renderState.stencilTest.frontFail = StencilOp::Keep;
	_renderState.stencilTest.frontDepthFail = StencilOp::Keep;
	_renderState.stencilTest.frontPass = StencilOp::Keep;
	_renderState.stencilTest.frontFunc = CompareFunc::Always;
	_renderState.stencilTest.backFail = StencilOp::Keep;
	_renderState.stencilTest.backDepthFail = StencilOp::Keep;
	_renderState.stencilTest.backPass = StencilOp::Keep;
	_renderState.stencilTest.backFunc = CompareFunc::Always;
	_currentRenderState = _renderState;

}

void Graphics::ResetRenderTargets()
{
	SetRenderTarget(nullptr, nullptr);
}
void Graphics::ResetViewport()
{
	SetViewport(RectInt(0, 0, _renderTargetSize._x, _renderTargetSize._y));
}

void Graphics::SetRenderTargets(VECTOR<SharedPtr<Texture> >& renderTargets, SharedPtr<Texture> depthStencil)
{
	for (size_t i = 0; i < MAX_RENDERTARGETS && i < renderTargets.size(); ++i)
		_renderTargets[i] = (renderTargets[i] && renderTargets[i]->IsRenderTarget()) ? renderTargets[i] : nullptr;

	for (size_t i = renderTargets.size(); i < MAX_RENDERTARGETS; ++i)
		_renderTargets[i] = nullptr;

	_depthStencil = (depthStencil && depthStencil->IsDepthStencil()) ? depthStencil : nullptr;

	if (_renderTargets[0])
		_renderTargetSize = Vector2F(_renderTargets[0]->Width(), _renderTargets[0]->Height());
	else if (depthStencil)
		_renderTargetSize = Vector2F(depthStencil->Width(), depthStencil->Height());
	else
		_renderTargetSize = _backbufferSize;

	_framebufferDirty = true;
}

void Graphics::SetRenderTarget(SharedPtr<Texture> renderTarget, SharedPtr<Texture> depthStencil)
{
	_renderTargetVector.resize(1);
	_renderTargetVector[0] = renderTarget;
	SetRenderTargets(_renderTargetVector, depthStencil);
}

void Graphics::SetViewport(const RectInt& viewport)
{
	prepareFramebuffer();

	/// \todo Implement a member function in IntRect for clipping
	_viewport._left = Clamp(viewport._left, 0, (int)_renderTargetSize._x - 1);
	_viewport._top = Clamp(viewport._top, 0, (int)_renderTargetSize._y - 1);
	_viewport._right = Clamp(viewport._right, _viewport._left + 1, (int)_renderTargetSize._x);
	_viewport._bottom = Clamp(viewport._bottom, _viewport._top + 1, (int)_renderTargetSize._y);

	// When rendering to the backbuffer, use Direct3D convention with the vertical coordinates ie. 0 is top
	if (!_framebuffer)
		glViewport(_viewport._left, _renderTargetSize._y - _viewport._bottom, _viewport.Width(), _viewport.Height());
	else
		glViewport(_viewport._left, _viewport._top, _viewport.Width(), _viewport.Height());
}

void Graphics::SetTexture(size_t index, SharedPtr<Texture> texture)
{
	if (index < MAX_TEXTURE_UNITS && texture != _textures[index])
	{
		_textures[index] = texture;

		if (index != _activeTexture)
		{
			glActiveTexture(GL_TEXTURE0 + (unsigned)index);
			_activeTexture = index;
		}

		if (texture)
		{
			unsigned target = texture->GLTarget();
			// Make sure we do not have multiple targets bound in the same unit
			if (_textureTargets[index] && _textureTargets[index] != target)
				glBindTexture(_textureTargets[index], 0);
			glBindTexture(target, texture->GLTexture());
			_textureTargets[index] = target;
		}
		else if (_textureTargets[index])
		{
			glBindTexture(_textureTargets[index], 0);
			_textureTargets[index] = 0;
		}
	}
}

bool Graphics::BeginFrame()
{

	return true;
}
void Graphics::EndFrame()
{
	if (!IsInitialized())
		return;
	SDL_GL_SwapWindow(_window);
}


void Graphics::CreateSamplePoint(int num)
{
	if (num)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, num);
	}
}

void Graphics::CleanupFramebuffers()
{
	// Make sure the framebuffer is switched first if there are pending rendertarget changes
	prepareFramebuffer();

	// Clear all except the framebuffer currently in use
	for (auto it = _framebuffers.begin(); it != _framebuffers.end();)
	{
		if (it->second != _framebuffer)
			it = _framebuffers.erase(it);
		else
			++it;
	}
}
bool Graphics::IsDeviceLost()
{
#if defined(IOS) || defined(TVOS)
	if (_window && (SDL_GetWindowFlags(_window) & SDL_WINDOW_MINIMIZED) != 0)
		return true;
#endif
	return _glContext == nullptr;
}

unsigned Graphics::GetAlphaFormat()
{
	return GL_R8;
}

unsigned Graphics::GetLuminanceFormat()
{
	return GL_R8;
}

unsigned Graphics::GetLuminanceAlphaFormat()
{
	return GL_RG8;
}

unsigned Graphics::GetRGBFormat()
{
	return GL_RGB;
}

unsigned Graphics::GetRGBAFormat()
{
	return GL_RGBA;
}

unsigned Graphics::GetRGBA16Format()
{
	return GL_RGBA16;
}

unsigned Graphics::GetRGBAFloat16Format()
{
	return GL_RGBA16F;
}

unsigned Graphics::GetRGBAFloat32Format()
{
	return GL_RGBA32F;
}

unsigned Graphics::GetRG16Format()
{
	return GL_RG16;
}

unsigned Graphics::GetRGFloat16Format()
{
	return GL_RG16F;
}

unsigned Graphics::GetRGFloat32Format()
{
	return GL_RG32F;
}

unsigned Graphics::GetFloat16Format()
{
	return GL_R16F;
}

unsigned Graphics::GetFloat32Format()
{
	return GL_R32F;
}

unsigned Graphics::GetLinearDepthFormat()
{
	return GL_R32F;
}

unsigned Graphics::GetDepthStencilFormat()
{
	return GL_DEPTH24_STENCIL8;
}

unsigned Graphics::GetReadableDepthFormat()
{
	return GL_DEPTH_COMPONENT24;
}

unsigned Graphics::GetFormat(const STRING& formatName)
{
	STRING nameLower = formatName.ToLower().Trimmed();

	if (nameLower == "a")
		return GetAlphaFormat();
	if (nameLower == "l")
		return GetLuminanceFormat();
	if (nameLower == "la")
		return GetLuminanceAlphaFormat();
	if (nameLower == "rgb")
		return GetRGBFormat();
	if (nameLower == "rgba")
		return GetRGBAFormat();
	if (nameLower == "rgba16")
		return GetRGBA16Format();
	if (nameLower == "rgba16f")
		return GetRGBAFloat16Format();
	if (nameLower == "rgba32f")
		return GetRGBAFloat32Format();
	if (nameLower == "rg16")
		return GetRG16Format();
	if (nameLower == "rg16f")
		return GetRGFloat16Format();
	if (nameLower == "rg32f")
		return GetRGFloat32Format();
	if (nameLower == "r16f")
		return GetFloat16Format();
	if (nameLower == "r32f" || nameLower == "float")
		return GetFloat32Format();
	if (nameLower == "lineardepth" || nameLower == "depth")
		return GetLinearDepthFormat();
	if (nameLower == "d24s8")
		return GetDepthStencilFormat();
	if (nameLower == "readabledepth" || nameLower == "hwdepth")
		return GetReadableDepthFormat();

	return GetRGBFormat();
}

bool Graphics::createContext(int multisample)
{

	// Query OpenGL capabilities
	int numBlocks;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &numBlocks);
	_vsConstantBuffers = numBlocks;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &numBlocks);
	_psConstantBuffers = numBlocks;

	// Create and bind a vertex array object that will stay in use throughout
	/// \todo Investigate performance gain of using multiple VAO's
	unsigned vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// These states are always enabled to match Direct3D
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glEnable(GL_POLYGON_OFFSET_FILL);

	// Set up texture data read/write alignment. It is important that this is done before uploading any texture data
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	return true;
}

void Graphics::prepareFramebuffer()
{
	if (_framebufferDirty)
	{
		_framebufferDirty = false;

		unsigned newDrawBuffers = 0;
		bool useBackbuffer = true;

		// If rendertarget changes, scissor rect may need to be re-evaluated
		if (_currentRenderState.scissorEnable)
		{
			_renderState.scissorRect = RectInt::ZERO;
			_rasterizerStateDirty = true;
		}

		for (size_t i = 0; i < MAX_RENDERTARGETS; ++i)
		{
			if (_renderTargets[i])
			{
				useBackbuffer = false;
				newDrawBuffers |= (1 << i);
			}
		}
		if (_depthStencil)
			useBackbuffer = false;

		if (useBackbuffer)
		{
			if (_framebuffer)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				_framebuffer = nullptr;
			}
			return;
		}

		// Search for a new framebuffer based on format & size, or create new
		ImageFormat format = ImageFormat::NONE;
		if (_renderTargets[0])
			format = _renderTargets[0]->Format();
		else if (_depthStencil)
			format = _depthStencil->Format();
		unsigned long long key = ((int)_renderTargetSize._x << 16 | (int)_renderTargetSize._y) | (((unsigned long long)format) << 32);

		auto it = _framebuffers.find(key);
		if (it == _framebuffers.end())
			it = _framebuffers.insert(it, MakePair(key, MakeShared<Framebuffer>()));

		if (it->second != _framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, it->second->_buffer);
			_framebuffer = it->second;
		}

		// Setup readbuffers & drawbuffers
		if (_framebuffer->_firstUse)
		{
			glReadBuffer(GL_NONE);
			_framebuffer->_firstUse = false;
		}

		if (newDrawBuffers != _framebuffer->_drawBuffers)
		{
			if (!newDrawBuffers)
				glDrawBuffer(GL_NONE);
			else
			{
				int drawBufferIds[MAX_RENDERTARGETS];
				unsigned drawBufferCount = 0;

				for (unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
				{
					if (newDrawBuffers & (1 << i))
						drawBufferIds[drawBufferCount++] = GL_COLOR_ATTACHMENT0 + i;
				}
				glDrawBuffers(drawBufferCount, (const GLenum*)drawBufferIds);
			}

			_framebuffer->_drawBuffers = newDrawBuffers;
		}

		// Setup color attachments
		for (size_t i = 0; i < MAX_RENDERTARGETS; ++i)
		{
			if (_renderTargets[i] != _framebuffer->_renderTargets[i])
			{
				if (_renderTargets[i])
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (unsigned)i, _renderTargets[i]->GLTarget(),
						_renderTargets[i]->GLTexture(), 0);
				}
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (unsigned)i, GL_TEXTURE_2D, 0, 0);

				_framebuffer->_renderTargets[i] = _renderTargets[i];
			}
		}

		// Setup depth & stencil attachments
		if (_depthStencil != _framebuffer->_depthStencil)
		{
			if (_depthStencil)
			{
				bool hasStencil = _depthStencil->Format() == ImageFormat::D24S8;
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthStencil->GLTarget(),
					_depthStencil->GLTexture(), 0);
				if (hasStencil)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, _depthStencil->GLTarget(),
						_depthStencil->GLTexture(), 0);
				}
				else
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
			}

			_framebuffer->_depthStencil = _depthStencil;
		}
	}
}


}
#endif //AUTO_OPENGL