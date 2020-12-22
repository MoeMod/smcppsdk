#pragma once

namespace sm {
    inline namespace sourcemod {
        inline bool SDK_OnLoad(char* error, size_t maxlen, bool late) {
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
            g_ShakeMsg = usermsgs->GetMessageIndex("Shake");
            g_SayTextMsg = usermsgs->GetMessageIndex("SayText");
            g_FadeMsg = usermsgs->GetMessageIndex("Fade");
            g_SayText2Msg = usermsgs->GetMessageIndex("SayText2");
            g_HintTextMsg = usermsgs->GetMessageIndex("HintText");
            g_TextMsg = usermsgs->GetMessageIndex("TextMsg");

            g_pSM->AddGameFrameHook(&OnGameFrame);

            return true;
        }

        inline void SDK_OnUnload() {
            sm::sdkhooks::detail::SDK_OnUnload();
            sm::sdktools::detail::SDK_OnUnload();
            sm::cstrike::detail::SDK_OnUnload();
            g_pSM->RemoveGameFrameHook(&OnGameFrame);
        }

        inline bool SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
        {
            GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
            GET_V_IFACE_CURRENT(GetEngineFactory, serverpluginhelpers, IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);
            GET_V_IFACE_CURRENT(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
            GET_V_IFACE_ANY(GetEngineFactory, enginesound, IEngineSound, IENGINESOUND_SERVER_INTERFACE_VERSION);
            sm::sdktools::detail::SDK_OnMetamodLoad(ismm, error, maxlen, late);

            return true;
        }
    }
}
