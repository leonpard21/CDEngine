// Header Files
//=============
#include "../Common/GraphicsInternal.h"
#include "../Common/Device.h"
#include "../Common/AOSMesh.h"
#include "../Common/Effect.h"
#include "../GraphicsInclude.h"
#include <cassert>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		void CleanBuffer(DWORD clearFlag)
		{
			// Every frame an entirely new image will be created.
			// Before drawing anything, then, the previous image will be erased
			// by "clearing" the image buffer (filling it with a solid color)

			// If depth writing is disabled, OpenGL won't clear the depth buffer even if you ask it to.
			// So we have to make sure to enable the depth mask before the glClear 
			glDepthMask(GL_TRUE);
			// Black is usually used
			glClearColor(0.0f, 0.f, 0.f, 1.0f);
			assert(glGetError() == GL_NO_ERROR);
			const GLbitfield clearBufferFlag = (clearFlag & ClearFlag::COLOR ? GL_COLOR_BUFFER_BIT : 0x0 )|
				(clearFlag & ClearFlag::DEPTH ? GL_DEPTH_BUFFER_BIT : 0x0) | (clearFlag & ClearFlag::STENCIL ? GL_STENCIL_BUFFER_BIT : 0x0);
			glClear(clearBufferFlag);
			assert(glGetError() == GL_NO_ERROR);
		}

		void SwapBuffer()
		{
			// Everything has been drawn to the "back buffer", which is just an image in memory.
			// In order to display it, the contents of the back buffer must be swapped with the "front buffer"
			// (which is what the user sees)
			{
				BOOL result = SwapBuffers(GetDeviceContext());
				assert(result != FALSE);
			}
		}

		bool SetCurrentRenderMesh(const AOSMesh *pAOSMesh)
		{
			// Bind a specific vertex buffer to the device as a data source
			{
				glBindVertexArray(pAOSMesh->GetVertexArrayID());
				assert(glGetError() == GL_NO_ERROR);
			}
			return glGetError() == GL_NO_ERROR;
		}

		bool BindCurrentEffect(Effect* pEffect)
		{
			// Set the vertex and fragment shaders
			if(pEffect)
				glUseProgram(pEffect->GetProgram());
			else
				glUseProgram(0);
			return glGetError() == GL_NO_ERROR;
		}

		void ChangeEffectRenderState(uint32_t renderState)
		{
			if (renderState & RenderState::Alpha)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glDisable(GL_BLEND);
			}
			if (renderState & RenderState::DepthTesting)
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}
			if (renderState & RenderState::DepthWriting)
			{
				glDepthMask(GL_TRUE);
			}
			else
			{
				glDepthMask(GL_FALSE);
			}
			if (renderState & RenderState::FaceCulling)
			{
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}
		}
	}
}