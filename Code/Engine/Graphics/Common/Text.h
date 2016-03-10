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
		bool OnDrag(const RectTransform& rectTransform);

		struct Text 
		{
			Text() = default;
			std::string _value;
			RectTransform _rectTransform;
		};

		struct UIImage
		{
			UIImage(Image* pImage, RectTransform rect) : _pImage(pImage), _rectTransform(rect) {}
			UIImage(const UIImage& i_other):_pImage(i_other._pImage), _rectTransform(i_other._rectTransform){}
			UIImage& operator= (const UIImage&) = default;
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
			void SetHandle(float handleValue) { _handleValue = handleValue; }
			void UpdateHandle();
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
			Toggle& operator= (const Toggle&) = default;
			void Update();
			UIImage _backgroundImage;
			UIImage _checkMarkImage;
			bool _checked;
			RectTransform _rectTransform;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_UI_TEXT_H