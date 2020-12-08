#include "extension.h"

#include "sm/sourcemod_usermessages.h"
#include <memory>

#if SOURCE_ENGINE == SE_CSGO
// fix marco conflict from basetypes.h
#undef schema
#include <cstrike15_usermessages.pb.h>
#endif

#include "../core/CellRecipientFilter.h"

namespace sm{
    inline namespace sourcemod {
        inline namespace usermessages {
            extern int g_HudMsgNum;
            extern int g_ShakeMsgNum;
            extern int g_FadeMsgNum;
            void SendHudText(CellRecipientFilter &crf, const hud_text_parms& textparms, const char* pMessage) {

#if SOURCE_ENGINE == SE_CSGO
                std::unique_ptr<CCSUsrMsg_HudMsg> msg = std::make_unique<CCSUsrMsg_HudMsg>();
                msg->set_channel(textparms.channel & 0xFF);

                CMsgVector2D* pos = msg->mutable_pos();
                pos->set_x(textparms.x);
                pos->set_y(textparms.y);

                CMsgRGBA* color1 = msg->mutable_clr1();
                color1->set_r(textparms.r1);
                color1->set_g(textparms.g1);
                color1->set_b(textparms.b1);
                color1->set_a(textparms.a1);

                CMsgRGBA* color2 = msg->mutable_clr2();
                color2->set_r(textparms.r2);
                color2->set_g(textparms.g2);
                color2->set_b(textparms.b2);
                color2->set_a(textparms.a2);

                msg->set_effect(textparms.effect);
                msg->set_fade_in_time(textparms.fadeinTime);
                msg->set_fade_out_time(textparms.fadeoutTime);
                msg->set_hold_time(textparms.holdTime);
                msg->set_fx_time(textparms.fxTime);
                msg->set_text(pMessage);
                engine->SendUserMessage(crf, g_HudMsgNum, *msg);
#else
                bf_write* bf = usermsgs->StartBitBufMessage(g_HudMsgNum, players, 1, 0);
                bf->WriteByte(textparms.channel & 0xFF);
                bf->WriteFloat(textparms.x);
                bf->WriteFloat(textparms.y);
                bf->WriteByte(textparms.r1);
                bf->WriteByte(textparms.g1);
                bf->WriteByte(textparms.b1);
                bf->WriteByte(textparms.a1);
                bf->WriteByte(textparms.r2);
                bf->WriteByte(textparms.g2);
                bf->WriteByte(textparms.b2);
                bf->WriteByte(textparms.a2);
                bf->WriteByte(textparms.effect);
                bf->WriteFloat(textparms.fadeinTime);
                bf->WriteFloat(textparms.fadeoutTime);
                bf->WriteFloat(textparms.holdTime);
                bf->WriteFloat(textparms.fxTime);
                bf->WriteString(pMessage);
                usermsgs->EndMessage();
#endif
            }

            void SendHudText(int client, const hud_text_parms &textparms, const char *pMessage) {
                cell_t players[1];
                players[0] = client;
                CellRecipientFilter crf;
                crf.Initialize(players, 1);
                return SendHudText(crf, textparms, pMessage);
            }

            void ShakeScreen(CellRecipientFilter& crf, float flAmplitude, float flFrequency, float flDurationTime)
            {
#if SOURCE_ENGINE == SE_CSGO
                std::unique_ptr<CCSUsrMsg_Shake> msg = std::make_unique<CCSUsrMsg_Shake>();
                msg->set_command(0);
                msg->set_local_amplitude(flAmplitude);
                msg->set_frequency(flFrequency);
                msg->set_duration(flDurationTime);
                engine->SendUserMessage(crf, g_ShakeMsgNum, *msg);
#else
                bf_write* bf = usermsgs->StartBitBufMessage(g_ShakeMsgNum, players, 1, 0);
                bf->WriteByte(0);
                bf->WriteFloat(flAmplitude);
                bf->WriteFloat(flFrequency);
                bf->WriteFloat(flDurationTime);
                usermsgs->EndMessage();
#endif
            }
            void CreateShakeScreen(int client, float flAmplitude, float flFrequency, float flDurationTime)
            {
                cell_t players[1];
                players[0] = client;
                CellRecipientFilter crf;
                crf.Initialize(players, 1);
                return ShakeScreen(crf, flAmplitude, flFrequency, flDurationTime);
            }
            /**
             * @brief Fade a client screen with specific parameters.
             *
             * @param crf               The client filter.
             * @param flDuration        The duration of fade in the seconds.
             * @param flHoldTime        The holding time of fade in the seconds.
             * @param iFlags            The bits with some flags.
             * @param vColor            The array with RGB color.
             **/
            void FadeScreen(CellRecipientFilter &crf, int iDuration, float iHoldTime, int iFlags, Color color)
            {
#if SOURCE_ENGINE == SE_CSGO
                std::unique_ptr<CCSUsrMsg_Fade> msg = std::make_unique<CCSUsrMsg_Fade>();
                msg->set_duration(iDuration);
                msg->set_hold_time(iHoldTime);
                msg->set_flags(iFlags);

                CMsgRGBA* clrBuffer = msg->mutable_clr();
                clrBuffer->set_r(color.r());
                clrBuffer->set_g(color.g());
                clrBuffer->set_b(color.b());
                clrBuffer->set_a(color.a());
                engine->SendUserMessage(crf, g_FadeMsgNum, *msg);
#else // SOURCE_ENGINE != SE_CSGO
                bf_write* bf = usermsgs->StartBitBufMessage(g_FadeMsgNum, players, 1, 0);
                bf->WriteByte(iDuration);
                bf->WriteByte(iHoldTime);
                bf->WriteByte(iFlags);
                bf->WriteByte(color.r());
                bf->WriteByte(color.g());
                bf->WriteByte(color.b());
                bf->WriteByte(color.a());
                usermsgs->EndMessage();
#endif // SOURCE_ENGINE == SE_CSGO
            }

            void CreateFadeScreen(int client, int iDuration, float iHoldTime, int iFlags, Color color)
            {
                cell_t players[1];
                players[0] = client;
                CellRecipientFilter crf;
                crf.Initialize(players, 1);
                return FadeScreen(crf, iDuration, iHoldTime, iFlags, color);
            }
        }
    }
}