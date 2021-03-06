#pragma once

#include "extension.h"

namespace sm {
    inline namespace sourcemod {

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();
        bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late);
    }
};

#include "impl/sourcemod_impl_global.hpp"

#include "sourcemod_types.h"
#include "sourcemod_enums.h"
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
#include "sourcemod_string.h"

#include "cstrike.h"
#include "sdkhooks.h"
#include "sdktools.h"
#include "impl/sourcemod_impl.hpp"