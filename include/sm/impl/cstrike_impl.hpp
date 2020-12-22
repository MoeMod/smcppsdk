#pragma once

#include <CDetour/detours.h>
#include <extensions/IBinTools.h>
#include <extensions/ISDKTools.h>

#include "cstrike_impl_global.hpp"
#include "cstrike_impl_hook.hpp"

namespace sm {
    namespace cstrike {

        inline void* FindSig(const char* name)
        {
            void* addr;
            if (!g_pGameConf->GetMemSig(name, &addr))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return addr;
        }

        inline void TerminateRound(CGameRules* gamerules, float delay, int reason) {
#if SOURCE_ENGINE == SE_CSGO
            reason++;
#endif
#ifndef _WIN32
            static MemFuncCaller<void (CGameRules::*)(float, int, int, int)> caller(g_pBinTools, FindSig("TerminateRound"));
            return caller(gamerules, delay, reason, 0, 0);
#else
            static void* addr = FindSig("TerminateRound");
            __asm
            {
                push 0
                push 0
                push reason
                movss xmm1, delay
                mov ecx, gamerules
                call addr
            }
#endif
        }

        inline void CS_TerminateRound(float delay, CSRoundEndReason_e reason, bool blockhook) {
            if (blockhook && g_pTerminateRoundDetoured)
                g_pIgnoreTerminateDetour = true;
            return TerminateRound(static_cast<CGameRules*>(g_pSDKTools->GetGameRules()), delay, reason);
        }

        inline void CS_RespawnPlayer(CBasePlayer* pEntity) {
            static MemFuncCaller<void (CBasePlayer::*)()> caller(g_pBinTools, FindSig("RoundRespawn"));
            return caller(pEntity);
        }

        inline void CS_UpdateClientModel(CBasePlayer* pEntity) {
            static MemFuncCaller<void (CBasePlayer::*)()> caller(g_pBinTools, FindSig("SetModelFromClass"));
            return caller(pEntity);
        }

        inline void SwitchTeam(CGameRules* gamerules, CBasePlayer* pEntity, CSTeam_e team) {

#ifndef _WIN32
            static MemFuncCaller<void (CBasePlayer::*)(int)> caller(g_pBinTools, FindSig("SwitchTeam"));
            return caller(pEntity, team);
#else
            static void* addr = FindSig("SwitchTeam");
            __asm
            {
                push team
                mov ecx, pEntity
                mov ebx, gamerules
                call addr
            }
#endif
        }

        inline void CS_SwitchTeam(CBasePlayer* pEntity, CSTeam_e team) {
            return SwitchTeam(static_cast<CGameRules*>(g_pSDKTools->GetGameRules()), pEntity, team);
        }

        inline void CS_DropWeapon(CBasePlayer* pEntity, CBaseCombatWeapon* pWeapon, bool toss) {
            static MemFuncCaller<void (CBasePlayer::*)(CBaseCombatWeapon*, bool)> caller(g_pBinTools, FindSig("DropWeaponBB"));
            return caller(pEntity, pWeapon, toss);
        }

        namespace detail
        {
            inline bool SDK_OnLoad(char* error, size_t maxlength, bool late) {
                char conf_error[255];
                if (!gameconfs->LoadGameConfigFile("sm-cstrike.games", &g_pGameConf, conf_error, sizeof(conf_error)))
                {
                    if (error)
                    {
                        ke::SafeSprintf(error, maxlength, "Could not read sm-cstrike.games: %s", conf_error);
                    }
                    return false;
                }
                sharesys->AddDependency(myself, "bintools.ext", true, true);
                sharesys->AddDependency(myself, "sdktools.ext", true, true);

                SM_GET_IFACE(BINTOOLS, g_pBinTools);
                SM_GET_IFACE(SDKTOOLS, g_pSDKTools);

                CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);
                g_TerminateRoundDetourEnabled = CreateTerminateRoundDetour();

                return true;

            }

            inline void SDK_OnUnload() {

                if (g_TerminateRoundDetourEnabled)
                {
                    RemoveTerminateRoundDetour();
                    g_TerminateRoundDetourEnabled = false;
                }

                gameconfs->CloseGameConfigFile(g_pGameConf);
            }
        }
    }
}
