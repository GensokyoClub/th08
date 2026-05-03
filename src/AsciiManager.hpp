#pragma once
#include "AnmManager.hpp"
#include "Global.hpp"
#include "Supervisor.hpp"
#include "ZunColor.hpp"
#include "ZunResult.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"

#include <d3dx8.h>

#define ASCII_MAX_STRINGS 256
#define ASCII_MAX_SCORE_POPUPS 720
#define ASCII_MAX_PLAYER_POPUPS 3
#define ASCII_MAX_TIME_POPUPS 128

namespace th08
{

struct PauseMenu
{
    i32 OnUpdate();
    i32 OnDraw();

    u32 curState;
    i32 numFrames;
    AnmVm menuSprites[10];
    AnmVm menuBackground;
};

C_ASSERT(sizeof(PauseMenu) == 0x1d14);

struct RetryMenu
{
    i32 OnUpdate();
    i32 OnDraw();

    u32 curState;
    i32 numFrames;

    AnmVm menuSprites[6];
    AnmVm menuBackground;
};

C_ASSERT(sizeof(RetryMenu) == 0x1284);

struct AsciiManagerString
{
    char text[64];
    D3DXVECTOR3 position;
    D3DCOLOR color;
    f32 scaleX;
    f32 scaleY;
    ZunBool isSelected;
    ZunBool isGui;
};

C_ASSERT(sizeof(AsciiManagerString) == 0x60);

struct AsciiManagerPopup
{
    char text[12];
    D3DXVECTOR3 position;
    D3DCOLOR color;
    ZunTimer timer;
    f32 scaleX;
    f32 scaleY;
    bool inUse;
    BYTE characterCount;
    u32 unk0x34;
};

C_ASSERT(sizeof(AsciiManagerPopup) == 0x38);

struct AsciiManager
{
    static ChainCallbackResult OnUpdate(AsciiManager *mgr);
    static ChainCallbackResult OnDrawLowPrio(AsciiManager *mgr);
    static ChainCallbackResult OnDrawHighPrio(AsciiManager *mgr);
    void Reset();
    void InitializeVms();
    static ZunResult RegisterChain();
    static ZunResult AddedCallback(AsciiManager *mgr);
    static ZunResult DeletedCallback(AsciiManager *mgr);
    static void CutChain();
    void AddString(D3DXVECTOR3 *position, const char *string);
    void AddFormatText(D3DXVECTOR3 *position, const char *fmt, ...);
    int AddFormatText2(D3DXVECTOR3 *position, const char *fmt, ...);
    void OnDrawLowPrioImpl();
    void CreateScorePopup(D3DXVECTOR3 *position, i32 number, D3DCOLOR color);
    void CreatePlayerPointPopup(D3DXVECTOR3 *position, i32 number, D3DCOLOR color);
    void CreateTimePopup(D3DXVECTOR3 *position, i32 number, i32 param3, D3DCOLOR color);
    void CreateFamiliarPopup(D3DXVECTOR3 *position, i32 number, i32 param3, D3DCOLOR color);
    void OnDrawHighPrioImpl();
    void DrawPercentage(D3DXVECTOR3 *position, i32 percentage, D3DCOLOR color);
    void UpdateVms();
    void SetGaugeInterrupt(i32 interrupt);
    i32 GetGaugeInterrupt();
    void ResetStrings();
    void SetSpaceWidth(i32 spaceWidth);

    void SetColor(D3DCOLOR color)
    {
        this->color = color;
    }

    void SetIsSelected(ZunBool selected)
    {
        this->isSelected = selected;
    }

    void SetScale(float scaleX, float scaleY)
    {
        this->scaleX = scaleX;
        this->scaleY = scaleY;
    }

    AnmVm largeText;
    AnmVm smallScoreText;
    AnmVm popupText;
    AnmVm youkaiGauge;
    AnmVm youkaiGaugeHumanIcon;
    AnmVm youkaiGaugeYoukaiIcon;
    AnmVm youkaiGaugeCursor;
    AnmVm percentageText;
    AnmVm unk_1520;

    AnmVm bossMarkers[4];
    i32 bossMarkerStates[4];

    AsciiManagerString strings[ASCII_MAX_STRINGS];
    i32 numStrings;

    D3DCOLOR color;
    f32 scaleX;
    f32 scaleY;
    ZunBool isGui;
    ZunBool isSelected;

    i32 gaugeInterrupt;
    i32 spaceWidth;
    u32 unk_8284;

    AnmLoaded *asciiAnm;
    AnmLoaded *captureAnm;

    i32 nextScorePopupIndex;
    i32 nextPlayerPointPopupIndex;
    i32 nextTimePopupIndex;

    ZunBool unk0x829c;

    PauseMenu pauseMenu;
    RetryMenu retryMenu;

    AnmVm demoIcon;

    AsciiManagerPopup scorePopups[ASCII_MAX_SCORE_POPUPS + ASCII_MAX_PLAYER_POPUPS];
    AsciiManagerPopup timePopups[ASCII_MAX_TIME_POPUPS];

    f32 unk_16f04;
    i32 unk_16f08;

    AnmVm unk_16f0c;
};

C_ASSERT(sizeof(AsciiManager) == 0x171b0);
DIFFABLE_EXTERN(AsciiManager, g_AsciiManager);

} // namespace th08