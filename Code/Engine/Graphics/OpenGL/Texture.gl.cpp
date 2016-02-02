#include "../Common/Texture.h"

#include <algorithm>	// For std::max
#include <sstream>
#include <cassert>
#include "Windows/WindowsFunctions.h"
#include "General/MemoryOp.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		bool LoadDDSTextureStatic(const char* const i_path, GLuint& o_texture, std::string* o_errorMessage)
		{
			bool wereThereErrors = false;
			HANDLE fileHandle = INVALID_HANDLE_VALUE;
			void* fileContents = NULL;
			o_texture = 0;

			// Open the texture file
			{
				const DWORD desiredAccess = FILE_GENERIC_READ;
				const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
				SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
				const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
				const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
				const HANDLE dontUseTemplateFile = NULL;
				fileHandle = CreateFile(i_path, desiredAccess, otherProgramsCanStillReadTheFile,
					useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile);
				if (fileHandle == INVALID_HANDLE_VALUE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage(GetLastWindowsError());
						std::stringstream errorMessage;
						errorMessage << "Windows failed to open the texture file: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Get the file's size
			size_t fileSize;
			{
				LARGE_INTEGER fileSize_integer;
				if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
				{
					assert(fileSize_integer.QuadPart <= SIZE_MAX);
					fileSize = static_cast<size_t>(fileSize_integer.QuadPart);
				}
				else
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage(GetLastWindowsError());
						std::stringstream errorMessage;
						errorMessage << "Windows failed to get the size of the texture file: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Read the file's contents into temporary memory
			fileContents = malloc(fileSize);
			if (fileContents)
			{
				DWORD bytesReadCount;
				OVERLAPPED* readSynchronously = NULL;
				assert(fileSize < (uint64_t(1) << (sizeof(DWORD) * 8)));
				if (ReadFile(fileHandle, fileContents, static_cast<DWORD>(fileSize),
					&bytesReadCount, readSynchronously) == FALSE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage(GetLastWindowsError());
						std::stringstream errorMessage;
						errorMessage << "Windows failed to read the contents of the texture file: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Failed to allocate " << fileSize << " bytes to read in the texture " << i_path;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}

			// Create a new texture and make it active
			{
				const GLsizei textureCount = 1;
				glGenTextures(textureCount, &o_texture);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					// This code only supports 2D textures;
					// if you want to support other types you will need to improve this code.
					glBindTexture(GL_TEXTURE_2D, o_texture);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to bind a new texture: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							*o_errorMessage = errorMessage.str();
						}
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get an unused texture ID: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}

			// Extract the data
			const uint8_t* currentPosition = reinterpret_cast<uint8_t*>(fileContents);
			// Verify that the file is a valid DDS
			{
				const size_t fourCcCount = 4;
				const uint8_t* const fourCc = currentPosition;
				const uint8_t fourCc_dds[fourCcCount] = { 'D', 'D', 'S', ' ' };
				// Each of the four characters can be compared in a single operation by casting to a uint32_t
				const bool isDds = *reinterpret_cast<const uint32_t*>(fourCc) == *reinterpret_cast<const uint32_t*>(fourCc_dds);
				if (isDds)
				{
					currentPosition += fourCcCount;
				}
				else
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						char fourCcString[fourCcCount + 1] = { 0 };	// Add NULL terminator
						memcpy(fourCcString, currentPosition, fourCcCount);
						std::stringstream errorMessage;
						errorMessage << "The texture file \"" << i_path << "\" isn't a valid DDS. The Four CC is \"" << fourCcString << "\""
							" instead of \"DDS \"";
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Extract the header
			// (this struct can also be found in Dds.h in the DirectX header files
			// or here as of this comment: https://msdn.microsoft.com/en-us/library/windows/desktop/bb943982(v=vs.85).aspx )
			struct sDdsHeader
			{
				uint32_t structSize;
				uint32_t flags;
				uint32_t height;
				uint32_t width;
				uint32_t pitchOrLinearSize;
				uint32_t depth;
				uint32_t mipMapCount;
				uint32_t reserved1[11];
				struct
				{
					uint32_t structSize;
					uint32_t flags;
					uint8_t fourCc[4];
					uint32_t rgbBitCount;
					uint32_t bitMask_red;
					uint32_t bitMask_green;
					uint32_t bitMask_blue;
					uint32_t bitMask_alpha;
				} pixelFormat;
				uint32_t caps[4];
				uint32_t reserved2;
			};
			const sDdsHeader* ddsHeader = reinterpret_cast<const sDdsHeader*>(currentPosition);
			currentPosition += sizeof(sDdsHeader);
			// Convert the DDS format into an OpenGL format
			GLenum format;
			{
				// This code can only handle the two basic formats that the example TextureBuilder will create.
				// If a DDS in a different format is provided to TextureBuilder it will be passed through unchanged
				// and this code won't work.
				// Similarly, if you improve the example TextureBuilder to support more formats
				// you will also have to update this code to support them.
				const uint8_t fourCc_dxt1[] = { 'D', 'X', 'T', '1' };	// No alpha channel
				const uint8_t fourCc_dxt5[] = { 'D', 'X', 'T', '5' };	// Alpha channel
				const uint32_t fourCc_texture = *reinterpret_cast<const uint32_t*>(ddsHeader->pixelFormat.fourCc);
				if (fourCc_texture == *reinterpret_cast<const uint32_t*>(fourCc_dxt1))
				{
					format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				}
				else if (fourCc_texture == *reinterpret_cast<const uint32_t*>(fourCc_dxt5))
				{
					format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				}
				else
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						char fourCcString[5] = { 0 };	// Add NULL terminator
						memcpy(fourCcString, ddsHeader->pixelFormat.fourCc, 4);
						std::stringstream errorMessage;
						errorMessage << "The texture file \"" << i_path << "\" has an unsupported format \"" << fourCcString << "\"";
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Iterate through each MIP map level and fill in the OpenGL texture
			{
				GLsizei currentWidth = ddsHeader->width;
				GLsizei currentHeight = ddsHeader->height;
				const GLsizei blockSize = format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ? 8 : 16;
				const GLint borderWidth = 0;
				for (uint32_t mipMapLevel = 0; mipMapLevel < ddsHeader->mipMapCount; ++mipMapLevel)
				{
					const GLsizei mipMapSize = ((currentWidth + 3) / 4) * ((currentHeight + 3) / 4) * blockSize;
					glCompressedTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(mipMapLevel), format, currentWidth, currentHeight,
						borderWidth, mipMapSize, currentPosition);
					const GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						currentPosition += static_cast<size_t>(mipMapSize);
						currentWidth = std::max(1, (currentWidth / 2));
						currentHeight = std::max(1, (currentHeight / 2));
					}
					else
					{
						wereThereErrors = true;
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "OpenGL rejected compressed texture data: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							*o_errorMessage = errorMessage.str();
						}
						goto OnExit;
					}
				}
			}
			assert(currentPosition == (reinterpret_cast<uint8_t*>(fileContents) + fileSize));

		OnExit:
			if (fileContents != NULL)
			{
				free(fileContents);
				fileContents = NULL;
			}
			if (fileHandle != INVALID_HANDLE_VALUE)
			{
				if (CloseHandle(fileHandle) == FALSE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage(GetLastWindowsError());
						std::stringstream errorMessage;
						errorMessage << "\nWindows failed to close the texture file handle: " << windowsErrorMessage;
						*o_errorMessage += errorMessage.str();
					}
				}
				fileHandle = INVALID_HANDLE_VALUE;
			}
			if (wereThereErrors && (o_texture != 0))
			{
				const GLsizei textureCount = 1;
				glDeleteTextures(textureCount, &o_texture);
				assert(glGetError == GL_NO_ERROR);
				o_texture = 0;
			}

			return !wereThereErrors;
		}


		void TextureDesc::SetTexture(uint32_t textureUnit)
		{
			// We need three things to setup the texture:
			// The texture, the samplerID, and the textureUnit number.
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, _texture);
			glUniform1i(_samplerID, textureUnit);
		}

////////////////////////////////////////////////Member Functions////////////////////////////////////////////
		tTexture TextureManager::LoadTexture(const char* pTexturePath)
		{
			std::string key = GetFileNameWithoutExtension(pTexturePath);
			for (std::map<const char*, tTexture>::const_iterator iter = _textures.begin(); iter != _textures.end(); ++iter)
			{
				if (strcmp(iter->first, key.c_str()) == 0)
				{
					return iter->second;
				}
			}
			GLuint o_texture = 0;
			std::string o_errorMessage;
			bool result = LoadDDSTextureStatic(pTexturePath, o_texture, &o_errorMessage);
			glBindTexture(GL_TEXTURE_2D, 0);
			if(result)
				_textures.insert(std::pair<const char*, tTexture>(_strdup(key.c_str()), o_texture));
			return o_texture;
		}

		void TextureManager::Clean()
		{
			for (std::map<const char*, tTexture>::const_iterator iter = _textures.begin(); iter != _textures.end();)
			{
				char* pKey = const_cast<char*>(iter->first);
				SAFE_DELETE(pKey);
				tTexture pValue = iter++->second;
				glDeleteTextures(1, &pValue);
			}
			_textures.clear();
		}
	}
}

