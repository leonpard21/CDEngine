#include "Text.h"
#include "General/BasicShapes.h"
#include "Math/MathTool.h"
#include "Device.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		void Slider::SetHandle(float handleValue)
		{
			// the handle's anchor should same with the background
			_handleImage._rectTransform.SetAnchor(_backgroundImage._rectTransform.GetAnchor());
			// get the range of the background image, in screen coordinate
			Rectangle range = _backgroundImage._rectTransform.GetScreenSpaceRect();
			// limit the screenXpos to be the correct range.
			float screenXpos = (handleValue - _min) / (_max - _min) * (range._right - range._left) + range._left;
			screenXpos = Math::max<float>(screenXpos, range._left);
			screenXpos = Math::min<float>(screenXpos, range._right);
			// calculate and convert the screenYpos from windows Screen coordinate to our Unity style screen coordinate.
			float screenYpos = -1.0f * (GetScreenRect()._height - (range._bottom + range._top) / 2.0f);
			Rectangle handleRect = { screenXpos, screenYpos, 16.0f, 16.0f};
			_handleImage._rectTransform.SetRect(handleRect);
		}

		void Toggle::Check(bool checked)
		{
			// the _checkMartImage's anchor and rect should same with the background
			_checkMarkImage._rectTransform.SetAnchor(_backgroundImage._rectTransform.GetAnchor());
			_checkMarkImage._rectTransform.SetRect(_backgroundImage._rectTransform.GetRect());
			_checked = checked;
		}

	}
}

