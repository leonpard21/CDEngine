#include "DebugMenu.h"



namespace EAE_Engine
{
	namespace Debug
	{
#ifdef DEBUG_MENU	// This wraps the whole file
		// You may end up using these:
		class Slider;
		class CheckBox;
		class Text;
		class Button;

		//////////////////Members////////////////////////////////
		DebugMenu::DebugMenu()
		{
			// Because the interface includes static functions, 
			// you may want to store off the 'this' pointer here, like a singleton.
			// That, or make all the data members static too.
		}

		DebugMenu::~DebugMenu()
		{
			// ..
		}

		void DebugMenu::Activate(bool active)
		{
			// It should start off inactive, then call this function to turn it on.
		}

		void DebugMenu::Update()
		{
			// You may need to continue doing work here every frame, even when inactive.
			// Try to avoid that if possible.
		}

		void DebugMenu::CreateSlider(const char* name, float* value, float min, float max, float interval)
		{
		
		}
		void DebugMenu::CreateCheckBox(const char* name, bool* value) 
		{
		
		}
		void DebugMenu::CreateText(const char* name, char* value) 
		{
		
		}
		void DebugMenu::CreateButton(const char* name, void(*callback)(void*), void *param) 
		{
		
		}
//////////////////////////////////////////////////////////////////////////////////
		void AddSlider(const char* name, float* value, float min, float max, float interval) 
		{
		
		}
		void AddCheckBox(const char* name, bool* value)
		{
		
		}
		void AddText(const char* name, char* value) 
		{
		
		}
		
		void AddButton(const char* name, void(*callback)(void*), void *param) 
		{
		
		}

#endif	// DEBUG_MENU
	}
}

