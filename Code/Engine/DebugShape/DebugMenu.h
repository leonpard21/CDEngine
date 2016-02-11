#ifndef EAE_ENGINE_DEBUG_DEBUG_MENU_H
#define EAE_ENGINE_DEBUG_DEBUG_MENU_H

#ifdef _DEBUG
#define DEBUG_MENU
#endif

#include "General/Singleton.hpp"

namespace EAE_Engine 
{
	namespace Debug 
	{
#ifdef DEBUG_MENU
		class DebugMenu : public Singleton<DebugMenu>
		{
		public:
			DebugMenu();
			~DebugMenu();
			void Activate(bool active);
			void Update();
			// Note that once a widget is created, there is no way to modify it or delete it later.
			// You could add a return value (like an int handle) if you'd like to do that.
			void CreateSlider(const char* name, float* value, float min, float max, float interval);
			void CreateCheckBox(const char* name, bool* value);
			void CreateText(const char* name, char* value);
			void CreateButton(const char* name, void(*callback)(void*), void *param);

		private:
			// ... data to store created widgets
		};

		void AddSlider(const char* name, float* value, float min, float max, float interval);
		void AddCheckBox(const char* name, bool* value);
		void AddText(const char* name, char* value);
		void AddButton(const char* name, void(*callback)(void*), void *param);
#else

		// All of these functions will compile out to nothing.
		inline void AddSlider(const char *name, float* value, float min, float max) {}
		inline void AddCheckBox(const char *name, bool* value) {}
		inline void AddText(const char *name, char* value) {}
		inline void AddButton(const char *name, void(*callback)(void*), void *param) {}
#endif	// DEBUG_MENU

	}
}


#endif//EAE_ENGINE_DEBUG_DEBUG_MENU_H