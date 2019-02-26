#pragma once

#include "Matrix4.h"

namespace Auto3D
{

/// 3x4 matrix for scene node transform calculations.
class AUTO_API Matrix3x4
{
public:
    /// Matrix values.
    float _m00, _m01, _m02, _m03;
    float _m10, _m11, _m12, _m13;
    float _m20, _m21, _m22, _m23;
    
    /// Construct undefined.
    Matrix3x4()
    {
    }
    
    /// Copy-construct.
    Matrix3x4(const Matrix3x4& matrix) :
        _m00(matrix._m00), _m01(matrix._m01), _m02(matrix._m02), _m03(matrix._m03),
        _m10(matrix._m10), _m11(matrix._m11), _m12(matrix._m12), _m13(matrix._m13),
        _m20(matrix._m20), _m21(matrix._m21), _m22(matrix._m22), _m23(matrix._m23)
    {
    }
    
    /// Copy-construct from a 3x3 matrix and set the extra elements to identity.
    Matrix3x4(const Matrix3& matrix) :
        _m00(matrix._m00), _m01(matrix._m01), _m02(matrix._m02), _m03(0.0f),
        _m10(matrix._m10), _m11(matrix._m11), _m12(matrix._m12), _m13(0.0f),
        _m20(matrix._m20), _m21(matrix._m21), _m22(matrix._m22), _m23(0.0f)
    {
    }
    
    /// Copy-construct from a 4x4 matrix which is assumed to contain no projection.
    Matrix3x4(const Matrix4& matrix) :
        _m00(matrix._m00), _m01(matrix._m01), _m02(matrix._m02), _m03(matrix._m03),
        _m10(matrix._m10), _m11(matrix._m11), _m12(matrix._m12), _m13(matrix._m13),
        _m20(matrix._m20), _m21(matrix._m21), _m22(matrix._m22), _m23(matrix._m23)
    {
    }
    
    // Construct from values.
    Matrix3x4(float v00, float v01, float v02, float v03,
            float v10, float v11, float v12, float v13,
            float v20, float v21, float v22, float v23) :
        _m00(v00), _m01(v01), _m02(v02), _m03(v03),
        _m10(v10), _m11(v11), _m12(v12), _m13(v13),
        _m20(v20), _m21(v21), _m22(v22), _m23(v23)
    {
    }
    
    /// Construct from a float array.
    Matrix3x4(const float* data) :
        _m00(data[0]), _m01(data[1]), _m02(data[2]), _m03(data[3]),
        _m10(data[4]), _m11(data[5]), _m12(data[6]), _m13(data[7]),
        _m20(data[8]), _m21(data[9]), _m22(data[10]), _m23(data[11])
    {
    }
    
    /// Construct from translation, rotation and uniform scale.
    Matrix3x4(const Vector3& translation, const Quaternion& rotation, float scale);
    /// Construct from translation, rotation and nonuniform scale.
    Matrix3x4(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
    
    /// Construct by parsing a string.
    Matrix3x4(const String& str)
    {
        FromString(str);
    }
    
    /// Construct by parsing a C string.
    Matrix3x4(const char* str)
    {
        FromString(str);
    }
    
    /// Assign from another matrix.
    Matrix3x4& operator = (const Matrix3x4& rhs)
    {
        _m00 = rhs._m00; _m01 = rhs._m01; _m02 = rhs._m02; _m03 = rhs._m03;
        _m10 = rhs._m10; _m11 = rhs._m11; _m12 = rhs._m12; _m13 = rhs._m13;
        _m20 = rhs._m20; _m21 = rhs._m21; _m22 = rhs._m22; _m23 = rhs._m23;
        return *this;
    }
    
    /// Assign from a 3x3 matrix and set the extra elements to identity.
    Matrix3x4& operator = (const Matrix3& rhs)
    {
        _m00 = rhs._m00; _m01 = rhs._m01; _m02 = rhs._m02; _m03 = 0.0;
        _m10 = rhs._m10; _m11 = rhs._m11; _m12 = rhs._m12; _m13 = 0.0;
        _m20 = rhs._m20; _m21 = rhs._m21; _m22 = rhs._m22; _m23 = 0.0;
        return *this;
    }
    
    /// Assign from a 4x4 matrix which is assumed to contain no projection.
    Matrix3x4& operator = (const Matrix4& rhs)
    {
        _m00 = rhs._m00; _m01 = rhs._m01; _m02 = rhs._m02; _m03 = rhs._m03;
        _m10 = rhs._m10; _m11 = rhs._m11; _m12 = rhs._m12; _m13 = rhs._m13;
        _m20 = rhs._m20; _m21 = rhs._m21; _m22 = rhs._m22; _m23 = rhs._m23;
        return *this;
    }
    
    /// Test for equality with another matrix without epsilon.
    bool operator == (const Matrix3x4& rhs) const
    {
        const float* leftData = Data();
        const float* rightData = rhs.Data();
        
        for (size_t i = 0; i < 12; ++i)
        {
            if (leftData[i] != rightData[i])
                return false;
        }
        
        return true;
    }
    
    /// Test for inequality with another matrix without epsilon.
    bool operator != (const Matrix3x4& rhs) const { return !(*this == rhs); }
    
    /// Multiply a Vector3 which is assumed to represent _position.
    Vector3 operator * (const Vector3& rhs) const
    {
        return Vector3(
            (_m00 * rhs._x + _m01 * rhs._y + _m02 * rhs._z + _m03),
            (_m10 * rhs._x + _m11 * rhs._y + _m12 * rhs._z + _m13),
            (_m20 * rhs._x + _m21 * rhs._y + _m22 * rhs._z + _m23)
        );
    }
    
    /// Multiply a Vector4.
    Vector3 operator * (const Vector4& rhs) const
    {
        return Vector3(
            (_m00 * rhs._x + _m01 * rhs._y + _m02 * rhs._z + _m03 * rhs._w),
            (_m10 * rhs._x + _m11 * rhs._y + _m12 * rhs._z + _m13 * rhs._w),
            (_m20 * rhs._x + _m21 * rhs._y + _m22 * rhs._z + _m23 * rhs._w)
        );
    }
    
    /// Add a matrix.
    Matrix3x4 operator + (const Matrix3x4& rhs) const
    {
        return Matrix3x4(
            _m00 + rhs._m00, _m01 + rhs._m01, _m02 + rhs._m02, _m03 + rhs._m03,
            _m10 + rhs._m10, _m11 + rhs._m11, _m12 + rhs._m12, _m13 + rhs._m13,
            _m20 + rhs._m20, _m21 + rhs._m21, _m22 + rhs._m22, _m23 + rhs._m23
        );
    }
    
    /// Subtract a matrix.
    Matrix3x4 operator - (const Matrix3x4& rhs) const
    {
        return Matrix3x4(
            _m00 - rhs._m00, _m01 - rhs._m01, _m02 - rhs._m02, _m03 - rhs._m03,
            _m10 - rhs._m10, _m11 - rhs._m11, _m12 - rhs._m12, _m13 - rhs._m13,
            _m20 - rhs._m20, _m21 - rhs._m21, _m22 - rhs._m22, _m23 - rhs._m23
        );
    }
    
    /// Multiply with a scalar.
    Matrix3x4 operator * (float rhs) const
    {
        return Matrix3x4(
            _m00 * rhs, _m01 * rhs, _m02 * rhs, _m03 * rhs,
            _m10 * rhs, _m11 * rhs, _m12 * rhs, _m13 * rhs,
            _m20 * rhs, _m21 * rhs, _m22 * rhs, _m23 * rhs
        );
    }
    
    /// Multiply a matrix.
    Matrix3x4 operator * (const Matrix3x4& rhs) const
    {
        return Matrix3x4(
            _m00 * rhs._m00 + _m01 * rhs._m10 + _m02 * rhs._m20,
            _m00 * rhs._m01 + _m01 * rhs._m11 + _m02 * rhs._m21,
            _m00 * rhs._m02 + _m01 * rhs._m12 + _m02 * rhs._m22,
            _m00 * rhs._m03 + _m01 * rhs._m13 + _m02 * rhs._m23 + _m03,
            _m10 * rhs._m00 + _m11 * rhs._m10 + _m12 * rhs._m20,
            _m10 * rhs._m01 + _m11 * rhs._m11 + _m12 * rhs._m21,
            _m10 * rhs._m02 + _m11 * rhs._m12 + _m12 * rhs._m22,
            _m10 * rhs._m03 + _m11 * rhs._m13 + _m12 * rhs._m23 + _m13,
            _m20 * rhs._m00 + _m21 * rhs._m10 + _m22 * rhs._m20,
            _m20 * rhs._m01 + _m21 * rhs._m11 + _m22 * rhs._m21,
            _m20 * rhs._m02 + _m21 * rhs._m12 + _m22 * rhs._m22,
            _m20 * rhs._m03 + _m21 * rhs._m13 + _m22 * rhs._m23 + _m23
        );
    }
    
    /// Multiply a 4x4 matrix.
    Matrix4 operator * (const Matrix4& rhs) const
    {
        return Matrix4(
            _m00 * rhs._m00 + _m01 * rhs._m10 + _m02 * rhs._m20 + _m03 * rhs._m30,
            _m00 * rhs._m01 + _m01 * rhs._m11 + _m02 * rhs._m21 + _m03 * rhs._m31,
            _m00 * rhs._m02 + _m01 * rhs._m12 + _m02 * rhs._m22 + _m03 * rhs._m32,
            _m00 * rhs._m03 + _m01 * rhs._m13 + _m02 * rhs._m23 + _m03 * rhs._m33,
            _m10 * rhs._m00 + _m11 * rhs._m10 + _m12 * rhs._m20 + _m13 * rhs._m30,
            _m10 * rhs._m01 + _m11 * rhs._m11 + _m12 * rhs._m21 + _m13 * rhs._m31,
            _m10 * rhs._m02 + _m11 * rhs._m12 + _m12 * rhs._m22 + _m13 * rhs._m32,
            _m10 * rhs._m03 + _m11 * rhs._m13 + _m12 * rhs._m23 + _m13 * rhs._m33,
            _m20 * rhs._m00 + _m21 * rhs._m10 + _m22 * rhs._m20 + _m23 * rhs._m30,
            _m20 * rhs._m01 + _m21 * rhs._m11 + _m22 * rhs._m21 + _m23 * rhs._m31,
            _m20 * rhs._m02 + _m21 * rhs._m12 + _m22 * rhs._m22 + _m23 * rhs._m32,
            _m20 * rhs._m03 + _m21 * rhs._m13 + _m22 * rhs._m23 + _m23 * rhs._m33,
            rhs._m30,
            rhs._m31,
            rhs._m32,
            rhs._m33
        );
    }
    
    /// Set translation elements.
    void SetTranslation(const Vector3& translation)
    {
        _m03 = translation._x;
        _m13 = translation._y;
        _m23 = translation._z;
    }
    
    /// Set rotation elements from a 3x3 matrix.
    void SetRotation(const Matrix3& rotation)
    {
        _m00 = rotation._m00; _m01 = rotation._m01; _m02 = rotation._m02;
        _m10 = rotation._m10; _m11 = rotation._m11; _m12 = rotation._m12;
        _m20 = rotation._m20; _m21 = rotation._m21; _m22 = rotation._m22;
    }
    
    /// Set scaling elements.
    void SetScale(const Vector3& scale)
    {
        _m00 = scale._x;
        _m11 = scale._y;
        _m22 = scale._z;
    }
    
    /// Set uniform scaling elements.
    void SetScale(float scale)
    {
        _m00 = scale;
        _m11 = scale;
        _m22 = scale;
    }
    
    /// Parse from a string. Return true on success.
    bool FromString(const String& str);
    /// Parse from a C string. Return true on success.
    bool FromString(const char* str);
    
    /// Return the combined rotation and scaling matrix.
    Matrix3 ToMatrix3() const
    {
        return Matrix3(
            _m00, _m01, _m02,
            _m10, _m11, _m12,
            _m20, _m21, _m22
        );
    }
    
    /// Convert to a 4x4 matrix by filling in an identity last row.
    Matrix4 ToMatrix4() const
    {
        return Matrix4(
            _m00, _m01, _m02, _m03,
            _m10, _m11, _m12, _m13,
            _m20, _m21, _m22, _m23,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
    
    /// Return the rotation matrix with scaling removed.
    Matrix3 RotationMatrix() const
    {
        Vector3 invScale(
            1.0f / sqrtf(_m00 * _m00 + _m10 * _m10 + _m20 * _m20),
            1.0f / sqrtf(_m01 * _m01 + _m11 * _m11 + _m21 * _m21),
            1.0f / sqrtf(_m02 * _m02 + _m12 * _m12 + _m22 * _m22)
        );
        
        return ToMatrix3().Scaled(invScale);
    }
    
    /// Return the translation part.
    Vector3 Translation() const
    {
        return Vector3(
            _m03,
            _m13,
            _m23
        );
    }
    
    /// Return the rotation part.
    Quaternion Rotation() const { return Quaternion(RotationMatrix()); }
    
    /// Return the scaling part.
    Vector3 Scale() const
    {
        return Vector3(
            sqrtf(_m00 * _m00 + _m10 * _m10 + _m20 * _m20),
            sqrtf(_m01 * _m01 + _m11 * _m11 + _m21 * _m21),
            sqrtf(_m02 * _m02 + _m12 * _m12 + _m22 * _m22)
        );
    }
    
    /// Test for equality with another matrix with epsilon.
    bool Equals(const Matrix3x4& rhs) const
    {
        const float* leftData = Data();
        const float* rightData = rhs.Data();
        
        for (size_t i = 0; i < 12; ++i)
        {
            if (!Auto3D::Equals(leftData[i], rightData[i]))
                return false;
        }
        
        return true;
    }
    
    /// Return decomposition to translation, rotation and scale.
    void Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const;
    /// Return inverse.
    Matrix3x4 Inverse() const;
    
    /// Return float data.
    const float* Data() const { return &_m00; }
    /// Return as string.
    String ToString() const;
    
    /// Zero matrix.
    static const Matrix3x4 ZERO;
    /// Identity matrix.
    static const Matrix3x4 IDENTITY;
};

/// Multiply a 3x4 matrix with a scalar.
inline Matrix3x4 operator * (float lhs, const Matrix3x4& rhs) { return rhs * lhs; }

/// Multiply a 3x4 matrix with a 4x4 matrix.
inline Matrix4 operator * (const Matrix4& lhs, const Matrix3x4& rhs)
{
    return Matrix4(
        lhs._m00 * rhs._m00 + lhs._m01 * rhs._m10 + lhs._m02 * rhs._m20,
        lhs._m00 * rhs._m01 + lhs._m01 * rhs._m11 + lhs._m02 * rhs._m21,
        lhs._m00 * rhs._m02 + lhs._m01 * rhs._m12 + lhs._m02 * rhs._m22,
        lhs._m00 * rhs._m03 + lhs._m01 * rhs._m13 + lhs._m02 * rhs._m23 + lhs._m03,
        lhs._m10 * rhs._m00 + lhs._m11 * rhs._m10 + lhs._m12 * rhs._m20,
        lhs._m10 * rhs._m01 + lhs._m11 * rhs._m11 + lhs._m12 * rhs._m21,
        lhs._m10 * rhs._m02 + lhs._m11 * rhs._m12 + lhs._m12 * rhs._m22,
        lhs._m10 * rhs._m03 + lhs._m11 * rhs._m13 + lhs._m12 * rhs._m23 + lhs._m13,
        lhs._m20 * rhs._m00 + lhs._m21 * rhs._m10 + lhs._m22 * rhs._m20,
        lhs._m20 * rhs._m01 + lhs._m21 * rhs._m11 + lhs._m22 * rhs._m21,
        lhs._m20 * rhs._m02 + lhs._m21 * rhs._m12 + lhs._m22 * rhs._m22,
        lhs._m20 * rhs._m03 + lhs._m21 * rhs._m13 + lhs._m22 * rhs._m23 + lhs._m23,
        lhs._m30 * rhs._m00 + lhs._m31 * rhs._m10 + lhs._m32 * rhs._m20,
        lhs._m30 * rhs._m01 + lhs._m31 * rhs._m11 + lhs._m32 * rhs._m21,
        lhs._m30 * rhs._m02 + lhs._m31 * rhs._m12 + lhs._m32 * rhs._m22,
        lhs._m30 * rhs._m03 + lhs._m31 * rhs._m13 + lhs._m32 * rhs._m23 + lhs._m33
    );
}

}
