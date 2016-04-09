#include "Networking.h"

#include <stdio.h>
#include <string.h>
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakNetTypes.h"  // MessageID
#include "NetworkIDManager.h"

#include "Engine/Common/Interfaces.h"
#include "Engine/Core/Entirety/World.h"
#include "Gameplay.h"

#pragma pack(push, 1)
struct PlayerTransform
{
  PlayerTransform() {}
  //  unsigned char useTimeStamp; // Assign ID_TIMESTAMP to this
  //  RakNet::Time timeStamp; // Put the system time in here returned by RakNet::GetTime() or some other method that returns a similar value
  unsigned char _dataTypeID;
  union
  {
    float _data[7];
    struct
    {
      float x, y, z;
      float p, q, r, s;
    };
    struct
    {
      EAE_Engine::Math::Vector3 _pos;
      EAE_Engine::Math::Quaternion _rotation;
    };
  };
  RakNet::RakNetGUID _networkGUID; // NetworkID of the player, used as a common method to refer to the mine on different computers
};
#pragma pack(pop)

// Define our custom packet ID's
enum GameMessages
{
  ID_GAME_GET_TRANSFORM = ID_USER_PACKET_ENUM + 1,
  ID_GAME_NEW_PLAYER,

};

#define MAX_CLIENTS 10
#define SERVER_PORT 60000


NetworkPeer::NetworkPeer() : 
  _peer(nullptr), _isServer(false)
{
}

NetworkPeer::~NetworkPeer()
{
  if (_peer != nullptr) 
  {
    _peer->Shutdown(300);
    RakNet::RakPeerInterface::DestroyInstance(_peer);
  }
}

void NetworkPeer::Init(bool isServer, std::string ipaddress)
{
  _serverAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
  _peer = RakNet::RakPeerInterface::GetInstance();
  _isServer = isServer;
  if (!isServer)
  {
    RakNet::SocketDescriptor sd;
    _peer->Startup(1, &sd, 1);
  }
  else 
  {
    RakNet::SocketDescriptor sd(SERVER_PORT, 0);
    _peer->Startup(MAX_CLIENTS, &sd, 1);
  }

  if (isServer)
  {
    printf("Starting the server.\n");
    // We need to let the server accept incoming connections from the clients
    _peer->SetMaximumIncomingConnections(MAX_CLIENTS);
  }
  else 
  {
    printf("Starting the client.\n");
    if (ipaddress == "") 
    {
      _peer->Connect("127.0.0.1", SERVER_PORT, 0, 0);
    }
    else 
    {
      _peer->Connect(ipaddress.c_str(), SERVER_PORT, 0, 0);
    }
  }

}



void NetworkPeer::Update(EAE_Engine::Common::ITransform* pLocalPlayer)
{
  RakNet::Packet* packet = nullptr;
  for (packet = _peer->Receive(); packet; _peer->DeallocatePacket(packet), packet = _peer->Receive())
  {
    switch (packet->data[0])
    {
    case ID_REMOTE_DISCONNECTION_NOTIFICATION:
      printf("Another client has disconnected.\n");
      break;
    case ID_REMOTE_CONNECTION_LOST:
      printf("Another client has lost the connection.\n");
      break;
    case ID_REMOTE_NEW_INCOMING_CONNECTION:
      printf("Another client has connected.\n");
      break;
    case ID_CONNECTION_REQUEST_ACCEPTED:
    {
      printf("Our connection request has been accepted.\n");
      _serverAddress = packet->systemAddress;
    }
    break;
    case ID_NEW_INCOMING_CONNECTION:
    {
      printf("A connection is incoming.\n");
      RakNet::RakNetGUID id = _peer->GetGuidFromSystemAddress(packet->systemAddress);
      _clients.push_back(id);
    }
    break;
    case ID_NO_FREE_INCOMING_CONNECTIONS:
      printf("The server is full.\n");
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      if (_isServer) {
        printf("A client has disconnected.\n");
      }
      else {
        printf("We have been disconnected.\n");
      }
      break;
    case ID_CONNECTION_LOST:
      if (_isServer) {
        printf("A client lost the connection.\n");
      }
      else {
        printf("Connection lost.\n");
      }
      break;
    case ID_GAME_GET_TRANSFORM:
    {
      PlayerTransform* pTransofrm = (PlayerTransform*)packet->data;
      const char* pGUID = pTransofrm->_networkGUID.ToString();
      EAE_Engine::Common::IGameObj* pClientPlayer = EAE_Engine::Core::World::GetInstance().GetGameObj(pGUID);
      if (!pClientPlayer)
      {
        CreateOtherPlayer(pGUID, pTransofrm->_pos, pTransofrm->_rotation);
        EAE_Engine::Common::IGameObj* pClientPlayer2 = EAE_Engine::Core::World::GetInstance().GetGameObj(pGUID);
        size_t t = 0;
      }
      else
      {
        pClientPlayer->GetTransform()->SetPos(pTransofrm->_pos);
        pClientPlayer->GetTransform()->SetRotation(pTransofrm->_rotation);
      }
    }
    break;

    default:
      printf("Message with identifier %i has arrived.\n", packet->data[0]);
      break;
    }
  }
  
  if (_isServer)
  {
    // send all transform information of each player
    for (auto outter : _clients)
    {
      const char* pOutterName = outter.ToString();
      EAE_Engine::Common::IGameObj* pClientPlayer = EAE_Engine::Core::World::GetInstance().GetGameObj(pOutterName);
      if (!pClientPlayer)
        continue;
      // 1. send the information of server to this player
      {
        PlayerTransform localTransform;
        localTransform._dataTypeID = ID_GAME_GET_TRANSFORM;
        localTransform._pos = pLocalPlayer->GetPos();
        localTransform._rotation = pLocalPlayer->GetRotation();
        localTransform._networkGUID = _peer->GetMyGUID();
        _peer->Send((char*)&localTransform, sizeof(PlayerTransform), HIGH_PRIORITY, RELIABLE_ORDERED, 0, _peer->GetSystemAddressFromGuid(outter), false);
      }
      // 2. send the information of each player to all of the players
      PlayerTransform transform;
      {
        transform._dataTypeID = ID_GAME_GET_TRANSFORM;
        transform._pos = pClientPlayer->GetTransform()->GetPos();
        transform._rotation = pClientPlayer->GetTransform()->GetRotation();
        transform._networkGUID = outter;
      };
      for (auto inner : _clients)
      {
        // don't send to itself
        if (outter == inner)
          continue;
        _peer->Send((char*)&transform, sizeof(PlayerTransform), HIGH_PRIORITY, RELIABLE_ORDERED, 0, _peer->GetSystemAddressFromGuid(inner), false);
      }
    }
  }
  else if(_serverAddress != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
  {
    // Use asending casted structures
    PlayerTransform transform;
    {
      transform._dataTypeID = ID_GAME_GET_TRANSFORM;
      transform._pos = pLocalPlayer->GetPos();
      transform._rotation = pLocalPlayer->GetRotation();
      transform._networkGUID = _peer->GetMyGUID();
    };
    // send local data to server
    _peer->Send((char*)&transform, sizeof(PlayerTransform), HIGH_PRIORITY, RELIABLE_ORDERED, 0, _serverAddress, false);
  }
}


void NetworkPeer::RemoveClient(RakNet::RakNetGUID id)
{

}



