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
#include "Engine/CollisionDetection/RigidBody.h"
#include "Engine/CollisionDetection/MeshCollider.h"
#include "Engine/SpatialPartition/Octree.h"

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
const char* const pathPlayer = "data/Meshes/sphere.aosmesh";
const char* const pathCollisionData = "data/Meshes/collisionData.aosmesh";
EAE_Engine::Collider::Collider* pPlayerCollider = nullptr;
EAE_Engine::Physics::RigidBody* pRigidBody = nullptr;

EAE_Engine::Graphics::AOSMeshRender* pRenderGround = nullptr;
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
void btnCallBack(void*) 
{
	if (pSlider)
		pSlider->_handleValue = 20.0f;
}

EAE_Engine::Core::CompleteOctree* g_pCompleteOctree = nullptr;

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
	result = EAE_Engine::Graphics::LoadMeshFromBinary(pathCollisionData);
	if (!result)
	{
		return false;
	}
	EAE_Engine::Graphics::LoadMaterial("data/Materials/white.material");
	EAE_Engine::Graphics::LoadMaterial("data/Materials/phongShading.material");
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

	g_pCompleteOctree = new EAE_Engine::Core::CompleteOctree();
	g_pCompleteOctree->InitFromFile("data/Scene/CollisionOctree.octree");
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
		float elpasedTime = EAE_Engine::Time::GetSecondsElapsedThisFrame();
		_resetCDRemain -= elpasedTime;
		//DebugInformations
		
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
			// DebugSpheres
			{
				//EAE_Engine::Math::Vector3 start1 = EAE_Engine::Math::Vector3(-5.0f, 0.0f, 0.0f);
				//EAE_Engine::Debug::AddSphere(start1, 10.0f, green);
				//EAE_Engine::Math::Vector3 start2 = EAE_Engine::Math::Vector3(0.0f, 1.0f, 0.0f);
				//EAE_Engine::Debug::AddSphere(start2, 10.0f, blue);
			}
		}
		
		if (EAE_Engine::UserInput::Input::GetInstance()->GetKeyState('C') == EAE_Engine::UserInput::KeyState::OnPressed)
		{
			bool playerControllerState = pPlayerController->IsActive();
			pPlayerController->SetActive(!playerControllerState);
			pCamController->SetActive(playerControllerState);
			if (!playerControllerState)
			{
				pCamController->GetTransform()->SetLocalRotation(EAE_Engine::Math::Quaternion::Identity);
				pCamController->ResetCamera(pPlayerObj->GetTransform());
				pCamController->GetTransform()->SetParent(pPlayerObj->GetTransform());
			}
			else 
			{
				pCamController->GetTransform()->SetParent(nullptr);
			}
		}

		char text[20];
		float fps = 1.0f / elpasedTime;
		sprintf_s(text, "FPS:%.2f", fps);
		pFrameText->_value = text;
		static float radisu = 1.0f;
		//EAE_Engine::Debug::CleanDebugShapes();
		if (pToggle->_checked)
		{
			EAE_Engine::Math::Quaternion rotation = EAE_Engine::Math::Quaternion::Identity;
			uint32_t levelIndex = 3;
			uint32_t countOfNodes = g_pCompleteOctree->GetCountOfNodesInLevel(levelIndex);
			EAE_Engine::Core::OctreeNode* pNodes = g_pCompleteOctree->GetNodesInLevel(levelIndex);
			for (uint32_t index = 0; index < countOfNodes; ++index)
			{
				EAE_Engine::Debug::DebugShapes::GetInstance().AddBox(pNodes[index]._extent, pNodes[index]._pos, rotation, red);
			}
		}
	}
}

void GameplayExit()
{
	SAFE_DELETE(g_pCompleteOctree);
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

		std::vector<std::string> materialList2;
		materialList2.push_back("lambert");
		EAE_Engine::Common::IGameObj* pCollisionDataObj = EAE_Engine::Core::World::GetInstance().AddGameObj("collisionData", zero);
	//	EAE_Engine::Graphics::AddMeshRender(pathCollisionData, materialList2, pCollisionDataObj->GetTransform());
		EAE_Engine::Collider::MeshCollider* pMeshCollider = new EAE_Engine::Collider::MeshCollider(pCollisionDataObj->GetTransform());
		pMeshCollider->Init("collisionData");
		EAE_Engine::Collider::ColliderManager::GetInstance()->AddToColliderList(pMeshCollider);
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
			EAE_Engine::Math::Vector3 playerinitPos(0.0f, 0.0f, 0.0f);
			pPlayerObj = EAE_Engine::Core::World::GetInstance().AddGameObj("player", playerinitPos);
			EAE_Engine::Math::Vector3 axis(0.0f, 1.0f, 0.0f);
			float radian = EAE_Engine::Math::ConvertDegreesToRadians(20.0f);
			EAE_Engine::Math::Quaternion player_rotation(radian, axis);
			pPlayerObj->GetTransform()->SetRotation(player_rotation);
			std::vector<std::string> materialList;
			materialList.push_back("phongShading");
			EAE_Engine::Graphics::AddMeshRender(pathPlayer, materialList, pPlayerObj->GetTransform());
			pRigidBody = EAE_Engine::Physics::Physics::GetInstance()->AddRigidBody(pPlayerObj->GetTransform());
			pPlayerObj->AddComponent({ pRigidBody, pRigidBody->GetTypeID() });
			EAE_Engine::Common::ICompo* pCompo = pPlayerObj->GetTransform()->GetComponent(getTypeID<EAE_Engine::Physics::RigidBody>());
			EAE_Engine::Physics::RigidBody* pRBA = dynamic_cast<EAE_Engine::Physics::RigidBody*>(pCompo);
			
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
		EAE_Engine::Math::Vector3 camera_pos = EAE_Engine::Math::Vector3(0.0f, 0.0f, 0.0f);
		EAE_Engine::Math::Vector3 axis(1.0f, 0.0f, 0.0f);
		float radian = EAE_Engine::Math::ConvertDegreesToRadians(0.0f);
		EAE_Engine::Math::Quaternion camera_rotation(radian, axis);
		//EAE_Engine::Math::Quaternion camera_rotation = EAE_Engine::Math::Quaternion::Identity;
		pCamera = EAE_Engine::Engine::CreateCamera("mainCamera", camera_pos, camera_rotation,
			_windowWidth, _windowHeight);
		pCameraObj = pCamera->GetTransform()->GetGameObj();
		pCamera->GetTransform()->SetParent(pPlayerObj->GetTransform());
		//Camera Controller
		pCamController = new CameraController(pCamera);
		pCamController->ResetCamera(pPlayerObj->GetTransform());
		pCamController->SetActive(false);
		EAE_Engine::Controller::ControllerManager::GetInstance().AddController(pCamController);
	}

	void CreateSprite() 
	{
	}

	void CreateDebugMenu()
	{
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
		}
	}

}

