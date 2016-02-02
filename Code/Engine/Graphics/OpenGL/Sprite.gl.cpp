#include "../Common/Sprite.h"
#include "General/MemoryOp.h"
#include <string>

namespace EAE_Engine
{
	namespace Graphics
	{
		SpriteRender::SpriteRender() :
			_pName(nullptr)
		{
			
		}
		
		SpriteRender::~SpriteRender()
		{
			SAFE_DELETE(_pName);
		}
	}
}