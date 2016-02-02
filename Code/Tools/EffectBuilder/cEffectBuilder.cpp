// Header Files
//=============

#include "cEffectBuilder.h"
#include <fstream>
#include <sstream>
#include <cstdint>
#include "Engine/Windows/WindowsFunctions.h"
#include "LoadLua/LuaEffectLoader.h"
// Interface
//==========

// Build
//------

bool EAE_Engine::Tools::cEffectBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;
	// Copy the source to the target
	{
		const bool dontFailIfTargetAlreadyExists = false;
		const bool updateTheTargetFileTime = true;
		std::ofstream outfile(_path_target, std::ofstream::binary);
		if (!outfile)
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to output effect to: " << _path_target;
			OutputErrorMessage(decoratedErrorMessage.str().c_str(), __FILE__);
			wereThereErrors = true;
			return !wereThereErrors;
		}
		char* o_pBuffer = nullptr;
		size_t o_sizeOfBuffer = 0;
		wereThereErrors = !GenerateBinaryEffectData(_path_source, o_pBuffer, o_sizeOfBuffer);
		if (!wereThereErrors)
		{
			// write to outfile
			outfile.write(o_pBuffer, o_sizeOfBuffer);
			// release dynamically-allocated memory
			delete[] o_pBuffer;
		}
		outfile.close();
		if (wereThereErrors)
		{
			wereThereErrors = true;
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to build effect from\"" << _path_source << "\" to \"" << _path_target;
			OutputErrorMessage(decoratedErrorMessage.str().c_str(), __FILE__);
		}
	}
	return !wereThereErrors;
}
