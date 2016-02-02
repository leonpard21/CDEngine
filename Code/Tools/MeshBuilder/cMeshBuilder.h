#ifndef EAE_ENGINE_CMESHBUILDER_H
#define EAE_ENGINE_CMESHBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"

// Class Declaration
//==================

namespace EAE_Engine
{
	namespace Tools 
	{
		class cMeshBuilder : public cbBuilder
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

#endif	// EAE_ENGINE_CMESHBUILDER_H
