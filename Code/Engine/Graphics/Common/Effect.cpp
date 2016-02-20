#include "Effect.h"

namespace EAE_Engine
{
	namespace Graphics
	{

//////////////////////////////////////EffectManager//////////////////////////////////
		EffectsManager* EffectsManager::s_pEffectsManager = nullptr;

		EffectsManager* EffectsManager::GetInstance()
		{
			if (!s_pEffectsManager)
				s_pEffectsManager = new EffectsManager();
			return s_pEffectsManager;
		}

		void EffectsManager::CleanInstance()
		{
			if (!s_pEffectsManager) return;

			for (std::map<const char*, Effect*>::const_iterator iter = s_pEffectsManager->_effectManager.begin(); iter != s_pEffectsManager->_effectManager.end();)
			{
				char* pKey = const_cast<char*>(iter->first);
				SAFE_DELETE(pKey);
				Effect* pEffect = (iter++)->second;
				pEffect->Release();
				SAFE_DELETE(pEffect);
			}
			s_pEffectsManager->_effectManager.clear();
			if (s_pEffectsManager)
				delete s_pEffectsManager;
			s_pEffectsManager = nullptr;
		}

		Effect* EffectsManager::GetEffect(const char* pKey)
		{
			for (std::map<const char*, Effect*>::const_iterator iter = s_pEffectsManager->_effectManager.begin(); iter != s_pEffectsManager->_effectManager.end(); ++iter)
			{
				if (strcmp(iter->first, pKey) == 0)
				{
					return iter->second;
				}
			}
			return nullptr;
		}

		Effect* EffectsManager::CreateEffect(const char*pKey, const char* i_pVSfilePath, const char* i_pFSfilePath, uint32_t renderState)
		{
			Effect* pEffect = new Effect();
			bool result = pEffect->Init(i_pVSfilePath, i_pFSfilePath, renderState);
			if (!result)
			{
				SAFE_DELETE(pEffect);
				return nullptr;
			}
			_effectManager.insert(std::pair<const char*, Effect*>(_strdup(pKey), pEffect));
			return pEffect;
		}

		void Effect::OnNotify(UniformBlock* pBlock)
		{
			_updateBlockList[pBlock->GetName()] = pBlock;
		}

	}
}



