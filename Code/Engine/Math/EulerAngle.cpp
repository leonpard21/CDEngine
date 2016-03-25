#include "EulerAngle.h"
#include "ColMatrix.h"
#include "Quaternion.h"
#include <cmath>
#include <cassert>

namespace EAE_Engine
{
  namespace Math
  {

    Quaternion EulerAngle::GetQuaternion(Vector3 eulerAngle)
    {
      // In convertion from EulerAngle to Quaternion(from obj to upright), 
      // we have 2 sources of "backwardness" and they cancel each other out.
      // 1. because we're using fixed-axis rotations, 
      // so the order of rotation should be bank(around z), pitch(around x) and heading(around Y)
      // 2. because we also need to multiply the quaternion from right to left,
      // so at the end: Q_object?upright(h, p, b) = hpb
      float heading = eulerAngle._y;
      float pitch = eulerAngle._x;
      float bank = eulerAngle._z;
      float chhalf = std::cosf(heading * 0.5f);
      float cphalf = std::cosf(pitch * 0.5f);
      float cbhalf = std::cosf(bank * 0.5f);

      float shhalf = std::sinf(heading * 0.5f);
      float sphalf = std::sinf(pitch * 0.5f);
      float sbhalf = std::sinf(bank * 0.5f);

      Quaternion result = Quaternion::Identity;
      result._x = chhalf * cphalf * cbhalf + shhalf * sphalf * sbhalf;
      result._y = chhalf * sphalf * cbhalf + shhalf * cbhalf * sbhalf;
      result._z = shhalf * cphalf * cbhalf - chhalf * sphalf * sbhalf;
      result._w = chhalf * cphalf * sbhalf - shhalf * sphalf * cbhalf;
      return result;
    }

    ColMatrix44 EulerAngle::GetColMatrix(Vector3 eulerAngle)
    {
      // For the from obj to upright case:
      // the euler angles, axes of rotations are body axes, 
      // after the first rotation, the axes will be changed to arbitary direction.
      // So we'll not rotation under the cardinal axis.
      // So we should do fixed-axis rotations(about the upright axes), 
      // instead of doing euler rotations about the body axes.
      // For object->upright, The final correct order is: 
      // Bank(around z), Pitch(around X) and heading(around Y).
      float heading = eulerAngle._y;
      float pitch = eulerAngle._x;
      float bank = eulerAngle._z;
      float ch = std::cosf(heading);
      float cp = std::cosf(pitch);
      float cb = std::cosf(bank);

      float sh = std::sinf(heading);
      float sp = std::sinf(pitch);
      float sb = std::sinf(bank);

      ColMatrix44 result = ColMatrix44::Identity;
      result._m00 = ch * cb + sh * sp * sb;
      result._m10 = -ch * sb + sh * sp * cb;
      result._m20 = sh * cp;

      result._m01 = sb * cp;
      result._m11 = cb * cp;
      result._m21 = -sp;

      result._m02 = -sh * cb + ch * sp * sb;
      result._m21 = sb * sh + ch * sp * cb;
      result._m22 = ch * cp;
      return result;
    }

  }
}