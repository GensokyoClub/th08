#pragma once

#include <windows.h>
#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include "Supervisor.hpp"
#include "ZunResult.hpp"

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

struct GameManager
{
    static ZunResult RegisterChain();
    static void CutChain();
    void AdvanceToNextStage();

    ZunBool IsUnknown()
    {
        return m_Unk2C;
    }

    i32 unk0x0;
    GameConfiguration *m_Cfg;
    ZunGlobals *m_Globals;
    Flsp flsp;
    i8 m_Unk2C;
    i8 m_Unk2D;
    /* 2 bytes pad */
    i32 m_Difficulty;
    i32 m_DifficultyMask;
    u32 m_Unk38;
    i32 m_Unk3c;
    Catk m_catkData[444];
    Clrd m_clrdData[13];
    Pscr m_pscrData[12];
    Plst plst;
    Hscr hscr;
    i32 m_Unk3D294;
    i32 m_Unk3D298;
    i32 m_Unk3D29C;
    i32 m_Unk3D2A0;
    i32 m_Unk3D2A4;
    u8 m_character;
    u8 m_shotType;
    u8 m_fullShotType;
    u8 m_Unk3dbaa;
    /* 1 byte pad */
    u32 m_Flags;
    u16 m_Unk3DBB0;
    u8 m_IsInGameMenu;
    u8 m_showRetryMenu;
    u8 m_Unk3DBB4;
    u8 m_Unk3DBB5;
    u8 m_Unk3DBB6;
    u8 m_Unk3DBB7;

    u32 m_Unk3DBB8;
    char m_ReplayFilename[512];
    u32 m_Unk3ddbc;
    u32 m_Unk3ddc0;
    i32 m_CurrentStage;
    i32 m_CurrentStage2;
    u32 m_Unk3ddcc;
    u16 m_Unk3DDD0;
    u16 m_Unk3DDD2;
    D3DXVECTOR2 m_arcadeRegionTopLeftPos;
    D3DXVECTOR2 m_arcadeRegionSize;
    D3DXVECTOR2 m_playerMovementTopLeftPos;
    D3DXVECTOR2 m_playerMovementAreaSize;
    f32 m_antiTamperExpectedValue;
    i16 m_youkaiGaugeHumanLimit;
    i16 m_youkaiGaugeYoukaiLimit;
    i16 m_youkaiGaugeHumanEffectsThreshold;
    i16 m_youkaiGaugeYoukaiEffectsThreshold;
    i16 m_youkaiGaugeHumanTintThreshold;
    i16 m_youkaiGaugeYoukaiTintThreshold;

    u32 m_Unk3de04;
    u32 m_Unk3de08;
    u32 m_Unk3de0c;
    u32 m_Unk3de10;
    u32 m_Unk3de14;
    u32 m_Unk3de18;
    u32 m_Unk3de1c;
    u32 m_Unk3de20;
    u32 m_Unk3de24;
    u32 m_Unk3de28;

    i32 m_rank;
    i32 m_maxRank;
    i32 m_minRank;
    i32 m_subRank;
};

C_ASSERT(sizeof(GameManager) == 0x3de3c);

DIFFABLE_EXTERN(GameManager, g_GameManager);
}; // Namespace th08

