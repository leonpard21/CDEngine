#ifndef GAMEPLAY_H
#define GAMEPLAY_H
#include "Engine/Windows/WindowsFunctions.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"

extern bool GameplayInit(float, float);

extern void GameplayUpdate();

extern void GameplayExit();

void CreateOtherPlayer(const char* pname, EAE_Engine::Math::Vector3 pos, EAE_Engine::Math::Quaternion rotation);



#endif//GAMEPLAY_H