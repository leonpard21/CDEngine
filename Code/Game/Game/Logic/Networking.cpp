#include "Networking.h"

#include <stdio.h>
#include <string.h>
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakNetTypes.h"  // MessageID
#include "NetworkIDManager.h"

#include "Engine/Common/Interfaces.h"
#include "Engine/Core/Entirety/World.h"
#include "Engine/Engine/Engine.h"
#include "Gameplay.h"
#include "FlagController.h"

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
  int _score;
  EAE_Engine::Math::Vector3 _flag0Pos;
  EAE_Engine::Math::Vector3 _flag1Pos;
  RakNet::RakNetGUID _networkGUID; // NetworkID of the player, used as a common method to refer to the mine on different computers
};

struct NewPlayer
{
  NewPlayer() {}
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
  RakNet::RakNetGUID _networkGUID;
};

struct RemovePlayer
{
  RemovePlayer() {}
  //  unsigned char useTimeStamp; // Assign ID_TIMESTAMP to this
  //  RakNet::Time timeStamp; // Put the system time in here returned by RakNet::GetTime() or some other method that returns a similar value
  unsigned char _dataTypeID;
  RakNet::RakNetGUID _networkGUID;
};
#pragma pack(pop)

// Define our custom packet ID's
enum GameMessages
{
  ID_GAME_GET_TRANSFORM = ID_USER_PACKET_ENUM + 1,
  ID_GAME_NEW_PLAYER,
  ID_GAME_REMOVE_PLAYER,
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
      if (!_isServer) 
      {
        // send the server the package to create this client.
        NewPlayer newPlayer;
        newPlayer._dataTypeID = ID_GAME_NEW_PLAYER;
        newPlayer._pos = pLocalPlayer->GetPos();
        newPlayer._rotation = pLocalPlayer->GetRotation();
        newPlayer._networkGUID = _peer->GetMyGUID();
        _peer->Send((char*)&newPlayer, sizeof(NewPlayer), HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, false);
      }
    }
    break;
    case ID_NEW_INCOMING_CONNECTION:
      printf("A connection is incoming.\n");
      if (_isServer) 
      {
        // 1. send server data to the new client
        NewPlayer serverPlayer;
        serverPlayer._dataTypeID = ID_GAME_NEW_PLAYER;
        serverPlayer._pos = pLocalPlayer->GetPos();
        serverPlayer._rotation = pLocalPlayer->GetRotation();
        serverPlayer._networkGUID = _peer->GetMyGUID();
        _peer->Send((char*)&serverPlayer, sizeof(NewPlayer), HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, false);
        // 2. send data of all clients on server to the new client.
        for (auto clientID : _clients)
        {
          EAE_Engine::Common::IGameObj* pClientPlayer = EAE_Engine::Core::World::GetInstance().GetGameObj(clientID.ToString());
          if (!pClientPlayer)
            continue;
          NewPlayer clientPlayer;
          clientPlayer._dataTypeID = ID_GAME_NEW_PLAYER;
          clientPlayer._pos = pClientPlayer->GetTransform()->GetPos();
          clientPlayer._rotation = pClientPlayer->GetTransform()->GetRotation();
          clientPlayer._networkGUID = clientID;
          _peer->Send((char*)&clientPlayer, sizeof(NewPlayer), HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, false);
        }
      }
      break;
    case ID_NO_FREE_INCOMING_CONNECTIONS:
      printf("The server is full.\n");
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      if (_isServer) 
      {
        printf("A client has disconnected.\n");
        RakNet::RakNetGUID id = packet->guid;
        EAE_Engine::Common::IGameObj* pClientPlayer = EAE_Engine::Core::World::GetInstance().GetGameObj(id.ToString());
        if (pClientPlayer)
        {
          if (_isServer)
          {
            FlagController* pController = (FlagController*)EAE_Engine::Core::World::GetInstance().GetGameObj("flag0")->GetComponent(getTypeID<FlagController>());
            if (pController)
              pController->SetTarget(nullptr);
          }
          //EAE_Engine::Core::World::GetInstance().Remove(pClientPlayer->GetTransform());
          EAE_Engine::Engine::AddToRemoveList(pClientPlayer->GetTransform());
        }
        // notice all other clients to remove the lost connect one
        for (auto clientID : _clients)
        {
          RemovePlayer removePlayer;
          removePlayer._dataTypeID = ID_GAME_REMOVE_PLAYER;
          removePlayer._networkGUID = id;
          _peer->Send((char*)&removePlayer, sizeof(RemovePlayer), HIGH_PRIORITY, RELIABLE, 0, _peer->GetSystemAddressFromGuid(clientID), false);
        }
      }
      else
      {
        printf("We have been disconnected.\n");
      }
      break;
    case ID_CONNECTION_LOST:
      if (_isServer)
      {
        printf("A client lost the connection.\n");
        RakNet::RakNetGUID id = packet->guid;
        EAE_Engine::Common::IGameObj* pClientPlayer = EAE_Engine::Core::World::GetInstance().GetGameObj(id.ToString());
        if (pClientPlayer)
        {
          //EAE_Engine::Core::World::GetInstance().Remove(pClientPlayer->GetTransform());
          if (_isServer)
          {
            FlagController* pController = (FlagController*)EAE_Engine::Core::World::GetInstance().GetGameObj("flag0")->GetComponent(getTypeID<FlagController>());
            if (pController)
              pController->SetTarget(nullptr);
          }
          EAE_Engine::Engine::AddToRemoveList(pClientPlayer->GetTransform());
        }
        // notice all other clients to remove the lost connect one
        for(auto clientID: _clients) 
        {
          RemovePlayer removePlayer;
          removePlayer._dataTypeID = ID_GAME_REMOVE_PLAYER;
          removePlayer._networkGUID = id;
          _peer->Send((char*)&removePlayer, sizeof(RemovePlayer), HIGH_PRIORITY, RELIABLE, 0, _peer->GetSystemAddressFromGuid(clientID), false);
        }
      }
      else 
      {
        printf("Connection lost.\n");
      }
      break;
    case ID_GAME_GET_TRANSFORM:
    {
      PlayerTransform* pTransofrm = (PlayerTransform*)packet->data;
      const char* pGUID = pTransofrm->_networkGUID.ToString();
      EAE_Engine::Common::IGameObj* pClientPlayer = EAE_Engine::Core::World::GetInstance().GetGameObj(pGUID);
      if (pClientPlayer)
      {
        pClientPlayer->GetTransform()->SetPos(pTransofrm->_pos);
        pClientPlayer->GetTransform()->SetRotation(pTransofrm->_rotation);
      }
      // for each client, get the position of the flag. 
      if (!_isServer && pTransofrm->_networkGUID != _peer->GetMyGUID())
      {
        EAE_Engine::Core::World::GetInstance().GetGameObj("flag0")->GetTransform()->SetPos(pTransofrm->_flag0Pos);
        EAE_Engine::Core::World::GetInstance().GetGameObj("flag1")->GetTransform()->SetPos(pTransofrm->_flag1Pos);
      }
    }
    break;
    case ID_GAME_NEW_PLAYER:
    {
      NewPlayer* pNewplayer = (NewPlayer*)packet->data;
      RakNet::RakNetGUID id;
      if (_isServer)
      {
        // calculate the new GUID
        id = _peer->GetGuidFromSystemAddress(packet->systemAddress);
        pNewplayer->_networkGUID = id;
        // send the info of new client to all perivous clients.
        for (auto clientID : _clients)
        {
          _peer->Send((char*)pNewplayer, sizeof(NewPlayer), HIGH_PRIORITY, RELIABLE, 0, _peer->GetSystemAddressFromGuid(clientID), false);
        }
      }
      id = pNewplayer->_networkGUID;
      _clients.push_back(id);
      CreateOtherPlayer(id.ToString(), pNewplayer->_pos, pNewplayer->_rotation);
    }
    break;
    case ID_GAME_REMOVE_PLAYER:
    {
      RemovePlayer* pRemovePlayer = (RemovePlayer*)packet->data;
      EAE_Engine::Common::IGameObj* pClientPlayer = EAE_Engine::Core::World::GetInstance().GetGameObj(pRemovePlayer->_networkGUID.ToString());
      if (pClientPlayer)
      {
        if (_isServer)
        {
          FlagController* pController = (FlagController*)EAE_Engine::Core::World::GetInstance().GetGameObj("flag0")->GetComponent(getTypeID<FlagController>());
          if (pController)
            pController->SetTarget(nullptr);
        }
        EAE_Engine::Engine::AddToRemoveList(pClientPlayer->GetTransform());
      }
    }
    break;
    default:
      printf("Message with identifier %i has arrived.\n", packet->data[0]);
      break;
    }
  }
  // send the local transofrm information to clients or server.
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
        // Set the flag Position information
        localTransform._flag0Pos = EAE_Engine::Core::World::GetInstance().GetGameObj("flag0")->GetTransform()->GetPos();
        localTransform._flag1Pos = EAE_Engine::Core::World::GetInstance().GetGameObj("flag1")->GetTransform()->GetPos();
        // Send 
        _peer->Send((char*)&localTransform, sizeof(PlayerTransform), LOW_PRIORITY, UNRELIABLE, 0, _peer->GetSystemAddressFromGuid(outter), false);
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
        _peer->Send((char*)&transform, sizeof(PlayerTransform), LOW_PRIORITY, UNRELIABLE, 0, _peer->GetSystemAddressFromGuid(inner), false);
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
    _peer->Send((char*)&transform, sizeof(PlayerTransform), LOW_PRIORITY, UNRELIABLE, 0, _serverAddress, false);
  }
}


void NetworkPeer::DisConnect() 
{

}


