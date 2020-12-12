#pragma once
#include <PlayerManager.h>
#include <IPlayerHelpers.h>
#include "sm/sourcemod_types.h"
#include "sdktools.h"
//#include "sm/ranges.h"
namespace sm{
    inline namespace sourcemod {
        inline namespace players {
            using SourceMod::IGamePlayer;
            // core/logic/smn_players.cpp
            inline void ChangeClientTeam(AutoEntity<IGamePlayer*> pPlayer, int team)
            {
                IPlayerInfo* pInfo = pPlayer->GetPlayerInfo();
                return pInfo->ChangeTeam(team);
            }

            inline bool IsClientConnected(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer && pPlayer->IsConnected();
            }

            inline bool IsPlayerAlive(AutoEntity<IGamePlayer*> player)
            {
                if (!player) return false;
            	
                static int lifestate_offset = -1;
                if (lifestate_offset == -1)
                {
                    if (!g_pGameConf->GetOffset("m_lifeState", &lifestate_offset)) lifestate_offset = -2;
                }

                if (lifestate_offset < 0)
                {
                    IPlayerInfo* info = player->GetPlayerInfo();
                    if (info == nullptr) return false; 
                    return info->IsDead() ? PLAYER_LIFE_DEAD : PLAYER_LIFE_ALIVE;
                }

                auto edict = player->GetEdict();
                if (edict == nullptr) return false;

                CBaseEntity* pEntity;
                IServerUnknown* pUnknown = edict->GetUnknown();
                if (pUnknown == nullptr || (pEntity = pUnknown->GetBaseEntity()) == nullptr) return false;
            	
                return GetEntData<uint8_t>(pEntity, lifestate_offset) == LIFE_ALIVE;
            }

            inline int GetClientHealth(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer->GetPlayerInfo()->GetHealth();
            }

            inline int GetClientArmor(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer->GetPlayerInfo()->GetArmorValue();
            }

            inline const char* GetClientName(AutoEntity<IGamePlayer*> pPlayer)
            {
                if (!pPlayer) return icvar->FindVar("hostname")->GetString();
                return pPlayer->GetName();
            }

            inline bool IsClientInGame(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer->IsInGame();
            }

            inline bool IsFakeClient(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer->IsFakeClient();
            }

            inline bool IsClientInKickQueue(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer->IsInKickQueue();
            }

            inline bool IsClientSourceTV(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer->IsSourceTV();
            }
            
            inline int GetMaxClients() { return playerhelpers->GetMaxClients(); }

            inline int GetNumPlayers() { return playerhelpers->GetNumPlayers(); }

            inline Vector GetClientMaxs(AutoEntity<IGamePlayer*> player)
            {
                return player->GetPlayerInfo()->GetPlayerMaxs();
            }
            
            inline Vector GetClientMins(AutoEntity<IGamePlayer*> player)
            {
                return player->GetPlayerInfo()->GetPlayerMins();
            }

            inline Vector GetClientAbsOrigins(AutoEntity<IGamePlayer*> player)
            {
                return player->GetPlayerInfo()->GetAbsOrigin();
            }

            inline QAngle GetClientAbsAngles(AutoEntity<IGamePlayer*> player)
            {
                return player->GetPlayerInfo()->GetAbsAngles();
            }
            inline int GetMaxHumanPlayers()
            {
                int count = -1;
#if SOURCE_ENGINE >= SE_LEFT4DEAD
                count = serverClients->GetMaxHumanPlayers();
#endif
                return (count == -1) ? GetMaxClients() : count;
            }
            inline int GetClientCount(bool inGameOnly = true)
            {
                if (inGameOnly) return GetNumPlayers();

                int maxplayers = GetMaxClients();
                int cnt = 0;
                for (int i = 1; i <= maxplayers; ++i)
                    // 原版就是这么写的
                    if (IsClientConnected(i) && !IsClientInGame(i)) cnt++;
                return (GetNumPlayers() + cnt);
            }

            inline const char* GetClientIP(AutoEntity<IGamePlayer*> player)
            {
                if (player && IsClientConnected(player)) return player->GetIPAddress();
            }
            //GetClientAuthId
            //GetSteamAccountID
            //GetClientUserId
            //IsClientAuthorized
            //IsClientReplay
            //IsClientObserver
            //GetClientInfo
            //GetClientTeam
            //CreateFakeClient
            //SetFakeClientConVar
            //GetClientModel
            //GetClientWeapon
            //GetClientFrags
            //GetClientDeaths
            //GetClientDataRate
            //IsClientTimingOut
            //GetClientTime
            //GetClientLatency
            //GetClientAvgLatency
            //GetClientAvgLoss
            //GetClientAvgChoke
            //GetClientAvgData
            //GetClientAvgPackets
            //GetClientOfUserId
            //GetClientSerial
            //GetClientFromSerial
        }
    }
}