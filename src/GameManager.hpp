#pragma once

#include "Supervisor.hpp"
#include "ZunResult.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include <windows.h>

namespace th08
{
struct Th8k
{
    u32 magic;
    u16 th8kLen;
    u16 unkLen;
    u8 version;
    unknown_fields(0x9, 0x3);
};
C_ASSERT(sizeof(Th8k) == 0xC);

struct PlstPlayCounts
{
    i32 attemptsTotal;
    i32 attemptsPerCharacter[12];
    unknown_fields(0x34, 0x4);
    i32 clears;
    i32 continues;
    i32 practices;
};
C_ASSERT(sizeof(PlstPlayCounts) == 0x44);

struct Plst
{
    Th8k base;
    u32 totalHours;
    u32 totalMinutes;
    u32 totalSeconds;
    u32 totalMilliseconds;
    u32 gameHours;
    u32 gameMinutes;
    u32 gameSeconds;
    u32 gameMilliseconds;
    PlstPlayCounts playDataByDifficulty[6];
    PlstPlayCounts playDataTotals;
    u8 bgmUnlocked[32];
};

C_ASSERT(sizeof(Plst) == 0x228);

struct Flsp
{
    Th8k base;
    BYTE idx[17];
    unknown_fields(0x1d, 0x3);
};

C_ASSERT(sizeof(Flsp) == 0x20);

struct CatkHistory
{
    i32 maxBonusPerShot[12];
    i32 maxBonusBest;
    i32 attemptsPerShot[12];
    i32 attemptsTotal;
    i32 capturesPerShot[12];
    i32 capturesTotal;
};

struct Catk
{
    Th8k base;
    i32 unk0xc;

    char spellName[48];
    char spellOwnerName[48];
    char spellCommentLine1[64];
    char spellCommentLine2[64];
    CatkHistory inGameHistory;
    CatkHistory spellPracticeHistory;
    i32 unk0x228;
};

C_ASSERT(sizeof(Catk) == 0x22c);

struct Clrd
{
    Th8k base;
    u16 flags[10];
    bool unk_20;
};

C_ASSERT(sizeof(Clrd) == 0x24);

struct Pscr
{
    Th8k base;

    unknown_fields(0xc, 0x16c);
};

C_ASSERT(sizeof(Pscr) == 0x178);

struct Hscr
{
    Th8k base;
    u32 score;
    f32 lagPercentage;
    u8 character;
    u8 difficulty;
    u8 stage;
    char name[9];
    char date[6];
    u8 numRetries;
    u8 unk0x27;
    GameConfiguration cfg;
    i32 playtimeFrames;
    i32 numPointItemsCollected;
    i32 unk_6c;
    i32 numDeaths;
    i32 numBombsUsed;
    i32 numLastSpells;
    i32 numPauses;
    i32 numTimeOrbsCollected;
    i32 humanityRate;
    u8 spellCounters[222];
    u8 unk0x166;
    u8 unk0x167;
};

C_ASSERT(sizeof(Hscr) == 0x168);

struct GameManagerFlags
{
    u32 unk0 : 1;
    u32 unk1 : 1;
    u32 unk2 : 1;
    u32 unk3 : 1;
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
    u32 unk14 : 1;
    u32 finalBClearedWithAnyTeam : 1;
    u32 finalAClearedWithAnyTeam : 1;
    u32 finalBClearedWithAllTeams : 1;
};

struct GameManager
{
    static ZunResult RegisterChain();
    static void CutChain();
    void AdvanceToNextStage()
    {
    }
    BOOL FinalBClearedWithAnyTeam()
    {
        return FALSE;
    }
    BOOL FinalAClearedWithAnyTeam()
    {
        return FALSE;
    }
    BOOL FinalBClearedWithAllTeams()
    {
        return FALSE;
    }

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
    Catk catkData[444];
    Clrd clrdData[13];
    Pscr pscrData[12];
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
    u16 unk3DBB0;
    u8 isInGameMenu;
    u8 showRetryMenu;
    u8 unk3DBB4;
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
    D3DXVECTOR2 arcadeRegionTopLeftPos;
    D3DXVECTOR2 arcadeRegionSize;
    D3DXVECTOR2 playerMovementTopLeftPos;
    D3DXVECTOR2 playerMovementAreaSize;
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
