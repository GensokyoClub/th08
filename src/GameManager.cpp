#include "th_pch.h"

#include "Background.hpp"

#include "EnemyManager.hpp"
#include "EffectManager.hpp"

#include "BulletManager.hpp"
#include "Config.hpp"
#include "GameManager.hpp"
#include "Global.hpp"
#include "Gui.hpp"
#include "ItemManager.hpp"
#include "Player.hpp"
#include "ReplayManager.hpp"
#include "ResultScreen.hpp"
#include "SoundPlayer.hpp"
#include "SpellCard.hpp"
#include "ScreenEffect.hpp"

#include "i18n.hpp"

namespace th08
{

DIFFABLE_STATIC(GameManager, g_GameManager);
DIFFABLE_STATIC(ChainElem, g_GameManagerCalcChain);
DIFFABLE_STATIC(ChainElem, g_GameManagerDrawChain);

DIFFABLE_STATIC_ASSIGN(i32, g_TimeRequirementParams[MAX_STAGES][EXTRA]) =
{
    { 2000, 2500, 2700, 3000 }, // Stage 1
    { 6500, 7200, 7200, 7200 }, // Stage 2
    { 7500, 8500, 8800, 8800 }, // Stage 3
    { 9999, 9999, 9999, 9999 }, // Stage 4A
    { 7500, 8500, 8500, 8500 }, // Stage 4B
    { 9999, 9999, 9999, 9999 }, // Stage 5
    { 0,    0,    0,    0    }, // Stage 6A
    { 0,    0,    0,    0    }, // Stage 6B
    { 0,    0,    0,    0    }, // Extra Stage (unused, see comment in GameplaySetupThread)
};

struct RankInfo
{
    i32 rank;
    i32 minRank;
    i32 maxRank;
};

DIFFABLE_STATIC_ASSIGN(RankInfo, g_RankParams[MAX_DIFFICULTIES]) =
{
    { 10, 8,  16 },
    { 10, 8,  16 },
    { 8,  8,  12 },
    { 8,  8,  12 },
    { 16, 15, 16 },
};

struct SpellcardMusicEntry
{
    i32 spellcardNumber;
    i32 songNumber;
    const char *songPath;
    i32 songNameSpriteIdx;
    ZunBool musicPausesInSpellPractice;
};

SpellcardMusicEntry g_SpellcardMusicInfo[] = {{1, 1, "th08_00.mid", 0, FALSE},
                                              {12, 2, "th08_03.mid", 1, FALSE},
                                              {16, 3, "th08_04.mid", 0, FALSE},
                                              {31, 4, "th08_05.mid", 1, FALSE},
                                              {35, 5, "th08_06.mid", 0, FALSE},
                                              {53, 6, "th08_07.mid", 1, FALSE},
                                              {76, 8, "th08_09.mid", 1, FALSE},
                                              {99, 9, "th08_10.mid", 1, FALSE},
                                              {118, 11, "th08_12.mid", 1, FALSE},
                                              {122, 12, "th08_13.mid", 0, FALSE},
                                              {142, 13, "th08_14.mid", 1, FALSE},
                                              {146, 15, "th08_13b.mid", 2, TRUE},
                                              {150, 12, "th08_13.mid", 0, FALSE},
                                              {170, 14, "th08_15.mid", 1, FALSE},
                                              {190, 15, "th08_13b.mid", 2, TRUE},
                                              {193, 16, "th08_18.mid", 0, FALSE},
                                              {204, 17, "th08_19.mid", 1, FALSE},
                                              {222, 20, "th08_20.mid", 2, FALSE},
                                              {-1, 0, " ", 0, FALSE}};

ZunBool GameManager::ShouldPauseMusicInSpellPractice(i32 spellcardNumber)
{
    i32 i = 0;

    while (g_SpellcardMusicInfo[i].spellcardNumber >= 0)
    {
        if (g_GameManager.currentSpellCardNumber <= g_SpellcardMusicInfo[i].spellcardNumber)
        {
            return g_SpellcardMusicInfo[i].musicPausesInSpellPractice;
        }

        i++;
    }

    return FALSE;
}

i32 GameManager::GetSongNameSpriteIdx(i32 spellcardNumber)
{
    i32 i = 0;

    while (g_SpellcardMusicInfo[i].spellcardNumber >= 0)
    {
        if (g_GameManager.currentSpellCardNumber <= g_SpellcardMusicInfo[i].spellcardNumber)
        {
            return g_SpellcardMusicInfo[i].songNameSpriteIdx;
        }

        i++;
    }

    return FALSE;
}

ZunBool GameManager::IsWithinPlayfield(float x, float y, float w, float h)
{
    if ((w / 2) + x < 0)
    {
        return 0;
    }

    if (x - (w / 2) > ARCADE_WIDTH)
    {
        return 0;
    }

    if ((h / 2) + y < 0.0f)
    {
        return 0;
    }

    if (y - (h / 2) > ARCADE_HEIGHT)
    {
        return 0;
    }

    return 1;
}

i32 GameManager::CalcAntiTamperChecksum()
{
    i32 sum;

    // There is zero chance ZUN actually used intptr_t here, but the codegen matches
    // and not making assumptions about pointer size is always nice
    sum = CalcChecksum((u8 *)&g_GameManager.globals->rng1,
                       (intptr_t)&globals->antiTamperValue - (intptr_t)&globals->rng1);
    sum += CalcChecksum((u8 *)&g_GameManager.globals->rng8, sizeof(g_GameManager.globals->rng8));
    sum += CalcChecksum((u8 *)g_GameManager.cfg, sizeof(GameConfiguration));
    sum += CalcChecksum((u8 *)&g_Supervisor.cfg, sizeof(GameConfiguration));
    sum += CalcChecksum((u8 *)&this->hscr, sizeof(Hscr));

    return sum;
}

i32 GameManager::CalcChecksum(u8 *address, i32 size)
{
    i32 sum;
    i32 i;

    for (sum = 0, i = 0; i < size; i++, address++)
    {
        sum += *address;
        g_GameManager.globals->antiTamperValue += g_GameManager.globals->rng8[2];
    }

    return sum;
}

void GameManager::CollectExtend()
{
    if (this->GetLives() < 8)
    {
        this->AddLives(1);
        g_SoundPlayer.PlaySoundByIdx(SOUND_1UP, 0);
        this->IncreaseSubrank(200);
        g_Gui.flags.lifeDisplayUpdateFrames = 2;
    }
    else if (this->GetBombsRemaining() < 8)
    {
        this->AddToBombCount(1);
        g_SoundPlayer.PlaySoundByIdx(SOUND_1UP, 0);
        this->IncreaseSubrank(200);
        g_Gui.flags.bombDisplayUpdateFrames = 2;
    }
}

#pragma var_order(sum, i, scoreIncrement)
ChainCallbackResult GameManager::OnUpdate(GameManager *gameManager)
{
    i32 i;
    i32 sum;
    u32 scoreIncrement;

    g_GameManager.unk3DB94++;

    if (gameManager->flags.gameState != GAME_STATE_DEFAULT)
    {
        if (gameManager->flags.gameState == GAME_STATE_STAGE_CLEAR)
        {
            gameManager->flags.gameState = GAME_STATE_FINISHING_STAGE;
            g_GameManager.loadState = GAME_LOAD_IN_PROGRESS;
            g_GameManager.nextSupervisorState = SupervisorState_ExitGame;

            if (!g_GameManager.IsReplay())
            {
                // Set the current stage as cleared.
                if (g_GameManager.globals->numRetries == 0)
                {
                    g_GameManager.clrdData[g_GameManager.character].difficultiesClearedWithoutRetries[g_GameManager.difficulty] |= g_GameManager.stageMask;
                    g_GameManager.clrdData[SHOT_ALL].difficultiesClearedWithoutRetries[g_GameManager.difficulty] |= g_GameManager.stageMask;
                }
                g_GameManager.clrdData[g_GameManager.character].difficultiesClearedWithRetries[g_GameManager.difficulty] |= g_GameManager.stageMask;
                g_GameManager.clrdData[SHOT_ALL].difficultiesClearedWithRetries[g_GameManager.difficulty] |= g_GameManager.stageMask;
            }

            gameManager->globals->displayScore = gameManager->globals->score;

            if (gameManager->IsPracticeMode())
            {
                g_GameManager.globals->displayScore = g_GameManager.globals->score;
                g_GameManager.nextSupervisorState = SupervisorState_ResultScreenFromGame;

                return CHAIN_CALLBACK_RESULT_BREAK;
            }

            if (g_GameManager.currentStage != STAGE6A
                && g_GameManager.currentStage != STAGE6B
                && g_GameManager.currentStage != EXTRASTAGE)
            {
                if (g_GameManager.IsReplay())
                {
                    i32 replayStage = 0;
                    i32 i;

                    // Find the next stage in the replay. Sometimes there
                    // might be a stage missing from a replay.
                    for (i = g_GameManager.currentStage + 1; i < MAX_STAGES; i++)
                    {
                        if (g_ReplayManager->IsStageInReplay(i))
                        {
                            replayStage = i;
                            break;
                        }
                    }

                    if (replayStage == 0)
                    {
                        g_Supervisor.curState = SupervisorState_FinishReplay;
                    }
                    else
                    {
                        g_GameManager.currentStage = replayStage;
                        g_Supervisor.curState = SupervisorState_GameManagerReInit;
                    }
                }
                else
                {
                    // Finish the game if the time exceeds 5:00 AM.
                    if (g_GameManager.GetClockTime() >= CLOCK_TIME_5_00)
                    {
                        g_GameManager.flags.unk4 = 0;
                        g_GameManager.nextSupervisorState = SupervisorState_Ending;

                        return CHAIN_CALLBACK_RESULT_BREAK;
                    }

                    g_GameManager.AdvanceToNextStage();
                    g_Supervisor.curState = SupervisorState_GameManagerReInit;
                }
            }
            else if (g_GameManager.IsReplay())
            {
                g_GameManager.nextSupervisorState = SupervisorState_FinishReplay;
            }
            else
            {
                if (g_GameManager.difficulty >= EXTRA)
                {
                    if (g_GameManager.difficulty == EXTRA)
                    {
                        g_GameManager.clrdData[g_GameManager.character].difficultiesClearedWithoutRetries[g_GameManager.difficulty] |= SPELL_PRACTICE_UNLOCKED_FLAG;
                        g_GameManager.clrdData[SHOT_ALL].difficultiesClearedWithRetries[g_GameManager.difficulty] |= SPELL_PRACTICE_UNLOCKED_FLAG;

                    }

                    g_GameManager.plst.playData[g_GameManager.difficulty].clears++;
                    g_GameManager.flags.unk4 = 1;

                    g_GameManager.globals->displayScore = g_GameManager.globals->score;
                    g_GameManager.nextSupervisorState = SupervisorState_ResultScreenFromGame;

                    return CHAIN_CALLBACK_RESULT_BREAK;
                }

                g_GameManager.flags.unk4 = 1;
                g_GameManager.nextSupervisorState = SupervisorState_Ending;

                return CHAIN_CALLBACK_RESULT_BREAK;
            }

            if (g_GameManager.nextSupervisorState < 0)
            {
                g_Gui.CaptureArcade();
            }
        }

        // This is for the "Press Shot Button" prompt when you clear a stage.
        // The other conditions are for automatically advancing the stage
        // without user input.
        if (WAS_PRESSED(TH_BUTTON_SELECTMENU)
            || g_GameManager.IsReplay()
            || g_GameManager.currentStage == STAGE6A
            || g_GameManager.currentStage == STAGE6B
            || g_GameManager.currentStage == EXTRASTAGE)
        {
            gameManager->flags.gameState = GAME_STATE_DEFAULT;
            if (g_GameManager.nextSupervisorState >= SupervisorState_Init)
            {
                g_Supervisor.curState = g_GameManager.nextSupervisorState;
            }
        }
    }

    if (gameManager->loadState != GAME_LOAD_FINISHED)
    {
        if (gameManager->loadState == GAME_LOAD_FAILED)
        {
            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
        }
        else
        {
            gameManager->loadFrames++;
            return CHAIN_CALLBACK_RESULT_BREAK;
        }

    }

    if (gameManager->unk3de28 != 0)
    {
        Gui::UpdateEnemyNameTexture();
        g_AnmManager->ReleaseSurface(8);
        g_Supervisor.loadingVmsHaveBeenSetup = 0;

        if (gameManager->unk3de28 == 1)
        {
            if (!g_GameManager.IsSpellPractice())
            {
                g_Supervisor.PlayMusic(0, g_StageBgmIndices[g_GameManager.currentStage][0]);
            }
            else
            {
                i32 i = 0;

                while (g_SpellcardMusicInfo[i].spellcardNumber >= 0)
                {
                    if (g_GameManager.currentSpellCardNumber <= g_SpellcardMusicInfo[i].spellcardNumber)
                    {
                        g_Supervisor.PlayMusic(0, g_SpellcardMusicInfo[i].songNumber);
                        break;
                    }

                    i++;
                }
            }
        }

        gameManager->unk3de28 = 0;
    }

    // Pause menu
    if (!gameManager->showRetryMenu
        && !gameManager->pauseState
        && !gameManager->IsDemoMode()
        && !gameManager->stickyInput
        && WAS_PRESSED(TH_BUTTON_MENU))
    {
        gameManager->pauseState = PAUSE_STATE_PAUSE_HIT;

        g_GameManager.arcadeRegionTopLeftPos.x = ARCADE_LEFT;
        g_GameManager.arcadeRegionTopLeftPos.y = ARCADE_TOP;
        g_GameManager.arcadeRegionSize.x = ARCADE_WIDTH;
        g_GameManager.arcadeRegionSize.y = ARCADE_HEIGHT;

        gameManager->unk3DB98 = 1;

        g_SoundPlayer.Pause();
        g_SoundPlayer.PlaySoundByIdx(SOUND_PAUSE, 0);

        g_Supervisor.UpdateGameTime();

        g_Rng.SaveSeed();

        gameManager->hscr.numPauses++;
        g_GameManager.UpdateAntiTamper();

        g_Rng.RestoreSeed();
    }

    g_Supervisor.viewport.X = gameManager->arcadeRegionTopLeftPos.x;
    g_Supervisor.viewport.Y = gameManager->arcadeRegionTopLeftPos.y;
    g_Supervisor.viewport.Width = gameManager->arcadeRegionSize.x;
    g_Supervisor.viewport.Height = gameManager->arcadeRegionSize.y;
    g_Supervisor.viewport.MinZ = 0.0f;
    g_Supervisor.viewport.MaxZ = 1.0f;

    g_AnmManager->ClearCameraSettings();

    if (g_GameManager.IsReplay()
        && g_GameManager.replayMode == REPLAY_MODE_SLOWDOWN
        && !g_Gui.IsDialoguePresent())
    {
        gameManager->playTimeSlowMode++;

        if ((g_Supervisor.unk198 < 20 && gameManager->playTimeSlowMode % 3 != 0)
            || ((g_Supervisor.unk198 >= 20 && g_Supervisor.unk198 < 30)
                && gameManager->playTimeSlowMode % 2 != 0)
            || ((g_Supervisor.unk198 >= 30 && g_Supervisor.unk198 < 40)
                && gameManager->playTimeSlowMode % 3 == 0) ||
            ((g_Supervisor.unk198 >= 40 && g_Supervisor.unk198 < 50)
             && gameManager->playTimeSlowMode % 6 == 0))
        {
            return CHAIN_CALLBACK_RESULT_BREAK;
        }
    }

    if (gameManager->IsDemoMode())
    {
        if (WAS_PRESSED(TH_BUTTON_ANY))
        {
            g_Supervisor.curState = SupervisorState_TitleScreen;
        }

        gameManager->demoFrameCount++;

        // Fade out arcade and music in the demo play after a certain time
        if ((gameManager->currentDemoReplay == 0 && gameManager->demoFrameCount == 6000)
            || (gameManager->currentDemoReplay == 1 && gameManager->demoFrameCount == 4800)
            || (gameManager->currentDemoReplay == 2 && gameManager->demoFrameCount == 4920)
            || (gameManager->currentDemoReplay == 3 && gameManager->demoFrameCount == 6900))
        {
            ScreenEffect::RegisterChain(SCREEN_EFFECT_ARCADE_FADE_OUT, 120, 0, 0, 0, 21);
            g_Supervisor.FadeOutMusic(3.0f);
        }

        // Go back to title screen in the demo play two seconds after the fade out starts
        if ((gameManager->currentDemoReplay == 0 && gameManager->demoFrameCount >= 6000 + (2 * 60))
            || (gameManager->currentDemoReplay == 1 && gameManager->demoFrameCount >= 4800 + (2 * 60))
            || (gameManager->currentDemoReplay == 2 && gameManager->demoFrameCount >= 4920 + (2 * 60))
            // Copy paste mistake? Given the context, it should be >=, not ==
            || (gameManager->currentDemoReplay == 3 && gameManager->demoFrameCount == 6900 + (2 * 60)))
        {
            g_Supervisor.curState = SupervisorState_TitleScreen;
            return CHAIN_CALLBACK_RESULT_BREAK;
        }
    }

    g_GameManager.globals->antiTamperValue = g_GameManager.globals->rng1[2];
    sum = gameManager->CalcAntiTamperChecksum();
    g_GameManager.antiTamperExpectedValue = (f32)sum + (f32)g_GameManager.globals->rng7[3];

    for (i = 0; i < ARRAY_SIZE(gameManager->globals->rng1); i++)
    {
        if (gameManager->globals->rng1[i] < ANTITAMPER_RNG_ADD
            || gameManager->globals->rng1[i] > ANTITAMPER_RNG_ADD + ANTITAMPER_RNG_RANGE)
        {
            g_GameManager.antiTamperExpectedValue = -9999;
        }
    }

    for (i = 0; i < ARRAY_SIZE(gameManager->globals->rng3); i++)
    {
        if (gameManager->globals->rng3[i] < ANTITAMPER_RNG_ADD
            || gameManager->globals->rng3[i] > ANTITAMPER_RNG_ADD + ANTITAMPER_RNG_RANGE)
        {
            g_GameManager.antiTamperExpectedValue = -9999;
        }
    }

    gameManager->flags.isActive = (!gameManager->showRetryMenu && gameManager->pauseState != PAUSE_STATE_NOT_PAUSED);

    for (i = 0; i < ARRAY_SIZE(gameManager->globals->rng2); i++)
    {
        if (gameManager->globals->rng2[i] < ANTITAMPER_RNG_ADD
            || gameManager->globals->rng2[i] > ANTITAMPER_RNG_ADD + ANTITAMPER_RNG_RANGE)
        {
            g_GameManager.antiTamperExpectedValue = -9999;
        }
    }

    for (i = 0; i < ARRAY_SIZE(gameManager->globals->rng7); i++)
    {
        if (gameManager->globals->rng7[i] < ANTITAMPER_RNG_ADD
            || gameManager->globals->rng7[i] > ANTITAMPER_RNG_ADD + ANTITAMPER_RNG_RANGE)
        {
            g_GameManager.antiTamperExpectedValue = -9999;
        }
    }

    // ZUN bloat: only clearing the Z buffer but also providing the clear color?
    g_Supervisor.d3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, g_Background.skyFog.color.d3dColor, 1.0f, 0.0);

    // If pause or retry menu are active, interrupt normal gameplay.
    if (gameManager->pauseState == PAUSE_STATE_PAUSE_HIT
        || gameManager->pauseState == PAUSE_STATE_PAUSED
        || gameManager->showRetryMenu)
    {
        return CHAIN_CALLBACK_RESULT_BREAK;
    }

    if (gameManager->globals->score >= MAX_SCORE + 1)
    {
        gameManager->globals->score = MAX_SCORE;
    }

    // Handle score increment
    if (gameManager->globals->displayScore != gameManager->globals->score)
    {
        if (gameManager->globals->score < gameManager->globals->displayScore)
        {
            gameManager->globals->score = gameManager->globals->displayScore;
        }

        scoreIncrement = (gameManager->globals->score - gameManager->globals->displayScore) / 32;

        if (scoreIncrement >= MAX_SCORE_INCREMENT)
        {
            scoreIncrement = MAX_SCORE_INCREMENT;
        }
        else if (scoreIncrement == 0)
        {
            scoreIncrement = 1;
        }

        if (gameManager->globals->scoreIncrement < scoreIncrement)
        {
            gameManager->globals->scoreIncrement = scoreIncrement;
        }

        if ((gameManager->globals->displayScore + gameManager->globals->scoreIncrement) > gameManager->globals->score)
        {
            gameManager->globals->scoreIncrement = gameManager->globals->score - gameManager->globals->displayScore;
        }

        gameManager->globals->displayScore += gameManager->globals->scoreIncrement;

        if (gameManager->globals->displayScore >= gameManager->globals->score)
        {
            gameManager->globals->scoreIncrement = 0;
            gameManager->globals->displayScore = gameManager->globals->score;
        }

        if (gameManager->globals->displayedHighScore < gameManager->globals->displayScore)
        {
            gameManager->globals->displayedHighScore = gameManager->globals->displayScore;
            gameManager->globals->continuesUsedInHighScore = gameManager->globals->numRetries;
        }
    }

    for (i = 0; i < ARRAY_SIZE(gameManager->globals->rng4); i++)
    {
        if (gameManager->globals->rng4[i] < ANTITAMPER_RNG_ADD
            || gameManager->globals->rng4[i] > ANTITAMPER_RNG_ADD + ANTITAMPER_RNG_RANGE)
        {
            g_GameManager.antiTamperExpectedValue = -9999;
        }
    }

    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng5); i++)
    {
        if (gameManager->globals->rng5[i] < ANTITAMPER_RNG_ADD
            || gameManager->globals->rng5[i] > ANTITAMPER_RNG_ADD + ANTITAMPER_RNG_RANGE)
        {
            g_GameManager.antiTamperExpectedValue = -9999;
        }
    }

    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng8); i++)
    {
        if (gameManager->globals->rng8[i] < ANTITAMPER_RNG_ADD
            || gameManager->globals->rng8[i] > ANTITAMPER_RNG_ADD + ANTITAMPER_RNG_RANGE)
        {
            g_GameManager.antiTamperExpectedValue = -9999;
        }
    }

    if (g_GameManager.cfg->slowMode)
    {
        g_GameManager.stickyInput = 0;

        gameManager->playTimeSlowMode++;

        if ((g_BulletManager.numActiveBullets >= 320 && gameManager->playTimeSlowMode % 3 == 0)
            || ((g_BulletManager.numActiveBullets < 320 && g_BulletManager.numActiveBullets >= 224)
                && gameManager->playTimeSlowMode % 4 == 0)
            || ((g_BulletManager.numActiveBullets < 224 && g_BulletManager.numActiveBullets >= 128)
                && gameManager->playTimeSlowMode % 5 == 0))
        {
            g_GameManager.stickyInput = 1;
            return CHAIN_CALLBACK_RESULT_BREAK;
        }

        if (g_BulletManager.numActiveBullets < 128)
        {
            gameManager->playTimeSlowMode = 0;
        }
    }

    if (g_GameManager.IsTampered())
    {
        // ZUN commented out code here most likely as well
    }

    gameManager->unk3ddc0++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult GameManager::OnDraw(GameManager *gameManager)
{
    if (gameManager->pauseState != PAUSE_STATE_NOT_PAUSED)
    {
        gameManager->pauseState = PAUSE_STATE_PAUSED;
    }

    if (g_Supervisor.curState != SupervisorState_GameManager)
    {
        return CHAIN_CALLBACK_RESULT_BREAK;
    }

    if (gameManager->flags.gameState == GAME_STATE_SKIP_DRAWING)
    {
        return CHAIN_CALLBACK_RESULT_BREAK;
    }

    if (gameManager->loadState != GAME_LOAD_FINISHED)
    {
        return CHAIN_CALLBACK_RESULT_BREAK;
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult GameManager::RegisterChain()
{
    GameManager *gameManager = &g_GameManager;

    g_GameManagerCalcChain.SetCallback((ChainCallback) GameManager::OnUpdate);
    g_GameManagerCalcChain.addedCallback = (ChainLifetimeCallback) GameManager::AddedCallback;
    g_GameManagerCalcChain.deletedCallback = (ChainLifetimeCallback) GameManager::DeletedCallback;
    g_GameManagerCalcChain.arg = gameManager;

    gameManager->unk3ddc0 = 0;

    if (g_Chain.AddToCalcChain(&g_GameManagerCalcChain, CHAIN_PRIO_CALC_GAMEMANAGER) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    g_GameManagerDrawChain.SetCallback((ChainCallback) GameManager::OnDraw);
    g_GameManagerDrawChain.arg = gameManager;

    g_Chain.AddToDrawChain(&g_GameManagerDrawChain, CHAIN_PRIO_DRAW_GAMEMANAGER);

    return ZUN_SUCCESS;
}

ZunResult GameManager::AddedCallback(GameManager *gameManager)
{
    if (g_Supervisor.curState != SupervisorState_GameManagerReInit
        && g_Supervisor.curState != SupervisorState_SpellcardPracticeRestart
        && g_Supervisor.curState != SupervisorState_GameManagerNextStageWeird)
    {
        g_Supervisor.isInitialStageLoad = TRUE;
    }
    else
    {
        g_Supervisor.isInitialStageLoad = FALSE;
    }

    g_GameManager.loadState = GAME_LOAD_IN_PROGRESS;

    if (g_Supervisor.wantedState2 == SupervisorState_TitleScreen)
    {
        Float3 pos(500.0f, 440.0f, 0.0f);

        g_Supervisor.ShowLoadingVmsAndCapture(&pos);
        g_Supervisor.StartEffect(0);
    }
    else
    {
        Float3 pos(280.0f, 430.0f, 0.0f);

        g_Supervisor.ShowLoadingVmsAndCapture(&pos);
    }

    if (gameManager->flags.gameState >= GAME_STATE_STAGE_CLEAR)
    {
        gameManager->flags.gameState = GAME_STATE_SKIP_DRAWING;
    }

    g_Supervisor.ThreadStart((LPTHREAD_START_ROUTINE)GameManager::GameplaySetupThread, NULL);

    return ZUN_SUCCESS;
}

static void IncrementTruncate(u32 *value, u32 threshold)
{
    // ?! why not use the parameter?
    if (*value < 999999)
    {
        (*value)++;
    }
}

#pragma var_order(gameManager, random)
void GameManager::GameplaySetupThread(LPVOID param)
{
    GameManager *gameManager = &g_GameManager;
    i32 random;

    gameManager->loadFrames = 0;
    g_Supervisor.systemTime = timeGetTime();
    gameManager->stageMask = ZUN_BIT(gameManager->currentStage);
    gameManager->currentStage2 = gameManager->currentStage;

    if (gameManager->difficulty < EXTRA)
    {
        gameManager->difficultyMask = ZUN_BIT(gameManager->difficulty);
    }
    else
    {
        gameManager->difficultyMask = ZUN_BIT(EASY) | ZUN_BIT(NORMAL) | ZUN_BIT(HARD) | ZUN_BIT(LUNATIC);
    }

    gameManager->characterShotType = gameManager->character + gameManager->shotType;

    g_Supervisor.framerateMultiplier = 1.0f;

    gameManager->flags.unk10 = 0;

    if (IsInitialStageLoad() || gameManager->IsSpellPractice() || g_GameManager.IsPracticeMode() || g_GameManager.difficulty >= EXTRA)
    {
        if (gameManager->cfg != NULL)
        {
            ZUN_DELETE(gameManager->cfg);
        }

        if (gameManager->globals != NULL)
        {
            ZUN_DELETE(gameManager->globals);
        }


        random = g_Rng.GetRandomU32InRange(0xffff) + 16;
        gameManager->decoyBuffer = ZUN_ALLOC(random);

        // We don't know the true names of these data types, sadly.
        gameManager->cfg = ZUN_NEW(GameConfiguration, "");
        gameManager->globals = ZUN_NEW(ZunGlobals, "");

        GameManager::InitializeAntiTamper();

        *gameManager->cfg = g_Supervisor.cfg;

        // This buffer is probably a poor man's attempt at tricking reverse
        // engineers like us. Fortunately, it is not effective.
        ZUN_FREE(gameManager->decoyBuffer);

        gameManager->powerItemCountForScore = 0;
        gameManager->SetYoukaiGauge(0);

        gameManager->SetClockTime(g_GameManager.currentStage == EXTRASTAGE ? CLOCK_TIME_2_00 : CLOCK_TIME_11_00);

        if (g_GameManager.difficulty >= EXTRA)
        {
            gameManager->cfg->lifeCount = 2;
        }
        if (g_GameManager.IsPracticeMode())
        {
            gameManager->cfg->lifeCount = 8;
        }

        if (Player::RegisterChain(0) != ZUN_SUCCESS)
        {
            if (g_Supervisor.subthreadCloseRequestActive)
            {
                return;
            }

            g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_INITIALIZE_PLAYER);
            goto err;
        }

        if (!g_GameManager.IsReplay())
        {
            g_GameManager.SetLives(gameManager->cfg->lifeCount);
            g_GameManager.SetBombCount(g_Player.player1ShtFile->bombCount);
        }

        gameManager->InitArcadeRegionParams();

        gameManager->SetPower(0);
        gameManager->unk3de04 = 0;
        g_GameManager.unk3DBA4 = 0;
        g_GameManager.unk3DBA0 = 0;
        gameManager->globals->displayScore = 0;
        gameManager->globals->score = 0;
        gameManager->globals->scoreIncrement = 0;
        gameManager->globals->displayedHighScore = 100000;
        gameManager->globals->numRetries = 0;
        gameManager->globals->graze = 0;
        gameManager->globals->pointItemsCollected = 0;

        if (gameManager->difficulty >= EXTRA || gameManager->IsPracticeMode() || gameManager->IsSpellPractice())
        {
            gameManager->cfg->slowMode = 0;
        }

        switch (g_GameManager.difficulty)
        {
        case EASY:
            gameManager->globals->pointItemValue = 60000;
            break;
        case NORMAL:
            gameManager->globals->pointItemValue = 100000;
            break;
        case HARD:
            gameManager->globals->pointItemValue = 200000;
            break;
        case LUNATIC:
            gameManager->globals->pointItemValue = 300000;
            break;
        case EXTRA:
            gameManager->globals->pointItemValue = 300000;
            break;
        }

        gameManager->globals->pointItemExtendsSoFar = 0;

        ItemManager::UpdatePointItemExtendThreshold();
        if (GameManager::InitScore() != ZUN_SUCCESS)
        {
            goto err;
        }

        GameManager::InitRankParams(gameManager);

        gameManager->SetDeaths(0);
        gameManager->SetDeathsInStage(0);
        gameManager->SetBombsUsed(0);
        gameManager->SetBombsUsedInStage(0);

        gameManager->globals->spellcardsCaptured = 0;

        gameManager->unk3de10 = 0;
        gameManager->unk3de18 = 0;
        gameManager->unk3de1c = 0;

        if (!g_GameManager.IsReplay() && !g_GameManager.IsSpellPractice())
        {
            if (!gameManager->cfg->slowMode)
            {
                IncrementTruncate(&g_GameManager.plst.playData[g_GameManager.difficulty].attemptsTotal, 999999);
                IncrementTruncate(&g_GameManager.plst.playData[MAX_DIFFICULTIES + 1].attemptsTotal, 999999);
                IncrementTruncate(
                    &g_GameManager.plst.playData[g_GameManager.difficulty].attemptsPerCharacter[gameManager->character],
                    999999);
                IncrementTruncate(
                    &g_GameManager.plst.playData[MAX_DIFFICULTIES + 1].attemptsPerCharacter[gameManager->character],
                    999999);

                if (g_Supervisor.curState == SupervisorState_GameManagerRestartFromBeginning)
                {
                    IncrementTruncate(&g_GameManager.plst.playData[g_GameManager.difficulty].restarts, 999999);
                    IncrementTruncate(&g_GameManager.plst.playData[MAX_DIFFICULTIES + 1].restarts, 999999);
                }

                if (g_GameManager.IsPracticeMode() && !g_GameManager.IsSpellPractice())
                {
                    IncrementTruncate(&g_GameManager.plst.playData[g_GameManager.difficulty].practices, 999999);
                    IncrementTruncate(&g_GameManager.plst.playData[MAX_DIFFICULTIES + 1].practices, 999999);
                }
            }
        }
        else
        {
            gameManager->cfg->slowMode = 0;
        }
    }
    else
    {
        gameManager->globals->displayScore = gameManager->globals->score;
        gameManager->globals->scoreIncrement = 0;
        gameManager->SetDeathsInStage(0);
        gameManager->SetBombsUsedInStage(0);

        if (Player::RegisterChain(0) != ZUN_SUCCESS)
        {
            if (g_Supervisor.subthreadCloseRequestActive)
            {
                return;
            }

            g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_INITIALIZE_PLAYER);
            goto err;
        }
    }

    gameManager->subRank = 0;
    gameManager->globals->pointItemsCollectedInStage = 0;
    gameManager->globals->grazeInStage = 0;
    gameManager->pauseState = PAUSE_STATE_NOT_PAUSED;
    gameManager->flags.unk7 = 0;
    gameManager->flags.unk13 = 0;
    gameManager->unk3de14 = 0;
    gameManager->unk3de20 = 0;
    gameManager->unk3de24 = 0;
    gameManager->globals->youkaiGaugeCopy = gameManager->globals->youkaiGauge;
    gameManager->globals->currentTimeOrbs = 0;
    gameManager->globals->totalTimeOrbs = 0;

    if (!g_GameManager.IsSpellPractice())
    {
#ifndef FIX_REALLY_BAD_BUGS
        // There is a funny bug here. This retrieves the amount of time
        // needed to get the Last Spell of the stage boss and pass only
        // 30 minutes in the lock time. This works fine until the Extra
        // Stage: the table is arranged by all the stages (STAGE1 until
        // EXTRASTAGE) and difficulty (EASY until LUNATIC). But the
        // Extra Stage is considered by the game as the 5th difficulty.
        //
        // So what happens? This performs an out-of-bounds read! It reads
        // the first value of g_RankParams, which is 10. That is why the
        // Last Spell time threshold is 10 in the Extra Stage.
        gameManager->globals->lastSpellTimeOrbThreshold = g_TimeRequirementParams[gameManager->currentStage][g_GameManager.difficulty];
#else
        if (g_GameManager.difficulty < EXTRA)
        {
            gameManager->globals->lastSpellTimeOrbThreshold = g_TimeRequirementParams[gameManager->currentStage][g_GameManager.difficulty];
        }
        else
        {
            // Preserve the original behavior: in a well defined manner
            gameManager->globals->lastSpellTimeOrbThreshold = g_RankParams[EASY].rank;
        }
#endif
    }
    else
    {
        gameManager->globals->lastSpellTimeOrbThreshold = 0;
    }

    if (gameManager->IsPracticeMode())
    {
        if (!gameManager->IsSpellPractice())
        {
            switch (gameManager->currentStage)
            {
            case STAGE1:
                gameManager->SetPower(0);
                break;
            case STAGE2:
                gameManager->SetPower(112);
                break;
            default:
                gameManager->SetPower(128);
                break;
            }
        }
        else if (gameManager->currentSpellCardNumber <= SPELLCARD_ST1_MBOSS_1L)
        {
            gameManager->SetPower(30);
        }
        else if (gameManager->currentSpellCardNumber <= SPELLCARD_ST1_BOSS_LSL)
        {
            gameManager->SetPower(80);
        }
        else
        {
            gameManager->SetPower(128);
        }
    }

    if (g_GameManager.IsReplay())
    {
        InitRankParams(gameManager);

        ReplayManager::RegisterChain(1, g_GameManager.replayFilename);

        u16 seed = g_Rng.GetSeed();

        gameManager->UpdateAntiTamper();

        g_Rng.SetSeed(seed);
    }

    gameManager->replaySeed = g_Rng.GetSeed();

    if (Background::RegisterChain(gameManager->currentStage) != ZUN_SUCCESS)
    {
        if (g_Supervisor.subthreadCloseRequestActive)
        {
            return;
        }

        g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_INITIALIZE_BACKGROUND);
        goto err;
    }

    if (BulletManager::RegisterChain("etama.anm") != ZUN_SUCCESS)
    {
        if (g_Supervisor.subthreadCloseRequestActive)
        {
            return;
        }

        g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_INITIALIZE_BULLETMANAGER);
        goto err;
    }

    if (EnemyManager::RegisterChain() != ZUN_SUCCESS)
    {
        if (g_Supervisor.subthreadCloseRequestActive)
        {
            return;
        }

        g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_INITIALIZE_ENEMYMANAGER);
        goto err;
    }

    if (EffectManager::RegisterChain() != ZUN_SUCCESS)
    {
        if (g_Supervisor.subthreadCloseRequestActive)
        {
            return;
        }

        g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_INITIALIZE_EFFECTMANAGER);
        goto err;
    }

    if (Gui::RegisterChain() != ZUN_SUCCESS)
    {
        if (g_Supervisor.subthreadCloseRequestActive)
        {
            return;
        }

        g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_INITIALIZE_GUI);
        goto err;
    }

    if (Spellcard::RegisterChain() != ZUN_SUCCESS)
    {
        if (g_Supervisor.subthreadCloseRequestActive)
        {
            return;
        }

        g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_INITIALIZE_SPELLCARD);
        goto err;
    }

    if (!g_GameManager.IsReplay())
    {
        ReplayManager::RegisterChain(0, "replay/th8_00.rpy");
    }

    if (g_GameManager.IsSpellPractice())
    {
        switch (g_GameManager.currentStage)
        {
        case STAGE5:
            if (g_GameManager.IsSpellNumberEqualTo(SPELLCARD_LW_TEWI))
            {
                // ZUN most likely commented out code here. Possibly this:
                // g_Background.unk_b34 = 2;
            }
            break;
        case STAGE6A:
            if (!g_GameManager.IsSpellNumberInRange(SPELLCARD_ST6A_MBOSS_1E, SPELLCARD_ST6A_MBOSS_1L))
            {
                g_Background.unk_b34 = 2;
            }
            break;
        case STAGE6B:
            if (!g_GameManager.IsSpellNumberInRange(SPELLCARD_ST6B_MBOSS_1E, SPELLCARD_ST6B_MBOSS_1L))
            {
                g_Background.unk_b34 = 2;
            }
            break;
        case EXTRASTAGE:
            if (!g_GameManager.IsSpellNumberInRange(SPELLCARD_EX_MBOSS_1, SPELLCARD_EX_MBOSS_3)
                && !g_GameManager.IsSpellNumberEqualTo(SPELLCARD_LW_KEINEEX))
            {
                g_Background.unk_b34 = 2;
            }
            break;
        }
    }

    if (!KeepStageResources())
    {
        if (g_GameManager.IsSpellPractice())
        {
            i32 i = 0;

            while (g_SpellcardMusicInfo[i].spellcardNumber >= 0)
            {
                if (g_GameManager.currentSpellCardNumber <= g_SpellcardMusicInfo[i].spellcardNumber)
                {
                    g_Supervisor.LoadMusic(0, g_SpellcardMusicInfo[i].songPath);
                    break;
                }

                i++;
            }
        }
        else
        {
            g_Supervisor.LoadMusic(0, g_Background.stdData->songPaths[0]);
            if (g_Background.stdData->songPaths[1][0] != ' ')
            {
                g_Supervisor.LoadMusic(1, g_Background.stdData->songPaths[1]);
            }
            if (g_Background.stdData->songPaths[2][0] != ' ')
            {
                g_Supervisor.LoadMusic(2, g_Background.stdData->songPaths[2]);
            }
        }
    }

    gameManager->showRetryMenu = 0;
    gameManager->flags.isActive = 1;

    if (KeepStageResources()
        && g_GameManager.IsSpellPractice()
        && !ShouldPauseMusicInSpellPractice(g_GameManager.currentSpellCardNumber))
    {
        gameManager->unk3de28 = 2;
    }
    else
    {
        gameManager->unk3de28 = 1;
    }

    if (g_Supervisor.curState != SupervisorState_GameManagerReInit)
    {
        g_Supervisor.lagNumerator = 0.0f;
        g_Supervisor.lagDenominator = 0.0f;
    }

    gameManager->isTimeStopped = 0;
    gameManager->globals->score = 0;
    gameManager->flags.unk4 = 0;

    g_AsciiManager.Reset();
    g_AsciiManager.InitializeVms();

    g_GameManager.stickyInput = 0;

    g_AsciiManager.nightBlindnessColor.d3dColor = 0;

    Supervisor::CalculateFps(0);

    if (g_GameManager.IsReplay())
    {
        while (gameManager->loadFrames < 80)
        {
            Sleep(17);
        }
    }
    else
    {
        while (gameManager->loadFrames < 30)
        {
            Sleep(17);
        }
    }

    g_Supervisor.HideLoadingVms();

    while (gameManager->flags.gameState != GAME_STATE_DEFAULT)
    {
        Sleep(17);
    }

    g_GameManager.loadState = GAME_LOAD_FINISHED;

    g_Supervisor.runningSubthreadHandle = NULL;
    g_Supervisor.subthreadCloseRequestActive = 0;
    g_Supervisor.unk290 = 0;
    g_Supervisor.unk174 = 60;
    gameManager->flags.unk9 = 0;
    g_Supervisor.keepStageResources = 0;
    g_ScreenEffectCounter = 2;

    return;
err:
    g_GameManager.loadState = GAME_LOAD_FAILED;

    g_Supervisor.HideLoadingVms();
    g_Supervisor.runningSubthreadHandle = NULL;
    g_Supervisor.subthreadCloseRequestActive = 0;
    g_Supervisor.unk290 = 0;
    g_Supervisor.keepStageResources = 0;
    g_ScreenEffectCounter = 2;
}

void GameManager::InitRankParams(GameManager *gameManager)
{
    gameManager->rank = g_RankParams[g_GameManager.difficulty].rank;
    gameManager->minRank = g_RankParams[g_GameManager.difficulty].minRank;
    gameManager->maxRank = g_RankParams[g_GameManager.difficulty].maxRank;
}

#pragma var_order(sum, i)
void GameManager::InitializeAntiTamper()
{
    i32 sum;
    u32 i;

    g_GameManager.globals->rng6 = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng1); i++)
    {
        g_GameManager.globals->rng1[i] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    }
    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng7); i++)
    {
        g_GameManager.globals->rng7[i] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    }
    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng2); i++)
    {
        g_GameManager.globals->rng2[i] = g_Rng.GetRandomF32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    }
    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng3); i++)
    {
        g_GameManager.globals->rng3[i] = g_Rng.GetRandomF32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    }
    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng4); i++)
    {
        g_GameManager.globals->rng4[i] = g_Rng.GetRandomF32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    }
    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng5); i++)
    {
        g_GameManager.globals->rng5[i] = g_Rng.GetRandomF32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    }
    for (i = 0; i < ARRAY_SIZE(g_GameManager.globals->rng8); i++)
    {
        g_GameManager.globals->rng8[i] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    }
    g_GameManager.globals->antiTamperValue = g_GameManager.globals->rng1[2];
    sum = g_GameManager.CalcAntiTamperChecksum();
    g_GameManager.globals->antiTamperChecksum = sum;
    g_GameManager.antiTamperExpectedValue = (f32)sum + (f32)g_GameManager.globals->rng7[3];
}

#pragma var_order(catk, i, scoreDat, j)
ZunResult GameManager::InitScore()
{
    i32 i;
    i32 j;
    Catk *catk = &g_GameManager.catkData[0];
    ScoreDat *scoreDat;

    ResultScreen::RegisterChain(2);

    memset(g_GameManager.catkData, 0, sizeof(g_GameManager.catkData));

    for (i = 0; i < SPELLCARD_COUNT_SPELLCARDS; i++, catk++)
    {
        catk->base.magic = CATK_MAGIC;
        catk->base.unkLen = sizeof(Catk);
        catk->base.th8kLen = sizeof(Catk);
        catk->base.version = CATK_VERSION;
        catk->spellcardNumber = i;
        for (j = 0; j < MAX_DIFFICULTIES + 2; j++)
        {
            catk->inGameHistory.attempts[j] = 0;
            catk->inGameHistory.captures[j] = 0;
            catk->inGameHistory.maxBonus[j] = 0;
        }
    }

    scoreDat = ScoreDat::OpenScore(SCORE_FILE_PATH);
    if (scoreDat == NULL)
    {
        g_GameErrorContext.Log(TH_ERR_GAMEMANAGER_FAILED_TO_READ_SCORE);
        return ZUN_ERROR;
    }

    g_GameManager.globals->displayedHighScore = ScoreDat::GetHighScore(scoreDat, 0, g_GameManager.character, g_GameManager.difficulty, &g_GameManager.globals->continuesUsedInHighScore);

    ScoreDat::ParseCATK(scoreDat, g_GameManager.catkData);
    ScoreDat::ParseCLRD(scoreDat, g_GameManager.clrdData);
    ScoreDat::ParsePSCR(scoreDat, g_GameManager.pscrData);

    if (g_GameManager.IsPracticeMode())
    {
        g_GameManager.globals->displayedHighScore = g_GameManager.pscrData[g_GameManager.character].highScores[g_GameManager.currentStage][g_GameManager.difficulty];
        g_GameManager.pscrData[g_GameManager.character].attempts[g_GameManager.currentStage][g_GameManager.difficulty]++;
        g_GameManager.pscrData[g_GameManager.character].unk0x175 = 1;
    }

    ScoreDat::ReleaseScore(scoreDat);

    memcpy(g_GameManager.catkData2, g_GameManager.catkData, sizeof(g_GameManager.catkData));

    memset(&g_GameManager.hscr, 0, sizeof(Hscr));

    g_GameManager.hscr.character = g_GameManager.character;
    g_GameManager.hscr.difficulty = g_GameManager.difficulty;
    g_GameManager.hscr.cfg = g_Supervisor.cfg;

    g_GameManager.unk3DB94 = 0;

    return ZUN_SUCCESS;
}

ZunResult GameManager::DeletedCallback(GameManager *gameManager)
{
    g_ScreenEffectCounter = 1;

    g_AsciiManager.nightBlindnessColor.d3dColor = 0;

    if (g_Supervisor.curState != SupervisorState_GameManagerReInit
        && g_Supervisor.curState != SupervisorState_SpellcardPracticeRestart
        && g_Supervisor.curState != SupervisorState_GameManagerNextStageWeird)
    {
        g_Supervisor.releaseResourcesOnRestart = TRUE;
    }
    else
    {
        g_Supervisor.releaseResourcesOnRestart = FALSE;
    }

    if (!g_GameManager.IsSpellPractice() || ReleaseResourcesOnRestart())
    {
        g_Supervisor.StopAudio();

        if (g_Supervisor.cfg.musicMode == MIDI && g_Supervisor.midiOutput != NULL)
        {
            g_Supervisor.midiOutput->PlayFile(MIDI_FILE_INIT);
        }
    }

    while (g_SoundPlayer.ProcessQueues() != 0);

    Spellcard::CutChain();
    Background::CutChain();
    BulletManager::CutChain();
    Player::CutChain();
    EnemyManager::CutChain();
    EffectManager::CutChain();
    Gui::CutChain();

    if (!g_GameManager.IsReplay())
    {
        ReplayManager::StopRecording();
    }

    if (!g_GameManager.IsReplay())
    {
        g_Supervisor.UpdateGameTime();
    }

    g_Supervisor.systemTime = 0;
    g_Supervisor.UpdatePlayTime();

    gameManager->flags.isActive = 0;

    g_AsciiManager.Reset();

    g_GameManager.stickyInput = 0;
    g_GameManager.unk3ddc0 = 0;

    return ZUN_SUCCESS;
}

void GameManager::CutChain()
{
    g_Chain.Cut(&g_GameManagerCalcChain);
    g_Chain.Cut(&g_GameManagerDrawChain);
    if (g_GameManager.globals->score >= MAX_SCORE + 1)
    {
        g_GameManager.globals->score = MAX_SCORE;
    }
    g_GameManager.globals->displayScore = g_GameManager.globals->score;
    g_Supervisor.framerateMultiplier = 1.0f;
}


void GameManager::IncreaseSubrank(int amount)
{
    this->subRank += amount;
    while (this->subRank >= 100)
    {
        this->rank++;
        this->subRank -= 100;
    }
    if (this->rank > this->maxRank)
    {
        this->rank = this->maxRank;
    }
}

void GameManager::DecreaseSubrank(int amount)
{
    this->subRank -= amount;
    while (this->subRank < 0)
    {
        this->rank--;
        this->subRank += 100;
    }
    if (this->rank < this->minRank)
    {
        this->rank = this->minRank;
    }
}

// STUB: th08 0x43c0bb
void GameManager::AddToYoukaiGauge(u16 param_1, i32 param_2)
{
}

ZunBool GameManager::IsExtraUnlockedForCharacter(i32 character)
{
    return (character > SHOT_YOUMU_YUYUKO) ||
           (this->clrdData[character].difficultiesClearedWithoutRetries[EASY] & EXTRA_UNLOCKED_FLAG ||
            this->clrdData[character].difficultiesClearedWithoutRetries[NORMAL] & EXTRA_UNLOCKED_FLAG ||
            this->clrdData[character].difficultiesClearedWithoutRetries[HARD] & EXTRA_UNLOCKED_FLAG ||
            this->clrdData[character].difficultiesClearedWithoutRetries[LUNATIC] & EXTRA_UNLOCKED_FLAG);
}

ZunBool GameManager::IsExtraUnlocked()
{
    return this->IsExtraUnlockedForCharacter(SHOT_REIMU_YUKARI) ||
           this->IsExtraUnlockedForCharacter(SHOT_MARISA_ALICE) ||
           this->IsExtraUnlockedForCharacter(SHOT_SAKUYA_REMILIA) ||
           this->IsExtraUnlockedForCharacter(SHOT_YOUMU_YUYUKO);
}

ZunBool GameManager::IsSpellPracticeUnlockedForCharacter(i32 character)
{
    return (character > SHOT_YOUMU_YUYUKO) ||
           (this->clrdData[character].difficultiesClearedWithRetries[EASY] & SPELL_PRACTICE_UNLOCKED_FLAG ||
            this->clrdData[character].difficultiesClearedWithRetries[NORMAL] & SPELL_PRACTICE_UNLOCKED_FLAG ||
            this->clrdData[character].difficultiesClearedWithRetries[HARD] & SPELL_PRACTICE_UNLOCKED_FLAG ||
            this->clrdData[character].difficultiesClearedWithRetries[LUNATIC] & SPELL_PRACTICE_UNLOCKED_FLAG);
}

ZunBool GameManager::IsSpellPracticeUnlocked()
{
    return this->IsSpellPracticeUnlockedForCharacter(SHOT_REIMU_YUKARI) ||
           this->IsSpellPracticeUnlockedForCharacter(SHOT_MARISA_ALICE) ||
           this->IsSpellPracticeUnlockedForCharacter(SHOT_SAKUYA_REMILIA) ||
           this->IsSpellPracticeUnlockedForCharacter(SHOT_YOUMU_YUYUKO);
}

// Leftover from PCB.
ZunBool GameManager::IsPhantasmUnlocked()
{
    return FALSE;
}

ZunBool GameManager::IsPracticeReplay()
{
    return this->IsReplay() && g_ReplayManager->replayData->isPractice;
}

i32 GameManager::GetClockTimeIncrement()
{
    // ZUN bloat: Why not use switch case fallthrough?
    switch (g_GameManager.currentStage)
    {
    case STAGE1:
        if (g_GameManager.GetTimeOrbs() >= g_GameManager.GetLastSpellTimeOrbThreshold())
        {
            return 1;
        }
        else
        {
            return 2;
        }
    case STAGE2:
        if (g_GameManager.GetTimeOrbs() >= g_GameManager.GetLastSpellTimeOrbThreshold())
        {
            return 1;
        }
        else
        {
            return 2;
        }
    case STAGE3:
        if (g_GameManager.GetTimeOrbs() >= g_GameManager.GetLastSpellTimeOrbThreshold())
        {
            return 1;
        }
        else
        {
            return 2;
        }
    case STAGE4A:
        if (g_GameManager.GetTimeOrbs() >= g_GameManager.GetLastSpellTimeOrbThreshold())
        {
            return 1;
        }
        else
        {
            return 2;
        }
    case STAGE4B:
        if (g_GameManager.GetTimeOrbs() >= g_GameManager.GetLastSpellTimeOrbThreshold())
        {
            return 1;
        }
        else
        {
            return 2;
        }
    case STAGE5:
        if (g_GameManager.GetTimeOrbs() >= g_GameManager.GetLastSpellTimeOrbThreshold())
        {
            return 1;
        }
        else
        {
            return 2;
        }
    case STAGE6A:
        return 0;
    case STAGE6B:
        return 0;
    default:
        return 4;
    }
}

void GameManager::AdvanceToNextStage()
{
    switch (this->currentStage)
    {
    case STAGE1:
        this->currentStage = STAGE2;
        break;
    case STAGE2:
        this->currentStage = STAGE3;
        break;
    case STAGE3:
        switch (g_GameManager.character)
        {
        case SHOT_REIMU_YUKARI:
        case SHOT_REIMU:
        case SHOT_YUKARI:
            this->currentStage = STAGE4B;
            break;
        case SHOT_MARISA_ALICE:
        case SHOT_MARISA:
        case SHOT_ALICE:
            this->currentStage = STAGE4A;
            break;
        case SHOT_SAKUYA_REMILIA:
        case SHOT_SAKUYA:
        case SHOT_REMILIA:
            this->currentStage = STAGE4A;
            break;
        case SHOT_YOUMU_YUYUKO:
        case SHOT_YOUMU:
        case SHOT_YUYUKO:
            this->currentStage = STAGE4B;
            break;
        }
        break;
    case STAGE4A:
    case STAGE4B:
        this->currentStage = STAGE5;
        break;
    case STAGE5:
        this->currentStage = g_GameManager.flags.isGoingToFinalB ? STAGE6B : STAGE6A;
        break;
    case STAGE6A:
        this->currentStage = STAGE6B; // Was Kaguya meant to be a TLB at one point???
        break;
    }
}

GameManager::GameManager()
{
    memset(this, 0, sizeof(GameManager));
    this->arcadeRegionTopLeftPos.x = ARCADE_LEFT;
    this->arcadeRegionTopLeftPos.y = ARCADE_TOP;
    this->arcadeRegionSize.x = ARCADE_WIDTH;
    this->arcadeRegionSize.y = ARCADE_HEIGHT;
    this->currentDemoReplay = 3;
}

void GameManager::InitArcadeRegionParams()
{
    this->arcadeRegionTopLeftPos.x = ARCADE_LEFT;
    this->arcadeRegionTopLeftPos.y = ARCADE_TOP;
    this->arcadeRegionSize.x = ARCADE_WIDTH;
    this->arcadeRegionSize.y = ARCADE_HEIGHT;
    this->playerMovementTopLeftPos.x = 8.0f;
    this->playerMovementTopLeftPos.y = 16.0f;
    this->playerMovementAreaSize.x = 368.0f;
    this->playerMovementAreaSize.y = 416.0f;
}

}; // Namespace th08
