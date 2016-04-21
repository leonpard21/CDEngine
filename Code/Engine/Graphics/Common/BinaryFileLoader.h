#ifndef EAE_ENGINE_BINARYFILELOADER_H
#define EAE_ENGINE_BINARYFILELOADER_H

namespace EAE_Engine
{
  namespace Mesh
  {
    struct AOSMeshData;
  }

	namespace Graphics
	{
		class Effect;
		Effect* LoadEffectData(const char* i_pBinaryEffectFile);
		class AOSMesh;
		AOSMesh* LoadMeshData(const char* i_pBinaryMeshFile);
    AOSMesh* CreateAOSMesh(Mesh::AOSMeshData* pAOSMeshData);
		class ShaderContent;
		ShaderContent* LoadShader(const char* i_pBinaryShaderFilePath);
		
		void LoadMaterial(const char* i_pBinaryMaterialFile);
	}
}

#endif//EAE_ENGINE_BINARYFILELOADER_H