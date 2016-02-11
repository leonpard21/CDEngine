#ifndef EAE_ENGINE_GRAPHICS_UI_TEXT_H
#define EAE_ENGINE_GRAPHICS_UI_TEXT_H

#include "Image.h"
#include "RectTransform.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		struct Text 
		{
			Text() = default;
			std::string _value;
			RectTransform _rectTransform;
		};


		typedef void BtnCallBack(void*);
		struct Button
		{
			Button() = default;
			Text _text;
			Image* _pImage;
			BtnCallBack* _pBtnCallBack;
			RectTransform _rectTransform;
		};


		struct UIImage
		{
			UIImage() = default;
			Image* _pImage;
			RectTransform _rectTransform;
		};

		struct Slider 
		{
			Slider() = default;
			void SetHandle(float handleValue);
			UIImage _backgroundImage;
			UIImage _handleImage;
			float _min;
			float _max;
			float _interval;
			RectTransform _rectTransform;
		};

		struct Toggle
		{
			Toggle() = default;
			void Check(bool checked);
			UIImage _backgroundImage;
			UIImage _checkMarkImage;
			bool _checked;
			RectTransform _rectTransform;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_UI_TEXT_H