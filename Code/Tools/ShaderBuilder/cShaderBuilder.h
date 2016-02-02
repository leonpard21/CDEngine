/*
	This class builds shaders
*/

#ifndef EAE6320_CSHADERBUILDER_H
#define EAE6320_CSHADERBUILDER_H

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

#endif	// EAE6320_CSHADERBUILDER_H
