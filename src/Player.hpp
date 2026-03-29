#pragma once

#include "Global.hpp"

namespace th08
{

struct PlayerRawShtFile
{
};

struct Player
{
    static ZunResult RegisterChain(u32 param);
    static ChainCallbackResult OnUpdate(Player *player);
    static ChainCallbackResult OnDrawHighPrio(Player *player);
    static ZunResult AddedCallback(Player *player);
    static ZunResult DeletedCallback(Player *player);
    static void CutChain();

    static ZunResult LoadShtFile(PlayerRawShtFile **header, const char *path);
};

DIFFABLE_EXTERN(Player, g_Player);

} /* namespace th08 */