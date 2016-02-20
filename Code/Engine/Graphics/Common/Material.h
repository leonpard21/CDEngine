#ifndef EAE_ENGINE_MATERIALDESC_H
#define EAE_ENGINE_MATERIALDESC_H

#include <stdint.h>
#include <vector>
#include "Texture.h"

namespace EAE_Engine 
{
	namespace Graphics
	{			
		/* For whatever reasons, the material will have different cost on rendering.
		 * it can be effect it uses, it can be transparent or not. It can be anything...
		 * So I use a materialCost for each material so that we can sort the materials by it.
		 * Idealy, this cost will be calculated in the editor or by the effect.
		 * then we just return the cost value wherever we need.
		 */
		union MaterialCost
		{
			uint32_t _cost : 24;
			struct
			{
				uint32_t _effectCost : 12;
				uint32_t _renderingMode : 2;
			};
			MaterialCost() : _cost(0) {}
			MaterialCost(uint32_t materialCost) : _cost(materialCost){}
		};

		class Effect;
		struct UniformBlockDesc;
		struct UniformDesc;
		struct TextureDesc;
		struct MaterialDesc
		{
			// The Material should know what's the effect,
			// what's the parameters for the effect.
			Effect* _pEffect;
			uint32_t _sizeOfMaterialBuffer;// size of this material. 
			uint32_t _uniformBlockCount;
			uint32_t _offsetOfUniformBlockNameBuffer;
			uint32_t _uniformCount;// How many uniform variables we have to set in this material
			uint32_t _offsetOfUniformVariableValueBuffer;// offset to the Uniform Variable Value buffer
			uint32_t _offsetOfUniformVariableNameBuffer;// offset to the Uniform Variable Name buffer
			uint32_t _textureCount;// How many texture we use for this material
			uint32_t _offsetOfTexturePathBuffer;// offset to the TexturePath buffer
			uint32_t _offsetOfTextureSamplerBuffer;// offset to the Texture Sampler Buffer(sampler is the uniform variable)
			// Material Rendering Cost Information
			MaterialCost _materialCost;
			
			// Member functions:
			MaterialDesc() : _pEffect(nullptr), _sizeOfMaterialBuffer(0), 
				_uniformBlockCount(0), _offsetOfUniformBlockNameBuffer(0),
				_uniformCount(0), _offsetOfUniformVariableValueBuffer(0), _offsetOfUniformVariableNameBuffer(0), 
				_textureCount(0), _offsetOfTexturePathBuffer(0), _offsetOfTextureSamplerBuffer(0){}
			UniformBlockDesc* GetUniformBlockDesc();
			UniformDesc* GetUniformDesc();
			TextureDesc* GetTextureDesc();
			void SetUniformForEffect();
			void SetTexturesForEffect();
			void ChangeTexture(uint32_t index, TextureInfo* pTextureInfo);
		};

		struct MaterialKeyValuePair 
		{
			const char* _pKey;
			uint8_t* _pMaterialDesc;
			MaterialKeyValuePair() = default;
		};

		class MaterialManager 
		{
		public:
			void AddMaterial(const char* key, uint8_t* pValue);
			void Clean();
			MaterialDesc* GetMaterial(const char* key);

		public:
			static MaterialManager* GetInstance();
			static void CleanInstance();
		private:
			std::vector<MaterialKeyValuePair> _materials;
			static MaterialManager* s_pMaterialManager;
		};
	}
}

#endif//EAE_ENGINE_MATERIALDESC_H