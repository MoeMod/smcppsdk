#pragma once
// if possible, modife sourcemod_hudtext->sourcemod_usermsgs
#include <tier0/wchartypes.h>

namespace sm{
    inline namespace sourcemod {
        inline namespace usermessages {

            struct hud_text_parms {
                float x;
                float y;
                int effect;
                byte r1, g1, b1, a1;
                byte r2, g2, b2, a2;
                float fadeinTime;
                float fadeoutTime;
                float holdTime;
                float fxTime;
                int channel;
            };

            void SendHudText(int client, const hud_text_parms &textparms, const char *pMessage);
            void CreateShakeScreen(int client, float flAmplitude, float flFrequency, float flDurationTime);
            void CreateFadeScreen(int client, int iDuration, float iHoldTime, int iFlags, Color color);
        }
    }
}