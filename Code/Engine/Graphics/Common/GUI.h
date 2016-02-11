#ifndef EAE_ENGINE_GRAPHICS_GUI_H
#define EAE_ENGINE_GRAPHICS_GUI_H

#include "RectTransform.h"
#include "Engine/General/Singleton.hpp"
#include "Engine/Common/Interfaces.h"
#include <vector>

namespace EAE_Engine
{
	namespace Graphics
	{
		enum eGUIType 
		{
			TEXT_COM,
			BUTTON_COM,
			SLIDER_COM,
			CHECKBOX_COM,
			IMAGE_COM,
		};

		struct UIComponent 
		{
			eGUIType _type;
			RectTransform _rectTransform;
			void* _pComponent;
		};

		class UIElementManager : public Singleton<UIElementManager>
		{
		public:
			~UIElementManager();
			void Clean();
			void AddText(const char* pName, std::string value);

		private:
			std::vector<UIComponent> _uiElements;
		};


	}
}

#endif//EAE_ENGINE_GRAPHICS_UI_TEXT_H