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

		class UIElementManager : public Singleton<UIElementManager>
		{
		public:
			~UIElementManager();
			void Init();
			void Clean();
			void Update();

			Text* AddText(std::string value, Common::ITransform* pTrans);
			Button* AddButton(BtnCallBack* pCallBack, Common::ITransform* pTrans);
			Slider* AddSlider(float min, float max, Common::ITransform* pTrans);
			Toggle* AddToggle(bool checked, Common::ITransform* pTrans);
			std::vector<Toggle*>& GetToggles() { return _toggles; }

		private:
			void UpdateCanvasRender();

		private:
			std::vector<Text*> _texts;
			std::vector<Button*> _buttons;
			std::vector<Slider*> _sliders;
			std::vector<Toggle*> _toggles;
		};


	}
}

#endif//EAE_ENGINE_GRAPHICS_UI_TEXT_H