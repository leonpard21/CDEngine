/*
	This is the base class for builders of specific asset types
	(meaning that MeshBuilder, ShaderBuilder, TextureBuilder, etc.,
	should all derive from this base class)
*/

#ifndef EAE_ENGINE_CBBUILDER_H
#define EAE_ENGINE_CBBUILDER_H

// Header Files
//=============

#include <cstdlib>
#include <string>
#include <vector>

// This file doesn't need the build utility functions,
// but #including it here makes it available to every builder automatically
#include "UtilityFunctions.h"

// Class Declaration
//==================

namespace EAE_Engine
{
	namespace Tools 
	{
		// This only thing a specific builder project's main() entry point should do
		// is to call the following function with the derived builder class
		// as the template argument:
		template<class builder_t>
		int Build(char** i_arguments, const unsigned int i_argumentCount)
		{
			builder_t builder;
			return builder.ParseCommandArgumentsAndBuild(i_arguments, i_argumentCount) ?
				EXIT_SUCCESS : EXIT_FAILURE;
		}

		class cbBuilder
		{
			// Interface
			//==========

		public:

			// Build
			//------

			// The following function will be called from the Build<> templated function above
			// with the command line arguments directly from the main() entry point:
			bool ParseCommandArgumentsAndBuild(char** i_arguments, const unsigned int i_argumentCount);
			// And that function will extract the source and target paths
			// and then call this function in the derived class with any remaining (optional) arguments:
			virtual bool Build(const std::vector<std::string>& i_optionalArguments) = 0;

			// Initialization / Shut Down
			//---------------------------

			cbBuilder();

			// Inheritable Data
			//=================

		protected:

			const char* _path_source;
			const char* _path_target;
		};
	}
}

#endif	// EAE_ENGINE_CBBUILDER_H
