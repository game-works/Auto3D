#include "../Base/String.h"
#include "Color.h"

#include <cstdio>
#include <cstdlib>

#include "../Debug/DebugNew.h"

namespace Auto3D
{

const Color Color::WHITE(1.0f, 1.0f, 1.0f);
const Color Color::GRAY(0.5f, 0.5f, 0.5f);
const Color Color::BLACK(0.0f, 0.0f, 0.0f);
const Color Color::RED(1.0f, 0.0f, 0.0f);
const Color Color::GREEN(0.0f, 1.0f, 0.0f);
const Color Color::BLUE(0.0f, 0.0f, 1.0f);
const Color Color::CYAN(0.0f, 1.0f, 1.0f);
const Color Color::MAGENTA(1.0f, 0.0f, 1.0f);
const Color Color::YELLOW(1.0f, 1.0f, 0.0f);
const Color Color::TRANSPARENT(0.0f, 0.0f, 0.0f, 0.0f);

unsigned Color::ToUInt() const
{
    unsigned r_ = Clamp(((int)(_r * 255.0f)), 0, 255);
    unsigned g_ = Clamp(((int)(_g * 255.0f)), 0, 255);
    unsigned b_ = Clamp(((int)(_b * 255.0f)), 0, 255);
    unsigned a_ = Clamp(((int)(_a * 255.0f)), 0, 255);
    return (a_ << 24) | (b_ << 16) | (g_ << 8) | r_;
}

Vector3F Color::ToHSL() const
{
    float min, max;
    Bounds(&min, &max, true);

    float h = Hue(min, max);
    float s = SaturationHSL(min, max);
    float l = (max + min) * 0.5f;

    return Vector3F(h, s, l);
}

Vector3F Color::ToHSV() const
{
    float min, max;
    Bounds(&min, &max, true);

    float h = Hue(min, max);
    float s = SaturationHSV(min, max);
    float v = max;

    return Vector3F(h, s, v);
}

void Color::FromHSL(float h, float s, float l, float a_)
{
    float c;
    if (l < 0.5f)
        c = (1.0f + (2.0f * l - 1.0f)) * s;
    else
        c = (1.0f - (2.0f * l - 1.0f)) * s;

    float m = l - 0.5f * c;

    FromHCM(h, c, m);

    _a = a_;
}

void Color::FromHSV(float h, float s, float v, float a_)
{
    float c = v * s;
    float m = v - c;

    FromHCM(h, c, m);

    _a = a_;
}

bool Color::FromString(const String& str)
{
    return FromString(str.CString());
}

bool Color::FromString(const char* str)
{
    size_t elements = String::CountElements(str, ' ');
    if (elements < 3)
        return false;
    
    char* ptr = (char*)str;
    _r = (float)strtod(ptr, &ptr);
    _g = (float)strtod(ptr, &ptr);
    _b = (float)strtod(ptr, &ptr);
    if (elements > 3)
        _a = (float)strtod(ptr, &ptr);
    
    return true;
}

float Color::Chroma() const
{
    float min, max;
    Bounds(&min, &max, true);

    return max - min;
}

float Color::Hue() const
{
    float min, max;
    Bounds(&min, &max, true);

    return Hue(min, max);
}

float Color::SaturationHSL() const
{
    float min, max;
    Bounds(&min, &max, true);

    return SaturationHSL(min, max);
}

float Color::SaturationHSV() const
{
    float min, max;
    Bounds(&min, &max, true);

    return SaturationHSV(min, max);
}

float Color::Lightness() const
{
    float min, max;
    Bounds(&min, &max, true);

    return (max + min) * 0.5f;
}

void Color::Bounds(float* min, float* max, bool clipped) const
{
    assert(min && max);

    if (_r > _g)
    {
        if (_g > _b) // r > g > b
        {
            *max = _r;
            *min = _b;
        }
        else // r > g && g <= b
        {
            *max = _r > _b ? _r : _b;
            *min = _g;
        }
    }
    else
    {
        if (_b > _g) // r <= g < b
        {
            *max = _b;
            *min = _r;
        }
        else // r <= g && b <= g
        {
            *max = _g;
            *min = _r < _b ? _r : _b;
        }
    }

    if (clipped)
    {
        *max = *max > 1.0f ? 1.0f : (*max < 0.0f ? 0.0f : *max);
        *min = *min > 1.0f ? 1.0f : (*min < 0.0f ? 0.0f : *min);
    }
}

float Color::MaxRGB() const
{
    if (_r > _g)
        return (_r > _b) ? _r : _b;
    else
        return (_g > _b) ? _g : _b;
}

float Color::MinRGB() const
{
    if (_r < _g)
        return (_r < _b) ? _r : _b;
    else
        return (_g < _b) ? _g : _b;
}

float Color::Range() const
{
    float min, max;
    Bounds(&min, &max);
    return max - min;
}

void Color::Clip(bool clipAlpha)
{
    _r = (_r > 1.0f) ? 1.0f : ((_r < 0.0f) ? 0.0f : _r);
    _g = (_g > 1.0f) ? 1.0f : ((_g < 0.0f) ? 0.0f : _g);
    _b = (_b > 1.0f) ? 1.0f : ((_b < 0.0f) ? 0.0f : _b);

    if (clipAlpha)
        _a = (_a > 1.0f) ? 1.0f : ((_a < 0.0f) ? 0.0f : _a);
}

void Color::Invert(bool invertAlpha)
{
    _r = 1.0f - _r;
    _g = 1.0f - _g;
    _b = 1.0f - _b;

    if (invertAlpha)
        _a = 1.0f - _a;
}

Color Color::Lerp(const Color &rhs, float t) const
{
    float invT = 1.0f - t;
    return Color(
        _r * invT + rhs._r * t,
        _g * invT + rhs._g * t,
        _b * invT + rhs._b * t,
        _a * invT + rhs._a * t
    );
}

String Color::ToString() const
{
    char tempBuffer[CONVERSION_BUFFER_LENGTH];
    sprintf(tempBuffer, "%g %g %g %g", _r, _g, _b, _a);
    return String(tempBuffer);
}

float Color::Hue(float min, float max) const
{
    float chroma = max - min;

    // If chroma equals zero, hue is undefined
    if (chroma <= M_EPSILON)
        return 0.0f;

    // Calculate and return hue
    if (Auto3D::Equals(_g, max))
        return (_b + 2.0f*chroma - _r) / (6.0f * chroma);
    else if (Auto3D::Equals(_b, max))
        return (4.0f * chroma - _g + _r) / (6.0f * chroma);
    else
    {
        float h = (_g - _b) / (6.0f * chroma);
        return (h < 0.0f) ? 1.0f + h : ((h >= 1.0f) ? h - 1.0f : h);
    }

}

float Color::SaturationHSV(float min, float max) const
{
    // Avoid div-by-zero: result undefined
    if (max <= M_EPSILON)
        return 0.0f;

    // Saturation equals chroma:value ratio
    return 1.0f - (min / max);
}

float Color::SaturationHSL(float min, float max) const
{
    // Avoid div-by-zero: result undefined
    if (max <= M_EPSILON || min >= 1.0f - M_EPSILON)
        return 0.0f;

    // Chroma = max - min, lightness = (max + min) * 0.5
    float hl = (max + min);
    if (hl <= 1.0f)
        return (max - min) / hl;
    else
        return (min - max) / (hl - 2.0f);

}

void Color::FromHCM(float h, float c, float m)
{
    if (h < 0.0f || h >= 1.0f)
        h -= floorf(h);

    float hs = h * 6.0f;
    float x = c * (1.0f - Auto3D::Abs(fmodf(hs, 2.0f) - 1.0f));

    // Reconstruct r', g', b' from hue
    if (hs < 2.0f)
    {
        _b = 0.0f;
        if (hs < 1.0f)
        {
            _g = x;
            _r = c;
        }
        else
        {
            _g = c;
            _r = x;
        }
    }
    else if (hs < 4.0f)
    {
        _r = 0.0f;
        if (hs < 3.0f)
        {
            _g = c;
            _b = x;
        }
        else
        {
            _g = x;
            _b = c;
        }
    }
    else
    {
        _g = 0.0f;
        if (hs < 5.0f)
        {
            _r = x;
            _b = c;
        }
        else
        {
            _r = c;
            _b = x;
        }
    }

    _r += m;
    _g += m;
    _b += m;
}

}
