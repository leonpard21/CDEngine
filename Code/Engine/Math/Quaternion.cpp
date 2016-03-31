// Header Files
//=============
#include "Quaternion.h"
#include "Vector.h"
#include "ColMatrix.h"
#include "EulerAngle.h"
#include "MathTool.h"
#include "UserOutput/Source/Assert.h"
#include <cassert>
#include <cmath>
// Static Data Initialization
//===========================

namespace
{
  const float s_epsilon = 1.0e-9f;
}

// Interface
//==========

namespace EAE_Engine 
{
  namespace Math
  {
    // Concatenation
    Quaternion Quaternion::operator *(const Quaternion& i_rhs) const
    {
      return Quaternion(
        (_w * i_rhs._w) - ((_x * i_rhs._x) + (_y * i_rhs._y) + (_z * i_rhs._z)),
        (_w * i_rhs._x) + (_x * i_rhs._w) + ((_y * i_rhs._z) - (_z * i_rhs._y)),
        (_w * i_rhs._y) + (_y * i_rhs._w) + ((_z * i_rhs._x) - (_x * i_rhs._z)),
        (_w * i_rhs._z) + (_z * i_rhs._w) + ((_x * i_rhs._y) - (_y * i_rhs._x)));
    }

    Quaternion Quaternion::operator* (float value) const
    {
      return { _w * value, _x * value, _y * value, _z * value};
    }

    Quaternion Quaternion::operator+ (const Quaternion& i_rhs) const
    {
      return { _w + i_rhs._w, _x + i_rhs._x, _y + i_rhs._y, _z + i_rhs._z};
    }

    Quaternion Quaternion::operator- (const Quaternion& i_rhs) const
    {
      return{ _w - i_rhs._w, _x - i_rhs._x, _y - i_rhs._y, _z - i_rhs._z };
    }


    bool Quaternion::operator ==(const Quaternion& i_rhs) const
    {
      bool b0 = std::fabsf(_w - i_rhs._w) < 0.00001f;
      bool b1 = std::fabsf(_x - i_rhs._x) < 0.00001f;
      bool b2 = std::fabsf(_y - i_rhs._y) < 0.00001f;
      bool b3 = std::fabsf(_z - i_rhs._z) < 0.00001f;
      return b0 && b1 && b2 && b3;
    }

    Quaternion& Quaternion::operator= (const Quaternion& right)
    {
      _w = right._w;
      _x = right._x;
      _y = right._y;
      _z = right._z;
      return *this;
    }

    // Inversion
    void Quaternion::Invert()
    {
      // since in most cases we're only using the quaternion for rotation, 
      // which meanes the magnitude is 1.0f, 
      // we only need to conjugate it.
      float sqrtMangnitude = GetSqMagnitude();
      if (sqrtMangnitude - 1.0f < 0.0001f)
      {
        return Conjugate();
      }
      float magnitude = GetMagnitude() + FLT_EPSILON;
      _x = -_x / magnitude;
      _y = -_y / magnitude;
      _z = -_z / magnitude;
    }

    Quaternion Quaternion::GetInverse() const
    {
      Quaternion clone = *this;
      clone.Invert();
      return clone;
    }

    void Quaternion::Conjugate()
    {
      _x = -_x;
      _y = -_y;
      _z = -_z;
    }

    Quaternion Quaternion::GetConjugate() const
    {
      return {_w, -_x, -_y, -_z};
    }

    // Exponentiation
    // becareful this is exponentiate, q^exponent; not the exponential function, e^q. 
    // range of exponent is [0, 1].
    // so the range of q^exponent is [(1, Vector::zero), q]
    void Quaternion::Pow(float exponent)
    {
      exponent = clamp<float>(exponent, 0.0f, 1.0f);
      // Check for the identitry quaternion case.
      // this protects the divide zero case.
      if (Abs<float>(_w) < 0.9999f) 
      {
        // cos(theta/2.0f) = _w, 
        // theta is the actually angle this quaternion rotates
        float half_Theta = std::acosf(_w);
        float newHalfTheta = half_Theta * exponent;
        // update w
        _w = std::cosf(newHalfTheta);
        // update x, y, z
        float mult = std::sinf(newHalfTheta) / std::sinf(half_Theta);
        _x *= mult;
        _y *= mult;
        _z *= mult;
      }
      // ignore the identity quaternion 
    }

    // Get Exponentiation
    Quaternion Quaternion::GetPow(float exponent) const
    {
      Quaternion clone = *this;
      clone.Pow(exponent);
      return clone;
    }

    // Normalization
    void Quaternion::Normalize()
    {
      const float length = std::sqrt((_w * _w) + (_x * _x) + (_y * _y) + (_z * _z));
      assert(length > s_epsilon);
      const float length_reciprocal = 1.0f / length;
      _w *= length_reciprocal;
      _x *= length_reciprocal;
      _y *= length_reciprocal;
      _z *= length_reciprocal;
    }

    Quaternion Quaternion::CreateNormalized() const
    {
      const float length = std::sqrt((_w * _w) + (_x * _x) + (_y * _y) + (_z * _z));
      assert(length > s_epsilon);
      const float length_reciprocal = 1.0f / length;
      return Quaternion(_w * length_reciprocal, _x * length_reciprocal, _y * length_reciprocal, _z * length_reciprocal);
    }

    // For a rotation quaternion, v = (x, y, z) is a normal vector.
    // we will get |q| = sqrt(cos(theta/2)^2 + sin(theta/2)^2 * |n|^2)
    // which = 1.
    float Quaternion::GetMagnitude() const
    {
      return std::sqrtf(_w * _w + _x * _x + _y * _y + _z * _z);
    }

    float Quaternion::GetSqMagnitude() const
    {
      return _w * _w + _x * _x + _y * _y + _z * _z;
    }


    Vector3 Quaternion::GetVec() const
    {
      return Vector3(_x, _y, _z);
    }

    // Because for any angular displacement in 3D, 
    // it has exactly two distinct representations in quaternion format, 
    // and they are negatives of each other.
    // So actually Quaternion(-1.f, 0.f, 0.f, 0.f); is also Identity, 
    // but if we multi the w = -1f.0 version, we will get -q.
    // so we still use the w= 1.0f version.
    Quaternion Quaternion::Identity = Quaternion(1.f, 0.f, 0.f, 0.f);

    // Initialization / Shut Down
    //---------------------------

    Quaternion::Quaternion() :
      _w(1.0f), _x(0.0f), _y(0.0f), _z(0.0f)
    {
    }

    Quaternion::Quaternion(const float i_angleInRadians, const Vector3& i_axisOfRotation_normalized)
    {
      MessagedAssert(i_axisOfRotation_normalized.Magnitude() > 0.00001f, "axis of quaternion cannot be zero vector.");

      // [w, v] = [cos(?/2), sin(?/2)n]
      // [w, (x, y, z)] = [cos(?/2), (sin(?/2)nx, sin(?/2)ny, sin(?/2)nz)]
      const float theta_half = i_angleInRadians * 0.5f;
      _w = std::cosf(theta_half);
      const float sin_theta_half = std::sinf(theta_half);
      _x = i_axisOfRotation_normalized._x * sin_theta_half;
      _y = i_axisOfRotation_normalized._y * sin_theta_half;
      _z = i_axisOfRotation_normalized._z * sin_theta_half;
    }

    Quaternion::Quaternion(const Quaternion& i_other) :
      _w(i_other._w), _x(i_other._x), _y(i_other._y), _z(i_other._z)
    {
        
    }

    Quaternion::Quaternion(float pitch, float heading, float bank)
    {
      *this = EulerAngle::GetQuaternion({ pitch, heading, bank });
    }

    // Implementation
    //===============

    // Initialization / Shut Down
    //---------------------------
    Quaternion::Quaternion(const float i_w, const float i_x, const float i_y, const float i_z) :
       _w(i_w), _x(i_x), _y(i_y), _z(i_z)
    {

    }

    // The referense is: 
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
    ColMatrix44 Quaternion::CreateColMatrix(const Quaternion& i_rotation)
    {
      ColMatrix44 result = ColMatrix44::Identity;
      const float _2x = i_rotation._x + i_rotation._x;
      const float _2y = i_rotation._y + i_rotation._y;
      const float _2z = i_rotation._z + i_rotation._z;
      const float _2xx = i_rotation._x * _2x;
      const float _2xy = _2x * i_rotation._y;
      const float _2xz = _2x * i_rotation._z;
      const float _2xw = _2x * i_rotation._w;
      const float _2yy = _2y * i_rotation._y;
      const float _2yz = _2y * i_rotation._z;
      const float _2yw = _2y * i_rotation._w;
      const float _2zz = _2z * i_rotation._z;
      const float _2zw = _2z * i_rotation._w;
      result._m00 = 1.0f - _2yy - _2zz;
      result._m10 = _2xy + _2zw;
      result._m20 = _2xz - _2yw;
      result._m01 = _2xy - _2zw;
      result._m11 = 1.0f - _2xx - _2zz;
      result._m21 = _2yz + _2xw;
      result._m02 = _2xz + _2yw;
      result._m12 = _2yz - _2xw;
      result._m22 = 1.0f - _2xx - _2yy;
      return result;
    }

    // Converting an object-to-upright quaternion to Euler angles
    // Come from 3DMath Primier for Graphics and Game development 2nd, Chaptor 8.7.6
    // The x, y, and z angles represent a rotation 
    // z degrees around the z axis, (bank)
    // x degrees around the x axis, (pitch)
    // and y degrees around the y axis (heading) (in that order).
    Vector3 Quaternion::CreateEulerAngle(const Quaternion& i_rotation)
    {
      assert(fabsf(i_rotation.GetMagnitude() - 1.0f) < 0.001f);
      float w = i_rotation._w;
      float x = i_rotation._x;
      float y = i_rotation._y;
      float z = i_rotation._z;

      // Output Euler angles(radians)
      float heading = 0.0f, pitch = 0.0f, bank = 0.0f;

      // sin(Pitch)
      float sp = -2.0f * (y * z + w * x);
      // Check for Gimbal Lock
      if (std::fabsf(sp) > 0.999f) 
      {
        // This is the Gimbal Lock case.
        // the pitch is looking for stright up or down
        // We just calculate the heading and make bank to 0.0f
        pitch = Math::Pi * 0.5f; // pitch
        heading = std::atan2f(-x * z - w * y, 0.5f - y * y - z * z); //heading
        bank = 0.0f; // bank       
      }
      else 
      {
        pitch = std::asinf(sp); // pitch 
        heading = std::atan2f(x * z - w * y, 0.5f - x * x - y * y);// heading 
        bank = std::atan2f(x * y - w * z, 0.5f - x * x - z * z);// bank
      }
      // result is radians
      Vector3 result(bank, pitch, heading);
      return result;
    }

    // Just like lerp, Result = a0 + deltA * t, deltA = a1 - a0.
    // So we need three steps:
    // 1. comuter deltaA, 2. t * deltaA, 3. a0 + t * deltaA.
    // For Slerp, it's the same thing.
    // as a result, slerp(q0, q1, t) = (q1 * q0.inverse())^t * q0.
    // for speed reason, we use:
    // slerp(q0, q1, t) = sin((1 ? t) * Omega)/sin(Omega) * q0 + sin(t * Omega)/sin(Omega) * q1.
    // Omega is the angle between q0 and q1.
    Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, float t)
    {
      t = clamp<float>(t, 0.0f, 1.0f);
      Quaternion result = Quaternion::Identity;
      Quaternion target = q1;
      // cos of the angle between q0, q1
      float cosOmega = Dot(q0, target);
      // this cosOmega sign check is trying to: 
      // avoid some differnt for doing a longer or shorter rotation arc from q0 to q1.
      // by doing this, we only use the shortest rotation arc.
      if (cosOmega < 0.0f) 
      {
        target = target * -1.0f;
        cosOmega *= -1.0f;
      }
      // also avoid that q0 and q1 are almost the same
      // if they are too close, then we just use the lerp.
      float k0 = 1.0f - t;
      float k1 = t;
      if (cosOmega < 0.9999f)
      {
        float sinOmega = std::sqrtf(1.0f - cosOmega * cosOmega);
        float omega = std::atan2f(sinOmega, cosOmega);
        float oneOverSinOmega = 1.0f / sinOmega;

        k0 = std::sinf(k0 * omega) * oneOverSinOmega;
        k1 = std::sinf(k1 * omega) * oneOverSinOmega;
      }
      result = q0 * k0 + target * k1;
      return result;
    }

    // Get angular difference between 2 quaternions,
    // in fact, it's more like a division than a true difference.
    Quaternion Quaternion::GetDifference(const Quaternion& from, const Quaternion& to)
    {
      return to * from.GetInverse();
    }

    Quaternion Quaternion::RotationBetween2Vectors(Vector3 from, Vector3 to)
    {
      from.Normalize();
      to.Normalize();
      //http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
      Vector3 w = Vector3::Cross(from, to);
      Quaternion q = Quaternion(1.0f + Vector3::Dot(from, to), w._x, w._y, w._z);
      q.Normalize();
      return q;
      /*
      // Get the angle and axis between from and to.
      float cosTheta = Vector3::Dot(from, to);
      Vector3 rotationAxis;
      // if from and to are almost on opposite direction.
      if (cosTheta < -1.0f + 0.001f) 
      {
        rotationAxis = Vector3::Cross(Vector3::Forward, from);
        if (rotationAxis.Magnitude() < 0.01f)
          rotationAxis = Vector3::Cross(Vector3::Right, from);
        rotationAxis.Normalize();
        return Quaternion(Pi, rotationAxis);
      }
      // get the axis
      rotationAxis = Vector3::Cross(from, to);
      rotationAxis.Normalize();
      // _w = std::cos(theta/2), 
      // cos(theta) = 2 * cos(theta/2)^2 - 1.
      // cos(theta) = 1 - 2 * sin(theta/2)^2.
      float w = std::sqrtf((cosTheta + 1.0f) * 0.5f);
      float sinTheta_half = std::sqrtf((1.0f - cosTheta) * 0.5f);

      return Quaternion(
        w,
        rotationAxis._x * sinTheta_half,
        rotationAxis._y * sinTheta_half,
        rotationAxis._z * sinTheta_half);
        */
    }
    
    // Creates a rotation with the specified forward and upwards directions.
    // Assuming that the forwad and upward are orthogonal.
    Quaternion Quaternion::LookRotation(Vector3 forward, Vector3 upward)
    {
      forward = forward * -1.0f;
      forward.Normalize();
      upward.Normalize();

      Quaternion result = Quaternion::Identity;

      // make sure we are not deal with zero vector
      if (forward.Magnitude() < 0.0001f)
      return Quaternion::Identity;
      Vector3 right = Vector3::Cross(upward, forward).GetNormalize();
      Vector3 idealUp = Vector3::Cross(forward, right).GetNormalize();
      if (Vector3::Dot(idealUp, upward) < 0.0f)
      {
      right = right * -1.0f;
      idealUp = idealUp * -1.0f;
      }

      ColMatrix44 matrix = ColMatrix44::Identity;
      // right
      matrix._m00 = right._x;
      matrix._m10 = right._y;
      matrix._m20 = right._z;
      // up
      matrix._m01 = idealUp._x;
      matrix._m11 = idealUp._y;
      matrix._m21 = idealUp._z;
      // forward
      matrix._m02 = forward._x;
      matrix._m12 = forward._y;
      matrix._m22 = forward._z;
      return ColMatrix44::CreateQuaternion(matrix);
    }
    
    /*
    Quaternion Quaternion::LookRotation(Vector3 forward, Vector3 up)
    {
      forward.Normalize();
      up.Normalize();
      Vector3 right = Vector3::Cross(up, forward).GetNormalize();
      up = Vector3::Cross(forward, right);
      float m00 = right._x;
      float m01 = right._y;
      float m02 = right._z;
      float m10 = up._x;
      float m11 = up._y;
      float m12 = up._z;
      float m20 = forward._x;
      float m21 = forward._y;
      float m22 = forward._z;

      float num8 = (m00 + m11) + m22;
      Quaternion quaternion = Quaternion::Identity;
      if (num8 > 0.0f)
      {
        float num = sqrtf(num8 + 1.0f);
        quaternion._w = num * 0.5f;
        num = 0.5f / num;
        quaternion._x = (m12 - m21) * num;
        quaternion._y = (m20 - m02) * num;
        quaternion._z = (m01 - m10) * num;
        return quaternion;
      }
      if ((m00 >= m11) && (m00 >= m22))
      {
        float num7 = sqrtf(((1.0f + m00) - m11) - m22);
        float num4 = 0.5f / num7;
        quaternion._x = 0.5f * num7;
        quaternion._y = (m01 + m10) * num4;
        quaternion._z = (m02 + m20) * num4;
        quaternion._w = (m12 - m21) * num4;
        return quaternion;
      }
      if (m11 > m22)
      {
        float num6 = sqrtf(((1.0f + m11) - m00) - m22);
        float num3 = 0.5f / num6;
        quaternion._x = (m10 + m01) * num3;
        quaternion._y = 0.5f * num6;
        quaternion._z = (m21 + m12) * num3;
        quaternion._w = (m20 - m02) * num3;
        return quaternion;
      }
      float num5 = sqrtf(((1.0f + m22) - m00) - m11);
      float num2 = 0.5f / num5;
      quaternion._x = (m20 + m02) * num2;
      quaternion._y = (m21 + m12) * num2;
      quaternion._z = 0.5f * num5;
      quaternion._w = (m01 - m10) * num2;
      return quaternion.GetInverse();
    }
    */
    Vector3 Quaternion::MultiVector(const Quaternion& i_rotation, const Math::Vector3& i_vec)
    {
      float num = i_rotation._x * 2.0f;
      float num2 = i_rotation._y * 2.0f;
      float num3 = i_rotation._z * 2.0f;
      float num4 = i_rotation._x * num;
      float num5 = i_rotation._y * num2;
      float num6 = i_rotation._z * num3;
      float num7 = i_rotation._x * num2;
      float num8 = i_rotation._x * num3;
      float num9 = i_rotation._y * num3;
      float num10 = i_rotation._w * num;
      float num11 = i_rotation._w * num2;
      float num12 = i_rotation._w * num3;
      Vector3 result = Vector3::Zero;
      result._x = (1.0f - (num5 + num6)) * i_vec._x + (num7 - num12) * i_vec._y + (num8 + num11) * i_vec._z;
      result._y = (num7 + num12) * i_vec._x + (1.0f - (num4 + num6)) * i_vec._y + (num9 - num10) * i_vec._z;
      result._z = (num8 - num11) * i_vec._x + (num9 + num10) * i_vec._y + (1.0f - (num4 + num5)) * i_vec._z;
      return result;
    }

    // Products
    float Dot(const Quaternion& i_lhs, const Quaternion& i_rhs)
    {
      return (i_lhs._w * i_rhs._w) + (i_lhs._x * i_rhs._x) + (i_lhs._y * i_rhs._y) + (i_lhs._z * i_rhs._z);
    }

  }
}



