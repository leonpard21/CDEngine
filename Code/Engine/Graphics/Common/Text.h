#ifndef EAE_ENGINE_GRAPHICS_UI_TEXT_H
#define EAE_ENGINE_GRAPHICS_UI_TEXT_H

#include "Image.h"
#include "RectTransform.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		bool OnHovering(const RectTransform& rectTransform);
		bool OnClick(const RectTransform& rectTransform);

		struct Text 
		{
			Text() = default;
			std::string _value;
			RectTransform _rectTransform;
		};

		struct UIImage
		{
			UIImage() = default;
			Image* _pImage;
			RectTransform _rectTransform;
		};

		typedef void BtnCallBack(void*);
		struct Button
		{
			Button() = default;
			void Update();
			Text _text;
			UIImage _backgroundImage;
			BtnCallBack* _pBtnCallBack;
			RectTransform _rectTransform;
		};

		struct Slider 
		{
			Slider() = default;
			void Update();
			void SetHandle(float handleValue);
			void OnDrag();
			UIImage _backgroundImage;
			UIImage _handleImage;
			float _min;
			float _max;
			float _interval;
			float _handleValue;
			RectTransform _rectTransform;
		};

		struct Toggle
		{
			Toggle() = default;
			void Update();
			UIImage _backgroundImage;
			UIImage _checkMarkImage;
			bool _checked;
			RectTransform _rectTransform;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_UI_TEXT_H