#pragma once

#include "diffbuild.hpp"
#include "inttypes.hpp"

namespace th08
{

struct ReplayManager
{
    static void SaveReplay(const char *replayPath, const char *replayName);
};

DIFFABLE_EXTERN(ReplayManager *, g_ReplayManager);
DIFFABLE_EXTERN(i32, g_UnknownCounter);

} // namespace th08