#pragma once

#include <d3d8.h>
#include <d3dx8math.h>
#include <dinput.h>

#include "Midi.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include "ZunBool.hpp"
#include "Global.hpp"


namespace th08
{
#define GAME_VERSION 0x80001
#define ZWAV_MAGIC 'VAWZ'

enum MusicMode
{
    OFF = 0,
    WAV = 1,
    MIDI = 2
};

enum Difficulty
{
    EASY,
    NORMAL,
    HARD,
    LUNATIC,
    EXTRA,
};

enum EffectQuality
{
    MINIMUM,
    MODERATE,
    MAXIMUM
};

enum FogState
{
    FOG_DISABLED = 0,
    FOG_ENABLED = 1,
    FOG_UNSET = 0xff
};

struct GameConfigOpts
{
    u32 useSwTextureBlending : 1;
    u32 dontUseVertexBuf : 1;
    u32 force16bitTextures : 1;
    u32 clearBackBufferOnRefresh : 1;
    u32 displayMinimumGraphics : 1;
    u32 suppressUseOfGoroudShading : 1;
    u32 disableDepthTest : 1;
    u32 force60Fps : 1;
    u32 disableColorCompositing : 1;
    u32 referenceRasterizerMode : 1;
    u32 disableFog : 1;
    u32 dontUseDirectInput : 1;
    u32 redrawHUDEveryFrame : 1;
    u32 preloadMusic : 1;
    u32 disableVsync : 1;
    u32 dontDetectTextDrawingBackground : 1;
};

struct GameConfiguration
{
    ControllerMapping controllerMapping;
    i32 version;
    i16 padXAxis;
    i16 padYAxis;
    u8 lifeCount;
    u8 bombCount;
    u8 colorMode16bit;
    u8 musicMode;
    u8 playSounds;
    u8 defaultDifficulty;
    u8 windowed;
    // 0 = fullspeed, 1 = 1/2 speed, 2 = 1/4 speed.
    u8 frameskipConfig;
    u8 effectQuality;
    u8 slowMode;
    u8 shotSlow;
    u8 musicVolume;
    u8 sfxVolume;
    i8 unk29[15];
    GameConfigOpts opts;
};

struct SupervisorFlags
{
    u32 usingHardwareTL : 1;
    u32 unk1 : 1; // Unconditionally set in InitD3DRendering. Never cleared?
    u32 using32BitGraphics : 1;
    u32 unk3 : 1;
    u32 d3dDevDisconnectFlag : 1;
    u32 unk5 : 1;
    u32 unk6 : 1; // Set if LPTITLE is NULL in the startup info, which seems to never be true?
    u32 receivedCloseMsg : 1;
};

struct Supervisor
{
    static ZunResult RegisterChain();

    static ChainCallbackResult OnUpdate(Supervisor *s);
    static ZunResult AddedCallback(Supervisor *s);
    static ZunResult DeletedCallback(Supervisor *s);
    static ChainCallbackResult DrawFpsCounter(Supervisor *s);
    static ChainCallbackResult OnDraw2(Supervisor *s);
    static ChainCallbackResult OnDraw3(Supervisor *s);

    ZunResult LoadConfig(char *configFile);
    void ThreadClose();
    void InitializeCriticalSections();
    void DeleteCriticalSections();
    ZunBool TakeSnapshot(char *filePath);
    void SetRenderState(D3DRENDERSTATETYPE renderStateType, int value);
    i32 DisableFog();
    i32 EnableFog();
    static void UpdatePlayTime(Supervisor *s);
    static void UpdateGameTime(Supervisor *s);

    ZunResult ThreadStart(LPTHREAD_START_ROUTINE startFunction, void *startParam);

    ZunBool IsShotSlowEnabled()
    {
        return this->m_Cfg.shotSlow;
    }

    ZunBool ShouldForceBackbufferClear()
    {
        return this->m_Cfg.opts.clearBackBufferOnRefresh | this->m_Cfg.opts.displayMinimumGraphics;
    }

    ZunBool IsHardwareBlendingDisabled()
    {
        return m_Cfg.opts.useSwTextureBlending;
    }

    ZunBool IsVertexBufferDisabled()
    {
        return m_Cfg.opts.dontUseVertexBuf;
    }

    ZunBool Is16bitColorMode()
    {
        return m_Cfg.opts.force16bitTextures;
    }

    ZunBool IsDepthTestDisabled()
    {
        return m_Cfg.opts.disableDepthTest;
    }

    ZunBool IsColorCompositingDisabled()
    {
        return m_Cfg.opts.disableColorCompositing;
    }

    ZunBool IsFogDisabled()
    {
        return m_Cfg.opts.disableFog;
    }

    ZunBool Supervisor::IsHUDRedrawEnabled()
    {
        return this->m_Cfg.opts.redrawHUDEveryFrame;
    }

    ZunBool IsReferenceRasterizerMode()
    {
        return m_Cfg.opts.referenceRasterizerMode;
    }

    u32 IsMusicPreloadEnabled()
    {
        return this->m_Cfg.opts.preloadMusic;
    }

    ZunBool IsWindowed()
    {
        return m_Cfg.windowed;
    }

    void EnterCriticalSectionWrapper(int id)
    {
        EnterCriticalSection(&m_CriticalSections[id]);
        m_LockCounts[id]++;
    }

    void LeaveCriticalSectionWrapper(int id)
    {
        LeaveCriticalSection(&m_CriticalSections[id]);
        m_LockCounts[id]--;
    }

    HINSTANCE m_hInstance;
    PDIRECT3D8 m_D3dIface;
    PDIRECT3DDEVICE8 m_D3dDevice;
    LPDIRECTINPUTDEVICE8A m_Keyboard;
    LPDIRECTINPUTDEVICE8A m_Controller;
    DIDEVCAPS m_ControllerCaps;

    u8 unk40[0x04];

    HWND m_HwndGameWindow;
    D3DXMATRIX m_ViewMatrix;
    D3DXMATRIX m_ProjectionMatrix;
    D3DVIEWPORT8 m_Viewport;
    D3DPRESENT_PARAMETERS m_PresentParameters;
    MidiTimer *m_MidiTimer;
    GameConfiguration m_Cfg;
    i32 m_CalcCount;
    i32 m_WantedState;
    i32 m_CurState;
    i32 m_WantedState2;
    unknown_fields(0x164, 0x10);
    i32 m_Unk174; // Commonly set for screen transitions and decremented once per frame, but never actually used for anything
    unknown_fields(0x178, 0x4);
    BOOL m_DisableVsync;
    ZunBool m_CouldSetRefreshRate;
    i32 m_LastFrameTime; // Unused in IN
    f32 framerateMultiplier;
    MidiOutput *midiOutput;
    unknown_fields(0x190, 0x14);
    SupervisorFlags m_Flags;
    DWORD m_TotalPlayTime;
    DWORD m_SystemTime;
    D3DCAPS8 m_D3dCaps;
    HANDLE m_runningSubthreadHandle;
    DWORD m_runningSubthreadID;
    BOOL m_Unk28c;
    BOOL m_Unk290;
    unknown_fields(0x294, 0x4);
    CRITICAL_SECTION m_CriticalSections[4];
    u8 m_LockCounts[4];
    DWORD unk2fc;

    unknown_fields(0x300, 0x50);

    FogState m_FogState;
    u32 m_ExeChecksum;
    u32 m_ExeSize;

    unknown_fields(0x35c, 0x8);
};
C_ASSERT(sizeof(Supervisor) == 0x364);

DIFFABLE_EXTERN(Supervisor, g_Supervisor);
}; // namespace th08
