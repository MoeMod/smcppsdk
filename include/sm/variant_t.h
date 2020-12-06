#pragma once

#include <extensions/IBinTools.h>
#include <smsdk_ext.h>
#include "sourcemod_types.h"

namespace sm {
    class variant_t
    {
    public:
        // float[3] = 12U
        // CBaseHandle
        union
        {
            bool bVal;
            string_t iszVal;
            int iVal;
            float flVal;
            float vecVal[3];
            color32 rgbaVal;
        };

        CBaseHandle eVal; // unsigned long
        fieldtype_t fieldType;
    };
    static_assert(sizeof(variant_t) == 20);
}