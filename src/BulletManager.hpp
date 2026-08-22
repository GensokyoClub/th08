#pragma once

#include "AnmManager.hpp"
#include "Global.hpp"

namespace th08
{

struct BulletTypeSprites
{
};

struct Laser
{
};

struct Bullet
{
};

struct BulletManager
{
    void Initialize();

    // STUB: th08 0x415c60
    void FUN_00415c60()
    {
    }

    static ZunResult RegisterChain(const char *filename);
    static ChainCallbackResult OnUpdate(BulletManager *bulletManager);
    static ChainCallbackResult OnDraw(BulletManager *bulletManager);
    static ZunResult AddedCallback(BulletManager *bulletManager);
    static ZunResult DeletedCallback(BulletManager *bulletManager);
};

DIFFABLE_EXTERN(BulletManager, g_BulletManager);

} /* namespace th08 */
