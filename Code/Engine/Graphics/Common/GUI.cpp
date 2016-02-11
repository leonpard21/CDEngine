#include "GUI.h"
#include "CanvasRender.h"

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
		}

		void UIElementManager::Clean() 
		{
			
		}

		void UIElementManager::Update()
		{
			CanvasRenderManager* crManager = CanvasRenderManager::GetInstance();
			CanvasRenderManager::GetInstance()->Clean();
			// Iterate all of the Button
			for (std::vector<Button>::iterator it = _buttons.begin(); it != _buttons.end(); ++it)
			{
				Button* pBtn = &*it;
				ImageRender* pImageRender = crManager->AddImageRender(pBtn->_pImage, &pBtn->_rectTransform);
				pImageRender->SetImageRect(0);
				TextRender* pTextRender = crManager->AddTextRender(&pBtn->_text, &pBtn->_text._rectTransform);
			}
			// Iterate all of the Text
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
	}
}

