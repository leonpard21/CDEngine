#ifndef EAE_ENGINE_MATH_EULER_ANGLE_H
#define EAE_ENGINE_MATH_EULER_ANGLE_H
#include "Vector.h"

namespace EAE_Engine
{
  namespace Math 
  {
    class ColMatrix44;
    class Quaternion;
    class EulerAngle 
    {
    public:
      static Quaternion GetQuaternion(Vector3 eulerAngle);
      static ColMatrix44 GetColMatrix(Vector3 eulerAngle);
    };
  }
}

#endif//EAE_ENGINE_MATH_EULER_ANGLE_H