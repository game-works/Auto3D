#include "Launch.h"
#include "MultLightsSpace.h"
#include "TextureSpace.h"
#include "MoreCameraSpace.h"
#include "DepthTestSpace.h"
#include "StencilTestSpace.h"
#include "FrameBuffersSpace.h"
#include "SkyBoxSpace.h"
#include "VertexExplodeSpace.h"
#include "MassInstantiationSpace.h"
#include "AntiAliasingSpace.h"
#include "ShadowSpace.h"
#include "NormalMappingSpace.h"
#include "ParallaxMappingSpace.h"
#include "HDRSpace.h"
#include "BloomSpace.h"
#include "DeferredShadingSpace.h"
#include "SSAOSpace.h"
#include "PBRSpace.h"
#include "PBRSkyBoxSpace.h"
USING_AUTO


#ifdef __OPENGL__
int main()
{
	//TextureSpace work;
	//MultLightsSpace work;
	//MoreCameraSpace work;
	//DepthTestSpace work;
	//StencilTestSpace work;
	//FrameBuffersSpace work;
	SkyBoxSpace work;
	//VertexExplodeSpace work;
	//MassInstantiationSpace work;
	//AntiAliasingSpace work;
	//ShadowSpace work;
	//NormalMappingSpace work;
	//ParallaxMappingSpace work;
	//HDRSpace work;
	//BloomSpace work;
	//DeferredShadingSpace work;
	//SSAOSpace work;
	//PBRSpace work;
	//PBRSkyBoxSpace work;
	return work.Launch();
}

#endif //__OPENGL__