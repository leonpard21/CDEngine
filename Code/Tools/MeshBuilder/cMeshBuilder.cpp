// Header Files
//=============

#include "cMeshBuilder.h"
#include <fstream>
#include <sstream>
#include <cstdint>
#include "Engine/Windows/WindowsFunctions.h"
#include "LoadLua/LuaMeshLoader.h"
// Interface
//==========

// Build
//------

bool EAE_Engine::Tools::cMeshBuilder::Build( const std::vector<std::string>& )
{
	/*
	open may failed on Linux has about 30 conditions, some of them are:
	If the file exists and you don't have permission to write it.
	If the file doesn't exist, and you don't have permission (on the diretory) to create it.
	If you don't have search permission on some parent directory.
	If you pass in a bogus char* for the filename.
	If, while opening a device file, you press CTRL-C.
	If the kernel encountered too many symbolic links while resolving the name.
	If you try to open a directory for writing.
	If the pathname is too long.
	If your process has too many files open already.
	If the system has too many files open already.
	If the pathname refers to a device file, and there is no such device in the system.
	If the kernel has run out of memory.
	If the filesystem is full.
	If a component of the pathname is not a directory.
	If the file is on a read-only filesystem. 
	If the file is an executable file which is currently being executed.
	*/
	bool wereThereErrors = false;
	// Copy the source to the target
	{
		const bool dontFailIfTargetAlreadyExists = false;
		const bool updateTheTargetFileTime = true;
		std::ofstream outfile(_path_target, std::ofstream::binary);
		if (!outfile)
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to output mesh to: " << _path_target;
			OutputErrorMessage(decoratedErrorMessage.str().c_str(), __FILE__);
			wereThereErrors = true;
			return !wereThereErrors;
		}
		char* o_pBuffer = nullptr;
		uint32_t o_sizeOfBuffer = 0;
		wereThereErrors = !GenerateBinaryMeshData(_path_source, o_pBuffer, o_sizeOfBuffer);
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
			decoratedErrorMessage << "Failed to build mesh from\"" << _path_source << "\" to \"" << _path_target;
			OutputErrorMessage(decoratedErrorMessage.str().c_str(), __FILE__);
		}
	}
	
	return !wereThereErrors;
}
