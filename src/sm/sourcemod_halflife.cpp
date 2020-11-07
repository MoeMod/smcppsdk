#include "extension.h"

#include "sm/sourcemod_halflife.h"


namespace sm {
    inline namespace sourcemod {
        inline namespace halflife {
            int GetClientsInRange(const Vector &vOrigin, ClientRangeType rangeType, int *outPlayers, int maxPlayers) {


                CBitVec<ABSOLUTE_PLAYER_LIMIT> players;
                engine->Message_DetermineMulticastRecipients(rangeType == RangeType_Audibility, vOrigin, players);

                int curPlayers = 0;

                int index = players.FindNextSetBit(0);
                while (index > -1 && curPlayers < maxPlayers)
                {
                    int entidx = index + 1;
                    IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(entidx);
                    if (pPlayer && pPlayer->IsInGame())
                    {
                        outPlayers[curPlayers++] = entidx;
                    }

                    index = players.FindNextSetBit(index + 1);
                }

                return curPlayers;

            }

        }
    }
}