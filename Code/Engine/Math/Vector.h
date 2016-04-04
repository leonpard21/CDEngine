#ifndef TVECTOR3_H
#define TVECTOR3_H
#include <math.h>
#include "Engine/General/Implements.h"
#include "Engine/UserOutput/Source/EngineDebuger.h"

namespace EAE_Engine
{
  namespace Math
  {
    template <typename T>
    class TVector2
    {
    public:
      TVector2() {}
      TVector2(const T x, const T y);
      TVector2(const TVector2& in)
      {
          _x = in._x;
          _y = in._y;
      }
      inline void Set(const T x, const T y);
      const static TVector2 Zero;

      //protected:
      union
      {
          struct { T _x; T _y; };
          T _u[2];
      };

    };

    template <typename T>
    TVector2<T>::TVector2(const T x, const T y) :
        _x(x), _y(y)
    {
    }

    template <typename T>
    const TVector2<T> TVector2<T>::Zero(T(0), T(0));

    template <typename T>
    inline void TVector2<T>::Set(const T x, const T y)
    {
        _x = x;
        _y = y;
    }

    typedef TVector2<float> Vector2;

    //////////////////////////////////////
    template<typename T>
    class TVector4;

    template <typename T>
    class TVector3
    {
    public:
      TVector3() {}
      TVector3(const T x, const T y, const T z);
      TVector3(const TVector3& in)
      {
          _x = in._x;
          _y = in._y;
          _z = in._z;
      }

      inline void Set(const T x, const T y, const T z);
      inline T x() const{ return _x; }
      inline T y() const{ return _y; }
      inline T z() const{ return _z; }
      inline void x(const T tx){ _x = tx; }
      inline void y(const T ty){ _y = ty; }
      inline void z(const T tz){ _z = tz; }

      inline TVector3 Normalize();
      inline TVector3 GetNormalize() const;
      inline T Dot(const TVector3<T>& right) const;
      inline TVector3 Cross(const TVector3<T>& right) const;
      inline T SqMagnitude() const { return _x * _x + _y*_y + _z*_z; }
      inline T Magnitude() const { return sqrt(_x * _x + _y*_y + _z*_z); }
      inline TVector3 GetEdge(TVector3& point_another);

      inline TVector3& operator+= (const TVector3& right);
      inline TVector3& operator= (const TVector3& right);
      inline TVector3 operator* (T f) const;
      inline TVector3& operator*=(const T right);
            
      //Wrap a vector4 to create a vector3 
      inline TVector3(const TVector4<T>& in);
      inline TVector3& operator= (const TVector4<T>& right);

      // Static version of the functions
      static float Dot(const TVector3<T>& left, const TVector3<T>& right);
      static TVector3<T> Project(const TVector3<T>& left, const TVector3<T>& right);
      static TVector3<T> Cross(const TVector3<T>& left, const TVector3<T>& right);
      static TVector3<T> Lerp(const TVector3<T>& from, const TVector3<T>& to, float t);
      static TVector3<T> Slerp(const TVector3<T>& from, const TVector3<T>& to, float t);
      // Keeps up the same, make forward orthogonal to up
      static TVector3<T> OrthoNormalize(const TVector3<T>& forward, const TVector3<T> &up);
      //Some useful variables
      const static TVector3 Zero;
      const static TVector3 Right;
      const static TVector3 Up;
      const static TVector3 Forward;

  //protected:
      union
      {
          struct{ T _x; T _y; T _z; };
          T _u[3];
      };

    };

    template <typename T>
    float TVector3<T>::Dot(const TVector3<T>& left, const TVector3<T>& right)
    {
      return left._x * right._x + left._y * right._y + left._z * right._z;
    }

    template <typename T>
    TVector3<T> TVector3<T>::Project(const TVector3<T>& left, const TVector3<T>& right)
    {
      float dot = Dot(left, right);
      return right.GetNormalize() * dot;
    }

    template <typename T>
    TVector3<T> TVector3<T>::Cross(const TVector3<T>& left, const TVector3<T>& right)
    {
        //left.x,y,z can be u1, u2, u3
        //right.xy,z can be v1, v2, v3
        TVector3<T> result = TVector3<T>::Zero;
        result._x = (left._y * right._z - left._z * right._y);//u2*v3 - u3*v2
        result._y = (left._z * right._x - left._x * right._z);//-(u1*v3 - u3*v1)
        result._z = (left._x * right._y - left._y * right._x);//u1*v2 - u2*v1
        return result;
    }

    template <typename T>
    TVector3<T> TVector3<T>::Lerp(const TVector3<T>& from, const TVector3<T>& to, float t)
    {
      t = t < 0.0f ? 0.0f : t;
      t = t > 1.0f ? 1.0f : t;
      return from * (1.0f - t) + to * t;
    }

    // Geometric Slerp: https://en.wikipedia.org/wiki/Slerp
    template <typename T>
    TVector3<T> TVector3<T>::Slerp(const TVector3<T>& from, const TVector3<T>& to, float t)
    {
      t = t < 0.0f ? 0.0f : t;
      t = t > 1.0f ? 1.0f : t;
      // get cos of 2 vectors
      float cosOmega = TVector3<T>::Dot(from.GetNormalize(), to.GetNormalize());
      // if the 2 vectors's direction are too close, 
      // just use lerp. 
      float k0 = 1.0f - t;
      float k1 = t;
      if (cosOmega < 0.9999f)
      {
        float sinOmega = std::sqrt(1.0f - cosOmega * cosOmega);
        float omega = std::atan2f(sinOmega, cosOmega);
        float oneOverSinOmega = 1.0f / sinOmega;
        k0 = std::sinf(k0 * omega) * oneOverSinOmega;
        k1 = std::sinf(k1 * omega) * oneOverSinOmega;
      }
      return from * k0 + to * k1;
    }

    template <typename T>
    TVector3<T> TVector3<T>::OrthoNormalize(const TVector3<T>& forward, const TVector3<T> &upward)
    {
      TVector3<T> right = Cross(upward, forward);
      right.Normalize();
      TVector3<T> idealForward = Cross(right, upward);
      return idealForward;
    }

    template <typename T>
    TVector3<T>::TVector3(const T x, const T y, const T z) :
        _x(x), _y(y), _z(z)
    {
    }

    template <typename T>
    const TVector3<T> TVector3<T>::Zero(T(0), T(0), T(0));
    template <typename T>
    const TVector3<T> TVector3<T>::Right(T(1), T(0), T(0));
    template <typename T>
    const TVector3<T> TVector3<T>::Up(T(0), T(1), T(0));
    template <typename T>
    const TVector3<T> TVector3<T>::Forward(T(0), T(0), T(-1));

    inline float TVector3<float>::Magnitude() const
    {
        float length = sqrt(_x * _x + _y*_y + _z*_z);
        //MessagedAssert(!EAE_Engine::Implements::IsNaN(length), "oops, length should not be NaN!");
        return length + FLT_EPSILON;
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::Normalize()
    {
      T length = Magnitude();
      if (length == 0)
      {
          this->_x = (T)INT_MAX;
          this->_y = (T)INT_MAX;
          this->_z = (T)INT_MAX;
          return *this;
      }
      this->_x = _x / length;
      this->_y = _y / length;
      this->_z = _z / length;
      return *this;
    }
    //normalize function for float
    inline TVector3<float> TVector3<float>::Normalize()
    {
      float length = Magnitude();
      //MessagedAssert(!Engine::Implements::AlmostEqual2sComplement(length, 0.0f, 5), "opps, length should not be 0.0f!");
      if (Implements::AlmostEqual2sComplement(length, 0.0f, 5))
      {
          this->_x = FLT_MAX - 1.0f;
          this->_y = FLT_MAX - 1.0f;
          this->_z = FLT_MAX - 1.0f;
          return *this;
      }
      this->_x = _x / length;
      this->_y = _y / length;
      this->_z = _z / length;
      return *this;
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::GetNormalize() const
    {
        TVector3<T> result = *this;
        T length = Magnitude();
        if (length == 0)
        {
            result->_x = (T)INT_MAX;
            result->_y = (T)INT_MAX;
            result->_z = (T)INT_MAX;
            return result;
        }
        result->_x = result->_x / length;
        result->_y = result->_y / length;
        result->_z = result->_z / length;
        return result;
    }
    inline TVector3<float> TVector3<float>::GetNormalize() const
    {
        TVector3<float> result = *this;
        float length = Magnitude();
        //MessagedAssert(!Engine::Implements::AlmostEqual2sComplement(length, 0.0f, 5), "opps, length should not be 0.0f!");
        if (Implements::AlmostEqual2sComplement(length, 0.0f, 5))
        {
            result._x = FLT_MAX - 1.0f;
            result._y = FLT_MAX - 1.0f;
            result._z = FLT_MAX - 1.0f;
            return *this;
        }
        result._x = result._x / length;
        result._y = result._y / length;
        result._z = result._z / length;
        return result;
    }

    template <typename T>
    inline T TVector3<T>::Dot(const TVector3<T>& right) const
    {
        return _x * right._x + _y * right._y + _z * right._z;
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::Cross(const TVector3<T>& right) const
    {
        //this.x,y,z can be u1, u2, u3
        //right.xy,z can be v1, v2, v3
        TVector3<T> result = TVector3<T>::Zero;
        result._x = (_y * right._z - _z * right._y);//u2*v3 - u3*v2
        result._y = (T)(-1) *(_x * right._z - _z * right._x);//-(u1*v3 - u3*v1)
        result._z = (_x * right._y - _y * right._x);//u1*v2 - u2*v1
        return result;
    }

    template <typename T>
    inline void TVector3<T>::Set(const T x, const T y, const T z)
    {
        _x = x;
        _y = y;
        _z = z;
    }

    //create an edge by two points.
    template <typename T>
    inline TVector3<T> TVector3<T>::GetEdge(TVector3& point_another)
    {
        return TVector3<T>(point_another._x - _x, point_another._y - _y, point_another.z - _z);
    }

    template <typename T>
    inline TVector3<T>& TVector3<T>::operator+= (const TVector3<T>& right)
    {
        _x += right._x;
        _y += right._y;
        _z += right._z;
        return *this;
    }

    template <typename T>
    inline TVector3<T>& TVector3<T>::operator= (const TVector3<T>& right)
    {
        _x = right._x;
        _y = right._y;
        _z = right._z;
        return *this;
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::operator* (T f) const
    {
        return TVector3<T>(_x*f, _y*f, _z*f);
    }

    template <typename T>
    inline TVector3<T>& TVector3<T>::operator*=(const T right)
    {
        _x *= right;
        _y *= right;
        _z *= right;
        return *this;
    }

    template <typename T>
    TVector3<T>::TVector3(const TVector4<T>& in)
    {
        _x = in._x;
        _y = in._y;
        _z = in._z;
    }

    template <typename T>
    inline TVector3<T>& TVector3<T>::operator= (const TVector4<T>& right)
    {
        _x = right._x;
        _y = right._y;
        _z = right._z;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    inline TVector3<T> operator- (const TVector3<T>& left, const TVector3<T>& right)
    {
        return TVector3<T>(left.x() - right.x(), left.y() - right.y(), left.z() - right.z());
    }

    template <typename T>
    inline TVector3<T> operator+ (const TVector3<T>& left, const TVector3<T>& right)
    {
        return TVector3<T>(left.x() + right.x(), left.y() + right.y(), left.z() + right.z());
    }

    template <typename T>
    inline bool operator== (const TVector3<T>& left, const TVector3<T>& right)
    {
        bool bx = left.x() == right.x();
        bool by = left.y() == right.y();
        bool bz = left.z() == right.z();
        return bx&&by&&bz;
    }
    //the float version of the operator==
    inline bool operator== (const TVector3<float>& left, const TVector3<float>& right)
    {
        bool bx = EAE_Engine::Implements::AlmostEqual2sComplement(left.x(), right.x(), 4);
        bool by = EAE_Engine::Implements::AlmostEqual2sComplement(left.y(), right.y(), 4);
        bool bz = EAE_Engine::Implements::AlmostEqual2sComplement(left.z(), right.z(), 4);
        return bx&&by&&bz;
    }

    typedef TVector3<float> Vector3;

///////////////////////////////////////////////Vector4//////////////////////////////////////////
    template<typename T>
    class TVector4
    {
    public:
        TVector4(): _x(T(0)), _y(T(0)), _z(T(0)), _w(T(0)){}
        TVector4(const T x, const T y, const T z, const T w);
        TVector4(const TVector4& i_other);
        TVector4(const TVector3<T>& i_other);
        TVector4(const TVector3<T>& i_other, T w);
        inline TVector4<T>& operator=(const TVector4& i_other);
        inline TVector4<T>& operator=(const TVector3<T>& i_other);
        inline TVector4 operator* (T f);
        inline TVector4& operator*=(const T right);

        inline void Set(const T x, const T y, const T z, const T w);
        inline T x() const{ return _x; }
        inline T y() const{ return _y; }
        inline T z() const{ return _z; }
        inline T w() const{ return _w; }
        inline void x(const T tx){ _x = tx; }
        inline void y(const T ty){ _y = ty; }
        inline void z(const T tz){ _z = tz; }
        inline void w(const T tw){ _w = tw; }

        inline T Magnitude() const { return sqrt(_x * _x + _y*_y + _z*_z + _w*_w); }
        inline TVector4<T> Normalize();
        inline T Dot(const TVector4& right) const;
        const static TVector4 Zero;

//	private:
        union
        {
            struct{ T _x; T _y; T _z; T _w; };
            T _u[4];
        };

    };

    template <typename T>
    const TVector4<T> TVector4<T>::Zero(T(0), T(0), T(0), T(0));

    template <typename T>
    TVector4<T>::TVector4(const T x, const T y, const T z, const T w) :
        _x(x), _y(y), _z(z), _w(w)
    {}

    template <typename T>
    TVector4<T>::TVector4(const TVector4<T>& i_other) :
        _x(i_other._x), _y(i_other._y), _z(i_other._z), _w(i_other._w)
    {}

    template <typename T>
    TVector4<T>::TVector4(const TVector3<T>& i_other) :
        _x(i_other._x), _y(i_other._y), _z(i_other._z), _w(T(0))
    {}

    template <typename T>
    TVector4<T>::TVector4(const TVector3<T>& i_other, T w) :
        _x(i_other._x), _y(i_other._y), _z(i_other._z), _w(w)
    {}

    template <typename T>
    TVector4<T>& TVector4<T>::operator=(const TVector4<T>& i_other)
    {
        _x = i_other._x;
        _y = i_other._y;
        _z = i_other._z;
        _w = i_other._w;
        return *this;
    }

    template <typename T>
    TVector4<T>& TVector4<T>::operator=(const TVector3<T>& i_other)
    {
        _x = i_other.x();
        _y = i_other.y();
        _z = i_other.z();
        _w = (T)0;
        return *this;
    }

    template <typename T>
    inline T TVector4<T>::Dot(const TVector4<T>& right) const
    {
        return _x * right._x + _y * right._y + _z * right._z + _w * right._w;
    }
        
    template <typename T>
    inline TVector4<T> TVector4<T>::operator* (T f)
    {
        return TVector4<T>(_x*f, _y*f, _z*f, _w*f);
    }

    template <typename T>
    inline TVector4<T>& TVector4<T>::operator*=(const T right)
    {
        _x *= right;
        _y *= right;
        _z *= right;
        _w *= right;
        return *this;
    }

    inline float TVector4<float>::Magnitude() const
    {
        float length = sqrt(_x * _x + _y*_y + _z*_z + _w * _w);
        //MessagedAssert(!EAE_Engine::Implements::IsNaN(length), "oops, length should not be NaN!");
        return length + FLT_EPSILON;
    }

    template <typename T>
    inline TVector4<T> TVector4<T>::Normalize()
    {
        T length = Magnitude();
        if (length == 0)
        {
            this->_x = (T)INT_MAX;
            this->_y = (T)INT_MAX;
            this->_z = (T)INT_MAX;
            this->_w = (T)INT_MAX;
            return *this;
        }
        this->_x = _x / length;
        this->_y = _y / length;
        this->_z = _z / length;
        this->_w = _w / length;
        return *this;
    }
    //normalize function for float
    inline TVector4<float> TVector4<float>::Normalize()
    {
        float length = Magnitude();
        //MessagedAssert(!Engine::Implements::AlmostEqual2sComplement(length, 0.0f, 5), "opps, length should not be 0.0f!");
        if (Implements::AlmostEqual2sComplement(length, 0.0f, 5))
        {
            this->_x = FLT_MAX - 1.0f;
            this->_y = FLT_MAX - 1.0f;
            this->_z = FLT_MAX - 1.0f;
            this->_w = FLT_MAX - 1.0f;
            return *this;
        }
        this->_x = _x / length;
        this->_y = _y / length;
        this->_z = _z / length;
        this->_w = _w / length;
        return *this;
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
        
    template <typename T>
    inline TVector4<T> operator- (const TVector4<T>& left, const TVector4<T>& right)
    {
        return TVector4<T>(left.x() - right.x(), left.y() - right.y(), left.z() - right.z(), left.w() - right.w());
    }
    template <typename T>
    inline TVector4<T> operator+ (const TVector4<T>& left, const TVector4<T>& right)
    {
        return TVector4<T>(left.x() + right.x(), left.y() + right.y(), left.z() + right.z(), left.w() + right.w());
    }
        
    template <typename T>
    inline bool operator== (const TVector4<T>& left, const TVector4<T>& right)
    {
        bool bx = left.x() == right.x();
        bool by = left.y() == right.y();
        bool bz = left.z() == right.z();
        bool bw = left.w() == right.w();
        return bx&&by&&bz&&bw;
    }

    //the float version of the operator==
    inline bool operator== (const TVector4<float>& left, const TVector4<float>& right)
    {
        bool bx = EAE_Engine::Implements::AlmostEqual2sComplement(left.x(), right.x(), 4);
        bool by = EAE_Engine::Implements::AlmostEqual2sComplement(left.y(), right.y(), 4);
        bool bz = EAE_Engine::Implements::AlmostEqual2sComplement(left.z(), right.z(), 4);
        bool bw = EAE_Engine::Implements::AlmostEqual2sComplement(left.w(), right.w(), 4);
        return bx&&by&&bz&&bw;
    }

    typedef TVector4<float> Vector4;
  }

}


#endif //TVECTOR3_H

