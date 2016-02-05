#include "Screen.h"
#include "Windows/WindowsFunctions.h"

namespace EAE_Engine
{
	namespace Graphics 
	{
		ScreenRect GetScreenRect(const HWND i_renderingWindow)
		{
			ScreenRect result = { 0.0f, 0.0f, 100.0f, 100.0f };
			struct
			{
				long width;
				long height;
			}nonClientAreaSize;
			RECT windowCoordinates;
			RECT clientDimensions;
			{
				// Get the coordinates of the entire window
				if (GetWindowRect(i_renderingWindow, &windowCoordinates) == FALSE)
				{
					std::string errorMessage("Windows failed to get the coordinates of the main window: ");
					errorMessage += EAE_Engine::GetLastWindowsError();
					MessageBox(NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR);
					return result;
				}
				// Get the dimensions of the client area
				if (GetClientRect(i_renderingWindow, &clientDimensions) == FALSE)
				{
					std::string errorMessage("Windows failed to get the dimensions of the main window's client area: ");
					errorMessage += EAE_Engine::GetLastWindowsError();
					MessageBox(NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR);
					return result;
				}
				// Get the difference between them
				nonClientAreaSize.width = (windowCoordinates.right - windowCoordinates.left) - clientDimensions.right;
				nonClientAreaSize.height = (windowCoordinates.bottom - windowCoordinates.top) - clientDimensions.bottom;
			}
			result._width = (float)(clientDimensions.right - clientDimensions.left);
			result._height = (float)(clientDimensions.bottom - clientDimensions.top);
			result._x = (float)windowCoordinates.left + result._width * 0.5f;
			result._y = (float)windowCoordinates.top + result._height * 0.5f;
			return result;
		}
	}
};

