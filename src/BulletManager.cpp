#include "BulletManager.hpp"

namespace th08
{

DIFFABLE_STATIC(BulletManager, g_BulletManager);
DIFFABLE_STATIC(ChainElem, g_BulletManagerCalcChain);
DIFFABLE_STATIC(ChainElem, g_BulletManagerDrawChain);

void BulletManager::Initialize()
{
}

ZunResult RegisterChain()
{
    return ZUN_SUCCESS;
}

ChainCallbackResult OnUpdate(BulletManager *bulletManager)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult OnDraw(BulletManager *bulletManager)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult AddedCallback(BulletManager *bulletManager)
{
    return ZUN_SUCCESS;
}

ZunResult DeletedCallback(BulletManager *bulletManager)
{
    return ZUN_SUCCESS;
}

} /* namespace th08 */