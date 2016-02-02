/*
	This class defines a Maya file translator
*/

#ifndef EAEEGNIGNE_CMAYAMESHEXPORTER_H
#define EAEEGNIGNE_CMAYAMESHEXPORTER_H

// Header Files
//=============

// This must come before any Maya header files
// to make sure that windows.h is #included in a consistent way
#include "Engine/Windows/WindowsIncludes.h"
#include <maya/MPxFileTranslator.h>

// Class Definition
//=================

namespace EAE_Engine
{
	namespace Tools 
	{
		class cMayaMeshExporter : public MPxFileTranslator
		{
			// Inherited Interface
			//====================

		public:

			// The writer method is what exports the file
			virtual bool haveWriteMethod() const { return true; }
			virtual MStatus writer(const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode);

			// We won't implement a Maya importer in our class.
			// If you wanted to be able to import your mesh files into Maya
			// you would have to return true from haveReadMethod()
			// and then implement reader().

			// You can choose what the default file extension of an exported mesh is
			virtual MString defaultExtension() const { return ".aosmeshtext"; }

			// Interface
			//==========

		public:

			// This function is used by Maya to create an instance of the exporter (see registerFileTranslator() in EntryPoint.cpp)
			static void* Create()
			{
				return new cMayaMeshExporter;
			}
		};
	}
}

#endif	// EAEEGNIGNE_CMAYAMESHEXPORTER_H
