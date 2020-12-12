#pragma once

namespace sm {
    inline namespace sourcemod {

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();
        bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late);

        extern ICvar *icvar;
        extern IServerPluginHelpers* serverpluginhelpers;
        extern IGameConfig* g_pGameConf;
        extern IFileSystem* valvefs;

        // Must match clients.inc
        enum AuthIdType
        {
            AuthId_Engine = 0,     /**< The game-specific auth string as returned from the engine */

            // The following are only available on games that support Steam authentication.
            AuthId_Steam2,         /**< Steam2 rendered format, ex "STEAM_1:1:4153990" */
            AuthId_Steam3,         /**< Steam3 rendered format, ex "[U:1:8307981]" */
            AuthId_SteamID64,      /**< A SteamID64 (uint64) as a String, ex "76561197968573709" */
        };
    }
};

#include "sourcemod_types.h"
#include "sourcemod_convert.h"
#include "sourcemod_usermessages.h"
#include "sourcemod_halflife.h"
#include "sourcemod_timers.h"
#include "sourcemod_console.h"
#include "sourcemod_functions.h"
#include "sourcemod_entities.h"
#include "sourcemod_players.h"
#include "sourcemod_filesystem.h"
#include "sourcemod_translate.h"
