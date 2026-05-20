#include "GameManager.hpp"
#include "Global.hpp"

namespace th08
{

DIFFABLE_STATIC(GameManager, g_GameManager);
DIFFABLE_STATIC(ChainElem, g_GameManagerCalcChain);
DIFFABLE_STATIC(ChainElem, g_GameManagerDrawChain);

// STUB: th08 0x4399ac
ZunBool GameManager::IsWithinPlayfield()
{
    return FALSE;
}

// STUB: th08 0x439a2e
i32 GameManager::CalcAntiTamperChecksum()
{
    return 0;
}

// STUB: th08 0x439ac5
i32 GameManager::CalcChecksum(u8 *param_1, i32 param_2)
{
    return 0;
}

// STUB: th08 0x439b29
void GameManager::CollectExtend()
{
}

// STUB: th08 0x439bc7
ChainCallbackResult GameManager::OnUpdate(GameManager *gameManager)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

// STUB: th08 0x43aa03
ChainCallbackResult GameManager::OnDraw(GameManager *gameManager)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

// STUB: th08 0x43aa5c
ZunResult GameManager::RegisterChain()
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x43aaf4
ZunResult GameManager::AddedCallback(GameManager *gameManager)
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x43abd7
void GameManager::GameplaySetupThread()
{
}

// STUB: th08 0x43be2c
ZunResult GameManager::DeletedCallback(GameManager *gameManager)
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x43bfc3
void GameManager::IncreaseSubrank(int amount)
{
}

// STUB: th08 0x43c03f
void GameManager::DecreaseSubrank(int amount)
{
}

// STUB: th08 0x43c0bb
void GameManager::AddToYoukaiGauge(u16 param_1, i32 param_2)
{
}

ZunBool GameManager::IsPhantasmUnlocked()
{
    return FALSE;
}

void GameManager::CutChain()
{
    g_Chain.Cut(&g_GameManagerCalcChain);
    g_Chain.Cut(&g_GameManagerDrawChain);
    if (g_GameManager.globals->score >= 1000000000)
    {
        g_GameManager.globals->score = 999999999;
    }
    g_GameManager.globals->displayScore = g_GameManager.globals->score;
    g_Supervisor.framerateMultiplier = 1.0f;
}

// STUB: th08 0x43c35f
i32 GameManager::GetClockTimeIncrement()
{
    return 0;
}

// STUB: th08 0x43c4b3
void GameManager::AdvanceToNextStage()
{
}

// STUB: th08 0x43c5e1
GameManager::GameManager()
{
    memset(this, 0, sizeof(GameManager));
}

// STUB: th08 0x43c686
void GameManager::InitArcadeRegionParams()
{
}

}; // Namespace th08
