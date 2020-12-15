#pragma once

#include <stdexcept>

namespace sm {
    inline namespace sourcemod {

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();
        bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late);

        extern ICvar *icvar;
        extern IServerPluginHelpers* serverpluginhelpers;
        extern IGameConfig* g_pGameConf;

        // Must match clients.inc
        enum AuthIdType
        {
            AuthId_Engine = 0,     /**< The game-specific auth string as returned from the engine */

            // The following are only available on games that support Steam authentication.
            AuthId_Steam2,         /**< Steam2 rendered format, ex "STEAM_1:1:4153990" */
            AuthId_Steam3,         /**< Steam3 rendered format, ex "[U:1:8307981]" */
            AuthId_SteamID64,      /**< A SteamID64 (uint64) as a String, ex "76561197968573709" */
        };

        
        inline RenderMode_t GetEntityRenderMode(CBaseEntity* entity);

        inline void SetEntityRenderMode(CBaseEntity* entity, RenderMode_t mode);

        /**
         * Sets an entity's render mode.
         *
         * @param entity        Entity index.
         * @param mode          RenderMode value.
         * @error               Invalid entity index, or lack of mod compliance.
         */
        //stock void SetEntityRenderMode(int entity, RenderMode mode)
        //{
        //    static bool gotconfig = false;
        //    static char prop[32];

        //    if (!gotconfig)
        //    {
        //        GameData gc = new GameData("core.games");
        //        bool exists = gc.GetKeyValue("m_nRenderMode", prop, sizeof(prop));
        //        delete gc;

        //        if (!exists)
        //        {
        //            strcopy(prop, sizeof(prop), "m_nRenderMode");
        //        }

        //        gotconfig = true;
        //    }

        //    SetEntProp(entity, Prop_Send, prop, mode, 1);
        //}
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
