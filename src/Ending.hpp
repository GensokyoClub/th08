#pragma once

#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include "Global.hpp"
#include "ZunResult.hpp"

namespace th08
{

struct EndingInf
{
    static ZunResult RegisterChain();

    unknown_fields(0x0, 0x2ab8);
};

C_ASSERT(sizeof(EndingInf) == 0x2ab8);

} // namespace th08