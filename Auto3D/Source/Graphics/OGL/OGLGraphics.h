#pragma once

#include "../../Math/Color.h"
#include "../../Math/IntRect.h"
#include "../../Math/IntVector2.h"
#include "../../Object/Object.h"
#include "../GraphicsDefs.h"

namespace Auto3D
{

class BlendState;
class ConstantBuffer;
class DepthState;
class Framebuffer;
class GLContext;
class GPUObject;
class IndexBuffer;
class RasterizerState;
class ShaderProgram;
class ShaderVariation;
class Texture;
class VertexBuffer;
class Window;
class WindowResizeEvent;

typedef HashMap<Pair<ShaderVariation*, ShaderVariation*>, AutoPtr<ShaderProgram> > ShaderProgramMap;

/// Screen mode set _event.
class ScreenModeEvent : public Event
{
public:
    /// New backbuffer _size.
    IntVector2 _size;
    /// Fullscreen flag.
    bool _fullscreen;
    /// Window _resizable flag.
    bool _resizable;
    /// Multisample level.
    int _multisample;
};

/// 3D graphics rendering context. Manages the rendering _window and GPU objects.
class AUTO_API Graphics : public Object
{
	REGISTER_OBJECT_CLASS(Graphics, Object)

public:
    /// Construct and register subsystem. The graphics mode is not set & _window is not opened yet.
    Graphics();
    /// Destruct. Clean up the _window, rendering context and GPU objects.
    ~Graphics();

    /// Set graphics mode. Create the _window and rendering context if not created yet. Return true on success.
    bool SetMode(const IntVector2& size, bool fullscreen = false, bool resizable = false, int multisample = 1);
    /// Set _fullscreen mode on/off while retaining previous resolution. The initial graphics mode must have been set first. Return true on success.
    bool SetFullscreen(bool enable);
    /// Set new multisample level while retaining previous resolution. The initial graphics mode must have been set first. Return true on success.
    bool SetMultisample(int multisample);
    /// Set vertical sync on/off.
    void SetVSync(bool enable);
    /// Close the _window and destroy the rendering context and GPU objects.
    void Close();
    /// Present the contents of the backbuffer.
    void Present();
    /// Set the color rendertarget and depth stencil buffer.
    void SetRenderTarget(Texture* renderTarget, Texture* stencilBuffer);
    /// Set multiple color rendertargets and the depth stencil buffer.
    void SetRenderTargets(const Vector<Texture*>& renderTargets, Texture* stencilBuffer);
    /// Set the viewport rectangle. On _window resize the viewport will automatically revert to full _window.
    void SetViewport(const IntRect& viewport);
    /// Bind a vertex buffer.
    void SetVertexBuffer(size_t index, VertexBuffer* buffer);
    /// Bind an index buffer.
    void SetIndexBuffer(IndexBuffer* buffer);
    /// Bind a constant buffer.
    void SetConstantBuffer(ShaderStage stage, size_t index, ConstantBuffer* buffer);
    /// Bind a texture.
    void SetTexture(size_t index, Texture* texture);
    /// Bind vertex and pixel shaders.
    void SetShaders(ShaderVariation* vs, ShaderVariation* ps);
    /// Set color write and blending related state using an arbitrary blend mode.
    void SetColorState(const BlendModeDesc& blendMode, bool alphaToCoverage = false, unsigned char colorWriteMask = COLORMASK_ALL);
    /// Set color write and blending related state using a predefined blend mode.
    void SetColorState(BlendMode blendMode, bool alphaToCoverage = false, unsigned char colorWriteMask = COLORMASK_ALL);
    /// Set depth buffer related state.
    void SetDepthState(CompareFunc depthFunc, bool depthWrite, bool depthClip = true, int depthBias = 0, float slopeScaledDepthBias = 0.0f);
    /// Set rasterizer related state.
    void SetRasterizerState(CullMode cullMode, FillMode fillMode);
    /// Set scissor test.
    void SetScissorTest(bool scissorEnable = false, const IntRect& scissorRect = IntRect::ZERO);
    /// Set stencil test.
    void SetStencilTest(bool stencilEnable, const StencilTestDesc& stencilTest = StencilTestDesc(), unsigned char stencilRef = 0);
    /// Reset rendertarget and depth stencil buffer to the backbuffer.
    void ResetRenderTargets();
    /// Set the viewport to the entire rendertarget or backbuffer.
    void ResetViewport();
    /// Reset all bound vertex buffers.
    void ResetVertexBuffers();
    /// Reset all bound constant buffers.
    void ResetConstantBuffers();
    /// Reset all bound textures.
    void ResetTextures();
    /// Clear the current rendertarget. This is not affected by the defined viewport, but will always clear the whole target.
    void Clear(unsigned clearFlags, const Color& clearColor = Color::BLACK, float clearDepth = 1.0f, unsigned char clearStencil = 0);
    /// Draw non-indexed geometry.
    void Draw(PrimitiveType type, size_t vertexStart, size_t vertexCount);
    /// Draw indexed geometry.
    void DrawIndexed(PrimitiveType type, size_t indexStart, size_t indexCount, size_t vertexStart);
    /// Draw instanced non-indexed geometry.
    void DrawInstanced(PrimitiveType type, size_t vertexStart, size_t vertexCount, size_t instanceStart, size_t instanceCount);
    /// Draw instanced indexed geometry.
    void DrawIndexedInstanced(PrimitiveType type, size_t indexStart, size_t indexCount, size_t vertexStart, size_t instanceStart, size_t instanceCount);

    /// Return whether has the rendering _window and context.
    bool IsInitialized() const;
    /// Return backbuffer _size, or 0,0 if not initialized.
    const IntVector2& Size() const { return _backbufferSize; }
    /// Return backbuffer width, or 0 if not initialized.
    int Width() const { return _backbufferSize._x; }
    /// Return backbuffer height, or 0 if not initialized.
    int Height() const { return _backbufferSize._y; }
    /// Return multisample level, or 1 if not using multisampling.
    int Multisample() const { return _multisample; }
    /// Return current rendertarget width.
    int RenderTargetWidth() const { return _renderTargetSize._x; }
    /// Return current rendertarget height.
    int RenderTargetHeight() const { return _renderTargetSize._y; }
    /// Return whether is using _fullscreen mode.
    bool IsFullscreen() const;
    /// Return whether the _window is _resizable.
    bool IsResizable() const;
    /// Return whether is using vertical sync.
    bool VSync() const { return _vsync; }
    /// Return the rendering _window.
    Window* RenderWindow() const;
    /// Return the current color rendertarget by index, or null if rendering to the backbuffer.
    Texture* RenderTarget(size_t index) const;
    /// Return the current depth-stencil buffer, or null if rendering to the backbuffer.
    Texture* DepthStencil() const { return _depthStencil; }
    /// Return the current viewport rectangle.
    const IntRect& Viewport() const { return _viewport; }
    /// Return currently bound vertex buffer by index.
    VertexBuffer* GetVertexBuffer(size_t index) const;
    /// Return currently bound index buffer.
    IndexBuffer* GetIndexBuffer() const { return _indexBuffer; }
    /// Return currently bound constant buffer by shader stage and index.
    ConstantBuffer* GetConstantBuffer(ShaderStage stage, size_t index) const;
    /// Return currently bound texture by texture unit.
    Texture* GetTexture(size_t index) const;
    /// Return currently bound vertex shader.
    ShaderVariation* GetVertexShader() const { return _vertexShader; }
    /// Return currently bound pixel shader.
    ShaderVariation* GetPixelShader() const { return _pixelShader; }
    /// Return the current renderstate.
    const RenderState& GetRenderState() const { return _renderState; }
    /// Return number of supported constant buffer bindings for vertex shaders.
    size_t NumVSConstantBuffers() const { return _vsConstantBuffers; }
    /// Return number of supported constant buffer bindings for pixel shaders.
    size_t NumPSConstantBuffers() const { return _psConstantBuffers; }

    /// Register a GPU object to keep track of.
    void AddGPUObject(GPUObject* object);
    /// Remove a GPU object.
    void RemoveGPUObject(GPUObject* object);
    /// Cleanup shader programs when a vertex or pixel shader is destroyed.
    void CleanupShaderPrograms(ShaderVariation* shader);
    /// Remove all framebuffers except the currently bound one. Called automatically on backbuffer resize, but can also be called manually if you have used rendertarget resolutions or color formats that you will not need any more.
    void CleanupFramebuffers();
    /// Remove texture reference from framebuffers. Called by Texture when the texture GPU object is released.
    void CleanupFramebuffers(Texture* texture);
    /// Bind a VBO for editing or applying as a vertex source. Avoids redundant assignment.
    void BindVBO(unsigned vbo);
    /// Bind a UBO for editing. Avoids redundant assignment.
    void BindUBO(unsigned ubo);
    /// Return the currently bound VBO.
    unsigned BoundVBO() const { return _boundVBO; }
    /// Return the currently bound VBO.
    unsigned BoundUBO() const { return _boundUBO; }

    /// Screen mode changed _event.
    ScreenModeEvent _screenModeEvent;
    /// %Graphics context lost _event.
    Event _contextLossEvent;
    /// %Graphics context restored _event.
    Event _contextRestoreEvent;

private:
    /// Create and initialize the OpenGL context. Return true on success.
    bool CreateContext(int multisample);
    /// Handle _window resize _event.
    void HandleResize(WindowResizeEvent& event);
    /// Prepare framebuffer changes.
    void PrepareFramebuffer();
    /// Set state for the next draw call. Return false if the draw call should not be attempted.
    bool PrepareDraw(bool instanced = false, size_t instanceStart = 0);
    /// Reset internally tracked state.
    void ResetState();

    /// OpenGL context.
    AutoPtr<GLContext> _context;
    /// OS-level rendering _window.
    AutoPtr<Window> _window;
    /// Current _size of the backbuffer.
    IntVector2 _backbufferSize;
    /// Current _size of the active rendertarget.
    IntVector2 _renderTargetSize;
    /// Bound vertex buffers.
    VertexBuffer* _vertexBuffers[MAX_VERTEX_STREAMS];
    /// Enabled vertex attributes bitmask.
    unsigned _enabledVertexAttributes;
    /// Used vertex attributes bitmask.
    unsigned _usedVertexAttributes;
    /// Vertex attribute instancing bitmask for keeping track of divisors.
    unsigned _instancingVertexAttributes;
    /// Current mapping of vertex attributes by semantic.
    Vector<Vector<unsigned> > _attributesBySemantic;
    /// Bound index buffer.
    IndexBuffer* _indexBuffer;
    /// Bound constant buffers by shader stage.
    ConstantBuffer* _constantBuffers[MAX_SHADER_STAGES][MAX_CONSTANT_BUFFERS];
    /// Bound textures by texture unit.
    Texture* _textures[MAX_TEXTURE_UNITS];
    /// OpenGL active texture targets by texture unit.
    unsigned _textureTargets[MAX_TEXTURE_UNITS];
    /// Bound rendertarget textures.
    Texture* _renderTargets[MAX_RENDERTARGETS];
    /// Bound depth-stencil texture.
    Texture* _depthStencil;
    /// Helper vector for defining just one color rendertarget.
    Vector<Texture*> _renderTargetVector;
    /// Bound vertex shader.
    ShaderVariation* _vertexShader;
    /// Bound pixel shader.
    ShaderVariation* _pixelShader;
    /// Current renderstate requested by the application.
    RenderState _renderState;
    /// Renderstate applied to OpenGL.
    RenderState _glRenderState;
    /// Vertex attributes dirty (shader program changed) flag.
    bool _vertexAttributesDirty;
    /// Vertex buffers dirty flag.
    bool _vertexBuffersDirty;
    /// Blend state dirty flag.
    bool _blendStateDirty;
    /// Depth state dirty flag.
    bool _depthStateDirty;
    /// Rasterizer state dirty flag.
    bool _rasterizerStateDirty;
    /// Framebuffer assignment dirty flag.
    bool _framebufferDirty;
    /// Number of supported constant buffer bindings for vertex shaders.
    size_t _vsConstantBuffers;
    /// Number of supported constant buffer bindings for pixel shaders.
    size_t _psConstantBuffers;
    /// Bound shader program.
    ShaderProgram* _shaderProgram;
    /// Bound framebuffer object.
    Framebuffer* _framebuffer;
    /// Last used OpenGL texture unit.
    size_t _activeTexture;
    /// Last bound vertex buffer object.
    unsigned _boundVBO;
    /// Last bound uniform buffer object.
    unsigned _boundUBO;
    /// Current scissor rectangle.
    IntRect _scissorRect;
    /// Current viewport rectangle.
    IntRect _viewport;
    /// GPU objects.
    Vector<GPUObject*> _gpuObjects;
    /// Shader programs.
    ShaderProgramMap _shaderPrograms;
    /// Framebuffer objects keyed by resolution and color format.
    HashMap<unsigned long long, AutoPtr<Framebuffer> > _framebuffers;
    /// Multisample level.
    int _multisample;
    /// Vertical sync flag.
    bool _vsync;
};

/// Register Graphics related object factories and attributes.
AUTO_API void RegisterGraphicsLibrary();

}