#include "../Math/Matrix3x4.h"
#include "GraphicsDefs.h"

#include "../Debug/DebugNew.h"

namespace Auto3D
{

extern AUTO_API const size_t elementSizes[] =
{
    sizeof(int),
    sizeof(float),
    sizeof(Vector2F),
    sizeof(Vector3F),
    sizeof(Vector4F),
    sizeof(unsigned),
    sizeof(Matrix3x4F),
    sizeof(Matrix4x4F)
};

extern AUTO_API const char* elementSemanticNames[] =
{
    "POSITION",
    "NORMAL",
    "BINORMAL",
    "TANGENT",
    "TEXCOORD",
    "COLOR",
    "BLENDWEIGHT",
    "BLENDINDICES",
    nullptr
};

extern AUTO_API const char* resourceUsageNames[] =
{
    "default",
    "immutable",
    "dynamic",
    "rendertarget",
    nullptr
};

extern AUTO_API const char* elementTypeNames[] =
{
    "int",
    "float",
    "Vector2",
    "Vector3",
    "Vector4",
    "UByte4",
    "Matrix3x4",
    "Matrix4",
    nullptr
};

extern AUTO_API const char* blendFactorNames[] =
{
    "",
    "zero",
    "one",
    "srcColor",
    "invSrcColor",
    "srcAlpha",
    "invSrcAlpha",
    "destAlpha",
    "invDestAlpha",
    "destColor",
    "invDestColor",
    "srcAlphaSat",
    nullptr
};

extern AUTO_API const char* blendOpNames[] =
{
    "",
    "add",
    "subtract",
    "revSubtract",
    "min",
    "max",
    nullptr
};

extern AUTO_API const char* blendModeNames[] =
{
    "replace",
    "add",
    "multiply",
    "alpha",
    "addAlpha",
    "preMulAlpha",
    "invDestAlpha",
    "subtract",
    "subtractAlpha",
    nullptr
};

extern AUTO_API const char* fillModeNames[] =
{
    "",
    "",
    "wireframe",
    "solid",
    nullptr
};

extern AUTO_API const char* cullModeNames[] =
{
    "",
    "none",
    "front",
    "back",
    nullptr
};

extern AUTO_API const char* compareFuncNames[] =
{
    "",
    "never",
    "less",
    "equal",
    "lessEqual",
    "greater",
    "notEqual",
    "greaterEqual",
    "always",
    nullptr
};

extern AUTO_API const char* stencilOpNames[] =
{
    "",
    "keep",
    "zero",
    "replace",
    "incrSat",
    "descrSat",
    "invert",
    "incr",
    "decr",
    nullptr
};

extern AUTO_API const BlendModeDesc blendModes[] =
{
    BlendModeDesc(false, BlendFactor::ONE, BlendFactor::ONE, BlendOp::ADD, BlendFactor::ONE, BlendFactor::ONE, BlendOp::ADD),
    BlendModeDesc(true, BlendFactor::ONE, BlendFactor::ONE, BlendOp::ADD, BlendFactor::ONE, BlendFactor::ONE, BlendOp::ADD),
    BlendModeDesc(true, BlendFactor::DEST_COLOR, BlendFactor::ZERO, BlendOp::ADD, BlendFactor::DEST_COLOR, BlendFactor::ZERO, BlendOp::ADD),
    BlendModeDesc(true, BlendFactor::SRC_ALPHA, BlendFactor::INV_SRC_ALPHA, BlendOp::ADD, BlendFactor::SRC_ALPHA, BlendFactor::INV_SRC_ALPHA, BlendOp::ADD),
    BlendModeDesc(true, BlendFactor::SRC_ALPHA, BlendFactor::ONE, BlendOp::ADD, BlendFactor::SRC_ALPHA, BlendFactor::ONE, BlendOp::ADD),
    BlendModeDesc(true, BlendFactor::ONE, BlendFactor::INV_SRC_ALPHA, BlendOp::ADD, BlendFactor::ONE, BlendFactor::INV_SRC_ALPHA, BlendOp::ADD),
    BlendModeDesc(true, BlendFactor::INV_DEST_ALPHA, BlendFactor::DEST_ALPHA, BlendOp::ADD, BlendFactor::INV_DEST_ALPHA, BlendFactor::DEST_ALPHA, BlendOp::ADD),
    BlendModeDesc(true, BlendFactor::ONE, BlendFactor::ONE, BlendOp::REV_SUBTRACT, BlendFactor::ONE, BlendFactor::ONE, BlendOp::REV_SUBTRACT),
    BlendModeDesc(true, BlendFactor::SRC_ALPHA, BlendFactor::ONE, BlendOp::REV_SUBTRACT, BlendFactor::SRC_ALPHA, BlendFactor::ONE, BlendOp::REV_SUBTRACT)
};

}
