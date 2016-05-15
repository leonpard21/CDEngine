/*
	This class builds shaders
*/

#ifndef EAE_ENGINE__CSHADERBUILDER_H
#define EAE_ENGINE__CSHADERBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"

// Class Declaration
//==================

namespace EAE_Engine
{
	namespace Tools
	{
		class cShaderBuilder : public cbBuilder
		{
			// Interface
			//==========

		public:

			// Build
			//------

			virtual bool Build(const std::vector<std::string>& i_arguments);
		};
	}
}

#endif	// EAE_ENGINE__CSHADERBUILDER_H
