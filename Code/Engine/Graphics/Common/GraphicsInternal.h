#ifndef EAE_Engine_GRAPHICS_INNER_H
#define EAE_Engine_GRAPHICS_INNER_H
#include "Engine/General/MemoryOp.h"
#include "Engine/Windows/WindowsIncludes.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		class AOSMesh;
		bool SetCurrentRenderMesh(const AOSMesh *pAOSMesh);
		class Effect;
		bool BindCurrentEffect(Effect* pEffect);

		enum ClearFlag
		{
			COLOR = 0x01,
			DEPTH = 0x02,
			STENCIL = 0x03,
		};

		void CleanBuffer(DWORD clearFlag);
		void ChangeEffectRenderState(uint32_t renderState);
		void SwapBuffer();
	}
}

#endif//EAE_Engine_GRAPHICS_INNER_H