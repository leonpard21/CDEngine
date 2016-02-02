/*
	This is an example builder program that just copies the source file to the target path
	(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef EAE_ENGINE_CGENERICBUILDER_H
#define EAE_ENGINE_CGENERICBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"

// Class Declaration
//==================

namespace EAE_Engine
{
	namespace Tools 
	{
		class cGenericBuilder : public cbBuilder
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

#endif	// EAE6320_CGENERICBUILDER_H
