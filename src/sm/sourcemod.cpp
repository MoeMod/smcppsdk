
#include "extension.h"

#include "sm/sourcemod.h"

#include "sm/sdkhooks.h"
#include "sm/sdktools.h"
#include "sm/cstrike.h"

using SourceMod::IGameConfig;

namespace sm {
    inline namespace sourcemod {
        IGameConfig *g_pGameConf = nullptr;
        ICvar *icvar = nullptr;
        IServerPluginHelpers* serverpluginhelpers = nullptr;

        inline namespace hudtext {
            int g_HudMsgNum = -1;
        }
        inline namespace halflife {
            int g_SayTextMsg = -1;
        }
        inline namespace functions {
            ThinkQueue g_ThinkQueue;
        }
        void OnGameFrame(bool simulating)
        {
            g_ThinkQueue.CallAndClear();
        }

        bool SDK_OnLoad(char *error, size_t maxlen, bool late) {
            char conf_error[255];
            if (!gameconfs->LoadGameConfigFile("core.games", &g_pGameConf, conf_error, sizeof(conf_error)))
            {
                if (error)
                {
                    ke::SafeSprintf(error, maxlen, "Could not read core.games: %s", conf_error);
                }
                return false;
            }
            sm::sdkhooks::detail::SDK_OnLoad(error, maxlen, late);
            sm::sdktools::detail::SDK_OnLoad(error, maxlen, late);
            sm::cstrike::detail::SDK_OnLoad(error, maxlen, late);

            g_HudMsgNum = usermsgs->GetMessageIndex(g_pGameConf->GetKeyValue("HudTextMsg"));
            g_SayTextMsg = usermsgs->GetMessageIndex("SayText");
            g_pSM->AddGameFrameHook(&OnGameFrame);

            return true;
        }

        void SDK_OnUnload() {
            sm::sdkhooks::detail::SDK_OnUnload();
            sm::sdktools::detail::SDK_OnUnload();
            sm::cstrike::detail::SDK_OnUnload();
            g_pSM->RemoveGameFrameHook(&OnGameFrame);

        }

        bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
        {
            GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
            GET_V_IFACE_CURRENT(GetEngineFactory, serverpluginhelpers, IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);

            sm::sdktools::detail::SDK_OnMetamodLoad(ismm, error, maxlen, late);

            return true;
        }
    }
}

