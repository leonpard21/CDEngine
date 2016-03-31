/*
	This class represents a rotation
*/
#ifndef EAEENGINE_MATH_CQUATERNION_H
#define EAEENGINE_MATH_CQUATERNION_H


// Class Declaration
//==================

namespace EAE_Engine
{
  namespace Math
  {
    template<typename T>
    class TVector3;
    typedef TVector3<float> Vector3;
    class EulerAngle;
	class ColMatrix44;
	class Quaternion
	{
	public:
		// Concatenation
		Quaternion operator *( const Quaternion& i_rhs ) const;
    Quaternion operator* (float value) const;
    Quaternion operator+ (const Quaternion& i_rhs) const;
    Quaternion operator- (const Quaternion& i_rhs) const;
		bool operator ==(const Quaternion& i_rhs) const;
    Quaternion& operator= (const Quaternion& right);
		// Inversion
		void Invert();
    Quaternion GetInverse() const;
    void Conjugate();
    Quaternion GetConjugate() const;
    // Exponentiation
    // becareful this is exponentiate, q^t; not the exponential function, e^q. 
    void Pow(float t);
    Quaternion GetPow(float exponent) const;
		// Normalization
		void Normalize();
		Quaternion CreateNormalized() const;
    float GetMagnitude() const;
    float GetSqMagnitude() const;
    Vector3 GetVec() const;
        
		// Initialization / Shut Down
		//---------------------------
		Quaternion();	// Identity
		Quaternion(const float i_angleInRadians, const Vector3& i_axisOfRotation_normalized );
		Quaternion(const Quaternion& i_other);
    Quaternion(float pitch, float heading, float bank);
    Quaternion(const float i_w, const float i_x, const float i_y, const float i_z);
		// Static member functions
		// Create Matrix for a quaternion
		static ColMatrix44 CreateColMatrix(const Quaternion& i_rotation);
    static Vector3 CreateEulerAngle(const Quaternion& i_rotation);
    static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t);
    static Quaternion GetDifference(const Quaternion& from, const Quaternion& to);
    static Quaternion RotationBetween2Vectors(Vector3 from, Vector3 to);
    // Assuming that the forwad and upward are orthogonal.
    static Quaternion LookRotation(Vector3 forward, Vector3 upward);
    static Vector3 MultiVector(const Quaternion& i_rotation, const Math::Vector3& i_vec);

		// Data
		//=====
		static Quaternion Identity;
	private:
		union
		{
			struct { float _w; float _x; float _y; float _z; };
			float _u[4];
		};

	private:
		// Friend Classes
		//===============
    friend class ColMatrix33;
		friend class ColMatrix44;
    friend class EulerAngle;
		// Products
		friend float Dot(const Quaternion& i_lhs, const Quaternion& i_rhs);
	};

	// Products
	float Dot(const Quaternion& i_lhs, const Quaternion& i_rhs);
	
  }
}

#endif	// EAEENGINE_MATH_CQUATERNION_H
