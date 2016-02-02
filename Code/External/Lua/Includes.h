/*
	This file can be included by a project that wants to use Lua

	Using this intermediate file to store the version number
	means that no other source files will have to change
	if you ever change to a new version.
*/

#ifndef EAE6320_LUAINCLUDES_H
#define EAE6320_LUAINCLUDES_H

// Ideally I would want this #include statement to automatically use the version number from an evironment variable
// so that it would automatically update if the solution ever moved to a new version.
// I can't figure out way to do that, however, without requiring that
// every project that includes Lua must also use the LuaVersion.props property sheet,
// and that doesn't feel like a good requirement.
// I decided to leave it, which means that if the version ever changed it would have to happen in two places instead of one.
// (If anyone can figure out a way to do it better let me know!)

#include "5.2.3/src/lua.hpp"

#endif	// EAE6320_LUAINCLUDES_H
