#pragma once

#include "../AutoConfig.h"
#include "Math.h"

namespace Auto3D
{

class String;

/// Two-dimensional vector with integer values.
class AUTO_API IntVector2
{
public:
    /// X coordinate.
    int _x;
    /// Y coordinate.
    int _y;
    
    /// Construct undefined.
    IntVector2()
    {
    }
    
    /// Copy-construct.
    IntVector2(const IntVector2& vector) :
        _x(vector._x),
        _y(vector._y)
    {
    }
    
    /// Construct from coordinates.
    IntVector2(int x_, int y_) :
        _x(x_),
        _y(y_)
    {
    }
    
    /// Construct from an int array.
    IntVector2(const int* data) :
        _x(data[0]),
        _y(data[1])
    {
    }
    
    /// Construct by parsing a string.
    IntVector2(const String& str)
    {
        FromString(str);
    }
    
    /// Construct by parsing a C string.
    IntVector2(const char* str)
    {
        FromString(str);
    }
    
    /// Add-assign a vector.
    IntVector2& operator += (const IntVector2& rhs)
    {
        _x += rhs._x;
        _y += rhs._y;
        return *this;
    }
    
    /// Subtract-assign a vector.
    IntVector2& operator -= (const IntVector2& rhs)
    {
        _x -= rhs._x;
        _y -= rhs._y;
        return *this;
    }
    
    /// Multiply-assign a scalar.
    IntVector2& operator *= (int rhs)
    {
        _x *= rhs;
        _y *= rhs;
        return *this;
    }
    
    /// Divide-assign a scalar.
    IntVector2& operator /= (int rhs)
    {
        _x /= rhs;
        _y /= rhs;
        return *this;
    }
    
    /// Test for equality with another vector.
    bool operator == (const IntVector2& rhs) const { return _x == rhs._x && _y == rhs._y; }
    /// Test for inequality with another vector.
    bool operator != (const IntVector2& rhs) const { return !(*this == rhs); }
    /// Add a vector.
    IntVector2 operator + (const IntVector2& rhs) const { return IntVector2(_x + rhs._x, _y + rhs._y); }
    /// Return negation.
    IntVector2 operator - () const { return IntVector2(-_x, -_y); }
    /// Subtract a vector.
    IntVector2 operator - (const IntVector2& rhs) const { return IntVector2(_x - rhs._x, _y - rhs._y); }
    /// Multiply with a scalar.
    IntVector2 operator * (int rhs) const { return IntVector2(_x * rhs, _y * rhs); }
    /// Divide by a scalar.
    IntVector2 operator / (int rhs) const { return IntVector2(_x / rhs, _y / rhs); }

    /// Parse from a string. Return true on success.
    bool FromString(const String& str);
    /// Parse from a C string. Return true on success.
    bool FromString(const char* str);
    
    /// Return integer data.
    const int* Data() const { return &_x; }
    /// Return as string.
    String ToString() const;
    
    /// Zero vector.
    static const IntVector2 ZERO;
};

/// Multiply IntVector2 with a scalar.
inline IntVector2 operator * (int lhs, const IntVector2& rhs) { return rhs * lhs; }

}

