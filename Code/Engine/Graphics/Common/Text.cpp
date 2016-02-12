#include "Text.h"
#include "General/BasicShapes.h"
#include "Math/MathTool.h"
#include "Device.h"
#include "UserInput/UserInput.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		bool OnHovering(const RectTransform& rectTransform) 
		{
			Math::Vector2 screenPos = UserInput::GetMousePos();
			screenPos = ConvertWinToEngineScreenPos(screenPos);
			Rectangle range = rectTransform.GetScreenSpaceRect();
			bool result = true;
			if (screenPos._x < range._left || screenPos._x > range._right)
			{
				result = false;
			}
			if (screenPos._y < range._bottom || screenPos._y > range._top)
			{
				result = false;
			}
			return result;
		}

		bool OnClick(const RectTransform& rectTransform)
		{
			bool onHoving = OnHovering(rectTransform);
			bool clicking = UserInput::IsMouseButtonPressed(VK_LBUTTON);
			if (onHoving && clicking)
				return true;
			return false;
		}

		void Button::Update() 
		{
			// the text's anchor should same with the background image
			_text._rectTransform.SetAnchor(_backgroundImage._rectTransform.GetAnchor());
			if (OnClick(_backgroundImage._rectTransform))
			{
				_pBtnCallBack(nullptr);
			}
		}


		void Slider::Update() 
		{
			// the handle's anchor should same with the background
			_handleImage._rectTransform.SetAnchor(_backgroundImage._rectTransform.GetAnchor());
			if (OnClick(_handleImage._rectTransform))
			{
				OnDrag();
			}
			UpdateHandle();
		}

		void Slider::OnDrag()
		{
			Math::Vector2 screenPos = UserInput::GetMousePos();
			screenPos = ConvertWinToEngineScreenPos(screenPos);
			// get the range of the background image, in screen coordinate
			Rectangle range = _backgroundImage._rectTransform.GetScreenSpaceRect();
			screenPos._x = Math::max<float>(screenPos._x, range._left);
			screenPos._x = Math::min<float>(screenPos._x, range._right);

			// calculate and convert the screenYpos from windows Screen coordinate to our Unity style screen coordinate.
			float screenYpos = -1.0f * (GetScreenRect()._height - (range._bottom + range._top) / 2.0f);
			Rectangle handleRect = { screenPos._x, screenYpos, 16.0f, 16.0f };
			_handleImage._rectTransform.SetRect(handleRect);

			_handleValue = (screenPos._x - range._left) / (range._right - range._left) * (_max - _min) + _min;
		}

		// handleValue should be a value between _min and _max
		void Slider::UpdateHandle()
		{
			// get the range of the background image, in screen coordinate
			Rectangle range = _backgroundImage._rectTransform.GetScreenSpaceRect();
			// limit the screenXpos to be the correct range.
			float screenXpos = (_handleValue - _min) / (_max - _min) * (range._right - range._left) + range._left;
			screenXpos = Math::max<float>(screenXpos, range._left);
			screenXpos = Math::min<float>(screenXpos, range._right);
			// calculate and convert the screenYpos from windows Screen coordinate to our Unity style screen coordinate.
			float screenYpos = -1.0f * (GetScreenRect()._height - (range._bottom + range._top) / 2.0f);
			Rectangle handleRect = { screenXpos, screenYpos, 16.0f, 16.0f };
			_handleImage._rectTransform.SetRect(handleRect);
		}

		void Toggle::Update()
		{
			// the _checkMartImage's anchor and rect should same with the background
			_checkMarkImage._rectTransform.SetAnchor(_backgroundImage._rectTransform.GetAnchor());
			_checkMarkImage._rectTransform.SetRect(_backgroundImage._rectTransform.GetRect());
			bool clicked = EAE_Engine::Graphics::OnClick(_rectTransform);
			if (clicked)
			{
				_checked = !_checked;
			}

		}


	}
}

