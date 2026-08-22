#pragma once

#include "ScoreDat.hpp"
#include "SpellcardNumbers.hpp"
#include "Supervisor.hpp"
#include "ZunResult.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include <stddef.h>
#include <windows.h>

#define EXTRA_UNLOCKED_FLAG ZUN_BIT(14)
#define SPELL_PRACTICE_UNLOCKED_FLAG ZUN_BIT(15)

#define IS_STAGE_CLEARED(difficulty, stage) (difficulty & ZUN_BIT(stage))

#define ANTITAMPER_RNG_RANGE 100000
#define ANTITAMPER_RNG_ADD 6543

namespace th08
{

struct GameManagerFlags
{
    u32 isPracticeMode : 1;
    u32 isDemoMode : 1;
    u32 unk2 : 1;
    u32 isReplay : 1;
    u32 unk4 : 1;
    u32 unk5 : 2;
    u32 unk7 : 2;
    u32 unk9 : 1;
    u32 unk10 : 1;
    u32 isGoingToFinalB : 2; // why 2 bits?
    u32 unk13 : 1;
    u32 isSpellPractice : 1;

    u32 isExtraUnlocked : 1;
    u32 isSpellPracticeUnlocked : 1;
    u32 isExtraUnlockedWithAllTeams : 1;
};

enum
{
    REPLAY_MODE_NORMAL,
    REPLAY_MODE_SLOWDOWN,
    REPLAY_MODE_BOSS,
};

enum GameManagerLoadState
{
    GAME_LOAD_FINISHED,
    GAME_LOAD_IN_PROGRESS,
    GAME_LOAD_FAILED,
};

struct GameManager
{
    GameManager();

    static ZunBool ShouldPauseMusicInSpellPractice(i32 spellcardNumber);
    static i32 GetSongNameSpriteIdx(i32 spellcardNumber);
    ZunBool IsWithinPlayfield();
    i32 CalcAntiTamperChecksum();
    static i32 CalcChecksum(u8 *address, i32 size);
    void CollectExtend();
    static ZunResult InitScore();

    static ChainCallbackResult OnUpdate(GameManager *gameManager);
    static ChainCallbackResult OnDraw(GameManager *gameManager);

    static ZunResult RegisterChain();

    static ZunResult AddedCallback(GameManager *gameManager);
    static void GameplaySetupThread(LPVOID param);

    static void InitRankParams(GameManager *gameManager);

    static void InitializeAntiTamper();

    i32 GetTimeOrbs()
    {
        return this->globals->currentTimeOrbs;
    }

    i32 GetLastSpellTimeOrbThreshold()
    {
        return this->globals->lastSpellTimeOrbThreshold;
    }

    i32 GetLives()
    {
        return this->globals->livesRemaining;
    }

    i32 GetBombsRemaining()
    {
        return this->globals->bombsRemaining;
    }

    i32 GetDeaths()
    {
        return this->globals->deaths;
    }

    i32 GetBombsUsed()
    {
        return this->globals->bombsUsed;
    }

    void UpdateAntiTamper()
    {
        this->globals->rng1[2] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->rng7[3] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->antiTamperValue = this->globals->rng1[2];
        this->globals->antiTamperChecksum = CalcAntiTamperChecksum();
        this->antiTamperExpectedValue = this->globals->antiTamperChecksum + this->globals->rng7[3];
    }

    void RandomizeAntiTamper()
    {
        this->globals->rng1[0] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->rng1[1] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->rng1[2] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->rng1[3] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->rng1[4] = g_Rng.GetRandomU32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->rng4[0] = g_Rng.GetRandomF32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->rng4[1] = g_Rng.GetRandomF32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
        this->globals->rng4[2] = g_Rng.GetRandomF32InRange(ANTITAMPER_RNG_RANGE) + ANTITAMPER_RNG_ADD;
    }

    ZunBool IsTampered()
    {
        // There is zero chance ZUN actually used intptr_t here, but the codegen matches and not making
        // assumptions about pointer size is always nice
        return this->globals->antiTamperValue !=
                   this->globals->rng1[2] + this->globals->rng8[2] * ((intptr_t)&this->globals->antiTamperValue -
                                                                      (intptr_t)&this->globals->rng1 + 500) ||
               this->globals->antiTamperChecksum + this->globals->rng7[3] != (i32)this->antiTamperExpectedValue;
    }

    static ZunResult DeletedCallback(GameManager *gameManager);

    static void CutChain();

    void IncreaseSubrank(int amount);
    void DecreaseSubrank(int amount);
    void AddToYoukaiGauge(u16 param_1, i32 param_2);

    ZunBool IsPhantasmUnlocked();
    ZunBool IsReplayPractice();

    /* I know it's dumb but this is the only way to get it matching */
    void SetIsReplayWeird(ZunBool value)
    {
        ZunBool res = value;

        this->flags.isReplay = res;
    }

    ZunBool IsSpellPractice()
    {
        return this->flags.isSpellPractice;
    }

    ZunBool IsReplay()
    {
        return this->flags.isReplay;
    }

    ZunBool IsDemoMode()
    {
        return this->flags.isDemoMode;
    }

    ZunBool IsPracticeMode()
    {
        return this->flags.isPracticeMode;
    }

    i32 GetYoukaiGauge()
    {
        return this->globals->youkaiGauge;
    }

    ZunBool IsGaugeExtremelyHuman()
    {
        return this->globals->youkaiGauge <= this->youkaiGaugeHumanEffectsThreshold;
    }

    ZunBool IsGaugeModeratelyHuman()
    {
        return this->globals->youkaiGauge <= this->youkaiGaugeHumanTintThreshold;
    }

    ZunBool IsGaugeExtremelyYoukai()
    {
        return this->globals->youkaiGauge >= this->youkaiGaugeYoukaiEffectsThreshold;
    }

    ZunBool IsGaugeModeratelyYoukai()
    {
        return this->globals->youkaiGauge >= this->youkaiGaugeYoukaiTintThreshold;
    }

    i8 GetClockTime()
    {
        return this->globals->clockTime;
    }

    void AddToClockTime(i8 amount)
    {
        this->globals->clockTime += amount;
    }

    void SetLives(i32 lives)
    {
        this->globals->livesRemaining = lives;
        this->UpdateAntiTamper();
    }

    void SetBombCount(i32 bombs)
    {
        this->globals->bombsRemaining = bombs;
        this->globals->antiTamperValue = this->globals->rng1[2];
        this->globals->antiTamperChecksum = this->CalcAntiTamperChecksum();
        this->antiTamperExpectedValue = this->globals->antiTamperChecksum + this->globals->rng7[3];
    }

    void SetPower(i32 power)
    {
        this->globals->playerPower = power;
        this->UpdateAntiTamper();
    }

    void SetDeaths(i32 deaths)
    {
        this->globals->deaths = deaths;
    }

    void SetDeathsInStage(i32 deaths)
    {
        this->globals->deathsInStage = deaths;
        this->UpdateAntiTamper();
    }

    void SetBombsUsed(i32 bombs)
    {
        this->globals->bombsUsed = bombs;
    }

    void SetBombsUsedInStage(i32 bombs)
    {
        this->globals->bombsUsedInStage = bombs;
        this->UpdateAntiTamper();
    }

    void SetYoukaiGauge(i32 youkaiGauge)
    {
        this->globals->youkaiGauge = youkaiGauge;
    }

    void SetClockTime(i32 clockTime)
    {
        this->globals->clockTime = clockTime;
    }

    ZunBool IsStageClearedWithRetries(i32 stage, i32 character, i32 difficulty)
    {
        return IS_STAGE_CLEARED(this->clrdData[character].difficultiesClearedWithRetries[difficulty], stage);
    }

    ZunBool IsStageClearedWithoutRetries(i32 stage, i32 character, i32 difficulty)
    {
        return IS_STAGE_CLEARED(this->clrdData[character].difficultiesClearedWithoutRetries[difficulty], stage);
    }

    ZunBool IsExtraUnlockedForCharacter(i32 character);
    ZunBool IsExtraUnlocked();
    ZunBool IsSpellPracticeUnlockedForCharacter(i32 character);
    ZunBool IsSpellPracticeUnlocked();

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

    ZunBool IsSpellNumberEqualTo(i32 spellNumber)
    {
        return this->flags.isSpellPractice ? (this->currentSpellCardNumber == spellNumber) : FALSE;
    }

    ZunBool IsSpellNumberInRange(i32 min, i32 max)
    {
        return this->flags.isSpellPractice ? (this->currentSpellCardNumber >= min && this->currentSpellCardNumber <= max) : FALSE;
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

    void AddToBombCount(int amount)
    {
        if (this->IsTampered())
        {
            CRASH_GAME();
        }
        this->globals->bombsRemaining += amount;
        this->UpdateAntiTamper();
    }

    void AddScore(int amount)
    {
        this->globals->score += amount / 10;
    }

    void InitArcadeRegionParams();

    ZunBool IsStickyInput()
    {
        return this->stickyInput;
    }

    void *decoyBuffer;
    GameConfiguration *cfg;
    ZunGlobals *globals;
    Flsp flsp;
    i8 isTimeStopped;
    i8 stickyInput;
    /* 2 bytes pad */
    i32 difficulty;
    i32 difficultyMask;
    GameManagerLoadState loadState;
    i32 unk3c;
    Catk catkData[SPELLCARD_COUNT_SPELLCARDS];
    Catk catkData2[SPELLCARD_COUNT_SPELLCARDS];
    Clrd clrdData[SHOT_ALL + 1];
    Pscr pscrData[SHOT_ALL];
    Plst plst;
    Hscr hscr;
    i32 unk3DB94;
    i32 unk3DB98;
    i32 unk3DB9C;
    i32 unk3DBA0;
    i32 unk3DBA4;
    u8 powerItemCountForScore;
    u8 character;
    u8 shotType;
    u8 characterShotType;
    GameManagerFlags flags;
    i16 currentSpellCardNumber;
    u8 showPauseMenu;
    u8 showRetryMenu;
    u8 currentDemoReplay;
    u8 replayMode;
    u8 unk3DBB6;
    u8 unk3DBB7;

    i32 demoFrameCount;
    char replayFilename[512];
    u16 replaySeed;
    u32 unk3ddc0;
    i32 currentStage;
    i32 currentStage2;
    u32 unk3ddcc;
    u16 stageMask;
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

    i32 unk3de04;
    u32 unk3de08;
    u32 unk3de0c;
    u32 unk3de10;
    i32 unk3de14;
    u32 unk3de18;
    u32 unk3de1c;
    i32 unk3de20;
    i32 unk3de24;
    i32 unk3de28;

    i32 rank;
    i32 maxRank;
    i32 minRank;
    i32 subRank;
};

C_ASSERT(sizeof(GameManager) == 0x3de3c);

DIFFABLE_EXTERN(GameManager, g_GameManager);
}; // Namespace th08
