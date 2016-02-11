#include "Font.h"
#include "Device.h"

#if defined( EAEENGINE_PLATFORM_D3D9 )




namespace EAE_Engine
{
	namespace Graphics
	{
		FontManager::~FontManager()
		{
			if (_pFont) {
				_pFont->Release();
				_pFont = NULL;
			}
		}

		void FontManager::Init() 
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			HRESULT hr = D3DXCreateFont(pD3DDevice,     //D3D Device
				22,               //Font height
				0,                //Font width
				FW_NORMAL,        //Font Weight
				1,                //MipLevels
				false,            //Italic
				DEFAULT_CHARSET,  //CharSet
				OUT_DEFAULT_PRECIS, //OutputPrecision
				ANTIALIASED_QUALITY, //Quality
				DEFAULT_PITCH | FF_DONTCARE,//PitchAndFamily
				"Arial",          //pFacename,
				&_pFont);         //ppFont
		}

		void FontManager::DrawString(Rectangle rect, const char* pSentense)
		{
			RECT font_rect;
			SetRect(&font_rect, rect._left, rect._top, rect._right, rect._bottom);
			INT font_height = _pFont->DrawText(NULL, //pSprite
				pSentense,  //pString
				-1,          //Count
				&font_rect,  //pRect
				DT_LEFT | DT_NOCLIP,//Format,//DT_LEFT | DT_WORDBREAK, //Format,
				0xFFFF00FF); //Color, 0xFFFFFFFF
		}


	}
}

#elif defined( EAEENGINE_PLATFORM_GL )


#endif
