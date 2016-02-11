#include "GUI.h"
#include "CanvasRender.h"
#include "Color.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		UIElementManager::~UIElementManager() 
		{
			Clean();
		}

		void UIElementManager::Init()
		{
			ImageManager::GetInstance()->LoadCustomImage("data/Textures/UISprite.dds", 1, 1);
			ImageManager::GetInstance()->LoadCustomImage("data/Textures/grey.dds", 1, 1);
		}

		void UIElementManager::Clean() 
		{
			
		}

		void UIElementManager::Update()
		{
			CanvasRenderManager* crManager = CanvasRenderManager::GetInstance();
			CanvasRenderManager::GetInstance()->Clean();
			// Iterate all of the Buttons
			for (std::vector<Button>::iterator it = _buttons.begin(); it != _buttons.end(); ++it)
			{
				Button* pBtn = &*it;
				ImageRender* pImageRender = crManager->AddImageRender(pBtn->_pImage, &pBtn->_rectTransform);
				pImageRender->SetImageRect(0);
				TextRender* pTextRender = crManager->AddTextRender(&pBtn->_text, &pBtn->_text._rectTransform);
			}
			// Iterate all of the Sliders
			for (std::vector<Slider>::iterator it = _sliders.begin(); it != _sliders.end(); ++it)
			{
				Slider* pSlider = &*it;
				ImageRender* pImageRender = crManager->AddImageRender(pSlider->_backgroundImage._pImage, &pSlider->_backgroundImage._rectTransform);
				pImageRender->SetImageRect(0);
				pImageRender = crManager->AddImageRender(pSlider->_handleImage._pImage, &pSlider->_handleImage._rectTransform);
				pImageRender->SetImageRect(0);
			}
			// Iterate all of the Toggles
			for (std::vector<Toggle>::iterator it = _toggles.begin(); it != _toggles.end(); ++it)
			{
				Toggle* pToggle = &*it;
				ImageRender* pImageRender = crManager->AddImageRender(pToggle->_backgroundImage._pImage, &pToggle->_backgroundImage._rectTransform);
				pImageRender->SetImageRect(0);
				pImageRender = crManager->AddImageRender(pToggle->_checkMarkImage._pImage, &pToggle->_checkMarkImage._rectTransform);
				pImageRender->SetImageRect(0);
				if (pToggle->_checked)
					pImageRender->SetColor(Color::Red);
				else
					pImageRender->SetColor(Color::White);

			}
			// Iterate all of the Texts
			for (std::vector<Text>::iterator it = _texts.begin(); it != _texts.end(); ++it)
			{
				Text* pText = &*it;
				crManager->AddTextRender(&(*it), &pText->_rectTransform);
			}
		}

		Text* UIElementManager::AddText(std::string value, Common::ITransform* pTrans)
		{
			RectTransform _rectTransform;
			Text text = { value, _rectTransform};
			_texts.push_back(text);
			return &_texts.back();
		}

		Button* UIElementManager::AddButton(BtnCallBack* pCallBack, Common::ITransform* pTrans)
		{
			RectTransform _rectTransform;
			Image* btnImage = ImageManager::GetInstance()->GetImage("UISprite");
			Text text = { "btn", _rectTransform };
			_buttons.push_back({ text, btnImage, pCallBack, _rectTransform });
			return &_buttons.back();
		}

		Slider* UIElementManager::AddSlider(float min, float max, Common::ITransform* pTrans)
		{
			RectTransform _rectTransform;
			Image* pHandleImage = ImageManager::GetInstance()->GetImage("UISprite");
			Image* pBackGroundImage = ImageManager::GetInstance()->GetImage("grey");
			UIImage backgroundimage = { pBackGroundImage, _rectTransform};
			UIImage handleimage = { pHandleImage, _rectTransform };
			_sliders.push_back({ backgroundimage, handleimage, min, max, 1.0f });
			return &_sliders.back();
		}

		Toggle* UIElementManager::AddToggle(bool checked, Common::ITransform* pTrans)
		{
			RectTransform _rectTransform;
			Image* pcheckMarkImage = ImageManager::GetInstance()->GetImage("UISprite");
			Image* pBackGroundImage = ImageManager::GetInstance()->GetImage("grey");
			UIImage backgroundimage = { pBackGroundImage, _rectTransform };
			UIImage checkmarkimage = { pcheckMarkImage, _rectTransform };
			_toggles.push_back({ backgroundimage, checkmarkimage, checked, _rectTransform});
			return &_toggles.back();
		}

	}
}

