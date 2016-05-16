#ifndef IMPLEMENTS_H
#define IMPLEMENTS_H


#include <cmath>
#include <cstdio>
#include <float.h>
#include "Engine/UserOutput/Source/Assert.h"

namespace EAE_Engine 
{
	namespace Implements
	{
		/*
		Read the website: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm

		//Relative error is measured by comparing the error to the expected result.
		//One way of calculating it would be like this:
		//relativeError = fabs((result - expectedResult) / expectedResult);
		//If result is 99.5, and expectedResult is 100, then the relative error is 0.005.

		// Non-optimal AlmostEqual function - not recommended.
		bool AlmostEqualRelative(float A, float B, float maxRelativeError)
		{
		//when A & B equals 0, this would return true.
		if (A == B)
		return true;
		float relativeError;
		//always use the lager one to be the expectedResult
		if (fabs(B) > fabs(A))
		relativeError = fabs((A - B) / B);
		else
		relativeError = fabs((A - B) / A);
		if (relativeError <= maxRelativeError)
		return true;
		return false;
		}
		// Slightly better AlmostEqual function ? still not recommended
		inline bool AlmostEqualRelativeOrAbsolute(float A, float B,
		float maxRelativeError, float maxAbsoluteError = FLT_MAX)
		{
		//when A & B are close to 0, then the relative error works very bad.
		//So it is very good to use the maxAbsoluteError to Test.
		if (fabs(A - B) < maxAbsoluteError)
		return true;
		float relativeError;
		//always use the lager one to be the expectedResult
		if (fabs(B) > fabs(A))
		relativeError = fabs((A - B) / B);
		else
		relativeError = fabs((A - B) / A);
		if (relativeError <= maxRelativeError)
		return true;
		return false;
		}

		//Comparing float using integers
		// Initial AlmostEqualULPs version - fast and simple, but
		// some limitations.
		//This function will allow (maxUlps-1) floats between A and B.
		inline bool AlmostEqualUlps(float A, float B, int maxUlps)
		{
		//If your compiler has ints of a different size, find a 32-bit integral type and use it instead.
		MessagedAssert(sizeof(float) == sizeof(int), "opps, it seems that the platform cannot use this float equal function.");

		if (A == B)
		return true;
		int intDiff = abs(*(int*)&A - *(int*)&B);
		if (intDiff <= maxUlps)
		return true;
		return false;
		}
		*/

		//be careful that : NaN != NaN
		//NaN * 0 != 0, Infinity * 0 != 0
		//check float numbers:
		inline bool IsNaN(float i_val)
		{
			volatile float  v = i_val;
			return v != v;//A NAN will never return true on a <= comparison.
		}

		inline bool IsNotNaN(float i_val)
		{
			volatile float  v = i_val;
			return v == v;
		}

		inline bool IsNaNOrInfinity(float i_val)
		{
			volatile float v = i_val;

			return (v * 0) != 0;
		}

		inline bool IsInfinity(float i_val)
		{
			bool isNaNorInfinity = IsNaNOrInfinity(i_val);
			return isNaNorInfinity ? (IsNaN(i_val) ? false : true) : false;
		}

		//absolute error, it works sometime but not always. Becuase the difference 0.0001 is ok for float around 100,
		//but too big for float around 0.0001.
		inline bool IsFloatAbsoluteEqual(float f1, float f2, float epsilon = FLT_EPSILON)
		{
			if (fabs(f1 - f2) < epsilon)
				return true;
			return false;
		}


		inline bool AlmostEqual2sComplement(float A, float B, int maxUlps)
		{
			// Make sure maxUlps is non-negative and small enough that the
			// default NAN won't compare as equal to anything.
			// why it should be less than 1024* 1024* 4? Becuase the float point use 1bit for sign, 8bit for exponent and 23 bits to represent the signifand.
			// so the maxUlps should not be larger than the signifand.
			MessagedAssert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024, "maxUlps should be non-negative and small enough");
			int aInt = *(int*)&A;
			// Make aInt lexicographically ordered as a twos-complement int
			if (aInt < 0)
			{
				// a general way of handling negative numbers is to adjust them so that 
				// they are lexicographically ordered as twos-complement integers instead of as signed magnitude integers. 
				//0x80000000 = 2147483648d = 2^31; because we should 31 bits to represent the float and int, so the 0x80000000 means the largest int.
				//0x80000000 is also the value of negative zero, in decimal this is -2147483648
				aInt = 0x80000000 - aInt;
			}
			// Make bInt lexicographically ordered as a twos-complement int
			int bInt = *(int*)&B;
			if (bInt < 0)
				bInt = 0x80000000 - bInt;
			int intDiff = abs(aInt - bInt);
			if (intDiff <= maxUlps)
				return true;
			return false;
		}


		inline unsigned int GetStringLegth(const char* string)
		{
			unsigned int length = 0;
			while (string&&string[length] != '\0')
			{
				length++;
			}
			return length + 1;// add the length of '\0'
		}

    inline int Strcmp(const char *str1, const char *str2) {
      int result = 0;
      while (*str1 != '\0' && *str2 != '\0') {
        const char* pleft = str1++;
        const char* pright = str2++;
        if (*pleft == *pright)
          continue;
        if (*pleft < *pright)
          result = -1;
        else
          result = 1;
        break;
      }
      if (*str1 == '\0' && *str2 != '\0')
        return -1;
      else if (*str1 != '\0' && *str2 == '\0')
        return 1;
      return result;
    }


		/*
	//	extern AutoPtr<char> SplitString(char* pStringChar, char symbol, size_t startPoint = 0);
		inline AutoPtr<char> SplitString(char* pStringChar, char symbol, unsigned int startIndex = 0)
		{
			size_t index = startIndex;
			char current_c = *(pStringChar + index);
			unsigned int length = GetStringLegth(pStringChar);
			if (length < startIndex)
			{
				return AutoPtr<char>(nullptr);
			}

			while (current_c != '\0' && current_c != symbol)
			{
				current_c = *(pStringChar + ++index);
			}
			if (current_c == '\0' || index == 0)
			{
				return AutoPtr<char>(nullptr);
			}
			unsigned int bufferLength = (unsigned int)(index + 1 - startIndex);
			char* pResultBuff = new char[bufferLength];
			SetMem(pResultBuff, bufferLength, 0);
			CopyMem(pStringChar + startIndex, pResultBuff, bufferLength -1);
			AutoPtr<char> pTemp(pResultBuff);
			return pTemp;
		}
		*/
	}
}



#endif //IMPLEMENTS_H
