#pragma once

#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "utils.hpp"

namespace th08
{

struct ReplayData
{
    unknown_fields(0x0, 0x134);
};

struct ReplayManager
{
    static void SaveReplay(const char *replayPath, const char *replayName);
};

DIFFABLE_EXTERN(ReplayManager *, g_ReplayManager);

} // namespace th08