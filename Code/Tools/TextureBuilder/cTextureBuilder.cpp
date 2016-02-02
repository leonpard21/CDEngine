// Header Files
//=============

#include "cTextureBuilder.h"

#include <d3d9.h>
#include <d3dx9tex.h>
#include <sstream>

// Static Data Initialization
//===========================

namespace
{
	IDirect3D9* s_pDirect3dInterface = NULL;
	IDirect3DDevice9* s_pDirect3dDevice = NULL;
	IDirect3DBaseTexture9* s_pTexture = NULL;
}

// Helper Function Declarations
//=============================

namespace
{
	bool Initialize( const char* i_path_source );
	bool ShutDown();
}

// Interface
//==========

// Build
//------

bool EAE_Engine::Tools::cTextureBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	if ( !Initialize( _path_source ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}

	// Get information about the source image
	D3DXIMAGE_INFO imageInfo_source;
	{
		if ( FAILED( D3DXGetImageInfoFromFile( _path_source, &imageInfo_source ) ) )
		{
			wereThereErrors = true;
			OutputErrorMessage( "DirectX failed to get image info for the source image", _path_source );
			goto OnExit;
		}
	}
	// Load the source image and do any necessary processing (compress, generate MIP maps, change resolution, etc.)
	{
		// Try to match the original image's format as closely as possible
		D3DFORMAT format = D3DFMT_UNKNOWN;
		{
			if ( imageInfo_source.ImageFileFormat == D3DXIFF_DDS )
			{
				// DDS files will remain unchanged
				// (this could cause problems in the simplistic OpenGL texture loading code that I provide)
				format = imageInfo_source.Format;
			}
			else
			{
				// The decision of which DXT format to use is simplistic:
				//	* If it can easily be determined that the source image doesn't have an alpha channel then DXT1 is used
				//	* DXT5 is used for everything else
				const D3DFORMAT format_original = imageInfo_source.Format;
				const bool isAlphaChannelUnnecessary = ( format_original == D3DFMT_R8G8B8 ) || ( format_original == D3DFMT_X8R8G8B8 )
					|| ( format_original == D3DFMT_R5G6B5 ) || ( format_original == D3DFMT_X1R5G5B5 )
					|| ( format_original == D3DFMT_X4R4G4B4  ) || ( format_original == D3DFMT_X8B8G8R8 );
				if ( isAlphaChannelUnnecessary )
				{
					format = D3DFMT_DXT1;
				}
				else
				{
					format = D3DFMT_DXT5;
				}
			}
		}
		// The source width and height could be used, but ensuring that the dimensions are always a power-of-2 is more compatible
		// (and the image will probably end up taking the same amount of space anyway because of alignment issues)
		const unsigned int roundUpToAPowerOf2 = D3DX_DEFAULT;
		unsigned int requestedMipMapCount;
		{
			const bool doMipMapsExist = imageInfo_source.MipLevels > 1;
			if ( doMipMapsExist )
			{
				requestedMipMapCount = D3DX_FROM_FILE;
			}
			else
			{
				// This will generate all of the potential MIP map levels (down to the smallest 1x1)
				requestedMipMapCount = D3DX_DEFAULT;
			}
		}
		const DWORD staticTexture = 0;
		const D3DPOOL letD3dManageMemory = D3DPOOL_MANAGED;
		const DWORD useDefaultFiltering = D3DX_DEFAULT;
		const D3DCOLOR noColorKey = 0;
		PALETTEENTRY* noColorPalette = NULL;
		switch( imageInfo_source.ResourceType )
		{
		case D3DRTYPE_TEXTURE:
			{
				const HRESULT result = D3DXCreateTextureFromFileEx( s_pDirect3dDevice, _path_source, 
					roundUpToAPowerOf2, roundUpToAPowerOf2, requestedMipMapCount,
					staticTexture, format, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey, 
					&imageInfo_source, noColorPalette, reinterpret_cast<IDirect3DTexture9**>( &s_pTexture ) );
				if ( FAILED( result ) )
				{
					wereThereErrors = true;
					OutputErrorMessage( "DirectX failed to get image info for the source image", _path_source );
					goto OnExit;
				}
			}
			break;
		case D3DRTYPE_CUBETEXTURE:
			{
				const HRESULT result = D3DXCreateCubeTextureFromFileEx( s_pDirect3dDevice, _path_source, 
					roundUpToAPowerOf2, requestedMipMapCount,
					staticTexture, format, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey, 
					&imageInfo_source, noColorPalette, reinterpret_cast<IDirect3DCubeTexture9**>( &s_pTexture ) );
				if ( FAILED( result ) )
				{
					wereThereErrors = true;
					OutputErrorMessage( "DirectX failed to get image info for the source image", _path_source );
					goto OnExit;
				}
			}
			break;
		case D3DRTYPE_VOLUMETEXTURE:
			{
				const HRESULT result = D3DXCreateVolumeTextureFromFileEx( s_pDirect3dDevice, _path_source, 
					roundUpToAPowerOf2, roundUpToAPowerOf2, imageInfo_source.Depth, requestedMipMapCount,
					staticTexture, format, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey, 
					&imageInfo_source, noColorPalette, reinterpret_cast<IDirect3DVolumeTexture9**>( &s_pTexture ) );
				if ( FAILED( result ) )
				{
					wereThereErrors = true;
					OutputErrorMessage( "DirectX failed to get image info for the source image", _path_source );
					goto OnExit;
				}
			}
			break;
		default:
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "Unsupported texture resource type " << imageInfo_source.ResourceType;
				OutputErrorMessage( errorMessage.str().c_str(), _path_source );
				goto OnExit;
			}
		}
	}
	// Save the texture
	{
		const D3DXIMAGE_FILEFORMAT ddsFormat = D3DXIFF_DDS;
		PALETTEENTRY* noColorPalette = NULL;
		HRESULT result = D3DXSaveTextureToFile( _path_target, ddsFormat, s_pTexture, noColorPalette );
		if ( FAILED( result ) )
		{
			wereThereErrors = true;
			OutputErrorMessage( "DirectX failed to save the texture", _path_target );
			goto OnExit;
		}
	}

OnExit:

	if ( !ShutDown() )
	{
		wereThereErrors = true;
	}

	return !wereThereErrors;
}

// Helper Function Definitions
//============================

namespace
{
	bool Initialize( const char* i_path_source )
	{
		// Create the D3D9 interface
		{
			s_pDirect3dInterface = Direct3DCreate9( D3D_SDK_VERSION );
			if ( !s_pDirect3dInterface )
			{
				EAE_Engine::Tools::OutputErrorMessage( "DirectX failed to create a Direct3D9 interface", i_path_source );
				return false;
			}
		}
		// Create the D3D9 device interface
		{
			HWND mainWindow = GetConsoleWindow();	// If this is NULL it's ok
			const UINT useDefaultDevice = D3DADAPTER_DEFAULT;
			const D3DDEVTYPE useHardwareRendering = D3DDEVTYPE_HAL;
			const DWORD useSoftwareVertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			D3DPRESENT_PARAMETERS presentationParameters = { 0 };
			{
				presentationParameters.BackBufferWidth = mainWindow == NULL ? 1 : 0;
				presentationParameters.BackBufferHeight = mainWindow == NULL ? 1 : 0;
				presentationParameters.BackBufferFormat = D3DFMT_UNKNOWN;
				presentationParameters.BackBufferCount = mainWindow == NULL ? 1 : 0;
				presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
				presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
				presentationParameters.hDeviceWindow = mainWindow;
				presentationParameters.Windowed = TRUE;
				presentationParameters.EnableAutoDepthStencil = FALSE;
			}
			HRESULT result = s_pDirect3dInterface->CreateDevice( useDefaultDevice, useHardwareRendering,
				mainWindow, useSoftwareVertexProcessing, &presentationParameters, &s_pDirect3dDevice );
			if ( FAILED( result ) )
			{
				const D3DDEVTYPE useReferenceSoftwareImplementation = D3DDEVTYPE_REF;
				result = s_pDirect3dInterface->CreateDevice( useDefaultDevice, useReferenceSoftwareImplementation,
					mainWindow, useSoftwareVertexProcessing, &presentationParameters, &s_pDirect3dDevice );
				if ( FAILED( result ) )
				{
					EAE_Engine::Tools::OutputErrorMessage( "DirectX failed to create a Direct3D9 device", i_path_source );
					return false;
				}
			}
		}

		return true;
	}

	bool ShutDown()
	{
		bool wereThereErrors = false;

		if ( s_pTexture )
		{
			s_pTexture->Release();
			s_pTexture = NULL;
		}

		if ( s_pDirect3dInterface )
		{
			if ( s_pDirect3dDevice )
			{
				s_pDirect3dDevice->Release();
				s_pDirect3dDevice = NULL;
			}

			s_pDirect3dInterface->Release();
			s_pDirect3dInterface = NULL;
		}

		return !wereThereErrors;
	}
}
