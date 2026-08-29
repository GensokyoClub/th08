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

struct StdFogData
{
    f32 nearPlane;
    f32 farPlane;
    ZunColor color;
};

struct StdCameraData
{
    D3DXVECTOR3 unk0x0;
    D3DXVECTOR3 unk0xc;
    D3DXVECTOR3 unk0x18;
    D3DXVECTOR3 unk0x24;
    D3DXVECTOR3 unk0x30;
    D3DXVECTOR3 unk0x3c;
    float fov;
};

struct StdRawObject
{
};

struct StdRawQuad
{
};

struct StdRawInstruction
{
    i32 frame;
    i16 opcode;
    i16 size;
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

    AnmVm *quadVms;
    AnmVm vm0;
    AnmVm vm1;
    AnmVm vm2;
    AnmLoaded *backgroundAnm;
    StdRawHeader *stdData;
    i32 quadCount;
    i32 objectCount;
    StdRawObject **objects;
    StdRawQuad **quads;
    StdRawInstruction *beginningOfScript;
    ZunTimer currentTime;
    i32 instructionIdx;
    i32 timer;
    u32 stage;
    D3DXVECTOR3 position;
    D3DCOLOR clearColor;
    u8 unk0x834;
    ZunTimer unk_838;
    AnmVm unk_844;
    AnmVm *unk_ae8;

    StdFogData skyFog;
    StdFogData skyFogInterpInitial;
    StdFogData skyFogInterpFinal;
    i32 skyFogInterpDuration;
    ZunTimer skyFogInterpTimer;
    u8 skyFogNeedsSetup; // Leftover from earlier games. Never checked in IN

    i32 spellcardState;
    i32 ticksSinceSpellcardStarted;
    ZunBool unk_b2c;
    i32 numSpellcardBackgroundVms;
    i32 unk_b34;
    AnmVm spellcardBackgroundVms[32];
    AnmVm unk_5fb8;
    void *callback;
    u32 pendingInterrupt;
    StdCameraData cameraInterpFinal;
    StdCameraData cameraInterpInitial;
    StdCameraData cameraInterpFinalDeriv;
    StdCameraData cameraInterpInitialDeriv;
    StdCameraData camera;
    i32 cameraInterpDurations[5];
    ZunTimer cameraInterpTimers[5];
    i32 cameraInterpModes[5];
    Float3 nextIns0Pos;
    i32 nextIns0Time;
    Float3 prevIns0Pos;
    i32 prevIns0Time;
    u8 jumpFlag;
    ZunColor tintColor;
    ZunBool useTintColor;
    i32 unk0x6470;
    i32 unk0x6474;
    i32 unk0x6478;
    i32 unk0x647c;
    D3DXVECTOR3 unk_6480[32];
};

C_ASSERT(sizeof(Background) == 0x6600);

DIFFABLE_EXTERN(Background, g_Background);
}; // Namespace th08
