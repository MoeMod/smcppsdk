#pragma once
#include <PlayerManager.h>
#include <IPlayerHelpers.h>
#include "sm/sourcemod_types.h"
#include "sdktools.h"
#include <inetchannel.h>
//#include "sm/ranges.h"
namespace sm{
	inline namespace sourcemod {
		inline namespace players {
			using SourceMod::IGamePlayer;
			// core/logic/smn_players.cpp
			inline void ChangeClientTeam(AutoEntity<IGamePlayer*> pPlayer, int team) {
				IPlayerInfo* pInfo = pPlayer->GetPlayerInfo();
				return pInfo->ChangeTeam(team);
			}

			inline bool IsClientConnected(AutoEntity<IGamePlayer*> pPlayer) {
				return pPlayer && pPlayer->IsConnected();
			}

			inline bool IsPlayerAlive(AutoEntity<IGamePlayer*> player) {
				if (!player) return false;
				
				static int lifestate_offset = -1;
				if (lifestate_offset == -1) {
					if (!g_pGameConf->GetOffset("m_lifeState", &lifestate_offset)) lifestate_offset = -2;
				}

				if (lifestate_offset < 0) {
					IPlayerInfo* info = player->GetPlayerInfo();
					if (info == nullptr) return false; 
					return info->IsDead() ? PLAYER_LIFE_DEAD : PLAYER_LIFE_ALIVE;
				}

				auto edict = player->GetEdict();
				if (edict == nullptr) {
					return false;
				}

				CBaseEntity* pEntity;
				IServerUnknown* pUnknown = edict->GetUnknown();
				if (pUnknown == nullptr || (pEntity = pUnknown->GetBaseEntity()) == nullptr) {
					return false;
				}

				return GetEntData<uint8_t>(pEntity, lifestate_offset) == LIFE_ALIVE;
			}

			inline int GetClientHealth(AutoEntity<IGamePlayer*> pPlayer) {
				return pPlayer->GetPlayerInfo()->GetHealth();
			}

			inline int GetClientArmor(AutoEntity<IGamePlayer*> pPlayer) {
				return pPlayer->GetPlayerInfo()->GetArmorValue();
			}

			inline const char* GetClientName(AutoEntity<IGamePlayer*> pPlayer) {
				if (!pPlayer) {
					return icvar->FindVar("hostname")->GetString();
				}

				return pPlayer->GetName();
			}

			inline bool IsClientInGame(AutoEntity<IGamePlayer*> pPlayer) {
				return pPlayer->IsInGame();
			}

			inline bool IsFakeClient(AutoEntity<IGamePlayer*> pPlayer) {
				return pPlayer->IsFakeClient();
			}

			inline bool IsClientInKickQueue(AutoEntity<IGamePlayer*> pPlayer) {
				return pPlayer->IsInKickQueue();
			}

			inline bool IsClientSourceTV(AutoEntity<IGamePlayer*> pPlayer) {
				return pPlayer->IsSourceTV();
			}
			
			inline int GetMaxClients() { 
				return playerhelpers->GetMaxClients(); 
			}

			inline int GetNumPlayers() { 
				return playerhelpers->GetNumPlayers(); 
			}

			inline Vector GetClientMaxs(AutoEntity<IGamePlayer*> player) {
				return player->GetPlayerInfo()->GetPlayerMaxs();
			}
			
			inline Vector GetClientMins(AutoEntity<IGamePlayer*> player) {
				return player->GetPlayerInfo()->GetPlayerMins();
			}

			inline Vector GetClientAbsOrigins(AutoEntity<IGamePlayer*> player) {
				return player->GetPlayerInfo()->GetAbsOrigin();
			}

			inline QAngle GetClientAbsAngles(AutoEntity<IGamePlayer*> player) {
				return player->GetPlayerInfo()->GetAbsAngles();
			}

			inline int GetMaxHumanPlayers() {
				int count = -1;
#if SOURCE_ENGINE >= SE_LEFT4DEAD
				count = serverClients->GetMaxHumanPlayers();
#endif
				return (count == -1) ? GetMaxClients() : count;
			}

			inline int GetClientCount(bool inGameOnly) {
				if (inGameOnly) return GetNumPlayers();

				int maxplayers = GetMaxClients();
				int cnt = 0;
				for (int i = 1; i <= maxplayers; ++i) {
					// 原版就是这么写的
					if (IsClientConnected(i) && !IsClientInGame(i)) {
						cnt++;
					}
				}
				return (GetNumPlayers() + cnt);
			}

			inline const char* GetClientIP(AutoEntity<IGamePlayer*> player) {
				if (player && IsClientConnected(player)) return player->GetIPAddress();
			}
			inline const char* GetClientAuthId(AutoEntity<IGamePlayer*> client, AuthIdType authType, bool validate = true) {
				if (!client || !IsClientConnected(client)) {
					return nullptr;
				}

				const char* authStr;
				switch (authType) {
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
					if (IsFakeClient(client) || gamehelpers->IsLANServer()) {
						return nullptr;
					}

					uint64_t steam64 = client->GetSteamId64(validate);
					if (!steam64) {
						return nullptr;
					}

					char* buffer;
					ke::SafeSprintf(buffer, 64, "%" PRIu64, steam64);
					
					return buffer;
					break;
				}
			}

			inline int GetSteamAccountID(AutoEntity<IGamePlayer*> player, bool validate) {
				return player->GetSteamAccountID(validate);
			}

			inline int GetClientUserId(AutoEntity<IGamePlayer*> player) {
				return player->GetUserId();
			}

			inline bool IsClientAuthorized(AutoEntity<IGamePlayer*> player) {
				if (!player)
					return false;

				return playerhelpers->GetGamePlayer(sm::ent_cast<edict_t*>(player))->IsAuthorized();
			}

			inline bool IsClientReplay(AutoEntity<IGamePlayer*> player) {
				if (!player)
					return false;

				if (!IsClientConnected(player))
					return false;

				return player->IsReplay();
			}

			inline bool IsClientObserver(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					return false;
				}

				if (!player->IsInGame()){
					return false;
				}

				IPlayerInfo* pInfo = player->GetPlayerInfo();
				if (!pInfo) {
					return false;
				}

				return pInfo->IsObserver();
			}
			inline const char* GetClientInfo(AutoEntity<IGamePlayer*> player, const char* key) {
				if (!player) {
					return nullptr;
				}

				if (!IsClientConnected(player)) {
					return nullptr;
				}

				return engine->GetClientConVarValue(sm::ent_cast<cell_t>(player), key);
			}

			inline int GetClientTeam(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					return TEAM_INVALID;
				}

				if (!IsClientInGame(player)) {
					return TEAM_INVALID;
				}

				IPlayerInfo* pInfo = player->GetPlayerInfo();
				if (!pInfo) {
					return TEAM_INVALID;
				}

				return pInfo->GetTeamIndex();
			}

			inline CBasePlayer* CreateFakeClient(const char* name) {
				if (!g_pSM->IsMapRunning()) {
					return nullptr;
				}

				return sm::ent_cast<CBasePlayer*>(engine->CreateFakeClient(name));
			}
			inline bool SetFakeClientConVar(AutoEntity<IGamePlayer*> player, const char* cvar, const char* val) {
				if (!player) {
					return false;
				}

				if (!IsClientConnected(player)) {
					return false;
				}

				if (!IsFakeClient(player)) {
					return false;
				}

				engine->SetFakeClientConVarValue(sm::ent_cast<edict_t*>(player), cvar, val);
				return true;
			}

			inline const char* GetClientModel(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					return nullptr;
				}

				if (!IsClientInGame(player)) {
					return nullptr;
				}

				IPlayerInfo* pInfo = player->GetPlayerInfo();
				if (!pInfo) {
					return nullptr;
				}
				return pInfo->GetModelName();
			}
			inline const char* GetClientWeapon(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					return nullptr;
				}

				if (!IsClientInGame(player)) {
					return nullptr;
				}

				IPlayerInfo* pInfo = player->GetPlayerInfo();
				if (!pInfo) {
					return nullptr;
				}
				
				return pInfo->GetWeaponName();
			}

			// You must use try/catch to handle std::runtime_error !
			inline int GetClientFrags(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					throw std::runtime_error("Player is nullptr.");
				}
				if (!IsClientInGame(player)) {
					throw std::runtime_error("Client is not in game");
				}
				IPlayerInfo* pInfo = player->GetPlayerInfo();
				if (!pInfo) {
					throw std::runtime_error("IPlayerInfo is not supported by the game.");
				}
				return pInfo->GetFragCount();
			}

			// You must use try/catch to handle std::runtime_error !
			inline int GetClientDeaths(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					throw std::runtime_error("Player is nullptr.");
				}
				if (!IsClientInGame(player)) {
					throw std::runtime_error("Client is not in game");
				}
				IPlayerInfo* pInfo = player->GetPlayerInfo();
				if (!pInfo) {
					throw std::runtime_error("IPlayerInfo is not supported by the game.");
				}
				return pInfo->GetDeathCount();
			}

			inline int GetClientDataRate(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					return 0;
				}

				if (!IsClientConnected(player)) {
					return 0;
				}

				if (IsFakeClient(player)) {
					return 0;
				}

				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<int>(player));
				if (!pInfo) {
					return 0;
				}
				return pInfo->GetDataRate();
			}

			// You must use try/catch to handle std::runtime_error !
			inline bool IsClientTimingOut(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					throw std::runtime_error("Player is nullptr");
				}
				if (!IsClientConnected(player)) {
					throw std::runtime_error("Player is not connected");
				}
				if (IsFakeClient(player)) {
					throw std::runtime_error("Player is a bot");
				}
				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<int>(player));
				if (!pInfo) {
					return true;
				}
				return pInfo->IsTimingOut();
			}

			inline float GetClientTime(AutoEntity<IGamePlayer*> player) {
				if (!player) {
					return 0.f;
				}
				if (!IsClientConnected(player)) {
					return 0.f;
				}
				if (IsFakeClient(player)) {
					return 0.f;
				}
				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<cell_t>(player));
				if (!pInfo) {
					return 0.f;
				}
				return pInfo->GetTimeConnected();
			}
			
			inline float GetClientLatency(AutoEntity<IGamePlayer*> player, NetFlow flow) {
				if (!player) {
					return -1.f;
				}
				if (!IsClientConnected(player)) {
					return -1.f;
				}
				if (IsFakeClient(player)) {
					return -1.f;
				}

				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<cell_t>(player));
				if (!pInfo) {
					return -1.f;
				}

				if (flow == NetFlow::NetFlow_Both) {
					return pInfo->GetLatency(FLOW_INCOMING) + pInfo->GetLatency(FLOW_OUTGOING);
				}
				else {
					return pInfo->GetLatency(flow);
				}
			}

			inline float GetClientAvgLatency(AutoEntity<IGamePlayer*> player, NetFlow flow) {
				if (!player) {
					return -1.f;
				}
				if (!IsClientConnected(player)) {
					return -1.f;
				}
				if (IsFakeClient(player)) {
					return -1.f;
				}

				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<cell_t>(player));
				if (!pInfo) {
					return -1.f;
				}

				if (flow == NetFlow::NetFlow_Both) {
					return pInfo->GetAvgLatency(FLOW_INCOMING) + pInfo->GetAvgLatency(FLOW_OUTGOING);
				}
				else {
					return pInfo->GetAvgLatency(flow);
				}
			}

			inline float GetClientAvgLoss(AutoEntity<IGamePlayer*> player, NetFlow flow) {
				if (!player) {
					return -1.f;
				}
				if (!IsClientConnected(player)) {
					return -1.f;
				}
				if (IsFakeClient(player)) {
					return -1.f;
				}

				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<cell_t>(player));
				if (!pInfo) {
					return -1.f;
				}

				if (flow == NetFlow::NetFlow_Both) {
					return pInfo->GetAvgLoss(FLOW_INCOMING) + pInfo->GetAvgLoss(FLOW_OUTGOING);
				}
				else {
					return pInfo->GetAvgLoss(flow);
				}
			}

			inline float GetClientAvgChoke(AutoEntity<IGamePlayer*> player, NetFlow flow) {
				if (!player) {
					return -1.f;
				}
				if (!IsClientConnected(player)) {
					return -1.f;
				}
				if (IsFakeClient(player)) {
					return -1.f;
				}

				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<cell_t>(player));
				if (!pInfo) {
					return -1.f;
				}

				if (flow == NetFlow::NetFlow_Both) {
					return pInfo->GetAvgChoke(FLOW_INCOMING) + pInfo->GetAvgChoke(FLOW_OUTGOING);
				}
				else {
					return pInfo->GetAvgChoke(flow);
				}
			}

			inline float GetClientAvgData(AutoEntity<IGamePlayer*> player, NetFlow flow) {
				if (!player) {
					return -1.f;
				}
				if (!IsClientConnected(player)) {
					return -1.f;
				}
				if (IsFakeClient(player)) {
					return -1.f;
				}

				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<cell_t>(player));
				if (!pInfo) {
					return -1.f;
				}

				if (flow == NetFlow::NetFlow_Both) {
					return pInfo->GetAvgData(FLOW_INCOMING) + pInfo->GetAvgData(FLOW_OUTGOING);
				}
				else {
					return pInfo->GetAvgData(flow);
				}
			}
			inline float GetClientAvgPackets(AutoEntity<IGamePlayer*> player, NetFlow flow) {
				if (!player) {
					return -1.f;
				}
				if (!IsClientConnected(player)) {
					return -1.f;
				}
				if (IsFakeClient(player)) {
					return -1.f;
				}

				INetChannelInfo* pInfo = engine->GetPlayerNetInfo(sm::ent_cast<cell_t>(player));
				if (!pInfo) {
					return -1.f;
				}

				if (flow == NetFlow::NetFlow_Both) {
					return pInfo->GetAvgPackets(FLOW_INCOMING) + pInfo->GetAvgPackets(FLOW_OUTGOING);
				}
				else {
					return pInfo->GetAvgPackets(flow);
				}
			}
			
			inline int GetClientOfUserId(AutoEntity<IGamePlayer*> player) {
				return playerhelpers->GetClientOfUserId(player->GetUserId());
			}

			inline int GetClientSerial(AutoEntity<IGamePlayer*> player) {
				return player->GetSerial();
			}

			inline int GetClientFromSerial(AutoEntity<IGamePlayer*> player) {
				return playerhelpers->GetClientFromSerial(player->GetSerial());
			}
		}
	}
}

#define MaxClients sm::sourcemod::players::GetMaxClients();