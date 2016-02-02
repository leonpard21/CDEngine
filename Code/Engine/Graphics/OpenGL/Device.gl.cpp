#include "../Common/Device.h"
#include "../Common/Screen.h"
#include "../../UserOutput/UserOutput.h"
#include "General/MemoryOp.h"
#include "Engine/Windows/WindowsFunctions.h"
#include <cassert>
#include <sstream>
#include <string>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		class GLDevice
		{
		private:
			GLDevice() : _renderingWindow(NULL), _deviceContext(NULL), _openGlRenderingContext(NULL) {}
			~GLDevice() { ReleaseDM(); }
		public:
			HDC GetHDC() { return _deviceContext; }
			bool InitDM(const HWND i_renderingWindow);
			bool ReleaseDM();
			static void CleanDMInstance() { SAFE_DELETE(s_pDeviceManager); }
			static GLDevice* GetDMInstance() 
			{
				if (!s_pDeviceManager)
					s_pDeviceManager = new GLDevice();
				return s_pDeviceManager; 
			}
			HWND GetHWND() { return _renderingWindow; }
		private:
			bool CreateRenderingContext();
			HWND _renderingWindow;
			HDC _deviceContext;
			HGLRC _openGlRenderingContext;
			static GLDevice* s_pDeviceManager;
		};

		GLDevice* GLDevice::s_pDeviceManager = nullptr;

		bool GLDevice::InitDM(const HWND i_renderingWindow)
		{
			_renderingWindow = i_renderingWindow;
			// Create an OpenGL rendering context
			if (!CreateRenderingContext())
			{
				EAE_Engine::UserOutput::Print("Create Rendering Context failed at: ", __FILE__, __LINE__);
				return false;
			}
			// Load any required OpenGL extensions
			std::string errorMessage;
			if (!EAE_Engine::OpenGlExtensions::Load(&errorMessage))
			{
				UserOutput::Print(errorMessage.c_str());
				return false;
			}
			return true;
		}

		bool GLDevice::ReleaseDM()
		{
			bool wereThereErrors = false;
			if (_openGlRenderingContext != NULL)
			{
				if (wglMakeCurrent(_deviceContext, NULL) != FALSE)
				{
					if (wglDeleteContext(_openGlRenderingContext) == FALSE)
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to delete the OpenGL rendering context: " << EAE_Engine::GetLastWindowsError();
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						wereThereErrors = true;
					}
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Windows failed to unset the current OpenGL rendering context: " << EAE_Engine::GetLastWindowsError();
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					wereThereErrors = true;
				}
				_openGlRenderingContext = NULL;
			}
			if (_deviceContext != NULL)
			{
				// The documentation says that this call isn't necessary when CS_OWNDC is used
				ReleaseDC(_renderingWindow, _deviceContext);
				_deviceContext = NULL;
			}
			_renderingWindow = NULL;
			return !wereThereErrors;
		}

		bool GLDevice::CreateRenderingContext()
		{
			// A "device context" can be thought of an abstraction that Windows uses
			// to represent the graphics adaptor used to display a given window
			_deviceContext = GetDC(_renderingWindow);
			if (_deviceContext == NULL)
			{
				EAE_Engine::UserOutput::Print("Windows failed to get the device context");
				return false;
			}
			// Windows requires that an OpenGL "render context" is made for the window we want to use to render into
			{
				// Set the pixel format of the rendering window
				{
					PIXELFORMATDESCRIPTOR desiredPixelFormat = { 0 };
					{
						desiredPixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
						desiredPixelFormat.nVersion = 1;

						desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
						desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
						desiredPixelFormat.cColorBits = 32;
						desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

						desiredPixelFormat.cDepthBits = 16; // try and use a buffer format with 16 bits of depth for each pixel
					}
					// Get the ID of the desired pixel format
					int pixelFormatId;
					{
						pixelFormatId = ChoosePixelFormat(_deviceContext, &desiredPixelFormat);
						if (pixelFormatId == 0)
						{
							std::stringstream errorMessage;
							errorMessage << "Windows couldn't choose the closest pixel format: " << EAE_Engine::GetLastWindowsError();
							EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
							return false;
						}
					}
					// Set it
					if (SetPixelFormat(_deviceContext, pixelFormatId, &desiredPixelFormat) == FALSE)
					{
						std::stringstream errorMessage;
						errorMessage << "Windows couldn't set the desired pixel format: " << EAE_Engine::GetLastWindowsError();
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						return false;
					}
				}
				// Create the OpenGL rendering context
				_openGlRenderingContext = wglCreateContext(_deviceContext);
				if (_openGlRenderingContext == NULL)
				{
					std::stringstream errorMessage;
					errorMessage << "Windows failed to create an OpenGL rendering context: " << EAE_Engine::GetLastWindowsError();
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
				// Set it as the rendering context of this thread
				if (wglMakeCurrent(_deviceContext, _openGlRenderingContext) == FALSE)
				{
					std::stringstream errorMessage;
					errorMessage << "Windows failed to set the current OpenGL rendering context: " << EAE_Engine::GetLastWindowsError();
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
				// Enable culling in OpenGL, this must happen after the previous steps.
				glEnable(GL_CULL_FACE);
				// Enable the depth test
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glDepthFunc(GL_LEQUAL);
				GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "Failed to enable culling in OpenGL: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}

			return true;
		}

///////////////////////////////////Function Interfaces////////////////////////////////

		bool CreateDevice(const HWND i_renderingWindow)
		{
			GLDevice* pDevice = GLDevice::GetDMInstance();
			bool result = pDevice->InitDM(i_renderingWindow);
			if (!result)
				GLDevice::CleanDMInstance();
			return result;
		}

		void CleanDevice()
		{
			GLDevice::CleanDMInstance();
		}

		HDC GetDeviceContext() 
		{
			return GLDevice::GetDMInstance()->GetHDC();
		}

		Rect GetScreenRect() 
		{
			return GetScreenRect(GLDevice::GetDMInstance()->GetHWND());
		}

	}
}