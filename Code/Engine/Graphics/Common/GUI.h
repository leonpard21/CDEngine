#ifndef EAE_ENGINE_GRAPHICS_GUI_H
#define EAE_ENGINE_GRAPHICS_GUI_H


#include "Text.h"
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


		class UIElementManager : public Singleton<UIElementManager>
		{
		public:
			~UIElementManager();
			void Init();
			void Clean();
			void Update();

			Text* AddText(std::string value, Common::ITransform* pTrans);
			Button* AddButton(BtnCallBack* pCallBack, Common::ITransform* pTrans);

		private:
			std::vector<Button> _buttons;
			std::vector<Text> _texts;
		};


	}
}

#endif//EAE_ENGINE_GRAPHICS_UI_TEXT_H