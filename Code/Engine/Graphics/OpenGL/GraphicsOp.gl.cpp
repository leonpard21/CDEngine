#include "../Common/GraphicsOp.h"
#include "Memory/Source/New.h"
#include "UserOutput/UserOutput.h"
#include "Windows/WindowsFunctions.h"
#include <string>
#include <sstream>
#include <cassert>


namespace
{
	// This helper struct exists to be able to dynamically allocate memory to get "log info"
	// which will automatically be freed when the struct goes out of scope
	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
		~sLogInfo() { if (memory) free(memory); }
	};

	bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_pShaderSource, size_t& o_size, std::string* o_errorMessage)
	{
		bool wereThereErrors = false;
		// Load the shader source from disk
		o_pShaderSource = NULL;
		HANDLE fileHandle = INVALID_HANDLE_VALUE;
		{
			// Open the file
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
						std::string windowsErrorMessage = EAE_Engine::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to open the shader file: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Get the file's size
			{
				LARGE_INTEGER fileSize_integer;
				if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
				{
					assert(fileSize_integer.QuadPart <= SIZE_MAX);
					o_size = static_cast<size_t>(fileSize_integer.QuadPart);
				}
				else
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsError = EAE_Engine::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to get the size of shader: " << windowsError;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
				// Add an extra byte for a NULL terminator
				// Because we use the NULL to test whether the GL shaders has been ended.
				// So we add an extra NULL at the end of the file.
				o_size += 1;
			}
			// Read the file's contents into temporary memory
			o_pShaderSource = malloc(o_size);
			if (o_pShaderSource)
			{
				DWORD bytesReadCount;
				OVERLAPPED* readSynchronously = NULL;
				if (ReadFile(fileHandle, o_pShaderSource, o_size,
					&bytesReadCount, readSynchronously) == FALSE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsError = EAE_Engine::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to read the contents of shader: " << windowsError;
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
					errorMessage << "Failed to allocate " << o_size << " bytes to read in the shader program " << i_path;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
			// Add the NULL terminator, so that when we load the OpenGL Shader,
			// we can know when the shader files end by NULL 
			reinterpret_cast<char*>(o_pShaderSource)[o_size - 1] = '\0';
		}

	OnExit:
		if (wereThereErrors && o_pShaderSource)
		{
			free(o_pShaderSource);
			o_pShaderSource = NULL;
		}
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			if (CloseHandle(fileHandle) == FALSE)
			{
				if (!wereThereErrors && o_errorMessage)
				{
					std::string windowsError = EAE_Engine::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to close the shader file handle: " << windowsError;
					*o_errorMessage = errorMessage.str();
				}
				wereThereErrors = true;
			}
			fileHandle = INVALID_HANDLE_VALUE;
		}

		return !wereThereErrors;
	}

	void* LoadShaderFromFile(GLuint& o_shaderId, const char* i_pShaderFilePath, GLenum shaderType)
	{
		// Load the source code from file and set it into a shader
		void* pShaderSource = nullptr;
		{
			// Load the shader source code
			size_t fileSize;
			{
				const char* sourceCodeFileName = i_pShaderFilePath;
				std::string errorMessage;
				if (!LoadAndAllocateShaderProgram(sourceCodeFileName, pShaderSource, fileSize, &errorMessage))
				{
					EAE_Engine::UserOutput::Print(errorMessage.c_str());
					goto OnError;
				}
			}
			// Generate a shader
			o_shaderId = glCreateShader(shaderType);// Ex. GL_VERTEX_SHADER
			{
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused shader ID: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					goto OnError;
				}
				else if (o_shaderId == 0)
				{
					EAE_Engine::UserOutput::Print("OpenGL failed to get an unused shader ID");
					goto OnError;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 1;
				// Rather than specifying the length of each string, however, 
				// I am instructing OpenGL to treat them as NULL - terminated C strings.
				// This means that I also need to explicitly add a NULL terminator to the source code, 
				// which is why changes to LoadAndAllocateShaderProgram() also had to be made.
				const GLchar* shaderSources[] =
				{
					reinterpret_cast<GLchar*>(pShaderSource)
				};
				// Notice that we have added an extra NULL at the end of the buffer in LoadAndAllocateShaderProgram, 
				// so we load the buffer until the Null Terminated
				const GLint* sourcesAreNullTerminated = NULL;//If length is NULL, each string is assumed to be null terminated.
				glShaderSource(o_shaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
				// Or, we can use the following 2 lines, 
				// and we don't need to add an extra NULL at the end of the buffer in LoadAndAllocateShaderProgram
				// const GLint length = static_cast<GLuint>(fileSize);
				// glShaderSource(o_shaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&shaderSource), &length);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the vertex shader source code: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					goto OnError;
				}
			}
		}
		return pShaderSource;

	OnError:
		if (pShaderSource != NULL)
		{
			free(pShaderSource);
			pShaderSource = NULL;
		}
		return nullptr;
	}

	bool CompileShaderAfterLoaded(GLuint& o_shaderId)
	{
		// Compile the shader source code
		glCompileShader(o_shaderId);
		GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get compilation info
			// (this won't be used unless compilation fails
			// but it can be useful to look at when debugging)
			std::string compilationInfo;
			{
				GLint infoSize;
				glGetShaderiv(o_shaderId, GL_INFO_LOG_LENGTH, &infoSize);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					sLogInfo info(static_cast<size_t>(infoSize));
					GLsizei* dontReturnLength = NULL;
					glGetShaderInfoLog(o_shaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						compilationInfo = info.memory;
					}
					else
					{

						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get compilation info of the vertex shader source code: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						return false;
					}
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			// Check to see if there were compilation errors
			GLint didCompilationSucceed;
			{
				glGetShaderiv(o_shaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didCompilationSucceed == GL_FALSE)
					{
						std::stringstream errorMessage;
						errorMessage << "The vertex shader failed to compile:\n" << compilationInfo;
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						return false;
					}
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to compile the vertex shader source code: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));
			EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
			return false;
		}
		return true;
	}

}


namespace EAE_Engine
{
	namespace Graphics
	{
		bool CreateVertexArrayObj(GLuint& io_vertexArrayId)
		{
			// Create a vertex array object and make it active
			const GLsizei arrayCount = 1;
			glGenVertexArrays(arrayCount, &io_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindVertexArray(io_vertexArrayId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex array: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex array ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}

		bool DeleteVertexArrayObj(GLuint& o_vertexArrayId)
		{
			if (o_vertexArrayId != 0)
			{
				const GLsizei arrayCount = 1;
				glDeleteVertexArrays(arrayCount, &o_vertexArrayId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the vertex array: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					UserOutput::Print(errorMessage.str().c_str());
				}
				o_vertexArrayId = 0;
			}
			return true;
		}

		bool CreateBindBufferObj(GLuint& o_bufferID, GLenum bufferTarget)
		{
			o_bufferID = 0;
			// Create an index buffer object and make it active
			const GLsizei bufferCount = 1;
			glGenBuffers(bufferCount, &o_bufferID);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindBuffer(bufferTarget, o_bufferID);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the index buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused index buffer ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}

		bool DeleteBufferObj(GLuint& io_BufferID, GLsizei bufferCount)
		{
			// if we dpn't delete the vertex buffer object, it will cause the memory leaks.
			if (io_BufferID != 0)
			{
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &io_BufferID);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the vertex buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
				io_BufferID = 0;
			}
			return true;
		}

		bool InitVertexFormat(const MeshGLVertexElements& elements, GLsizei stride)
		{
			MeshGLVertexElement* pVertexElements = elements._pGlements;
			// Initialize the vertex format
			GLvoid* offset = 0;
			for (GLint index = 0; index < elements._elementCount; ++index)
			{
				const GLuint vertexElementLocation = pVertexElements[index]._vertexElementLocation;
				const GLint elementCount = pVertexElements[index]._elementCount;
				const GLenum vertexType = pVertexElements[index]._type;
				const GLboolean normalized = pVertexElements[index]._normalized;
				glVertexAttribPointer(vertexElementLocation, elementCount, vertexType, normalized, stride, offset);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glEnableVertexAttribArray(vertexElementLocation);
					const GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						offset = reinterpret_cast<GLvoid*>(reinterpret_cast<uint8_t*>(offset) + (elementCount * pVertexElements[index]._sizeOfType));
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to enable the vertex attribute: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						return false;
					}
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the vertex attribute: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			return true;
		}

		bool FillGLBuffer(GLenum bufferTarget, void* pVertices, const uint32_t bufferSize, GLenum usage)
		{
			// Assign the data to the vertex buffer
			glBufferData(bufferTarget, bufferSize, NULL, usage);
			glBufferSubData(bufferTarget, 0, bufferSize, reinterpret_cast<GLvoid*>(pVertices));
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to allocate the vertex buffer: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}
		
		// Create a program
		bool CreateProgram(GLuint& o_programId) 
		{
			o_programId = glCreateProgram();
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to create a program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			else if (o_programId == 0)
			{
				EAE_Engine::UserOutput::Print("OpenGL failed to create a program");
				return false;
			}
			return true;
		}

		bool DeleteProgram(GLuint& i_programId) 
		{
			bool wereThereErrors = false;
			if (i_programId != 0)
			{
				glDeleteProgram(i_programId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the program: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					wereThereErrors = true;
				}
				i_programId = 0;
			}
			return !wereThereErrors;
		}
		
		//Check if the runtime compiling shader is supported or not.
		bool IsRunTimeCompilingShdSupported()
		{
			// Verify that compiling shaders at run-time is supported 
			GLboolean isShaderCompilingSupported;
			glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
			if (!isShaderCompilingSupported)
			{
				EAE_Engine::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
				return false;
			}
			return true;
		}

		bool LoadCompileShader(GLuint& o_shaderId, const char* i_pShaderFilePath, GLenum shaderType)
		{
			if (!IsRunTimeCompilingShdSupported())
				return false;
			// Load the source code from file and set it into a shader
			void* pShaderSource = LoadShaderFromFile(o_shaderId, i_pShaderFilePath, shaderType);
			if (pShaderSource == nullptr)
				return false;
			bool result = CompileShaderAfterLoaded(o_shaderId);
			// Since we have compiled the Shader from the Source, 
			// we don't need the ShaderShource Anymore.
			if (pShaderSource != NULL)
			{
				free(pShaderSource);
				pShaderSource = NULL;
			}
			return result;
		}
	
		// Attach the shader to the program
		bool AttachShaderToProgram(GLuint& io_programId, GLuint& i_shaderId)
		{
			glAttachShader(io_programId, i_shaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the vertex shader to the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}

		// Even if the shader was successfully compiled
		// once it has been attached to the program we can (and should) delete our reference to it
		// (any associated memory that OpenGL has allocated internally will be freed
		// once the program is deleted)
		bool DeleteShader(GLuint& i_shaderId)
		{
			bool wereThereErrors = false;
			if (i_shaderId != 0)
			{
				glDeleteShader(i_shaderId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the vertex shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					wereThereErrors = true;
				}
				i_shaderId = 0;
			}
			return !wereThereErrors;
		}

		// Link the program
		bool LinkProgramAfterAttached(GLuint& io_programId)
		{
			glLinkProgram(io_programId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get link info
				// (this won't be used unless linking fails
				// but it can be useful to look at when debugging)
				std::string linkInfo;
				{
					GLint infoSize;
					glGetProgramiv(io_programId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						sLogInfo info(static_cast<size_t>(infoSize));
						GLsizei* dontReturnLength = NULL;
						glGetProgramInfoLog(io_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							linkInfo = info.memory;
						}
						else
						{
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get link info of the program: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
							return false;
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the program link info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						return false;
					}
				}
				// Check to see if there were link errors
				GLint didLinkingSucceed;
				{
					glGetProgramiv(io_programId, GL_LINK_STATUS, &didLinkingSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didLinkingSucceed == GL_FALSE)
						{
							std::stringstream errorMessage;
							errorMessage << "The program failed to link:\n" << linkInfo;
							EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
							return false;
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if linking of the program succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						return false;
					}
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to link the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}


	}
}


