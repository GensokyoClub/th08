#include "GameManager.hpp"
#include "Global.hpp"

namespace th08
{

DIFFABLE_STATIC(GameManager, g_GameManager);
DIFFABLE_STATIC(ChainElem, g_GameManagerCalcChain);
DIFFABLE_STATIC(ChainElem, g_GameManagerDrawChain);

void GameManager::CutChain()
{
    g_Chain.Cut(&g_GameManagerCalcChain);
    g_Chain.Cut(&g_GameManagerDrawChain);
    if (g_GameManager.m_Globals->m_Score > 999999999)
    {
        g_GameManager.m_Globals->m_Score = 999999999;
    }
    g_GameManager.m_Globals->m_DisplayScore = g_GameManager.m_Globals->m_Score;
    g_Supervisor.framerateMultiplier = 1.0f;
}

}; // Namespace th08
