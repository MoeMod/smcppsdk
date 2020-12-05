#pragma once
#include <PlayerManager.h>
#include <IPlayerHelpers.h>
#include "sm/sourcemod_types.h"

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
                if (!player)
                    return false;
            	
                static int lifestate_offset = -1;
                if (lifestate_offset == -1)
                {
                    if (!g_pGameConf->GetOffset("m_lifeState", &lifestate_offset))
                    {
                        lifestate_offset = -2;
                    }
                }

                if (lifestate_offset < 0)
                {
                    IPlayerInfo* info = player->GetPlayerInfo();
                    if (info == nullptr)
                    {
                        return false;
                    }
                    return info->IsDead() ? PLAYER_LIFE_DEAD : PLAYER_LIFE_ALIVE;
                }

                auto edict = player->GetEdict();
                if (edict == nullptr)
                {
                    return false;
                }

                CBaseEntity* pEntity;
                IServerUnknown* pUnknown = edict->GetUnknown();
                if (pUnknown == nullptr || (pEntity = pUnknown->GetBaseEntity()) == nullptr)
                {
                    return false;
                }
            	
                return GetEntData<uint8_t>(pEntity, lifestate_offset) == LIFE_ALIVE;
            }

            inline int GetHealth(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer->GetPlayerInfo()->GetHealth();
            }

            inline int GetArmorValue(AutoEntity<IGamePlayer*> pPlayer)
            {
                return pPlayer->GetPlayerInfo()->GetArmorValue();
            }

            inline const char* GetClientName(AutoEntity<IGamePlayer*> pPlayer)
            {
                if (!pPlayer)
                    return icvar->FindVar("hostname")->GetString();
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
        }
    }
}