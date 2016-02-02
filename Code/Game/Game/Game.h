/*
	This file contains all of the function declarations for this example program
*/

#ifndef EAE6320_WINDOWSPROGRAM_H
#define EAE6320_WINDOWSPROGRAM_H

// Header Files
//=============

#include <string>
#include "Engine/Windows/WindowsFunctions.h"

// Main Function
//==============

int CreateMainWindowAndReturnExitCodeWhenItCloses( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState );

// Helper Functions
//=================

// There are two things that have to happen:
//	* The main window must be created
//		(this will happen quickly)
//	* The main window must run until it gets closed
//		(this is up to the user)

bool CreateMainWindow( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState );
int WaitForMainWindowToCloseAndReturnExitCode( const HINSTANCE i_thisInstanceOfTheProgram );

// CreateMainWindow
//-----------------

HWND CreateMainWindowHandle( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState );
ATOM RegisterMainWindowClass( const HINSTANCE i_thisInstanceOfTheProgram );

// WaitForMainWindowToCloseAndReturnExitCode
//------------------------------------------

bool CleanupMainWindow();
bool OnMainWindowClosed( const HINSTANCE i_thisInstanceOfTheProgram );
LRESULT CALLBACK OnMessageReceived( HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam );
bool UnregisterMainWindowClass( const HINSTANCE i_thisInstanceOfTheProgram );
bool WaitForMainWindowToClose( int& o_exitCode );

#endif	// EAE_Engine_WINDOWSPROGRAM_H
