#pragma once

#include "AnmManager.hpp"
#include "Global.hpp"
#include "utils.hpp"

#define TRANSITION_SPRITE_ROWS 14
#define TRANSITION_SPRITE_COLS 12

namespace th08
{

enum MsgOpcode
{
    MsgOpcode_MsgDelete,
    MsgOpcode_PortraitAnmScript,
    MsgOpcode_PortraitAnmSprite,
    MsgOpcode_TextDialogue,
    MsgOpcode_Wait,
    MsgOpcode_AnmInterrupt,
    MsgOpcode_EclResume,
    MsgOpcode_Music,
    MsgOpcode_TextIntro,
    MsgOpcode_StageResults,
    MsgOpcode_MsgHalt,
    MsgOpcode_StageEnd,
    MsgOpcode_MusicFadeOut,
    MsgOpcode_WaitSkippable,
    MsgOpcode_ScreenFade,
    MsgOpcode_PortraitConfigureAll,
    MsgOpcode_TextSpeakerDialogue,
    MsgOpcode_PortraitConfigure,
    MsgOpcode_TextboxVisible,
    MsgOpcode_TextTopLine,
    MsgOpcode_TextBottomLine,
    MsgOpcode_SelectionBox,
    MsgOpcode_ReadSelected,
};

struct MsgRawInstrArgPortraitConfigureAll
{
    i32 portraitIndex;
    i32 playerHumanFaceSpriteIndex;
    i32 playerYoukaiFaceSpriteIndex;
    i32 enemyFaceSpriteIndex;
    i32 enemyFace2SpriteIndex;
};

struct MsgRawInstrArgPortraitConfigure
{
    i32 portraitIndex;
    i32 faceSpriteIndex;
};

struct MsgRawInstrArgPortraitAnmScript
{
    i16 portraitIndex;
    i16 anmScriptIndex;
};

struct MsgRawInstrArgPortraitAnmSprite
{
    i16 portraitIndex;
    i16 anmScriptIndex;
};

struct MsgRawInstrArgTextDialogue
{
    i16 textColor;
    i16 textLine;
    char text[1];
};

struct MsgRawInstrArgTextSpeakerDialogue
{
    char text[1];
};

struct MsgRawInstrArgSelectionBox
{
    i32 wait;
};

struct MsgRawInstrArgWait
{
    i32 wait;
};

struct MsgRawInstrArgAnmInterrupt
{
    i16 portraitIndex;
    u8 interrupt;
};

struct MsgRawInstrArgMusic
{
    i32 musicIndex;
};

struct MsgRawInstrArgWaitSkippable
{
    bool skippable;
};

struct MsgRawInstrArgTextboxVisible
{
    bool visible;
};

union MsgRawInstrArgs {
    MsgRawInstrArgPortraitConfigureAll portraitConfigureAll;
    MsgRawInstrArgPortraitConfigure portraitConfigure;
    MsgRawInstrArgPortraitAnmScript portraitAnmScript;
    MsgRawInstrArgPortraitAnmSprite portraitAnmSprite;
    MsgRawInstrArgTextDialogue textDialogue;
    MsgRawInstrArgTextSpeakerDialogue textSpeakerDialogue;
    MsgRawInstrArgSelectionBox selectionBox;
    MsgRawInstrArgWait wait;
    MsgRawInstrArgAnmInterrupt anmInterrupt;
    MsgRawInstrArgMusic music;
    MsgRawInstrArgWaitSkippable waitSkippable;
    MsgRawInstrArgTextboxVisible textboxVisible;
};

struct MsgRawInstr
{
    u16 time;
    u8 opcode;
    u8 instructionSize;
    MsgRawInstrArgs args;
};

struct MsgRawHeader
{
    i32 numInstrs;
    MsgRawInstr *instrs;
};

struct GuiMsgVm
{
    MsgRawHeader *msgFile;
    MsgRawInstr *currentInstr;
    i32 currentMsgIdx;
    ZunTimer timer;
    i32 framesElapsedDuringPause;
    i32 unk_1c;
    AnmVm portraits[4];
    AnmVm dialogueLines[2];
    AnmVm introLines[2];
    COLORREF textColorsA[4];
    COLORREF textColorsB[4];
    u32 fontSize;
    u32 ignoreWaitCounter;
    bool dialogueSkippable;
    u8 textColorIdx;
    bool resetDialogueLines;
    u8 dialogueLineIndex;
    u8 currentPortraitIndex;
    bool isTextBoxVisible;
    u8 selectedOption;
    /* 1 byte pad */
};
C_ASSERT(sizeof(GuiMsgVm) == 0x1570);

enum GuiDisplayArg
{
    GUI_DISPLAY_HIDDEN = 0,
    GUI_DISPLAY_SHOWN = 1,
    GUI_DISPLAY_FULL_POWER = 1,
    GUI_DISPLAY_BORDER = 2,
    GUI_DISPLAY_CHERRY_MAX = 3,
    GUI_DISPLAY_BORDER_BONUS = 4,
    GUI_DISPLAY_SPELL_BONUS_FAILED = 5,
    GUI_DISPLAY_LAST_SPELL_FAILED = 6,
};

struct GuiFormattedText
{
    Float3 pos;
    i32 fmtArg;
    i32 displayArg;
    ZunTimer timer;
};

struct GuiFlags
{
    u32 lifeDisplayUpdateFrames : 2;
    u32 bombDisplayUpdateFrames : 2;
    u32 powerDisplayUpdateFrames : 2;
    u32 grazeDisplayUpdateFrames : 2;
    u32 pointDisplayUpdateFrames : 2;
    u32 timeDisplayUpdateFrames : 2;
};

#define MAX_BOSS_LIFEBAR_SEGMENTS 8

struct Gui
{
    static ChainCallbackResult OnUpdate(Gui *gui);
    static ChainCallbackResult OnDraw(Gui *gui);

    static ZunResult AddedCallback(Gui *gui);
    static ZunResult DeletedCallback(Gui *gui);

    static ZunResult RegisterChain();
    static void CutChain();

    i32 MsgWait();
    i32 IsDialoguePresent();
    void UpdateStageElements();
    void DrawGameScene();
    void DrawStageElements();
    ZunBool IsStageFinished();
    ZunBool IsDialogueSkippable();
    void ShowBonusScore(i32 score);
    void ShowPopupText(i32 fmtArg, i32 popupType);
    void ShowSpellcardBonus(i32 bonus);
    static void CopyEnemyNameTexture(i32 param_1);
    static void UpdateEnemyNameTexture();
    void DrawStageClearScreen();
    void DrawAsciiText();
    void CaptureArcade();
    i32 ShowClockTime();
    i32 FlashClockTimeSlow();
    i32 FlashClockTimeFast();
    i32 HideClockTime();
    ZunResult ActualAddedCallback();
    void InitStageClearScreen();
    ZunResult LoadMsg(const char *path);
    void FreeMsgFile();
    void MsgRead(i32 msgIdx);

    u32 frameCounter;
    GuiFlags flags;
    struct GuiImpl *impl;
    AnmLoaded *frontAnm;
    AnmLoaded *stageTextAnm;
    AnmLoaded *timesAnm;
    AnmLoaded *loadingPortraitAnm;
    u32 bossUIOpacity;
    i32 eclSetLives;
    i32 spellcardSecondsRemaining;
    i32 previousSpellcardSecondsRemaining;
    bool bossPresent;
    f32 bossLifeBarMaxSize;
    f32 bossLifeBarSize;
    unknown_fields(0x38, 0x4);
    f32 bossLifeBarSegmentStop[MAX_BOSS_LIFEBAR_SEGMENTS];
    f32 bossLifeBarSegmentStart[MAX_BOSS_LIFEBAR_SEGMENTS];
    i32 bossLifeBarSegmentColor[MAX_BOSS_LIFEBAR_SEGMENTS];
};
C_ASSERT(sizeof(Gui) == 0x9c);

DIFFABLE_EXTERN(Gui, g_Gui);
DIFFABLE_EXTERN(i32, g_StageBgmIndices[][3]);

} /* namespace th08 */
