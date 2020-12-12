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
            inline int GetClientCount(bool inGameOnly)
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
            inline const char* GetClientAuthId(AutoEntity<IGamePlayer*> client, AuthIdType authType, bool validate = true)
            {
                if (!client || !IsClientConnected(client)) return nullptr;

                const char* authStr;
                switch (authType)
                {
                case AuthId_Engine:
                    authStr = client->GetAuthString(validate);
                    if (!authStr || authStr[0] == '\0') return nullptr;
                    return authStr;
                    break;
                case AuthId_Steam2:
                    authStr = client->GetSteam2Id(validate);
                    if (!authStr || authStr[0] == '\0') return nullptr;
                    return authStr;
                    break;
                case AuthId_Steam3:
                    authStr = client->GetSteam3Id(validate);
                    if (!authStr || authStr[0] == '\0') return nullptr;
                    return authStr;
                    break;
                case AuthId_SteamID64:
                    if (IsFakeClient(client) || gamehelpers->IsLANServer()) return nullptr;
                    uint64_t steam64 = client->GetSteamId64(validate);
                    if (!steam64) return nullptr;
                    
                    //ke::SafeSprintf(authStr, 64, "%" PRIu64, steam64);

                    break;
                }
            }
            inline int GetSteamAccountID(AutoEntity<IGamePlayer*> player, bool validate)
            {
                return player->GetSteamAccountID(validate);
            }
            inline int GetClientUserId(AutoEntity<IGamePlayer*> player)
            {
                return player->GetUserId();
            }
            inline bool IsClientAuthorized(AutoEntity<IGamePlayer*> player) = delete;
            inline bool IsClientReplay(AutoEntity<IGamePlayer*> player) = delete;
            inline bool IsClientObserver(AutoEntity<IGamePlayer*> player) = delete;
            inline int GetClientInfo(AutoEntity<IGamePlayer*> player) = delete;
            inline int GetClientTeam(AutoEntity<IGamePlayer*> player) = delete;
            inline bool CreateFakeClient(const char* name) = delete;
            inline bool SetFakeClientConVar(AutoEntity<IGamePlayer*> player, const char* cvar, const char* val) = delete;
            inline const char* GetClientModel(AutoEntity<IGamePlayer*> player) = delete;
            inline bool GetClientWeapon(AutoEntity<IGamePlayer*> player) = delete;
            inline int GetClientFrags(AutoEntity<IGamePlayer*> player) = delete;
            inline int GetClientDeaths(AutoEntity<IGamePlayer*> player) = delete;
            inline int GetClientDataRate(AutoEntity<IGamePlayer*> player) = delete;
            inline bool IsClientTimingOut(AutoEntity<IGamePlayer*> player) = delete;
            inline float GetClientTime(AutoEntity<IGamePlayer*> player) = delete;
            inline float GetClientLatency(AutoEntity<IGamePlayer*> player/*, NetFlow flow*/) = delete;
            inline float GetClientAvgLatency(AutoEntity<IGamePlayer*> player/*, NetFlow flow*/) = delete;
            inline float GetClientAvgLoss(AutoEntity<IGamePlayer*> player/*, NetFlow flow*/) = delete;
            inline float GetClientAvgChoke(AutoEntity<IGamePlayer*> player/*, NetFlow flow*/) = delete;
            inline float GetClientAvgData(AutoEntity<IGamePlayer*> player/*, NetFlow flow*/) = delete;
            inline float GetClientAvgPackets(AutoEntity<IGamePlayer*> player/*, NetFlow flow*/) = delete;
            inline int GetClientOfUserId(AutoEntity<IGamePlayer*> player) = delete;
            inline int GetClientSerial(AutoEntity<IGamePlayer*> player) = delete;
            inline int GetClientFromSerial(AutoEntity<IGamePlayer*> player) = delete;
        }
    }
}