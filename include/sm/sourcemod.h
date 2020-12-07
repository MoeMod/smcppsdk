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
    }
};

#include "sourcemod_types.h"
#include "sourcemod_convert.h"
#include "sourcemod_hudtext.h"
#include "sourcemod_halflife.h"
#include "sourcemod_timers.h"
#include "sourcemod_console.h"
#include "sourcemod_functions.h"
#include "sourcemod_entities.h"
#include "sourcemod_players.h"
#include "sourcemod_filesystem.h"
