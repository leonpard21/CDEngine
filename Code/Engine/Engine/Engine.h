#ifndef EAEENGINE_Engine_H
#define EAEENGINE_Engine_H

#include "EngineInclude.h"

namespace EAE_Engine
{
	namespace Math
	{
		template <typename T>
		class TVector3;
		typedef TVector3<float> Vector3;
		class Quaternion;
	}

	namespace Common
	{
		class IGameObj;
		class ICamera;
		class ITransform;
	}

	namespace Engine 
	{
		bool EngineBegin(const HWND i_renderingWindow);
		void EngineUpdate();
		void EngineRender();
		void EngineEnd();
		
		void AddToRemoveList(Common::ITransform* pTrans);
		Common::ICamera* CreateCamera(const char* pCamName, Math::Vector3& pos, Math::Quaternion& rotation,
			float windowWidth, float windowHeight);
	}
};

#endif//EAEENGINE_Engine_H

