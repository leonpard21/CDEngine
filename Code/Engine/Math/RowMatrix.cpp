#include "RowMatrix.h"

namespace EAE_Engine
{
	namespace Math
	{	
		RowMatrix33::RowMatrix33(const RowMatrix33& matrix33)
		{
			for (unsigned int i = 0; i < 9; ++i)
				_m[i] = matrix33._m[i]; 
		}


		const RowMatrix44 RowMatrix44::Identity;

	}
}


