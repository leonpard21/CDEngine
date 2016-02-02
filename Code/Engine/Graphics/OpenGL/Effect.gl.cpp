#include "../Common/Effect.h"
#include "../Common/UniformVariable.h"
#include <cstdint>
#include <string>
#include <sstream>
#include <cassert>
#include "Engine/Windows/WindowsFunctions.h"
#include "../../UserOutput/UserOutput.h"


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
}


namespace EAE_Engine
{
	namespace Graphics
	{
		bool Effect::Init(const char* i_pVSfilePath, const char* i_pFSfilePath, uint32_t renderState)
		{
			// Create a program
			_programId = glCreateProgram();
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to create a program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			else if (_programId == 0)
			{
				EAE_Engine::UserOutput::Print("OpenGL failed to create a program");
				return false;
			}
			// Load and attach the shaders
			if (!LoadVertexShader(_programId, i_pVSfilePath))
			{
				return false;
			}
			if (!LoadFragmentShader(_programId, i_pFSfilePath))
			{
				return false;
			}
			// Link the program
			{
				glLinkProgram(_programId);
				GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					// Get link info
					// (this won't be used unless linking fails
					// but it can be useful to look at when debugging)
					std::string linkInfo;
					{
						GLint infoSize;
						glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &infoSize);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							sLogInfo info(static_cast<size_t>(infoSize));
							GLsizei* dontReturnLength = NULL;
							glGetProgramInfoLog(_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
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
						glGetProgramiv(_programId, GL_LINK_STATUS, &didLinkingSucceed);
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
			}
			ExtractShaderUniforms();
			_renderState = renderState;
			return true;
		}

		bool Effect::Release()
		{
			bool wereThereErrors = false;
			if (_programId != 0)
			{
				glDeleteProgram(_programId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the program: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					wereThereErrors = true;
				}
				_programId = 0;
			}
			return !wereThereErrors;
		}

		bool Effect::BeginRender() 
		{
			assert(glGetError() == GL_NO_ERROR);
			return glGetError() == GL_NO_ERROR;
		}

		bool Effect::EndRender()
		{
			assert(glGetError() == GL_NO_ERROR);
			return glGetError() == GL_NO_ERROR;
		}

		bool Effect::LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage)
		{
			bool wereThereErrors = false;

			// Load the shader source from disk
			o_shader = NULL;
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
							std::string windowsErrorMessage = GetLastWindowsError();
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
				o_shader = malloc(o_size);
				if (o_shader)
				{
					DWORD bytesReadCount;
					OVERLAPPED* readSynchronously = NULL;
					if (ReadFile(fileHandle, o_shader, o_size,
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
				reinterpret_cast<char*>(o_shader)[o_size - 1] = '\0';
			}

		OnExit:

			if (wereThereErrors && o_shader)
			{
				free(o_shader);
				o_shader = NULL;
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

		bool Effect::LoadFragmentShader(const GLuint i_programId, const char* i_pFSFilePath)
		{
			// Verify that compiling shaders at run-time is supported
			{
				GLboolean isShaderCompilingSupported;
				glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
				if (!isShaderCompilingSupported)
				{
					EAE_Engine::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
					return false;
				}
			}

			bool wereThereErrors = false;
			// Load the source code from file and set it into a shader
			GLuint fragmentShaderId = 0;
			void* shaderSource = NULL;
			{
				// Load the shader source code
				size_t fileSize;
				{
					const char* sourceCodeFileName = i_pFSFilePath;
					std::string errorMessage;
					if (!LoadAndAllocateShaderProgram(sourceCodeFileName, shaderSource, fileSize, &errorMessage))
					{
						wereThereErrors = true;
						EAE_Engine::UserOutput::Print(errorMessage.c_str());
						goto OnExit;
					}
				}
				// Generate a shader
				fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
				{
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get an unused fragment shader ID: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						goto OnExit;
					}
					else if (fragmentShaderId == 0)
					{
						wereThereErrors = true;
						EAE_Engine::UserOutput::Print("OpenGL failed to get an unused fragment shader ID");
						goto OnExit;
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
						reinterpret_cast<GLchar*>(shaderSource)
					};
					const GLint* sourcesAreNullTerminated = NULL;//If length is NULL, each string is assumed to be null terminated.
					glShaderSource(fragmentShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to set the fragment shader source code: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						goto OnExit;
					}
				}
			}
			// Compile the shader source code
			{
				glCompileShader(fragmentShaderId);
				GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					// Get compilation info
					// (this won't be used unless compilation fails
					// but it can be useful to look at when debugging)
					std::string compilationInfo;
					{
						GLint infoSize;
						glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							sLogInfo info(static_cast<size_t>(infoSize));
							GLsizei* dontReturnLength = NULL;
							glGetShaderInfoLog(fragmentShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								compilationInfo = info.memory;
							}
							else
							{
								wereThereErrors = true;
								std::stringstream errorMessage;
								errorMessage << "OpenGL failed to get compilation info of the fragment shader source code: " <<
									reinterpret_cast<const char*>(gluErrorString(errorCode));
								EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
								goto OnExit;
							}
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get the length of the fragment shader compilation info: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
							goto OnExit;
						}
					}
					// Check to see if there were compilation errors
					GLint didCompilationSucceed;
					{
						glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							if (didCompilationSucceed == GL_FALSE)
							{
								wereThereErrors = true;
								std::stringstream errorMessage;
								errorMessage << "The fragment shader failed to compile:\n" << compilationInfo;
								EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
								goto OnExit;
							}
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to find out if compilation of the fragment shader source code succeeded: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
							goto OnExit;
						}
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to compile the fragment shader source code: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					goto OnExit;
				}
			}
			// Attach the shader to the program
			{
				glAttachShader(i_programId, fragmentShaderId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to attach the fragment shader to the program: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					goto OnExit;
				}
			}

		OnExit:
			if (fragmentShaderId != 0)
			{
				// Even if the shader was successfully compiled
				// once it has been attached to the program we can (and should) delete our reference to it
				// (any associated memory that OpenGL has allocated internally will be freed
				// once the program is deleted)
				glDeleteShader(fragmentShaderId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the fragment shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				}
				fragmentShaderId = 0;
			}
			if (shaderSource != NULL)
			{
				free(shaderSource);
				shaderSource = NULL;
			}
			return !wereThereErrors;
		}

		bool Effect::LoadVertexShader(const GLuint o_programId, const char* i_pVSFilePath)
		{
			// Verify that compiling shaders at run-time is supported 
			{
				GLboolean isShaderCompilingSupported;
				glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
				if (!isShaderCompilingSupported)
				{
					EAE_Engine::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
					return false;
				}
			}
			bool wereThereErrors = false;
			// Load the source code from file and set it into a shader
			GLuint vertexShaderId = 0;
			void* shaderSource = NULL;
			{
				// Load the shader source code
				size_t fileSize;
				{
					const char* sourceCodeFileName = i_pVSFilePath;
					std::string errorMessage;
					if (!LoadAndAllocateShaderProgram(sourceCodeFileName, shaderSource, fileSize, &errorMessage))
					{
						wereThereErrors = true;
						EAE_Engine::UserOutput::Print(errorMessage.c_str());
						goto OnExit;
					}
				}
				// Generate a shader
				vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
				{
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get an unused vertex shader ID: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						goto OnExit;
					}
					else if (vertexShaderId == 0)
					{
						wereThereErrors = true;
						EAE_Engine::UserOutput::Print("OpenGL failed to get an unused vertex shader ID");
						goto OnExit;
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
						reinterpret_cast<GLchar*>(shaderSource)
					};
					// Notice that we have added an extra NULL at the end of the buffer in LoadAndAllocateShaderProgram, 
					// so we load the buffer until the Null Terminated
					const GLint* sourcesAreNullTerminated = NULL;//If length is NULL, each string is assumed to be null terminated.
					glShaderSource(vertexShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
					// Or, we can use the following 2 lines, 
					// and we don't need to add an extra NULL at the end of the buffer in LoadAndAllocateShaderProgram
					// const GLint length = static_cast<GLuint>(fileSize);
					// glShaderSource(vertexShaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&shaderSource), &length);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to set the vertex shader source code: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						goto OnExit;
					}
				}
			}
			// Compile the shader source code
			{
				glCompileShader(vertexShaderId);
				GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					// Get compilation info
					// (this won't be used unless compilation fails
					// but it can be useful to look at when debugging)
					std::string compilationInfo;
					{
						GLint infoSize;
						glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							sLogInfo info(static_cast<size_t>(infoSize));
							GLsizei* dontReturnLength = NULL;
							glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								compilationInfo = info.memory;
							}
							else
							{
								wereThereErrors = true;
								std::stringstream errorMessage;
								errorMessage << "OpenGL failed to get compilation info of the vertex shader source code: " <<
									reinterpret_cast<const char*>(gluErrorString(errorCode));
								EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
								goto OnExit;
							}
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
							goto OnExit;
						}
					}
					// Check to see if there were compilation errors
					GLint didCompilationSucceed;
					{
						glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							if (didCompilationSucceed == GL_FALSE)
							{
								wereThereErrors = true;
								std::stringstream errorMessage;
								errorMessage << "The vertex shader failed to compile:\n" << compilationInfo;
								EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
								goto OnExit;
							}
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
							goto OnExit;
						}
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to compile the vertex shader source code: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					goto OnExit;
				}
			}
			// Attach the shader to the program
			{
				glAttachShader(_programId, vertexShaderId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to attach the vertex shader to the program: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					goto OnExit;
				}
			}

		OnExit:
			if (vertexShaderId != 0)
			{
				// Even if the shader was successfully compiled
				// once it has been attached to the program we can (and should) delete our reference to it
				// (any associated memory that OpenGL has allocated internally will be freed
				// once the program is deleted)
				glDeleteShader(vertexShaderId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the vertex shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				}
				vertexShaderId = 0;
			}
			if (shaderSource != NULL)
			{
				free(shaderSource);
				shaderSource = NULL;
			}
			return !wereThereErrors;
		}
	
		void Effect::ExtractShaderUniforms()
		{
			const int BUFF_SIZE = 64;
			GLint count;
			glGetProgramiv(_programId, GL_ACTIVE_UNIFORMS, &count);
			for (int i = 0; i < count; ++i)
			{
				char name[BUFF_SIZE]; // for holding the variable name
				GLint size = BUFF_SIZE;
				GLenum type;
				GLsizei length;
				GLsizei bufSize = BUFF_SIZE;
				glGetActiveUniform(_programId, i, bufSize, &length, &size, &type, name);
				// initialize the location to - 1, and be aware that glGetUniformLocation() returns - 1 if it didn't find the uniform.
				// (Note that if you declare a uniform but don't use it in the shader,
				// it will be optimized out and glGetUniformLocation() won't find it.)
				GLint location = glGetUniformLocation(_programId, name);
				if (location == -1)
					continue;
				if (type == GL_FLOAT_MAT4)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(name, UniformType::mat4);
					pUV->AddOwner(this, location);
				}
				else if (type == GL_FLOAT_VEC4)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(name, UniformType::float4);
					pUV->AddOwner(this, location);
				}
				else if (type == GL_FLOAT_VEC3)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(name, UniformType::float3);
					pUV->AddOwner(this, location);
				}
				else if (type == GL_FLOAT)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(name, UniformType::float1);
					pUV->AddOwner(this, location);
				}
			}
		}

		void Effect::SetUniform(UniformType type, GLint location, void* pArray, size_t count)
		{
			if (location == -1) 
				return;
			if (type == UniformType::float1)
			{
				// Set a single 3-dimensional float at that location
				glUniform1fv(location, 1, (float*)pArray);
			}
			if (type == UniformType::float2)
			{
				// Set a single 2-dimensional float at that location
				glUniform2fv(location, count / 2, (float*)pArray);
			}
			if (type == UniformType::float3)
			{
				// Set a single 3-dimensional float at that location
				glUniform3fv(location, 1, (float*)pArray);
			}
			else if (type == UniformType::mat4)
			{
				const GLboolean dontTranspose = false; // Matrices are already in the correct format
				const GLsizei uniformCountToSet = 1;
				// OpenGL does store matrices as columns, and so in this case we don't want the API to transpose anything.
				// reinterpret_cast trick is used as in Direct3D. 
				// This wouldn't work if GLfloat wasn't a float
				glUniformMatrix4fv(location, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(pArray));
				GLenum errorCode = glGetError();
				assert(GL_NO_ERROR == glGetError());
			}
		}

		void Effect::SetUniform(GLint location, void* pArray, size_t count)
		{
			if (location == -1) return;
			if (count == 1)
			{
				// Set a single 3-dimensional float at that location
				glUniform1fv(location, 1, (float*)pArray);
			}
			if (count == 2)
			{
				// Set a single 2-dimensional float at that location
				glUniform2fv(location, count / 2, (float*)pArray);
			}
			if (count == 3)
			{
				// Set a single 3-dimensional float at that location
				glUniform3fv(location, 1, (float*)pArray);
			}
			else if (count == 16)
			{
				const GLboolean dontTranspose = false; // Matrices are already in the correct format
				const GLsizei uniformCountToSet = 1;
				// OpenGL does store matrices as columns, and so in this case we don't want the API to transpose anything.
				// reinterpret_cast trick is used as in Direct3D. 
				// This wouldn't work if GLfloat wasn't a float
				glUniformMatrix4fv(location, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(pArray));
			}
		}

		GLint Effect::GetLocation(const char* pName)
		{
			GLint location = glGetUniformLocation(_programId, pName);
			const GLenum errorCode = glGetError();
			return location;
		}

		tSamplerID Effect::GetSamplerID(const char* pName, ShaderTypes shaderType)
		{
			GLint location = glGetUniformLocation(_programId, pName);
			return location;
			//return (tSamplerIDType)GetLocation(pName);
		}

		void Effect::OnNotify(UniformVariable* pVariable, GLint location)
		{
			// if any uniform variable has been changed, we need to record it
			_updateList[location] = pVariable;
		}

		void Effect::Update()
		{
			// for the changed uniform variables, we should update them for the graphic cards.
			for (std::map<GLint, UniformVariable*>::iterator iter = _updateList.begin(); iter != _updateList.end(); ++iter)
			{
				GLint location = iter->first;
				if (location == -1) return;
				UniformVariable* pValue = iter->second;
				SetUniform(pValue->GetUniformType(), location, pValue->GetElements(), pValue->GetElementCount());
			}
			_updateList.clear();
		}


		////////////////////////////////////////////////////////////////////
	}
}



