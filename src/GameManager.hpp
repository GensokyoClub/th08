#pragma once

#include "ScoreDat.hpp"
#include "Spellcard.hpp"
#include "Supervisor.hpp"
#include "ZunResult.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include <windows.h>

#define EXTRA_UNLOCKED_FLAG ZUN_BIT(14)
#define SPELL_PRACTICE_UNLOCKED_FLAG ZUN_BIT(15)

#define IS_STAGE_CLEARED(difficulty, stage) (difficulty & ZUN_BIT(stage))

namespace th08
{

struct GameManagerFlags
{
    u32 isPracticeMode : 1;
    u32 isDemoMode : 1;
    u32 unk2 : 1;
    u32 isReplay : 1;
    u32 unk4 : 1;
    u32 unk5 : 1;
    u32 unk6 : 1;
    u32 unk7 : 1;
    u32 unk8 : 1;
    u32 unk9 : 1;
    u32 unk10 : 1;
    u32 unk11 : 1;
    u32 unk12 : 1;
    u32 unk13 : 1;
    u32 isSpellPractice : 1;

    u32 isExtraUnlocked : 1;
    u32 isSpellPracticeUnlocked : 1;
    u32 isExtraUnlockedWithAllTeams : 1;
};

struct GameManager
{
    GameManager();

    ZunBool IsWithinPlayfield();
    i32 CalcAntiTamperChecksum();
    static i32 CalcChecksum(u8 *param_1, i32 param_2);
    void CollectExtend();

    static ChainCallbackResult OnUpdate(GameManager *gameManager);
    static ChainCallbackResult OnDraw(GameManager *gameManager);

    static ZunResult RegisterChain();

    static ZunResult AddedCallback(GameManager *gameManager);
    static void GameplaySetupThread();

    void InitRankParams()
    {
    }

    void InitializeAntiTamper()
    {
    }

    void UpdateAntiTamper()
    {
    }

    ZunBool IsTampered()
    {
        return FALSE;
    }

    static ZunResult DeletedCallback(GameManager *gameManager);

    static void CutChain();

    void IncreaseSubrank(int amount);
    void DecreaseSubrank(int amount);
    void AddToYoukaiGauge(u16 param_1, i32 param_2);

    ZunBool IsPhantasmUnlocked();

    /* I know it's dumb but this is the only way to get it matching */
    void SetIsReplayWeird(ZunBool value)
    {
        ZunBool res = value;

        this->flags.isReplay = res;
    }

    ZunBool IsPracticeMode()
    {
        return this->flags.isPracticeMode;
    }

    ZunBool IsReplay()
    {
        return this->flags.isReplay;
    }

    ZunBool IsSpellPractice()
    {
        return this->flags.isSpellPractice;
    }

    ZunBool IsDemoMode()
    {
        return this->flags.isDemoMode;
    }

    ZunBool IsStageClearedWithRetries(i32 stage, i32 character, i32 difficulty)
    {
        return IS_STAGE_CLEARED(this->clrdData[character].difficultiesClearedWithRetries[difficulty], stage);
    }

    ZunBool IsStageClearedWithoutRetries(i32 stage, i32 character, i32 difficulty)
    {
        return IS_STAGE_CLEARED(this->clrdData[character].difficultiesClearedWithoutRetries[difficulty], stage);
    }

    ZunBool IsExtraUnlockedForCharacter(i32 character)
    {
        return (character > SHOT_YOUMU_YUYUKO) ||
               (this->clrdData[character].difficultiesClearedWithoutRetries[EASY] & EXTRA_UNLOCKED_FLAG ||
                this->clrdData[character].difficultiesClearedWithoutRetries[NORMAL] & EXTRA_UNLOCKED_FLAG ||
                this->clrdData[character].difficultiesClearedWithoutRetries[HARD] & EXTRA_UNLOCKED_FLAG ||
                this->clrdData[character].difficultiesClearedWithoutRetries[LUNATIC] & EXTRA_UNLOCKED_FLAG);
    }

    ZunBool IsExtraUnlocked()
    {
        return this->IsExtraUnlockedForCharacter(SHOT_REIMU_YUKARI) ||
               this->IsExtraUnlockedForCharacter(SHOT_MARISA_ALICE) ||
               this->IsExtraUnlockedForCharacter(SHOT_SAKUYA_REMILIA) ||
               this->IsExtraUnlockedForCharacter(SHOT_YOUMU_YUYUKO);
    }

    ZunBool IsSpellPracticeUnlockedForCharacter(i32 character)
    {
        return (character > SHOT_YOUMU_YUYUKO) ||
               (this->clrdData[character].difficultiesClearedWithRetries[EASY] & SPELL_PRACTICE_UNLOCKED_FLAG ||
                this->clrdData[character].difficultiesClearedWithRetries[NORMAL] & SPELL_PRACTICE_UNLOCKED_FLAG ||
                this->clrdData[character].difficultiesClearedWithRetries[HARD] & SPELL_PRACTICE_UNLOCKED_FLAG ||
                this->clrdData[character].difficultiesClearedWithRetries[LUNATIC] & SPELL_PRACTICE_UNLOCKED_FLAG);
    }

    ZunBool IsSpellPracticeUnlocked()
    {
        return this->IsSpellPracticeUnlockedForCharacter(SHOT_REIMU_YUKARI) ||
               this->IsSpellPracticeUnlockedForCharacter(SHOT_MARISA_ALICE) ||
               this->IsSpellPracticeUnlockedForCharacter(SHOT_SAKUYA_REMILIA) ||
               this->IsSpellPracticeUnlockedForCharacter(SHOT_YOUMU_YUYUKO);
    }

    ZunBool IsExtraUnlockedWithAllTeams()
    {
        return this->IsExtraUnlockedForCharacter(SHOT_REIMU_YUKARI) &&
               this->IsExtraUnlockedForCharacter(SHOT_MARISA_ALICE) &&
               this->IsExtraUnlockedForCharacter(SHOT_SAKUYA_REMILIA) &&
               this->IsExtraUnlockedForCharacter(SHOT_YOUMU_YUYUKO);
    }

    ZunBool HasSpellCardBeenEncountered(i32 spellCardNumber, i32 shotType)
    {
        Catk *catk = &this->catkData[spellCardNumber];

        return catk->inGameHistory.attempts[shotType] > 0 || catk->spellPracticeHistory.attempts[shotType] != 0;
    }

    ZunBool IsLastWordSpellCardAttempted(i32 spellCardNumber)
    {
        return spellCardNumber < SPELLCARD_LAST_WORD_START &&
                   (this->catkData[spellCardNumber].inGameHistory.attempts[SHOT_ALL] != 0 ||
                    this->catkData[spellCardNumber].spellPracticeHistory.attempts[SHOT_ALL] != 0) ||
               this->flsp.unlockedLastWordSpellCards[spellCardNumber - SPELLCARD_LAST_WORD_START] == spellCardNumber;
    }

    i32 GetPower()
    {
        return this->globals->playerPower;
    }

    i32 GetClockTimeIncrement();
    void AdvanceToNextStage();

    void AddLives(int lives)
    {
        if (this->IsTampered())
        {
            CRASH_GAME();
        }
        this->globals->livesRemaining += lives;
        this->UpdateAntiTamper();
    }

    void AddPower(int power)
    {
        if (this->IsTampered())
        {
            CRASH_GAME();
        }
        this->globals->playerPower += power;
        this->UpdateAntiTamper();
    }

    void InitArcadeRegionParams();

    ZunBool IsUnknown()
    {
        return this->unk2D;
    }

    i32 unk0x0;
    GameConfiguration *cfg;
    ZunGlobals *globals;
    Flsp flsp;
    i8 unk2C;
    i8 unk2D;
    /* 2 bytes pad */
    i32 difficulty;
    i32 difficultyMask;
    u32 unk38;
    i32 unk3c;
    Catk catkData[SPELLCARD_COUNT_SPELLCARDS * 2];
    Clrd clrdData[SHOT_ALL + 1];
    Pscr pscrData[SHOT_ALL];
    Plst plst;
    Hscr hscr;
    i32 unk3D294;
    i32 unk3D298;
    i32 unk3D29C;
    i32 unk3D2A0;
    i32 unk3D2A4;
    u8 character;
    u8 shotType;
    u8 fullShotType;
    u8 unk3dbaa;
    GameManagerFlags flags;
    i16 currentSpellCardNumber;
    u8 isInGameMenu;
    u8 showRetryMenu;
    i8 currentDemoReplay;
    u8 unk3DBB5;
    u8 unk3DBB6;
    u8 unk3DBB7;

    u32 unk3DBB8;
    char replayFilename[512];
    u32 unk3ddbc;
    u32 unk3ddc0;
    i32 currentStage;
    i32 currentStage2;
    u32 unk3ddcc;
    u16 unk3DDD0;
    u16 unk3DDD2;
    Float2 arcadeRegionTopLeftPos;
    Float2 arcadeRegionSize;
    Float2 playerMovementTopLeftPos;
    Float2 playerMovementAreaSize;
    f32 antiTamperExpectedValue;
    i16 youkaiGaugeHumanLimit;
    i16 youkaiGaugeYoukaiLimit;
    i16 youkaiGaugeHumanEffectsThreshold;
    i16 youkaiGaugeYoukaiEffectsThreshold;
    i16 youkaiGaugeHumanTintThreshold;
    i16 youkaiGaugeYoukaiTintThreshold;

    u32 unk3de04;
    u32 unk3de08;
    u32 unk3de0c;
    u32 unk3de10;
    u32 unk3de14;
    u32 unk3de18;
    u32 unk3de1c;
    u32 unk3de20;
    u32 unk3de24;
    u32 unk3de28;

    i32 rank;
    i32 maxRank;
    i32 minRank;
    i32 subRank;
};

C_ASSERT(sizeof(GameManager) == 0x3de3c);

DIFFABLE_EXTERN(GameManager, g_GameManager);
}; // Namespace th08
