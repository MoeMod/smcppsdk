#include "extension.h"

#include "sm/sourcemod_usermessages.h"
#include <memory>

#if SOURCE_ENGINE == SE_CSGO
// fix marco conflict from basetypes.h
#undef schema
#include <cstrike15_usermessages.pb.h>
#endif

namespace sm{
    inline namespace sourcemod {
        inline namespace usermessages {
            extern int g_HudMsgNum;
            extern int g_ShakeMsg;
            extern int g_FadeMsg;
            extern int g_SayTextMsg;
            extern int g_SayText2Msg;
            extern int g_HintTextMsg;
            extern int g_TextMsg; // just coincidence, with same name with usermessage.
            extern bool g_SkipNextChatConSound; // does next chat stop sound and log?
            extern bool g_ChatConSnd; // chat with console sound
        	namespace detail {
            void HudText(IRecipientFilter&crf, const hud_text_parms& textparms, const char* pMessage) {

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

            void ShakeScreen(IRecipientFilter& crf, float flAmplitude, float flFrequency, float flDurationTime)
            {
#if SOURCE_ENGINE == SE_CSGO
                std::unique_ptr<CCSUsrMsg_Shake> msg = std::make_unique<CCSUsrMsg_Shake>();
                msg->set_command(0);
                msg->set_local_amplitude(flAmplitude);
                msg->set_frequency(flFrequency);
                msg->set_duration(flDurationTime);
                engine->SendUserMessage(crf, g_ShakeMsg, *msg);
#else
                bf_write* bf = usermsgs->StartBitBufMessage(g_ShakeMsg, players, 1, 0);
                bf->WriteByte(0);
                bf->WriteFloat(flAmplitude);
                bf->WriteFloat(flFrequency);
                bf->WriteFloat(flDurationTime);
                usermsgs->EndMessage();
#endif
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
            void FadeScreen(IRecipientFilter&crf, int iDuration, int iHoldTime, int iFlags, Color color)
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
                engine->SendUserMessage(crf, g_FadeMsg, *msg);
#else // SOURCE_ENGINE != SE_CSGO
                bf_write* bf = usermsgs->StartBitBufMessage(g_FadeMsg, players, 1, 0);
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

            void SayText(IRecipientFilter& crf, int ent_idx, const char* msg, bool chat)
            {
#if SOURCE_ENGINE == SE_CSGO || SOURCE_ENGINE == SE_BLADE
                assert(g_SayTextMsg > 0);
                std::unique_ptr<CCSUsrMsg_SayText> pMsg = std::make_unique<CCSUsrMsg_SayText>();

                pMsg->set_ent_idx(ent_idx);
                pMsg->set_text(msg);
                pMsg->set_chat(chat);
                engine->SendUserMessage(crf, g_SayTextMsg, *pMsg);
#else
                bf_write* pBitBuf = usermsgs->StartBitBufMessage(sm::g_SayTextMsg, players, 1, 0);
                pBitBuf->WriteByte(0);
                pBitBuf->WriteString(buffer);
                pBitBuf->WriteByte(1);
                usermsgs->EndMessage();
#endif
            }
            // Note: in some games(eg: CSGO) supports html tag
            // using CCSUsrMsg_TextMsg instead of CCSUsrMsg_HintText
            void HintText(IRecipientFilter& crf, bool hasColor, const char* pMessage)
            {
                if (hasColor)
                {
                    std::unique_ptr<CCSUsrMsg_TextMsg> msg = std::make_unique<CCSUsrMsg_TextMsg>();
                    msg->set_msg_dst(4);
                    msg->add_params("#SFUI_ContractKillStart");
                    std::string buffer = "</font>" + std::string(pMessage);
                    msg->add_params(buffer);
                    msg->add_params();
                    msg->add_params();
                    msg->add_params();
                    msg->add_params();
                    engine->SendUserMessage(crf, g_HintTextMsg, *msg);
                }
                else
                {
                    std::unique_ptr<CCSUsrMsg_HintText> msg = std::make_unique<CCSUsrMsg_HintText>();
                    msg->set_text(pMessage);
                    engine->SendUserMessage(crf, g_HintTextMsg, *msg);
                }
            }
            // https://github.com/Kxnrl/sourcemod-utils/blob/master/smutils.inc#L704
            // csgo/l4d2/insurgency/etc only
            // otherwise is not accepted
            void SayText2(IRecipientFilter& crf, int ent_idx, bool chat, const char* pMessage)
            {
                std::unique_ptr<CCSUsrMsg_SayText2> msg = std::make_unique<CCSUsrMsg_SayText2>();
                msg->set_ent_idx(ent_idx);
                msg->set_chat(chat);
                msg->set_msg_name(pMessage);
                msg->add_params();
                msg->add_params();
                msg->add_params();
                msg->add_params();
                engine->SendUserMessage(crf, g_SayText2Msg, *msg);
            }
            void ConstructHintTextAttribute(IRecipientFilter&crf, const char* message)
            {
                std::string buffer = std::string(message);
                if (buffer.find("<span") != std::string::npos || buffer.find("<font") != std::string::npos)
                {
                    return HintText(crf, true, message);
                }

                return HintText(crf, false, message);
            }
            }
        }
    }
}