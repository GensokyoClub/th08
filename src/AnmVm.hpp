#pragma once

#include "Global.hpp"
#include "Supervisor.hpp"
#include "ZunColor.hpp"

namespace th08
{

enum AnmVariable
{
    AnmVariable_I0 = 10000,
    AnmVariable_I1,
    AnmVariable_I2,
    AnmVariable_I3,
    AnmVariable_F0,
    AnmVariable_F1,
    AnmVariable_F2,
    AnmVariable_F3,
    AnmVariable_IC0,
    AnmVariable_IC1,
};

enum AnmInterp
{
    AnmInterp_Pos,
    AnmInterp_RGB1,
    AnmInterp_Alpha1,
    AnmInterp_Rotate,
    AnmInterp_Scale,
    AnmInterp_RGB2,
    AnmInterp_Alpha2,
    AnmInterp_Last
};

enum AnmInterpMode
{
    AnmInterpMode_Linear = 0,
    AnmInterpMode_EaseIn = 1,
    AnmInterpMode_EaseInCubic = 2,
    AnmInterpMode_EaseInQuartic = 3,
    AnmInterpMode_EaseOut = 4,
    AnmInterpMode_EaseOutCubic = 5,
    AnmInterpMode_EaseOutQuartic = 6
};

enum AnmOpcode
{
    AnmOpcode_EndOfScript = -1,
    AnmOpcode_Nop = 0,
    AnmOpcode_Delete = 1,
    AnmOpcode_Static = 2,
    AnmOpcode_Sprite = 3,
    AnmOpcode_Jmp = 4,
    AnmOpcode_JmpDec = 5,
    AnmOpcode_Pos = 6,
    AnmOpcode_Scale = 7,
    AnmOpcode_Alpha = 8,
    AnmOpcode_Color = 9,
    AnmOpcode_FlipX = 10,
    AnmOpcode_FlipY = 11,
    AnmOpcode_Rotate = 12,
    AnmOpcode_AngularVelocity = 13,
    AnmOpcode_ScaleGrowth = 14,
    AnmOpcode_AlphaTimeLinear = 15,
    AnmOpcode_AdditiveBlendMode = 16,
    AnmOpcode_PosTimeLinear = 17,
    AnmOpcode_PosTimeDecel = 18,
    AnmOpcode_PosTimeDecel2 = 19,
    AnmOpcode_Stop = 20,
    AnmOpcode_InterruptLabel = 21,
    AnmOpcode_AnchorTopLeft = 22,
    AnmOpcode_StopHide = 23,
    AnmOpcode_PosMode = 24,
    AnmOpcode_Ins25 = 25,
    AnmOpcode_AddU = 26,
    AnmOpcode_AddV = 27,
    AnmOpcode_Visible = 28,
    AnmOpcode_ScaleTimeLinear = 29,
    AnmOpcode_ZWriteDisable = 30,
    AnmOpcode_Ins31 = 31,
    AnmOpcode_PosTime = 32,
    AnmOpcode_ColorTime = 33,
    AnmOpcode_AlphaTime = 34,
    AnmOpcode_RotateTime = 35,
    AnmOpcode_ScaleTime = 36,
    AnmOpcode_ISet = 37,
    AnmOpcode_FSet = 38,
    AnmOpcode_IAdd = 39,
    AnmOpcode_FAdd = 40,
    AnmOpcode_ISub = 41,
    AnmOpcode_FSub = 42,
    AnmOpcode_IMul = 43,
    AnmOpcode_FMul = 44,
    AnmOpcode_IDiv = 45,
    AnmOpcode_FDiv = 46,
    AnmOpcode_IMod = 47,
    AnmOpcode_FMod = 48,
    AnmOpcode_ISetAdd = 49,
    AnmOpcode_FSetAdd = 50,
    AnmOpcode_ISetSub = 51,
    AnmOpcode_FSetSub = 52,
    AnmOpcode_ISetMul = 53,
    AnmOpcode_FSetMul = 54,
    AnmOpcode_ISetDiv = 55,
    AnmOpcode_FSetDiv = 56,
    AnmOpcode_ISetMod = 57,
    AnmOpcode_FSetMod = 58,
    AnmOpcode_ISetRand = 59,
    AnmOpcode_FSetRand = 60,
    AnmOpcode_FSin = 61,
    AnmOpcode_FCos = 62,
    AnmOpcode_FTan = 63,
    AnmOpcode_FAcos = 64,
    AnmOpcode_FAtan = 65,
    AnmOpcode_NormalizeAngle = 66,
    AnmOpcode_IJmpEq = 67,
    AnmOpcode_FJmpEq = 68,
    AnmOpcode_IJmpNeq = 69,
    AnmOpcode_FJmpNeq = 70,
    AnmOpcode_IJmpLess = 71,
    AnmOpcode_FJmpLess = 72,
    AnmOpcode_IJmpLessOrEq = 73,
    AnmOpcode_FJmpLessOrEq = 74,
    AnmOpcode_IJmpGreater = 75,
    AnmOpcode_FJmpGreater = 76,
    AnmOpcode_IJmpGreaterOrEq = 77,
    AnmOpcode_FJmpGreaterOrEq = 78,
    AnmOpcode_Wait = 79,
    AnmOpcode_UScroll = 80,
    AnmOpcode_VScroll = 81,
    AnmOpcode_BlendMode = 82,
    AnmOpcode_Ins83 = 83,
    AnmOpcode_Color2 = 84,
    AnmOpcode_Alpha2 = 85,
    AnmOpcode_Color2Time = 86,
    AnmOpcode_Alpha2Time = 87,
    AnmOpcode_Ins88 = 88,
    AnmOpcode_ReturnFromInterrupt = 89
};

struct AnmLoadedSprite
{
    i32 anmIdx;
    IDirect3DTexture8 *texture;
    Float2 startPixelInclusive;
    Float2 endPixelInclusive;
    float height;
    float width;
    Float2 uvStart;
    Float2 uvEnd;
    float heightPx;
    float widthPx;
    Float2 scaleFactor;
    u32 unk0x40;
};

C_ASSERT(sizeof(AnmLoadedSprite) == 0x44);

#define ANM_MAX_ARGS 10

struct AnmRawInstr
{
    i16 opcode;
    u16 instructionSize;
    i16 time;
    u16 varMask;
    union {
        i32 intArgs[ANM_MAX_ARGS];
        f32 floatArgs[ANM_MAX_ARGS];
        u8 byteArgs[ANM_MAX_ARGS * sizeof(i32)];
    };
};

struct AnmVmBase
{
    void Initialize()
    {
        memset(this, 0, sizeof(AnmVmBase));

        this->scale.x = 1.0f;
        this->scale.y = 1.0f;
        this->color1.d3dColor = COLOR_WHITE;
        D3DXMatrixIdentity(&this->matrix1);
        this->flags = 7;
        this->currentTimeInScript.Initialize();
    }

    ZunBool IsVisible()
    {
        return this->visible;
    }

    void SetInvisible()
    {
        this->visible = FALSE;
    }

    ZunBool IsStopped()
    {
        return this->stopped;
    }

    void SetInterrupt(i16 interrupt)
    {
        this->pendingInterrupt = interrupt;
    }

    Float3 rotation;
    Float3 angleVel;
    Float2 scale;
    Float2 scaleGrowth;
    Float2 spriteSize;
    Float2 uvScrollPos;
    ZunTimer currentTimeInScript;
    ZunTimer waitTimer;
    ZunTimer interpCurrentTimers[AnmInterp_Last];
    ZunTimer interpEndTimers[AnmInterp_Last];
    u8 interpModes[AnmInterp_Last];
    i32 intVar0;
    i32 intVar1;
    i32 intVar2;
    i32 intVar3;
    f32 floatVar0;
    f32 floatVar1;
    f32 floatVar2;
    f32 floatVar3;
    i32 counterVar0;
    i32 counterVar1;
    Float2 uvScrollVel;
    D3DXMATRIX matrix1;
    D3DXMATRIX matrix2;
    D3DXMATRIX matrix3;
    ZunColor color1;
    ZunColor color2;
    union {
        u16 flags;
        struct
        {
            u32 visible : 1;
            u32 flag1 : 1;
            u32 updateRotation : 1;
            u32 updateScale : 1;
            u32 blendMode : 2;
            u32 flag6 : 1;
            u32 flag7 : 1;
            u32 usePosOffset : 1;
            u32 flip : 2;
            u32 anchor : 2;
            u32 zWriteDisabled : 1;
            u32 stopped : 1;
            u32 flag15 : 1;
            u32 flag16 : 1;
            u32 flag17 : 1;
            u32 flag18 : 1;
            u32 flag19 : 1;
        };
    };
    i16 type;
    i16 pendingInterrupt;
    i32 playerBulletHitAnimationType;
    AnmLoaded *anmFile;
};

C_ASSERT(sizeof(AnmVmBase) == 0x208);

struct AnmVm : AnmVmBase
{
    Float3 pos;
    i16 activeSpriteIndex;
    i16 anmFileIndex;
    i16 baseSpriteIndex;
    i16 scriptIndex;
    AnmRawInstr *beginningOfScript;
    AnmRawInstr *currentInstruction;
    AnmLoadedSprite *loadedSprite;
    ZunTimer interruptReturnTime;
    AnmRawInstr *interruptReturnInstruction;

    Float3 posInitial;
    Float3 posFinal;
    Float3 rotateInitial;
    Float3 rotateFinal;
    Float2 scaleInitial;
    Float2 scaleFinal;
    ZunColor color1Initial;
    ZunColor color1Final;
    ZunColor color2Initial;
    ZunColor color2Final;

    Float3 pos2;
    i32 timeOfLastSpriteSet;
    u8 fontWidth;
    u8 fontHeight;
    unknown_fields(0x29a, 0xa);

    AnmVm()
    {
        memset(this, 0, sizeof(AnmVm));
        this->activeSpriteIndex = -1;
    }

    f32 GetFloatVar(f32 varId);
    i32 GetIntVar(i32 varId);
    f32 *GetFloatVarPtr(f32 *varPtr, u16 varMask, u32 variableNumber);
    i32 *GetIntVarPtr(i32 *varPtr, u16 varMask, u32 variableNumber);
};

C_ASSERT(sizeof(AnmVm) == 0x2a4);

} // namespace th08
