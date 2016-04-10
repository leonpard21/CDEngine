#ifndef EAE_GAME_NETWORKING_H
#define EAE_GAME_NETWORKING_H



#include "Engine/General/Singleton.hpp"
#include "Engine/Common/Interfaces.h"
#include <string>
#include <vector>
#include "RakPeerInterface.h"
//#include "NetworkIDObject.h"

#include "Engine/Math/Vector.h"
#include "Engine/Math/Quaternion.h"




class NetworkPeer : public EAE_Engine::Singleton<NetworkPeer>
{
public:
  NetworkPeer();
  ~NetworkPeer();
  void Init( bool isServer, std::string ipaddress);
  void Update(EAE_Engine::Common::ITransform* pLocalPlayer);
  void DisConnect();


private:
  RakNet::RakPeerInterface* _peer;
  bool _isServer;
  std::vector<RakNet::RakNetGUID> _clients;
  RakNet::SystemAddress _serverAddress;
};


#endif//EAE_GAME_NETWORKING_H