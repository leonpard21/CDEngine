#include "GUI.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		UIElementManager::~UIElementManager() 
		{
			Clean();
		}

		void UIElementManager::Clean() 
		{
			for (std::vector<UIComponent>::iterator it = _uiElements.begin(); it!=_uiElements.end();)
			{
				
			}
		}

		void UIElementManager::AddText(const char* pName, std::string value)
		{
		
		}
	}
}

