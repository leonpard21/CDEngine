#ifndef EAE_ENGINE_GRAPHICS_UI_TEXT_H
#define EAE_ENGINE_GRAPHICS_UI_TEXT_H

#include "GUI.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		struct Text : public UIComponent
		{
			Text();
			std::string _value;
		};


	}
}

#endif//EAE_ENGINE_GRAPHICS_UI_TEXT_H