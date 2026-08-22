#pragma once
#include "Global.hpp"
#include "ZunResult.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include <windows.h>

namespace th08
{

struct StdRawHeader
{
    i16 nbObjects;
    i16 nbQuads;
    u32 quadsOffset;
    u32 scriptOffset;
    i32 unk_c;
    char stageName[128];
    char songNames[4][128];
    char songPaths[4][128];
};

struct Background
{
    Background();

    static ChainCallbackResult OnUpdate(Background *background);
    static ChainCallbackResult OnDrawHighPrio(Background *background);
    static ChainCallbackResult OnDrawLowPrio(Background *background);
    static ZunResult AddedCallback(Background *background);
    static ZunResult RegisterChain(u32 stage);
    static ZunResult DeletedCallback();
    static void CutChain();
    ZunResult LoadStageData();

    void SetCamera1()
    {
    }

    void SetCamera2()
    {
    }

    unknown_fields(0x0, 0x7f4);
    StdRawHeader *stdData;
    unknown_fields(0x7f8, 0x325);
    u8 skyFogNeedsSetup; // Leftover from earlier games. Never checked in IN
    unknown_fields(0xb21, 0x13);
    i32 unk_b34;
    unknown_fields(0xb38, 0x5ac8);
};
C_ASSERT(sizeof(Background) == 0x6600);

DIFFABLE_EXTERN(Background, g_Background);
}; // Namespace th08
