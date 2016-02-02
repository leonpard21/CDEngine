/*
	Unlike a standard C/C++ program,
	WinMain() is the function where Windows programs start execution
*/
#include "Game.h"

// WindowsIncludes.h is a file I made that #includes the important windows.h file
// along with any associated #defines that we want in order to get Windows functionality.
// You should always only have a single way of #including windows.h that everything
// in your solution uses.
#include "Engine/Windows/WindowsIncludes.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _DEBUG

//#ifdef DEBUG
//	#include <vld.h>
//#endif

// Entry Point
//============
int WINAPI WinMain(
	// When you see a type in capital letters that starts with an "H"
	// it is a "handle" to something
	// (for example, an HSOMETHING would be handle to a "something").
	// A "handle" is kind of like a pointer:
	// It's an object that is used to represent a different object.
	// This first parameter, then, is a handle to the specific instance
	// of the program that is being run currently.
	// If someone runs this program twice then WinMain() will be called twice,
	// each time with a different handle.
	// When you interact with Windows and want to let it know which specific
	// instance of the program you are referring to you can use this handle.
	HINSTANCE i_thisInstanceOfTheProgram,
	// This instance handle is here for legacy reasons and is unused
	HINSTANCE,
	char* i_commandLineArguments,
	int i_initialWindowDisplayState )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(0);
#endif
	// A Windows program doesn't actually need any windows at all
	// but in most cases there will be a single "main" window
	// and when it is closed the program will exit
	const int exitCode = CreateMainWindowAndReturnExitCodeWhenItCloses( i_thisInstanceOfTheProgram, i_initialWindowDisplayState );

#if defined(DEBUG) | defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
	// Unlike standard C/C++ programs there is no standardized return value
	// to indicate that the program "succeeded".
	// Windows itself completely ignores the value that the program returns,
	// and so it is only useful if you have other programs that
	// are paying attention and expecting specific values.
	return exitCode;
}
