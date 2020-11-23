#pragma once

#include "sourcemod_types.h"
#include "hook_result.h"
#include "cstrike_const.h"

namespace sm {
    namespace cstrike {
        namespace detail {
            bool SDK_OnLoad(char *error, size_t maxlength, bool late);
            void SDK_OnUnload();
        }

        void CS_TerminateRound(float delay, CSRoundEndReason_e reason, bool blockhook = false);

        void SwitchTeam(CGameRules *gamerules, CBasePlayer *pEntity, CSTeam_e iTeam);
        void CS_SwitchTeam(CBasePlayer *pEntity, CSTeam_e iTeam);

        void CS_RespawnPlayer(CBasePlayer *pEntity);
        void CS_UpdateClientModel(CBasePlayer *pEntity);

        void CS_DropWeapon(CBasePlayer *pEntity, CBaseCombatWeapon *pWeapon, bool toss);

        inline namespace forwards {
            extern EventDispatcher<Action(float&, CSRoundEndReason_e&)> CS_OnTerminateRound;
        }
    }
}