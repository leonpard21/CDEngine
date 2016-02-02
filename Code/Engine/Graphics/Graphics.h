/*
	This file contains the function declarations for graphics
*/

#ifndef EAE_Engine_GRAPHICS_H
#define EAE_Engine_GRAPHICS_H

// Header Files
//=============
#include "../Windows/WindowsIncludes.h"
#include <cstdint>

// Interface
//==========
namespace EAE_Engine
{
	namespace Common 
	{
		class ITransform;
	}

	namespace Graphics
	{
		bool Initialize( const HWND i_renderingWindow );
		void Render();
		bool ShutDown();
		void CleanBeforeRestart();
		void RemoveMeshRender(Common::ITransform* pTrans);

		bool LoadMeshFromBinary(const char* i_luaMeshFile);
		bool LoadEffectFromBinary(const char* i_luaEffectFile);
	}
}


#endif	// EAE_Engine_GRAPHICS_H
