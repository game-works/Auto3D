#pragma once

#include "../AutoConfig.h"

#ifdef AUTO_DIRECT3D_12
    #include "D3D11/D3D11IndexBuffer.h"
#endif
#ifdef AUTO_OPENGL
    #include "OGL/OGLIndexBuffer.h"
#endif