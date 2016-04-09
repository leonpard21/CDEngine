#ifndef EAE_GAME_NETWORKING_H
#define EAE_GAME_NETWORKING_H

#include "Engine/General/Singleton.hpp"
#include "Engine/Common/Interfaces.h"
#include <string>
#include <vector>
#include "RakPeerInterface.h"


class NetworkPeer : public EAE_Engine::Singleton<NetworkPeer>
{
public:
  NetworkPeer();
  ~NetworkPeer();
  void Init( bool isServer, std::string ipaddress);
  void Update(EAE_Engine::Common::ITransform* pLocalPlayer);

private:
  RakNet::RakPeerInterface* _peer;
  bool _isServer;
  std::vector<RakNet::RakPeerInterface*> _clients;
};


#endif//EAE_GAME_NETWORKING_H