#include "Gameplay.h"
#include "Controllers.h"
#include "ColliderCallback.h"
#include "Engine/DebugShape/DebugShape.h"
#include "Engine/CollisionDetection/ColliderBase.h"
#include "Engine/Graphics/Graphics.h"
#include "Engine/Graphics/Common/MeshRender.h"
#include "Engine/Graphics/Common/ImageRender.h"
#include "Engine/Graphics/Common/CanvasRender.h"
#include "Engine/Graphics/Common/Image.h"
#include "Engine/Graphics/Common/Camera.h"
#include "Engine/Graphics/Common/Screen.h"
#include "Engine/Graphics/Common/BinaryFileLoader.h"
#include "Engine/General/BasicShapes.h"
#include "Engine/Common/Interfaces.h"
#include "Engine/Core/Entirety/World.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Engine/Engine.h"
#include "Engine/Graphics/Common/DebugMesh.h"
#include "Engine/Graphics/Common/Text.h"
#include "Engine/Graphics/Common/RectTransform.h"
#include "Engine/Graphics/Common/GUI.h"

namespace 
{
	// variables.
	float _windowWidth = 1200.0f;
	float _windowHeight = 800.0f;
	float _resetCDRemain = 0.0f;
	// function declears
	bool InitLevel();
	bool ResetLevel();

	void CreatePlayer();
	void CreateCamera();
	void CreateSprite();
	void CreateDebugMenu();
}

const char* const pathGround = "data/Meshes/warehouse.aosmesh";
const char* const pathPlayer = "data/Meshes/playerPlane.aosmesh";

EAE_Engine::Collider::Collider* pPlayerCollider = nullptr;

EAE_Engine::Graphics::MeshRender* pRenderGround = nullptr;
EAE_Engine::Common::IGameObj* pActorGround = nullptr;

EAE_Engine::Common::IGameObj* pActorBoard = nullptr;
EAE_Engine::Common::IGameObj* pPlayerObj = nullptr;
EAE_Engine::Common::IGameObj* pCameraObj = nullptr;
EAE_Engine::Common::ICamera* pCamera = nullptr;
CameraController* pCamController = nullptr;
PlayerController* pPlayerController = nullptr;

EAE_Engine::Graphics::ImageRender* pNumberSpriteRender = nullptr;

EAE_Engine::Graphics::Text* pFrameText = nullptr;
EAE_Engine::Graphics::Button* pControlBtn = nullptr;
EAE_Engine::Graphics::Slider* pSlider = nullptr;
EAE_Engine::Graphics::Toggle* pToggle = nullptr;
EAE_Engine::Debug::DebugSphere* pDebugSphere = nullptr;
void btnCallBack(void*) 
{

}

///////////////////////////////////////////////////////////////////////////////

bool GameplayInit(float windowWidth, float windowHeight)
{
	bool result = true;
	result = EAE_Engine::Graphics::LoadMeshFromBinary(pathGround);
	if (!result)
	{
		return false;
	}
	result = EAE_Engine::Graphics::LoadMeshFromBinary(pathPlayer);
	if (!result)
	{
		return false;
	}

//	EAE_Engine::Graphics::LoadMaterial("data/Materials/white.material");
	EAE_Engine::Graphics::LoadMaterial("data/Materials/lambert.material");
	EAE_Engine::Graphics::LoadMaterial("data/Materials/floor.material");
	EAE_Engine::Graphics::LoadMaterial("data/Materials/railing.material");
	EAE_Engine::Graphics::LoadMaterial("data/Materials/ceiling.material");
	EAE_Engine::Graphics::LoadMaterial("data/Materials/metal.material");
	EAE_Engine::Graphics::LoadMaterial("data/Materials/cement.material");
	EAE_Engine::Graphics::LoadMaterial("data/Materials/walls.material");
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	result = InitLevel();


	return true;
}
 
void GameplayUpdate()
{
	if ((_resetLevel || _resetLevelController) && _resetCDRemain <= 0.0f)
	{
		_resetLevelController = false;
		_resetLevel = false;
		ResetLevel();
		_resetCDRemain = 1.0f;
	}
	else
	{
		_resetLevelController = false;
		_resetLevel = false;
		EAE_Engine::Engine::EngineUpdate();
		float elpasedTime = EAE_Engine::Time::GetSecondsElapsedThisFrame();
		_resetCDRemain -= elpasedTime;
		//DebugInformations
		/*
		EAE_Engine::Debug::CleanDebugShapes();
		EAE_Engine::Math::Vector3 yellow(1.0f, 1.0f, 0.0f);
		EAE_Engine::Math::Vector3 red(1.0f, 0.0f, 0.0f);
		EAE_Engine::Math::Vector3 green(0.0f, 1.0f, 0.0f);
		EAE_Engine::Math::Vector3 blue(0.0f, 0.0f, 1.0f);
		{
			// DebugLines
			EAE_Engine::Math::Vector3 start = EAE_Engine::Math::Vector3(0.0f, 0.0f, -10.0f);
			EAE_Engine::Math::Vector3 end(0.0f, 100.0f, -10.0f);
			EAE_Engine::Math::Vector3 end2(100.0f, 0.0f, -10.0f);
			EAE_Engine::Debug::AddSegment(start, end, yellow);
			EAE_Engine::Debug::AddSegment(start, end2, red);
			// DebugBoxes
			{
				EAE_Engine::Math::Vector3 externs(5.0f, 5.0f, 5.0f);
				EAE_Engine::Math::Quaternion rotation = EAE_Engine::Math::Quaternion::Identity;
				EAE_Engine::Math::Vector3 boxPos0 = EAE_Engine::Math::Vector3(0.0f, 0.0f, 0.0f);
				EAE_Engine::Debug::AddBox(externs, boxPos0, rotation, red);
				EAE_Engine::Math::Vector3 externs1(10.0f, 10.0f, 10.0f);
				EAE_Engine::Math::Vector3 boxPos1 = EAE_Engine::Math::Vector3(25.0f, 5.0f, 0.0f);
				EAE_Engine::Debug::AddBox(externs1, boxPos1, rotation, yellow);
			}
			// DebugSpheres
			{
				EAE_Engine::Math::Vector3 start1 = EAE_Engine::Math::Vector3(-5.0f, 0.0f, 0.0f);
				EAE_Engine::Debug::AddSphere(start1, 10.0f, green);
				EAE_Engine::Math::Vector3 start2 = EAE_Engine::Math::Vector3(0.0f, 1.0f, 0.0f);
				EAE_Engine::Debug::AddSphere(start2, 10.0f, blue);
			}
		}
		*/

		char text[20];
		int fps = 1.0f / elpasedTime;
		sprintf_s(text, "FPS:%d", fps);
		pFrameText->_value = text;
		static float radisu = 1.0f;
		pDebugSphere->_radius = pSlider->_handleValue / 10.0f;

		bool hoving = EAE_Engine::Graphics::OnClick(pToggle->_rectTransform);
		pToggle->_checked = hoving;
	}
}

void GameplayExit()
{
	
}

namespace
{
	bool InitLevel()
	{
		EAE_Engine::Math::Vector3 zero = EAE_Engine::Math::Vector3::Zero;
		EAE_Engine::Math::Vector3 boardPos = EAE_Engine::Math::Vector3(0.0f, 0.0f, 3.5f);

		pActorGround = EAE_Engine::Core::World::GetInstance().AddGameObj("ground", zero);
		EAE_Engine::Math::Vector3 groundScale(1.0f, 1.0f, 1.0f);
		pActorGround->GetTransform()->SetLocalScale(groundScale);
		std::vector<std::string> materialList;
		materialList.push_back("lambert");
		materialList.push_back("floor");
		materialList.push_back("railing");
		materialList.push_back("ceiling");
		materialList.push_back("metal");
		materialList.push_back("cement");
		materialList.push_back("walls");
		pRenderGround = EAE_Engine::Graphics::AddMeshRender(pathGround, materialList, pActorGround->GetTransform());

		CreatePlayer();
		CreateCamera();
		CreateSprite();
		CreateDebugMenu();
		return true;
	}


	bool ResetLevel()
	{
		EAE_Engine::Controller::ControllerManager::CleanInstance();
		EAE_Engine::Collider::ColliderManager::CleanInstance();
		EAE_Engine::Graphics::CleanBeforeRestart();
		EAE_Engine::Core::World::CleanInstance();

		bool result = InitLevel();
		return result;
	}


	void CreatePlayer() 
	{
		// Player
		{
			EAE_Engine::Math::Vector3 zero = EAE_Engine::Math::Vector3::Zero;
			pPlayerObj = EAE_Engine::Core::World::GetInstance().AddGameObj("player", zero);
			//std::vector<std::string> materialList;
			//materialList.push_back("white");
			//EAE_Engine::Graphics::AddMeshRender(pathPlayer, materialList, pPlayerObj->GetTransform());
			//Set Collider
			//EAE_Engine::Math::Vector3 playerSize = EAE_Engine::Math::Vector3(0.3f, 1.0f, 0.3f);
			//pPlayerCollider = EAE_Engine::Collider::CreateOBBCollider(pPlayerObj->GetTransform(), playerSize);
			//pPlayerCollider->RegistOnCollideCallback(OnCollideRestart);
			//Set Controller
			pPlayerController = new PlayerController(pPlayerObj->GetTransform());
			EAE_Engine::Controller::ControllerManager::GetInstance().AddController(pPlayerController);
		}
	}

	void CreateCamera() 
	{
		EAE_Engine::Math::Vector3 camera_pos = EAE_Engine::Math::Vector3(0.0f, 3.0f, 5.0f);
		EAE_Engine::Math::Vector3 axis(1.0f, 0.0f, 0.0f);
		float radian = EAE_Engine::Math::ConvertDegreesToRadians(2.0f);
		EAE_Engine::Math::Quaternion camera_rotation(radian, axis);
		//EAE_Engine::Math::Quaternion camera_rotation = EAE_Engine::Math::Quaternion::Identity;
		pCamera = EAE_Engine::Engine::CreateCamera("mainCamera", camera_pos, camera_rotation,
			_windowWidth, _windowHeight);
		pCameraObj = pCamera->GetTransform()->GetGameObj();
		//Camera Controller
		pCamController = new CameraController(pCamera);
		pCamController->SetTarget(pPlayerObj->GetTransform());
		EAE_Engine::Controller::ControllerManager::GetInstance().AddController(pCamController);
	}

	void CreateSprite() 
	{
		/*
		{
			EAE_Engine::Math::Vector3 spritePos = EAE_Engine::Math::Vector3(-0.9f, 0.9f, 0.0f);
			EAE_Engine::Common::IGameObj* pSpriteObj = EAE_Engine::Core::World::GetInstance().AddGameObj("spriteObj", spritePos);
			EAE_Engine::Graphics::Image* pSprite = EAE_Engine::Graphics::ImageManager::GetInstance()->LoadCustomImage("data/Textures/logo.dds", 1, 1);
			EAE_Engine::Graphics::ImageRender* pRender1 = EAE_Engine::Graphics::CanvasRenderManager::GetInstance()->AddImageRender(pSprite, pSpriteObj->GetTransform());
			pRender1->SetAnchor({ 0.5f, 0.5f, 0.5f, 0.5f });
			EAE_Engine::Math::Vector4 screenRect = { 128.0f, 128.0f, 256.0f, 256.0f };
			pRender1->SetImageRect(screenRect, 1);
			//pRender1->SetAnchor({ 0.0f, 1.0f, 0.0f, 1.0f });
			//EAE_Engine::Math::Vector4 screenStretch = { 200.0f, 200.0f, 64.0f, 64.0f };
			//pRender1->SetImageRect(screenStretch, 1);
		}
		{
			EAE_Engine::Math::Vector3 spritePos = EAE_Engine::Math::Vector3(0.9f, 0.9f, 0.0f);
			EAE_Engine::Common::IGameObj* pSpriteObj = EAE_Engine::Core::World::GetInstance().AddGameObj("spriteObj2", spritePos);
			EAE_Engine::Graphics::Image* pSprite = EAE_Engine::Graphics::ImageManager::GetInstance()->LoadCustomImage("data/Textures/numbers.dds", 1, 10);
			pNumberSpriteRender = EAE_Engine::Graphics::CanvasRenderManager::GetInstance()->AddImageRender(pSprite, pSpriteObj->GetTransform());
		}
		*/
	}

	void CreateDebugMenu()
	{
		// Add debugSphere.
		EAE_Engine::Math::Vector3 green(0.0f, 1.0f, 0.0f);
		EAE_Engine::Math::Vector3 start1 = EAE_Engine::Math::Vector3(-5.0f, 0.0f, 0.0f);
		pDebugSphere = EAE_Engine::Debug::AddSphere(start1, 10.0f, green);
		// Add widgets
		EAE_Engine::Math::Vector3 textPos = EAE_Engine::Math::Vector3::Zero;
		EAE_Engine::Common::IGameObj* pTextObj = EAE_Engine::Core::World::GetInstance().AddGameObj("textObj", textPos);
		{
			pFrameText = EAE_Engine::Graphics::UIElementManager::GetInstance()->AddText("test", pTextObj->GetTransform());
			pFrameText->_rectTransform.SetAnchor({ 0.0f, 0.0f, 1.0f, 1.0f });
			pFrameText->_rectTransform.SetRect({ 32.0f, -32.0f, 64.0f, 64.0f });
		}
		EAE_Engine::Common::IGameObj* pBtnObj = EAE_Engine::Core::World::GetInstance().AddGameObj("btnObj", textPos);
		{
			pControlBtn = EAE_Engine::Graphics::UIElementManager::GetInstance()->AddButton(btnCallBack, pBtnObj->GetTransform());
			pControlBtn->_rectTransform.SetAnchor({ 0.0f, 0.0f, 1.0f, 1.0f });
			pControlBtn->_rectTransform.SetRect({ 32.0f, -64.0f, 64.0f, 64.0f });
			pControlBtn->_backgroundImage._rectTransform.SetAnchor({ 0.0f, 0.0f, 1.0f, 1.0f });
			pControlBtn->_backgroundImage._rectTransform.SetRect({ 32.0f, -64.0f, 64.0f, 64.0f });
			pControlBtn->_text._rectTransform.SetAnchor({ 0.0f, 0.0f, 1.0f, 1.0f });
			pControlBtn->_text._rectTransform.SetRect({ 32.0f, -64.0f, 64.0f, 64.0f });
		}
		EAE_Engine::Common::IGameObj* pSliderObj = EAE_Engine::Core::World::GetInstance().AddGameObj("sliderObj", textPos);
		{
			pSlider = EAE_Engine::Graphics::UIElementManager::GetInstance()->AddSlider(50.0f, 200.0f, pSliderObj->GetTransform());
			pSlider->_rectTransform.SetAnchor({ 0.0f, 0.0f, 1.0f, 1.0f });
			pSlider->_rectTransform.SetRect({ 64.0f, -128.0f, 64.0f, 64.0f });
			pSlider->_backgroundImage._rectTransform.SetAnchor({ 0.0f, 0.0f, 1.0f, 1.0f });
			pSlider->_backgroundImage._rectTransform.SetRect({ 64.0f, -128.0f, 128.0f, 16.0f });
			pSlider->SetHandle(50.0f);
		}
		EAE_Engine::Common::IGameObj* pToggleObj = EAE_Engine::Core::World::GetInstance().AddGameObj("toggleObj", textPos);
		{
			pToggle = EAE_Engine::Graphics::UIElementManager::GetInstance()->AddToggle(true, pToggleObj->GetTransform());
			pToggle->_rectTransform.SetAnchor({ 0.0f, 0.0f, 1.0f, 1.0f });
			pToggle->_rectTransform.SetRect({ 16.0f, -168.0f, 16.0f, 16.0f });
			pToggle->_backgroundImage._rectTransform.SetAnchor({ 0.0f, 0.0f, 1.0f, 1.0f });
			pToggle->_backgroundImage._rectTransform.SetRect({ 16.0f, -168.0f, 16.0f, 16.0f });
			pToggle->_checked = false;
			//pToggle->Check(true);
		}
	}

}

