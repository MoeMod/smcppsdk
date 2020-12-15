
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
        inline namespace filesystem {
            IFileSystem* valvefs = nullptr;
        }
        inline namespace translate {
            ITranslator* translator = nullptr;
            IPhraseCollection* collection = nullptr;
        }
        inline namespace usermessages {
            int g_HudMsgNum = -1;
            int g_ShakeMsg = -1;
            int g_FadeMsg = -1;
            int g_SayTextMsg = -1;
            int g_SayText2Msg = -1;
            int g_HintTextMsg = -1;
            int g_TextMsg = -1; // just coincidence, with same name with usermessage.
            bool g_SkipNextChatConSound = false;
            bool g_ChatConSnd = true;
        }
        inline namespace functions {
            ThinkQueue g_ThinkQueue;
        }
        void OnGameFrame(bool simulating)
        {
            g_ThinkQueue.CallAndClear();
        }

        /**
         * Gets an entity's render mode.
         *
         * @param entity        Entity index.
         * @return              RenderMode value.
         * @error               Invalid entity index, or lack of mod compliance.
         */
        inline RenderMode_t GetEntityRenderMode(CBaseEntity* entity)
        {
            const char* prop = g_pGameConf->GetKeyValue("m_nRenderMode");
            
            if (!prop) {
                std::string buffer("m_nRenderMode");
                prop = buffer.c_str();
            }

            return sm::GetEntProp<RenderMode_t>(entity, sm::Prop_Send, prop, 1);
        }

        /**
         * Sets an entity's render mode.
         *
         * @param entity        Entity index.
         * @param mode          RenderMode value.
         * @error               Invalid entity index, or lack of mod compliance.
         */
        inline void SetEntityRenderMode(CBaseEntity* entity, RenderMode_t mode)
        {
            const char* prop = g_pGameConf->GetKeyValue("m_nRenderMode");

            if (!prop) {
                std::string buffer("m_nRenderMode");
                prop = buffer.c_str();
            }

            sm::SetEntProp<RenderMode_t>(entity, sm::Prop_Send, prop, mode, 1);
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
            g_ShakeMsg = usermsgs->GetMessageIndex("Shake");
            g_SayTextMsg = usermsgs->GetMessageIndex("SayText");
            g_FadeMsg = usermsgs->GetMessageIndex("Fade");
            g_SayText2Msg = usermsgs->GetMessageIndex("SayText2");
            g_HintTextMsg = usermsgs->GetMessageIndex("HintText");
            g_TextMsg = usermsgs->GetMessageIndex("TextMsg");

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

