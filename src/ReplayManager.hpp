#pragma once

#include "diffbuild.hpp"

namespace th08
{

struct ReplayManager
{
    static void SaveReplay(char *replayPath, char *replayName);
};

DIFFABLE_EXTERN(ReplayManager *, g_ReplayManager);

} // namespace th08