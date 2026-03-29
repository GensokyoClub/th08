#include "GameManager.hpp"
#include "Global.hpp"

namespace th08
{

DIFFABLE_STATIC(GameManager, g_GameManager);
DIFFABLE_STATIC(ChainElem, g_GameManagerCalcChain);
DIFFABLE_STATIC(ChainElem, g_GameManagerDrawChain);

ZunBool GameManager::IsWithinPlayfield()
{
    return FALSE;
}

i32 GameManager::CalcAntiTamperChecksum()
{
    return 0;
}

i32 GameManager::CalcChecksum(u8 *param_1, i32 param_2)
{
    return 0;
}

void GameManager::CollectExtend()
{
}

ChainCallbackResult GameManager::OnUpdate(GameManager *gameManager)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult GameManager::OnDraw(GameManager *gameManager)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult GameManager::RegisterChain()
{
    return ZUN_SUCCESS;
}

ZunResult GameManager::AddedCallback(GameManager *gameManager)
{
    return ZUN_SUCCESS;
}

void GameManager::GameplaySetupThread()
{
}
ZunResult GameManager::DeletedCallback(GameManager *gameManager)
{
    return ZUN_SUCCESS;
}

void GameManager::IncreaseSubrank(int amount)
{
}

void GameManager::DecreaseSubrank(int amount)
{
}

void GameManager::AddToYoukaiGauge(u16 param_1, i32 param_2)
{
}

ZunBool GameManager::FinalBCleared()
{
    return FALSE;
}

ZunBool GameManager::FinalBClearedWithAnyTeam()
{
    return FALSE;
}

ZunBool GameManager::FinalACleared()
{
    return FALSE;
}

ZunBool GameManager::FinalAClearedWithAnyTeam()
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

i32 GameManager::GetClockIncrement()
{
    return 0;
}

void GameManager::AdvanceToNextStage()
{
}

GameManager::GameManager()
{
    memset(this, 0, sizeof(GameManager));
}

void GameManager::InitArcadeRegionParams()
{
}

}; // Namespace th08
