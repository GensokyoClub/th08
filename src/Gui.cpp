#include "th_pch.h"

#include "Background.hpp"
#include "BulletManager.hpp"
#include "EnemyManager.hpp"
#include "Gui.hpp"
#include "ItemManager.hpp"
#include "Player.hpp"
#include "ReplayManager.hpp"
#include "ScoreDat.hpp"
#include "ScreenEffect.hpp"
#include "Spellcard.hpp"
#include "ZunResult.hpp"
#include "i18n.hpp"
#include "utils.hpp"
#include <stdio.h>

namespace th08
{

struct GuiImpl
{
    void MsgRead(i32 msgIdx);
    ZunResult RunMsg();
    static void DecryptDialogue(char *dst, char *src)
    {
        i32 i = 0;
        char c;
        do
        {
            c = *src ^ 0x77;
            dst[i] = c;
            i++;
            src++;
        } while (c != 0);
    }
    ZunResult DrawDialogue();

    AnmVm frontSprites[16];
    u8 bossLifeBarState;
    /* 3 bytes pad */
    AnmVm stageTextSprites[4];
    AnmVm clockTimeIntroSprite;
    AnmVm loadingPortraitSprite;
    AnmVm unk_3a1c;
    AnmVm arcadeZoneSprite;
    AnmVm arcadeZoneMotionBlurSprites[8];
    AnmVm nullifySprite;
    AnmVm stageTransitionSprites[TRANSITION_SPRITE_ROWS * TRANSITION_SPRITE_COLS];
    AnmVm stageRankSprite;
    AnmVm clockTimeSprite;
    i32 stageTransitionActiveScriptCount;
    GuiMsgVm msg;
    i32 stageClearScreenCounter;
    i32 clearBonusTotal;
    GuiFormattedText bonusScore;
    GuiFormattedText popupText;
    GuiFormattedText spellcardBonus;
    i32 clearBonusStage;
    i32 clearBonusPower;
    i32 clearBonusPointItems;
    i32 clearBonusGraze;
    i32 clearBonusTime;
    i32 clockTimeIncrement;
    i32 clearScreenClockTimeOld;
    i32 clearScreenClockTime;
    i32 clearScreenDisplayedClockTime;
    i32 clearScreenClockTimeUpdateDelayCounter;
    AnmVm difficultySprite;
};
C_ASSERT(sizeof(GuiImpl) == 0x230b8);

DIFFABLE_STATIC(Gui, g_Gui);
DIFFABLE_STATIC(ChainElem, g_GuiCalcChain);
DIFFABLE_STATIC(ChainElem, g_GuiDrawChain);

i32 g_ClearStageBonuses[] = {1000000, 1500000, 2000000, 2500000, 2500000, 3000000, 4000000, 6000000, 6660000};
// These are all exactly the same??????? Wtf???????????
COLORREF g_GuiTextColors[][4] = {{0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0}, {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0},
                                 {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0}, {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0},
                                 {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0}, {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0},
                                 {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0}, {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0},
                                 {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0}, {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0},
                                 {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0}, {0xe8f0ff, 0xf0e8ff, 0xffe8f0, 0xffe8f0}};
i32 g_StageBgmIndices[][3] = {
    {1, 2, 0}, {3, 4, 0}, {5, 6, 0}, {7, 8, 0}, {7, 9, 0}, {10, 11, 0}, {12, 13, 15}, {12, 14, 15}, {16, 17, 0},
};
D3DCOLOR g_SpellcardTimeColors[] = {0xa0d0ff, 0xa080ff, 0xe080c0, 0xff4040};
const char *g_AmPmStrings[] = {"AM", "PM"};
const char *g_LoadingAnms[] = {"loading00.anm",  "loading01.anm",  "loading02.anm",  "loading03.anm",
                               "loading00h.anm", "loading00a.anm", "loading01h.anm", "loading01a.anm",
                               "loading02h.anm", "loading02a.anm", "loading03h.anm", "loading03a.anm"};
const char *g_StageTextAnms[] = {
    "stg1txt.anm", "stg2txt.anm", "stg3txt.anm", "stg4atxt.anm", "stg4btxt.anm",
    "stg5txt.anm", "stg6txt.anm", "stg7txt.anm", "stg8txt.anm",
};
const char *g_MsgFiles[][12] = {{"msg1a.dat", "msg1b.dat", "msg1c.dat", "msg1d.dat", "msg1a.dat", "msg1a.dat",
                                 "msg1b.dat", "msg1b.dat", "msg1c.dat", "msg1c.dat", "msg1d.dat", "msg1d.dat"},
                                {"msg2a.dat", "msg2b.dat", "msg2c.dat", "msg2d.dat", "msg2a.dat", "msg2a.dat",
                                 "msg2b.dat", "msg2b.dat", "msg2c.dat", "msg2c.dat", "msg2d.dat", "msg2d.dat"},
                                {"msg3a.dat", "msg3b.dat", "msg3c.dat", "msg3d.dat", "msg3a.dat", "msg3a.dat",
                                 "msg3b.dat", "msg3b.dat", "msg3c.dat", "msg3c.dat", "msg3d.dat", "msg3d.dat"},
                                {"msg4dm.dat", "msg4ab.dat", "msg4ac.dat", "msg4dm.dat", "msg4dm.dat", "msg4dm.dat",
                                 "msg4ab.dat", "msg4ab.dat", "msg4ac.dat", "msg4ac.dat", "msg4dm.dat", "msg4dm.dat"},
                                {"msg4ba.dat", "msg4dm.dat", "msg4dm.dat", "msg4bd.dat", "msg4ba.dat", "msg4ba.dat",
                                 "msg4dm.dat", "msg4dm.dat", "msg4dm.dat", "msg4dm.dat", "msg4bd.dat", "msg4bd.dat"},
                                {"msg5a.dat", "msg5b.dat", "msg5c.dat", "msg5d.dat", "msg5a.dat", "msg5a.dat",
                                 "msg5b.dat", "msg5b.dat", "msg5c.dat", "msg5c.dat", "msg5d.dat", "msg5d.dat"},
                                {"msg6a.dat", "msg6b.dat", "msg6c.dat", "msg6d.dat", "msg6a.dat", "msg6a.dat",
                                 "msg6b.dat", "msg6b.dat", "msg6c.dat", "msg6c.dat", "msg6d.dat", "msg6d.dat"},
                                {"msg7a.dat", "msg7b.dat", "msg7c.dat", "msg7d.dat", "msg7a.dat", "msg7a.dat",
                                 "msg7b.dat", "msg7b.dat", "msg7c.dat", "msg7c.dat", "msg7d.dat", "msg7d.dat"},
                                {"msg8a.dat", "msg8b.dat", "msg8c.dat", "msg8d.dat", "msg8a.dat", "msg8a.dat",
                                 "msg8b.dat", "msg8b.dat", "msg8c.dat", "msg8c.dat", "msg8d.dat", "msg8d.dat"}};

ChainCallbackResult Gui::OnUpdate(Gui *gui)
{
    if (g_GameManager.unk2C != 0)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

    gui->UpdateStageElements();
    gui->impl->RunMsg();
    if (IS_PRESSED(TH_BUTTON_SKIP) && g_Supervisor.unk174 < 8)
    {
        g_Supervisor.unk174 = 8;
    }
    gui->frameCounter++;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult Gui::OnDraw(Gui *gui)
{
    if (gui->impl->stageClearScreenCounter != 0)
    {
        gui->DrawStageClearScreen();
    }
    gui->impl->DrawDialogue();
    gui->DrawStageElements();
    gui->DrawGameScene();
    gui->DrawAsciiText();
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

void GuiImpl::MsgRead(i32 msgIdx)
{
    // \n\r?
    utils::GuiDebugPrint("msg start %d\n\r", msgIdx);
    MsgRawHeader *file = this->msg.msgFile;
    memset(&this->msg, 0, sizeof(GuiMsgVm));
    this->msg.msgFile = file;
    if (msgIdx == 0)
    {
        switch (g_GameManager.currentStage)
        {
        case STAGE5:
            Gui::CopyEnemyNameTexture(22);
            break;
        case STAGE6A:
            g_Background.unk_b34 = 2;
            break;
        case STAGE6B: {
            AnmLoaded *enemyFaceAnm = g_Spellcard.enemyFaceAnm;
            g_Spellcard.enemyFaceAnm = g_Spellcard.enemyFaceAnm2;
            g_Spellcard.enemyFaceAnm2 = enemyFaceAnm;
            g_Background.unk_b34 = 2;
            Gui::CopyEnemyNameTexture(24);
            break;
        }
        case EXTRASTAGE: {
            AnmLoaded *enemyFaceAnm = g_Spellcard.enemyFaceAnm;
            g_Spellcard.enemyFaceAnm = g_Spellcard.enemyFaceAnm2;
            g_Spellcard.enemyFaceAnm2 = enemyFaceAnm;
            g_Background.unk_b34 = 2;
            Gui::CopyEnemyNameTexture(25);
            break;
        }
        }
    }
    else if (msgIdx == 10)
    {
        switch (g_GameManager.currentStage)
        {
        case STAGE5:
            if (g_GameManager.globals->numRetries > 0)
            {
                msgIdx = 1;
                this->msg.selectedOption = 0;
            }
            else if (!g_GameManager.IsReplay())
            {
                if (g_GameManager.IsStageClearedWithoutRetries(STAGE6B, g_GameManager.shotType, EASY) ||
                    g_GameManager.IsStageClearedWithoutRetries(STAGE6B, g_GameManager.shotType, NORMAL) ||
                    g_GameManager.IsStageClearedWithoutRetries(STAGE6B, g_GameManager.shotType, HARD) ||
                    g_GameManager.IsStageClearedWithoutRetries(STAGE6B, g_GameManager.shotType, LUNATIC) ||
                    g_GameManager.shotType > SHOT_YOUMU_YUYUKO)
                {
                    msgIdx = 3;
                    this->msg.selectedOption = 1;
                }
                else if (g_GameManager.IsStageClearedWithRetries(STAGE6A, g_GameManager.shotType, EASY) ||
                         g_GameManager.IsStageClearedWithRetries(STAGE6A, g_GameManager.shotType, NORMAL) ||
                         g_GameManager.IsStageClearedWithRetries(STAGE6A, g_GameManager.shotType, HARD) ||
                         g_GameManager.IsStageClearedWithRetries(STAGE6A, g_GameManager.shotType, LUNATIC))
                {
                    msgIdx = 2;
                    this->msg.selectedOption = 1;
                }
                else
                {
                    msgIdx = 1;
                    this->msg.selectedOption = 0;
                }
            }
            else
            {
                if (g_ReplayManager->replayData->clearState == 2)
                {
                    msgIdx = 3;
                    this->msg.selectedOption = 1;
                }
                else if (g_ReplayManager->replayData->clearState == 1)
                {
                    msgIdx = 2;
                    this->msg.selectedOption = 1;
                }
                else
                {
                    msgIdx = 1;
                    this->msg.selectedOption = 0;
                }
            }
            g_GameManager.flags.isGoingToFinalB = this->msg.selectedOption;
        default:
            break;
        }
    }
    else if (msgIdx >= 6)
    {
        switch (g_GameManager.currentStage)
        {
        case STAGE6B:
            if (g_GameManager.GetClockTime() >= 12)
            {
                msgIdx = 5;
            }
            break;
        }
    }
    this->msg.currentMsgIdx = msgIdx;
    this->msg.currentInstr = (&this->msg.msgFile->instrs)[msgIdx];
    this->msg.dialogueLines[0].scriptIndex = -1;
    this->msg.dialogueLines[1].scriptIndex = -1;
    this->msg.isTextBoxVisible = true;
    this->msg.fontSize = 15;
    this->msg.textColorsA[0] = g_GuiTextColors[g_GameManager.shotType][0];
    this->msg.textColorsA[1] = g_GuiTextColors[g_GameManager.shotType][1];
    this->msg.textColorsA[2] = g_GuiTextColors[g_GameManager.shotType][2];
    this->msg.textColorsA[3] = g_GuiTextColors[g_GameManager.shotType][3];
    this->msg.textColorsB[0] = 0;
    this->msg.textColorsB[1] = 0;
    this->msg.textColorsB[2] = 0;
    this->msg.textColorsB[3] = 0;
    this->msg.dialogueSkippable = true;
    this->msg.unk_1c = 6;
    this->msg.textColorIdx = 0;
    this->msg.resetDialogueLines = true;
    this->msg.dialogueLineIndex = 0;
    this->msg.currentPortraitIndex = 255;
    g_BulletManager.FUN_00415c60();
    g_EnemyManager.DespawnAllEnemies(0, 0);
    g_ItemManager.AutoCollectAllItems();
}

ZunResult GuiImpl::RunMsg()
{
    MsgRawInstrArgs *args;

    if (this->msg.currentMsgIdx < 0)
    {
        return ZUN_ERROR;
    }

    if (this->msg.ignoreWaitCounter > 0)
    {
        this->msg.ignoreWaitCounter--;
    }

    if (this->msg.dialogueSkippable && IS_PRESSED_REPLAY(TH_BUTTON_SKIP))
    {
        this->msg.timer = this->msg.currentInstr->time;
    }

    if (g_Player.playerState != PLAYER_STATE_DEAD)
    {
        g_ItemManager.AutoCollectAllItems();
    }

    while (this->msg.timer >= (i32)this->msg.currentInstr->time)
    {
        switch (this->msg.currentInstr->opcode)
        {
        case MsgOpcode_MsgDelete:
            this->msg.currentMsgIdx = -1;
            return ZUN_ERROR;
#pragma var_order(i, args)
        case MsgOpcode_PortraitConfigureAll: {
            u32 i;
            MsgRawInstrArgs *args = &this->msg.currentInstr->args;
            if (this->msg.currentPortraitIndex != args->portraitConfigureAll.portraitIndex)
            {
                for (i = 0; i < 4; i++)
                {
                    if (this->msg.currentPortraitIndex == i)
                    {
                        if (this->msg.currentPortraitIndex / 2 != args->portraitConfigureAll.portraitIndex / 2)
                        {
                            this->msg.portraits[i].pendingInterrupt = 6;
                        }
                        else
                        {
                            this->msg.portraits[i].pendingInterrupt = 4;
                        }
                    }
                    else
                    {
                        this->msg.portraits[i].pendingInterrupt = 4;
                    }
                }
            }
            this->msg.portraits[args->portraitConfigureAll.portraitIndex].pendingInterrupt = 3;
            this->msg.currentPortraitIndex = (u8)args->portraitConfigureAll.portraitIndex;
            if (args->portraitConfigureAll.playerHumanFaceSpriteIndex >= 0)
            {
                g_Spellcard.playerHumanFaceAnm->SetSprite(&this->msg.portraits[0],
                                                          args->portraitConfigureAll.playerHumanFaceSpriteIndex);
            }
            if (args->portraitConfigureAll.playerYoukaiFaceSpriteIndex >= 0)
            {
                g_Spellcard.playerYoukaiFaceAnm->SetSprite(&this->msg.portraits[1],
                                                           args->portraitConfigureAll.playerYoukaiFaceSpriteIndex);
            }
            if (args->portraitConfigureAll.enemyFaceSpriteIndex >= 0)
            {
                g_Spellcard.enemyFaceAnm->SetSprite(&this->msg.portraits[2],
                                                    args->portraitConfigureAll.enemyFaceSpriteIndex);
            }
            if (args->portraitConfigureAll.enemyFace2SpriteIndex >= 0)
            {
                g_Spellcard.enemyFaceAnm2->SetSprite(&this->msg.portraits[3],
                                                     args->portraitConfigureAll.enemyFace2SpriteIndex);
            }
            this->msg.textColorIdx = (u8)args->portraitConfigureAll.portraitIndex;
            this->msg.resetDialogueLines = true;
            break;
        }
#pragma var_order(i, args)
        case MsgOpcode_PortraitConfigure: {
            u32 i;
            MsgRawInstrArgs *args = &this->msg.currentInstr->args;
            if (this->msg.currentPortraitIndex != args->portraitConfigure.portraitIndex)
            {
                for (i = 0; i < 4; i++)
                {
                    if (this->msg.currentPortraitIndex == i)
                    {
                        if (this->msg.currentPortraitIndex / 2 != args->portraitConfigure.portraitIndex / 2)
                        {
                            this->msg.portraits[i].pendingInterrupt = 6;
                        }
                        else
                        {
                            this->msg.portraits[i].pendingInterrupt = 4;
                        }
                    }
                    else
                    {
                        this->msg.portraits[i].pendingInterrupt = 4;
                    }
                }
            }
            this->msg.portraits[args->portraitConfigure.portraitIndex].pendingInterrupt = 3;
            this->msg.currentPortraitIndex = (u8)args->portraitConfigure.portraitIndex;
            if (args->portraitConfigure.faceSpriteIndex >= 0)
            {
                switch (args->portraitConfigure.portraitIndex)
                {
                case 0:
                    g_Spellcard.playerHumanFaceAnm->SetSprite(&this->msg.portraits[0],
                                                              args->portraitConfigure.faceSpriteIndex);
                    break;
                case 1:
                    g_Spellcard.playerYoukaiFaceAnm->SetSprite(&this->msg.portraits[1],
                                                               args->portraitConfigure.faceSpriteIndex);
                    break;
                case 2:
                    g_Spellcard.enemyFaceAnm->SetSprite(&this->msg.portraits[2],
                                                        args->portraitConfigure.faceSpriteIndex);
                    break;
                case 3:
                    g_Spellcard.enemyFaceAnm2->SetSprite(&this->msg.portraits[3],
                                                         args->portraitConfigure.faceSpriteIndex);
                    break;
                }
            }
            this->msg.textColorIdx = (u8)args->portraitConfigure.portraitIndex;
            this->msg.resetDialogueLines = true;
            break;
        }
        case MsgOpcode_PortraitAnmScript:
            args = &this->msg.currentInstr->args;
            switch (args->portraitAnmScript.portraitIndex)
            {
            case 0:
                g_Spellcard.playerHumanFaceAnm->SetAndExecuteScriptIdx(&this->msg.portraits[0],
                                                                       args->portraitAnmScript.anmScriptIndex);
                break;
            case 1:
                g_Spellcard.playerYoukaiFaceAnm->SetAndExecuteScriptIdx(&this->msg.portraits[1],
                                                                        args->portraitAnmScript.anmScriptIndex);
                break;
            case 2:
                g_Spellcard.enemyFaceAnm->SetAndExecuteScriptIdx(&this->msg.portraits[2],
                                                                 args->portraitAnmScript.anmScriptIndex);
                break;
            case 3:
                g_Spellcard.enemyFaceAnm2->SetAndExecuteScriptIdx(&this->msg.portraits[3],
                                                                  args->portraitAnmScript.anmScriptIndex);
                break;
            }
            if (this->msg.portraits[args->portraitAnmScript.portraitIndex].loadedSprite->widthPx > 128.0f)
            {
                this->msg.portraits[args->portraitAnmScript.portraitIndex].pos2.x = -112.0f;
            }
            else
            {
                this->msg.portraits[args->portraitAnmScript.portraitIndex].pos2.x = 0.0f;
            }
            break;
        case MsgOpcode_PortraitAnmSprite:
            args = &this->msg.currentInstr->args;
            switch (args->portraitAnmSprite.portraitIndex)
            {
            case 0:
                g_Spellcard.playerHumanFaceAnm->SetSprite(&this->msg.portraits[0],
                                                          args->portraitAnmSprite.anmScriptIndex);
                break;
            case 1:
                g_Spellcard.playerYoukaiFaceAnm->SetSprite(&this->msg.portraits[1],
                                                           args->portraitAnmSprite.anmScriptIndex);
                break;
            case 2:
                g_Spellcard.enemyFaceAnm->SetSprite(&this->msg.portraits[2], args->portraitAnmSprite.anmScriptIndex);
                break;
            case 3:
                g_Spellcard.enemyFaceAnm2->SetSprite(&this->msg.portraits[3], args->portraitAnmSprite.anmScriptIndex);
                break;
            }
            if (this->msg.portraits[args->portraitAnmSprite.portraitIndex].loadedSprite->widthPx > 256.0f)
            {
                this->msg.portraits[args->portraitAnmSprite.portraitIndex].pos2.x = -208.0f;
                this->msg.portraits[args->portraitAnmSprite.portraitIndex].pos2.y = -50.0f;
            }
            else if (this->msg.portraits[args->portraitAnmSprite.portraitIndex].loadedSprite->widthPx > 128.0f)
            {
                this->msg.portraits[args->portraitAnmSprite.portraitIndex].pos2.x = -80.0f;
            }
            else
            {
                this->msg.portraits[args->portraitAnmSprite.portraitIndex].pos2.x = 0.0f;
            }
            break;
        case MsgOpcode_TextDialogue: {
            char decryptedText[64];

            args = &this->msg.currentInstr->args;
            if (args->textDialogue.textLine == 0 && this->msg.dialogueLines[1].scriptIndex >= 0)
            {
                g_AnmManager->DrawTextLeft(&this->msg.dialogueLines[1],
                                           this->msg.textColorsA[args->textDialogue.textColor],
                                           this->msg.textColorsB[args->textDialogue.textColor], " ");
            }
            g_Supervisor.textAnm->SetAndExecuteScriptIdx(&this->msg.dialogueLines[args->textDialogue.textLine],
                                                         args->textDialogue.textLine);
            this->msg.dialogueLines[args->textDialogue.textLine].fontWidth =
                this->msg.dialogueLines[args->textDialogue.textLine].fontHeight = this->msg.fontSize;
            DecryptDialogue(decryptedText, args->textDialogue.text);
            g_AnmManager->DrawTextLeft(&this->msg.dialogueLines[args->textDialogue.textLine],
                                       this->msg.textColorsA[args->textDialogue.textColor],
                                       this->msg.textColorsB[args->textDialogue.textColor], decryptedText);
            this->msg.framesElapsedDuringPause = 0;
            break;
        }
        case MsgOpcode_TextSpeakerDialogue: {
            char decryptedText[64];

            args = &this->msg.currentInstr->args;
            if (this->msg.resetDialogueLines)
            {
                if (this->msg.dialogueLines[1].scriptIndex >= 0)
                {
                    g_AnmManager->DrawTextLeft(&this->msg.dialogueLines[1],
                                               this->msg.textColorsA[this->msg.textColorIdx],
                                               this->msg.textColorsB[this->msg.textColorIdx], " ");
                }
                this->msg.dialogueLineIndex = 0;
            }
            g_Supervisor.textAnm->SetAndExecuteScriptIdx(&this->msg.dialogueLines[this->msg.dialogueLineIndex],
                                                         this->msg.dialogueLineIndex);
            this->msg.dialogueLines[this->msg.dialogueLineIndex].fontWidth =
                this->msg.dialogueLines[this->msg.dialogueLineIndex].fontHeight = this->msg.fontSize;
            DecryptDialogue(decryptedText, args->textSpeakerDialogue.text);
            g_AnmManager->DrawTextLeft(&this->msg.dialogueLines[this->msg.dialogueLineIndex],
                                       this->msg.textColorsA[this->msg.textColorIdx],
                                       this->msg.textColorsB[this->msg.textColorIdx], decryptedText);
            this->msg.framesElapsedDuringPause = 0;
            this->msg.resetDialogueLines = false;
            this->msg.dialogueLineIndex++;
            break;
        }
        case MsgOpcode_TextTopLine: {
            char decryptedText[64];

            args = &this->msg.currentInstr->args;
            g_Supervisor.textAnm->SetAndExecuteScriptIdx(&this->msg.dialogueLines[0], 0);
            this->msg.dialogueLines[0].fontWidth = this->msg.dialogueLines[0].fontHeight = this->msg.fontSize;
            DecryptDialogue(decryptedText, args->textSpeakerDialogue.text);
            g_AnmManager->DrawTextLeft(&this->msg.dialogueLines[0], this->msg.textColorsA[0], this->msg.textColorsB[0],
                                       decryptedText);
            this->msg.framesElapsedDuringPause = 0;
            break;
        }
        case MsgOpcode_TextBottomLine: {
            char decryptedText[64];

            args = &this->msg.currentInstr->args;
            g_Supervisor.textAnm->SetAndExecuteScriptIdx(&this->msg.dialogueLines[1], 1);
            this->msg.dialogueLines[1].fontWidth = this->msg.dialogueLines[1].fontHeight = this->msg.fontSize;
            DecryptDialogue(decryptedText, args->textSpeakerDialogue.text);
            g_AnmManager->DrawTextLeft(&this->msg.dialogueLines[1], this->msg.textColorsA[0], this->msg.textColorsB[0],
                                       decryptedText);
            this->msg.framesElapsedDuringPause = 0;
            break;
        }
        case MsgOpcode_SelectionBox:
            if (WAS_PRESSED_REPLAY(TH_BUTTON_UP))
            {
                if (this->msg.selectedOption == 1)
                {
                    g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
                }
                this->msg.selectedOption = 0;
            }
            if (WAS_PRESSED_REPLAY(TH_BUTTON_DOWN))
            {
                if (this->msg.selectedOption == 0)
                {
                    g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
                }
                this->msg.selectedOption = 1;
            }
            this->msg.dialogueLines[this->msg.selectedOption].color1.d3dColor = 0xffffffff;
            this->msg.dialogueLines[1 - this->msg.selectedOption].color1.d3dColor = 0xe0606060;
            if (!WAS_PRESSED_REPLAY(TH_BUTTON_SHOOT) || this->msg.framesElapsedDuringPause < 60)
            {
                if (this->msg.framesElapsedDuringPause >= this->msg.currentInstr->args.selectionBox.wait)
                {
                    this->msg.resetDialogueLines = true;
                    this->msg.unk_1c = 30;
                    break;
                }
                this->msg.framesElapsedDuringPause++;
                goto SKIP_TIME_INCREMENT;
            }
            else
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
            }
            break;
        case MsgOpcode_ReadSelected:
            g_GameManager.flags.isGoingToFinalB = this->msg.selectedOption;
            g_Gui.MsgRead(this->msg.selectedOption + 1);
            continue;
        case MsgOpcode_Wait:
            if (!this->msg.dialogueSkippable || !IS_PRESSED_REPLAY(TH_BUTTON_SKIP))
            {
                if (!WAS_PRESSED_REPLAY(TH_BUTTON_SHOOT) || this->msg.framesElapsedDuringPause < this->msg.unk_1c)
                {
                    if (this->msg.framesElapsedDuringPause >= this->msg.currentInstr->args.wait.wait)
                    {
                        this->msg.resetDialogueLines = true;
                        this->msg.unk_1c = 30;
                        break;
                    }
                    this->msg.framesElapsedDuringPause++;
                    goto SKIP_TIME_INCREMENT;
                }
                this->msg.resetDialogueLines = true;
                this->msg.unk_1c = 8;
            }
            break;
        case MsgOpcode_AnmInterrupt:
            args = &this->msg.currentInstr->args;
            this->msg.portraits[args->anmInterrupt.portraitIndex].pendingInterrupt = args->anmInterrupt.interrupt;
            break;
        case MsgOpcode_EclResume:
            this->msg.ignoreWaitCounter++;
            break;
        case MsgOpcode_Music:
            if (this->msg.currentInstr->args.music.musicIndex < 0)
            {
                g_Supervisor.StopAudio();
            }
            else
            {
                g_Gui.stageTextAnm->SetAndExecuteScriptIdx(&this->stageTextSprites[3], 3);
                g_Gui.stageTextAnm->SetSprite(&this->stageTextSprites[3],
                                              this->msg.currentInstr->args.music.musicIndex + 3);
                if (g_Supervisor.PlayMusic(
                        this->msg.currentInstr->args.music.musicIndex,
                        g_StageBgmIndices[g_GameManager.currentStage][this->msg.currentInstr->args.music.musicIndex]))
                {
                    g_Supervisor.PlayAudio(
                        g_Background.stdData->songPaths[this->msg.currentInstr->args.music.musicIndex],
                        g_StageBgmIndices[g_GameManager.currentStage][this->msg.currentInstr->args.music.musicIndex]);
                }
            }
            break;
        case MsgOpcode_TextIntro:
            args = &this->msg.currentInstr->args;
            g_Spellcard.enemyFaceAnm->SetAndExecuteScriptIdx(&this->msg.introLines[0], 1);
            this->msg.framesElapsedDuringPause = 0;
            break;
        case MsgOpcode_StageResults:
            this->clearBonusPower = g_GameManager.GetPower();
            this->clearBonusPointItems = g_GameManager.globals->pointItemsCollectedInStage;
            this->clearBonusTime = g_GameManager.GetTimeOrbs();
            this->clearBonusGraze = g_GameManager.globals->grazeInStage;
            this->clearScreenClockTimeOld = g_GameManager.GetClockTime() * 30 + 660;
            this->clockTimeIncrement = g_GameManager.GetClockTimeIncrement();
            g_GameManager.AddToClockTime(this->clockTimeIncrement);
            this->clearBonusStage = g_ClearStageBonuses[g_GameManager.currentStage];
            this->clearScreenClockTime = g_GameManager.GetClockTime() * 30 + 660;
            this->clearScreenDisplayedClockTime = this->clearScreenClockTimeOld;
            this->clearScreenClockTimeUpdateDelayCounter = 0;
            this->stageClearScreenCounter = 1;
            g_GameManager.flags.unk9 = 1;
            if (g_GameManager.currentStage != STAGE6A && g_GameManager.currentStage != STAGE6B &&
                g_GameManager.currentStage != EXTRASTAGE)
            {
                g_AsciiManager.asciiAnm->SetAndExecuteScriptIdx(&this->stageRankSprite, 3);
                g_AsciiManager.asciiAnm->SetSprite(&this->stageRankSprite, this->clockTimeIncrement + 128);
            }
            else
            {
                this->stageRankSprite.currentInstruction = NULL;
            }
            this->stageRankSprite.SetInterrupt(1);
            if (g_GameManager.currentStage != STAGE6A && g_GameManager.currentStage != STAGE6B &&
                g_GameManager.currentStage != EXTRASTAGE)
            {
                g_Gui.loadingPortraitAnm->SetAndExecuteScriptIdx(&this->loadingPortraitSprite, 0);
                g_AsciiManager.captureAnm->SetAndExecuteScriptIdx(&this->arcadeZoneSprite, 1);
                g_AnmManager->SetTextureCaptureParams(
                    3, 32, 16, 384, 448, this->arcadeZoneSprite.loadedSprite->startPixelInclusive.x,
                    this->arcadeZoneSprite.loadedSprite->startPixelInclusive.y,
                    this->arcadeZoneSprite.loadedSprite->widthPx, this->arcadeZoneSprite.loadedSprite->heightPx);
                for (u32 i = 0; i < ARRAY_SIZE(this->arcadeZoneMotionBlurSprites); i++)
                {
                    g_AsciiManager.captureAnm->SetAndExecuteScriptIdx(&this->arcadeZoneMotionBlurSprites[i], 2);
                    this->arcadeZoneMotionBlurSprites[i].counterVar0 = i * 4 + 3;
                    this->arcadeZoneMotionBlurSprites[i].color1.a = 64 - i * 2;
                }
            }
            else
            {
                g_GameManager.globals->pointItemExtendsSoFar = -1;
            }
            if (g_GameManager.currentStage != STAGE6B && g_GameManager.currentStage != STAGE6A &&
                g_GameManager.currentStage != EXTRASTAGE && g_GameManager.GetBombsRemaining() < 3 &&
                (g_GameManager.shotType == SHOT_YOUMU_YUYUKO || g_GameManager.shotType == SHOT_YOUMU ||
                 g_GameManager.shotType == SHOT_YUYUKO))
            {
                g_GameManager.AddToBombCount(1);
                g_SoundPlayer.PlaySoundByIdx(SOUND_SPELL_CAPTURE, 0);
                g_Gui.flags.bombDisplayUpdateFrames = 2;
            }
            break;
        case MsgOpcode_MsgHalt:
            goto SKIP_TIME_INCREMENT;
        case MsgOpcode_MusicFadeOut:
            g_Supervisor.FadeOutMusic(4.0f);
            break;
        case MsgOpcode_ScreenFade:
            ScreenEffect::RegisterChain(SCREEN_EFFECT_FULL_FADE_OUT, 442, 0xffffff, 0, 0, 21);
            g_Supervisor.unk174 = 442;
            break;
        case MsgOpcode_StageEnd:
            if (g_GameManager.currentStage == STAGE6A || g_GameManager.currentStage == STAGE6B ||
                g_GameManager.currentStage == EXTRASTAGE)
            {
                g_GameManager.flags.unk5 = 2;
            }
            goto SKIP_TIME_INCREMENT;
        case MsgOpcode_WaitSkippable:
            this->msg.dialogueSkippable = this->msg.currentInstr->args.waitSkippable.skippable;
            break;
        case MsgOpcode_TextboxVisible:
            this->msg.isTextBoxVisible = this->msg.currentInstr->args.textboxVisible.visible;
            break;
        }
        this->msg.currentInstr =
            (MsgRawInstr *)((u8 *)&this->msg.currentInstr->args + this->msg.currentInstr->instructionSize);
    }
    this->msg.timer++;
SKIP_TIME_INCREMENT:
    g_AnmManager->ExecuteScript(&this->msg.portraits[0]);
    g_AnmManager->ExecuteScript(&this->msg.portraits[1]);
    g_AnmManager->ExecuteScript(&this->msg.portraits[2]);
    g_AnmManager->ExecuteScript(&this->msg.portraits[3]);
    g_AnmManager->ExecuteScript(&this->msg.dialogueLines[0]);
    g_AnmManager->ExecuteScript(&this->msg.dialogueLines[1]);
    g_AnmManager->ExecuteScript(&this->msg.introLines[0]);
    g_AnmManager->ExecuteScript(&this->msg.introLines[1]);
    if (this->msg.timer < 60 && this->msg.dialogueSkippable && IS_PRESSED_REPLAY(TH_BUTTON_SKIP))
    {
        this->msg.timer = 60;
    }
    return ZUN_SUCCESS;
}

#pragma var_order(height, dialogueBg)
ZunResult GuiImpl::DrawDialogue()
{
    f32 height;

    if (this->msg.currentMsgIdx < 0)
    {
        return ZUN_ERROR;
    }

    if (this->msg.timer < 60)
    {
        height = ((f32)this->msg.timer * 48.0f) / 60.0f;
    }
    else
    {
        height = 48.0f;
    }

    VertexDiffuseXyzrhw dialogueBg[4];
    dialogueBg[0].pos = Float3(g_GameManager.arcadeRegionTopLeftPos.x + 16.0f, 384.0f, 0.0f);
    dialogueBg[1].pos = Float3(g_GameManager.arcadeRegionTopLeftPos.x + 384.0f - 16.0f, 384.0f, 0.0f);
    dialogueBg[2].pos = Float3(g_GameManager.arcadeRegionTopLeftPos.x + 16.0f, 384.0f + height, 0.0f);
    dialogueBg[3].pos = Float3(g_GameManager.arcadeRegionTopLeftPos.x + 384.0f - 16.0f, 384.0f + height, 0.0f);
    dialogueBg[0].diffuse = dialogueBg[1].diffuse = 0xd0000000;
    dialogueBg[2].diffuse = dialogueBg[3].diffuse = 0x90000000;
    dialogueBg[0].w = dialogueBg[1].w = dialogueBg[2].w = dialogueBg[3].w = 1.0f;
    if (this->msg.portraits[0].pos.z >= this->msg.portraits[1].pos.z)
    {
        g_AnmManager->DrawNoRotation(&this->msg.portraits[0]);
        g_AnmManager->DrawNoRotation(&this->msg.portraits[1]);
    }
    else
    {
        g_AnmManager->DrawNoRotation(&this->msg.portraits[1]);
        g_AnmManager->DrawNoRotation(&this->msg.portraits[0]);
    }
    if (this->msg.portraits[2].pos.z >= this->msg.portraits[3].pos.z)
    {
        g_AnmManager->DrawNoRotation(&this->msg.portraits[2]);
        g_AnmManager->DrawNoRotation(&this->msg.portraits[3]);
    }
    else
    {
        g_AnmManager->DrawNoRotation(&this->msg.portraits[3]);
        g_AnmManager->DrawNoRotation(&this->msg.portraits[2]);
    }
    g_AnmManager->FlushVertexBuffer();
    if (this->msg.isTextBoxVisible)
    {
        if (!g_Supervisor.IsColorCompositingDisabled())
        {
            g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        }
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        if (!g_Supervisor.IsDepthTestDisabled())
        {
            g_Supervisor.SetRenderState(D3DRS_ZWRITEENABLE, 0);
        }
        g_Supervisor.d3dDevice->SetVertexShader(D3DFVF_DIFFUSE | D3DFVF_XYZRHW);
        g_Supervisor.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, dialogueBg, sizeof(VertexDiffuseXyzrhw));
        g_AnmManager->ClearVertexShader();
        g_AnmManager->ClearColorOp();
        g_AnmManager->ClearBlendMode();
        g_AnmManager->ClearZWrite();
        if (!g_Supervisor.IsColorCompositingDisabled())
        {
            g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        }
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    }
    g_AnmManager->DrawNoRotation(&this->msg.dialogueLines[0]);
    g_AnmManager->DrawNoRotation(&this->msg.dialogueLines[1]);
    g_AnmManager->DrawNoRotation(&this->msg.introLines[0]);
    g_AnmManager->DrawNoRotation(&this->msg.introLines[1]);
    return ZUN_SUCCESS;
}

i32 Gui::MsgWait()
{
    if (!this->impl)
    {
        return 0;
    }

    if (this->impl->msg.ignoreWaitCounter > 0)
    {
        return 0;
    }

    return this->impl->msg.currentMsgIdx >= 0;
}

i32 Gui::IsDialoguePresent()
{
    if (!this->impl)
    {
        return 0;
    }

    return this->impl->msg.currentMsgIdx >= 0 || this->impl->msg.currentMsgIdx == -2;
}

void Gui::UpdateStageElements()
{
    if (this->impl->msg.currentMsgIdx < 0)
    {
        if (this->bossPresent)
        {
            if (this->impl->bossLifeBarState == 0)
            {
                this->impl->frontSprites[12].SetInterrupt(1);
                this->impl->bossLifeBarState = 1;
                this->bossUIOpacity = 0;
            }
            else
            {
                if (this->impl->frontSprites[12].IsStopped())
                {
                    this->impl->bossLifeBarState = 2;
                }
                if (this->bossUIOpacity < 252)
                {
                    this->bossUIOpacity = this->bossUIOpacity + 4;
                }
                else
                {
                    this->bossUIOpacity = 255;
                }
            }
        }
        else if (this->impl->bossLifeBarState != 0)
        {
            if (this->impl->bossLifeBarState <= 2)
            {
                this->impl->frontSprites[12].SetInterrupt(2);
                this->impl->bossLifeBarState = 3;
            }
            if (this->bossUIOpacity > 0)
            {
                this->bossUIOpacity -= 4;
            }
            else
            {
                this->bossUIOpacity = 0;
            }
            if (this->impl->frontSprites[12].IsStopped())
            {
                this->impl->bossLifeBarState = 0;
                this->bossLifeBarSize = 0;
                this->bossUIOpacity = 0;
            }
        }
        if (this->impl->bossLifeBarState >= 2)
        {
            if (this->bossLifeBarMaxSize > this->bossLifeBarSize)
            {
                this->bossLifeBarSize += 0.01f;
                if (this->bossLifeBarMaxSize < this->bossLifeBarSize)
                {
                    this->bossLifeBarSize = this->bossLifeBarMaxSize;
                }
            }
            else if (this->bossLifeBarMaxSize < this->bossLifeBarSize)
            {
                this->bossLifeBarSize -= 0.02f;
                if (this->bossLifeBarMaxSize > this->bossLifeBarSize)
                {
                    this->bossLifeBarSize = this->bossLifeBarMaxSize;
                }
            }
        }
    }
    g_AnmManager->ExecuteScriptArray(this->impl->frontSprites, ARRAY_SIZE(this->impl->frontSprites));
    g_AnmManager->ExecuteScriptArray(this->impl->stageTextSprites, ARRAY_SIZE(this->impl->stageTextSprites));
    if (!g_GameManager.flags.isSpellPractice && this->impl->stageTextSprites[0].color1.a != 0)
    {
        g_AnmManager->ExecuteScriptArray(&this->impl->clockTimeIntroSprite, 1);
    }
    g_AnmManager->ExecuteScript(&this->impl->stageRankSprite);
    g_AnmManager->ExecuteScript(&this->impl->clockTimeSprite);
    if (this->impl->clockTimeSprite.color1.a != 0)
    {
        if (g_Player.position.x >= 64.0f && g_Player.position.y < 128.0f)
        {
            if (this->impl->clockTimeSprite.color1.a > 64)
            {
                this->impl->clockTimeSprite.color1.a -= 4;
            }
        }
        else if (this->impl->clockTimeSprite.color1.a < 255)
        {
            if (this->impl->clockTimeSprite.color1.a <= 251)
            {
                this->impl->clockTimeSprite.color1.a += 4;
            }
            else
            {
                this->impl->clockTimeSprite.color1.a = 255;
            }
        }
    }
    g_AnmManager->ExecuteScript(&this->impl->nullifySprite);
    g_AnmManager->ExecuteScript(&this->impl->difficultySprite);
    if (this->impl->loadingPortraitSprite.activeSpriteIndex >= 0)
    {
        if (g_AnmManager->ExecuteScript(&this->impl->loadingPortraitSprite))
        {
            this->impl->loadingPortraitSprite.activeSpriteIndex = -1;
        }
        if (g_AnmManager->ExecuteScript(&this->impl->arcadeZoneSprite))
        {
            this->impl->arcadeZoneSprite.activeSpriteIndex = -1;
        }
        for (u32 i = 0; i < 8; i++)
        {
            g_AnmManager->ExecuteScript(&this->impl->arcadeZoneMotionBlurSprites[i]);
        }
    }
    if (this->impl->stageTransitionActiveScriptCount != 0)
    {
        i32 activeCount = ARRAY_SIZE_SIGNED(this->impl->stageTransitionSprites);
        for (i32 i = 0; i < ARRAY_SIZE_SIGNED(this->impl->stageTransitionSprites); i++)
        {
            if (g_AnmManager->ExecuteScript(&this->impl->stageTransitionSprites[i]))
            {
                activeCount--;
            }
        }
        this->impl->stageTransitionActiveScriptCount = activeCount;
    }
    if (this->impl->bonusScore.displayArg != GUI_DISPLAY_HIDDEN)
    {
        if (this->impl->bonusScore.timer < 30)
        {
            this->impl->bonusScore.pos.x = ((f32)this->impl->bonusScore.timer * -312.0f) / 30.0f + 416.0f;
        }
        else
        {
            this->impl->bonusScore.pos.x = 104.0f;
        }
        if (this->impl->bonusScore.timer >= 250)
        {
            this->impl->bonusScore.displayArg = GUI_DISPLAY_HIDDEN;
        }
        this->impl->bonusScore.timer++;
    }
    if (this->impl->popupText.displayArg != GUI_DISPLAY_HIDDEN)
    {
        if (this->impl->popupText.timer < 30)
        {
            this->impl->popupText.pos.x = ((f32)this->impl->popupText.timer * -312.0f) / 30.0f + 416.0f;
        }
        else
        {
            this->impl->popupText.pos.x = 104.0f;
        }
        if (this->impl->popupText.timer >= 180)
        {
            this->impl->popupText.displayArg = GUI_DISPLAY_HIDDEN;
        }
        this->impl->popupText.timer++;
    }
    if (this->impl->spellcardBonus.displayArg != GUI_DISPLAY_HIDDEN)
    {
        if (this->impl->spellcardBonus.timer >= 280)
        {
            this->impl->spellcardBonus.displayArg = GUI_DISPLAY_HIDDEN;
        }
        this->impl->spellcardBonus.timer++;
    }
    if (this->impl->stageClearScreenCounter == 1)
    {
        i32 scoreBonus = 0;
        scoreBonus += this->impl->clearBonusStage;
        scoreBonus += this->impl->clearBonusGraze * 50;
        scoreBonus += this->impl->clearBonusPointItems * 5000;
        scoreBonus += this->impl->clearBonusTime * 100;
        if (g_GameManager.currentStage >= 6 && !g_GameManager.IsPracticeMode())
        {
            scoreBonus += g_GameManager.GetLives() * 2500000;
            scoreBonus += g_GameManager.GetBombsRemaining() * 500000;
        }
        if (g_GameManager.currentStage == 7)
        {
            scoreBonus += (12 - g_GameManager.GetClockTime()) * 2000000;
        }
        switch (g_GameManager.difficulty)
        {
        case EASY:
            scoreBonus /= 2;
            break;
        case HARD:
            scoreBonus = (scoreBonus * 12) / 10;
            break;
        case LUNATIC:
            scoreBonus = (scoreBonus * 15) / 10;
            break;
        case EXTRA:
            scoreBonus <<= 1;
            break;
        }
        switch (g_GameManager.cfg->lifeCount)
        {
        case 3:
            scoreBonus = (scoreBonus * 5) / 10;
            break;
        case 4:
            scoreBonus = (scoreBonus * 2) / 10;
            break;
        case 5:
            scoreBonus /= 10;
            break;
        case 6:
            scoreBonus /= 20;
            break;
        }
        this->impl->clearBonusTotal = scoreBonus;
        for (i32 i = 0; i < 10; i++)
        {
            g_GameManager.AddScore(scoreBonus);
        }
        this->impl->stageClearScreenCounter++;
    }
    if (g_GameManager.currentStage < 6 && this->impl->clearScreenDisplayedClockTime != 0 &&
        this->impl->clearScreenDisplayedClockTime >= this->impl->clearScreenClockTime && g_GameManager.flags.unk5 == 0)
    {
        g_GameManager.flags.unk5 = 2;
    }
    if (this->impl->clearScreenDisplayedClockTime != 0 &&
        this->impl->clearScreenDisplayedClockTime != this->impl->clearScreenClockTime)
    {
        if (this->impl->clearScreenClockTimeUpdateDelayCounter >= 60)
        {
            if (this->impl->clearScreenDisplayedClockTime < this->impl->clearScreenClockTime)
            {
                this->impl->clearScreenDisplayedClockTime++;
                if (IS_PRESSED_REPLAY(TH_BUTTON_SHOOT) || IS_PRESSED_REPLAY(TH_BUTTON_SKIP))
                {
                    this->impl->clearScreenDisplayedClockTime += 3;
                }
                if (this->impl->clearScreenDisplayedClockTime > this->impl->clearScreenClockTime)
                {
                    this->impl->clearScreenDisplayedClockTime = this->impl->clearScreenClockTime;
                }
            }
            else
            {
                this->impl->clearScreenClockTimeUpdateDelayCounter++;
            }
        }
        else
        {
            this->impl->clearScreenClockTimeUpdateDelayCounter++;
        }
    }
}

#pragma var_order(y, x, i, vm, textDrawPos)
void Gui::DrawGameScene()
{
    f32 y;
    f32 x;
    i32 i;
    AnmVm *vm;

    g_AnmManager->FlushVertexBuffer();
    g_Supervisor.viewport.X = 0;
    g_Supervisor.viewport.Y = 0;
    g_Supervisor.viewport.Width = 640;
    g_Supervisor.viewport.Height = 480;
    g_Supervisor.d3dDevice->SetViewport(&g_Supervisor.viewport);
    if (!g_Supervisor.IsMinimumGraphicsMode())
    {
        vm = &this->impl->frontSprites[15];
        x = 480.0f;
        vm->pos = Float3(x, 40.0f, 0.49f);
        g_AnmManager->DrawNoRotation(vm);
        vm->pos = Float3(x, 56.0f, 0.49f);
        g_AnmManager->DrawNoRotation(vm);
        if (this->flags.lifeDisplayUpdateFrames != 0)
        {
            vm->pos = Float3(x, 88.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->flags.bombDisplayUpdateFrames != 0)
        {
            vm->pos = Float3(x, 104.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->flags.powerDisplayUpdateFrames != 0)
        {
            vm->pos = Float3(x, 136.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->flags.grazeDisplayUpdateFrames != 0)
        {
            vm->pos = Float3(x, 152.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->flags.pointDisplayUpdateFrames != 0)
        {
            vm->pos = Float3(x, 168.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->flags.timeDisplayUpdateFrames != 0)
        {
            vm->pos = Float3(x, 184.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        vm->pos = Float3(512.0f, 464.0f, 0.48f);
        g_AnmManager->DrawNoRotation(vm);
    }
    vm = &this->impl->frontSprites[13];
    if (g_Supervisor.IsHUDRedrawEnabled() || vm->currentInstruction || g_Supervisor.unk174 != 0)
    {
        for (y = 0.0f; y < 464.0f; y += 32.0f)
        {
            vm->pos = Float3(0.0f, y, 0.49f);
            g_AnmManager->DrawNoRotation(vm);
        }
        for (x = 416.0f; x < 624.0f; x += 32.0f)
        {
            for (y = 16.0f; y < 464.0f; y += 32.0f)
            {
                vm->pos = Float3(x, y, 0.49f);
                g_AnmManager->DrawNoRotation(vm);
            }
        }
        vm = &this->impl->frontSprites[14];
        for (x = 0.0f; x < 624.0f; x += 128.0f)
        {
            vm->pos = Float3(x, 0.0f, 0.49f);
            g_AnmManager->DrawNoRotation(vm);
            vm->pos = Float3(x, 464.0f, 0.49f);
            g_AnmManager->DrawNoRotation(vm);
        }
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[0]);
        g_AnmManager->Draw2D(&this->impl->frontSprites[1]);
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[2]);
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[3]);
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[4]);
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[5]);
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[6]);
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[7]);
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[8]);
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[9]);
        g_AnmManager->DrawNoRotation(&this->impl->difficultySprite);
        this->flags.lifeDisplayUpdateFrames = 2;
        this->flags.bombDisplayUpdateFrames = 2;
        this->flags.grazeDisplayUpdateFrames = 2;
        this->flags.pointDisplayUpdateFrames = 2;
        this->flags.powerDisplayUpdateFrames = 2;
        this->flags.timeDisplayUpdateFrames = 2;
    }
    if (this->flags.lifeDisplayUpdateFrames != 0)
    {
        vm = &this->impl->frontSprites[10];
        for (i = 0, x = 488.0f; i < g_GameManager.GetLives(); i++, x += 16.0f)
        {
            vm->pos = Float3(x, 88.0f, 0.46f);
            g_AnmManager->DrawNoRotation(vm);
        }
    }
    if (this->flags.bombDisplayUpdateFrames != 0)
    {
        vm = &this->impl->frontSprites[11];
        for (i = 0, x = 488.0f; i < g_GameManager.GetBombsRemaining(); i++, x += 16.0f)
        {
            vm->pos = Float3(x, 104.0f, 0.46f);
            g_AnmManager->DrawNoRotation(vm);
        }
    }
    if ((this->flags.bombDisplayUpdateFrames != 0 || this->flags.lifeDisplayUpdateFrames != 0) &&
        g_GameManager.flags.unk7 == 1)
    {
        if (g_Spellcard.IsActive())
        {
            g_AnmManager->DrawNoRotation(&this->impl->nullifySprite);
        }
    }
    vm = &this->impl->frontSprites[14];
    for (x = 32.0f; x < 368.0f; x += 128.0f)
    {
        vm->pos = Float3(x, 464.0f, 0.49f);
        g_AnmManager->DrawNoRotation(vm);
    }

    Float3 textDrawPos(488.0f, 56.0f, 0.0f);
    g_AsciiManager.AddFormatText(&textDrawPos, "%.9d", g_GameManager.globals->displayScore);
    textDrawPos.x += 117.0f;
    g_AsciiManager.AddFormatText(&textDrawPos, "%1d",
                                 g_GameManager.globals->numRetries > 9 ? 9 : g_GameManager.globals->numRetries);
    g_AsciiManager.SetScale(1.0f, 1.0f);
    textDrawPos = Float3(488.0f, 40.0f, 0.0f);
    g_AsciiManager.AddFormatText(&textDrawPos, "%.9d", g_GameManager.globals->displayedHighScore);
    textDrawPos.x += 117.0f;
    g_AsciiManager.AddFormatText(
        &textDrawPos, "%1d",
        g_GameManager.globals->continuesUsedInHighScore > 9 ? 9 : g_GameManager.globals->continuesUsedInHighScore);
    g_AsciiManager.SetScale(1.0f, 1.0f);
    if (this->flags.grazeDisplayUpdateFrames != 0 || g_Supervisor.IsMinimumGraphicsMode())
    {
        textDrawPos = Float3(488.0f, 152.0f, 0.0f);
        g_AsciiManager.AddFormatText(&textDrawPos, "%d", g_GameManager.globals->graze);
    }
    if (this->flags.pointDisplayUpdateFrames != 0 || g_Supervisor.IsMinimumGraphicsMode())
    {
        textDrawPos = Float3(488.0f, 168.0f, 0.0f);
        textDrawPos.x +=
            g_AsciiManager.AddFormatText2(&textDrawPos, "%d", g_GameManager.globals->pointItemsCollected) * 13;
        g_AsciiManager.SetScale(0.5f, 1.0f);
        g_AsciiManager.AddFormatText(&textDrawPos, "/");
        g_AsciiManager.SetScale(1.0f, 1.0f);
        textDrawPos.x += 6.0f;
        g_AsciiManager.AddFormatText(&textDrawPos, "%d", g_GameManager.globals->nextPointItemExtendThreshold);
    }
    if (this->flags.timeDisplayUpdateFrames != 0 || g_Supervisor.IsMinimumGraphicsMode())
    {
        if (g_GameManager.GetTimeOrbs() >= g_GameManager.GetLastSpellTimeOrbThreshold())
        {
            g_AsciiManager.SetColor(0xfffff0c0);
        }
        textDrawPos = Float3(488.0f, 184.0f, 0.0f);
        textDrawPos.x += g_AsciiManager.AddFormatText2(&textDrawPos, "%d", g_GameManager.GetTimeOrbs()) * 13;
        g_AsciiManager.SetScale(0.5f, 1.0f);
        g_AsciiManager.AddFormatText(&textDrawPos, "/");
        g_AsciiManager.SetScale(1.0f, 1.0f);
        textDrawPos.x += 6.0f;
        g_AsciiManager.AddFormatText(&textDrawPos, "%d", g_GameManager.GetLastSpellTimeOrbThreshold());
        g_AsciiManager.SetColor(0xffffffff);
    }
    g_AnmManager->FlushVertexBuffer();
    if (this->flags.powerDisplayUpdateFrames != 0 || g_Supervisor.IsMinimumGraphicsMode())
    {
        VertexDiffuseXyzrhw powerBarVerts[4];
        if (g_GameManager.GetPower() > 0)
        {
            powerBarVerts[0].pos = Float3(488.0f, 136.0f, 0.1f);
            powerBarVerts[1].pos = Float3((f32)(g_GameManager.GetPower() + 488) + 0.0f, 136.0f, 0.1f);
            powerBarVerts[2].pos = Float3(488.0f, 152.0f, 0.1f);
            powerBarVerts[3].pos = Float3((f32)(g_GameManager.GetPower() + 488) + 0.0f, 152.0f, 0.1f);
            powerBarVerts[0].diffuse = powerBarVerts[2].diffuse = 0xe0e0e0ff;
            powerBarVerts[1].diffuse = powerBarVerts[3].diffuse = 0x80e0e0ff;
            powerBarVerts[0].w = powerBarVerts[1].w = powerBarVerts[2].w = powerBarVerts[3].w = 1.0f;
            if (!g_Supervisor.IsColorCompositingDisabled())
            {
                g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
            }
            g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
            g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
            if (!g_Supervisor.IsDepthTestDisabled())
            {
                g_Supervisor.SetRenderState(D3DRS_ZWRITEENABLE, 0);
            }
            g_Supervisor.d3dDevice->SetVertexShader(D3DFVF_DIFFUSE | D3DFVF_XYZRHW);
            g_Supervisor.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &powerBarVerts,
                                                    sizeof(VertexDiffuseXyzrhw));
            g_AnmManager->ClearVertexShader();
            g_AnmManager->ClearColorOp();
            g_AnmManager->ClearBlendMode();
            g_AnmManager->ClearZWrite();
            if (!g_Supervisor.IsColorCompositingDisabled())
            {
                g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            }
            g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        }
        if (g_GameManager.GetPower() < 128)
        {
            g_AsciiManager.AddFormatText(&Float3(488.0f, 136.0f, 0.0f), "%d", g_GameManager.GetPower());
        }
        else
        {
            g_AsciiManager.AddFormatText(&Float3(488.0f, 136.0f, 0.0f), "MAX");
        }
    }
    if (this->flags.lifeDisplayUpdateFrames != 0)
    {
        this->flags.lifeDisplayUpdateFrames--;
    }
    if (this->flags.powerDisplayUpdateFrames != 0)
    {
        this->flags.powerDisplayUpdateFrames--;
    }
    if (this->flags.bombDisplayUpdateFrames != 0)
    {
        this->flags.bombDisplayUpdateFrames--;
    }
    if (this->flags.grazeDisplayUpdateFrames != 0)
    {
        this->flags.grazeDisplayUpdateFrames--;
    }
    if (this->flags.pointDisplayUpdateFrames != 0)
    {
        this->flags.pointDisplayUpdateFrames--;
    }
    if (this->flags.timeDisplayUpdateFrames != 0)
    {
        this->flags.timeDisplayUpdateFrames--;
    }
}

#pragma var_order(i, color2, color1, healthBarRect, tmp, j, segmentEndHealth, timeColor, markerGap)
void Gui::DrawStageElements()
{
    u32 i;
    D3DCOLOR color2;
    D3DCOLOR color1;
    ZunRect healthBarRect;
    i32 tmp;
    i32 j;
    f32 segmentEndHealth;
    D3DCOLOR timeColor;
    i32 markerGap;

    for (i = 0; (i32)i < 4; i++)
    {
        g_AnmManager->Draw2D(&this->impl->stageTextSprites[i]);
    }
    g_AnmManager->Draw2D(&this->impl->clockTimeIntroSprite);
    g_AnmManager->Draw2D(&this->impl->clockTimeSprite);
    if (this->impl->loadingPortraitSprite.activeSpriteIndex >= 0)
    {
        g_AnmManager->DrawNoRotation(&this->impl->loadingPortraitSprite);
        g_AnmManager->DrawWorld(&this->impl->arcadeZoneSprite);
        for (i = 0; i < 8; i++)
        {
            g_AnmManager->DrawWorld(&this->impl->arcadeZoneMotionBlurSprites[i]);
        }
        if (this->impl->unk_3a1c.activeSpriteIndex >= 0)
        {
            this->impl->unk_3a1c.pos = Float3(304.0f, 448.0f, 0.0f);
            g_AnmManager->DrawNoRotation(&this->impl->unk_3a1c);
        }
    }
    if (this->impl->stageTransitionActiveScriptCount != 0)
    {
        for (i = 0; (i32)i < ARRAY_SIZE_SIGNED(this->impl->stageTransitionSprites); i++)
        {
            g_AnmManager->DrawWorld(&this->impl->stageTransitionSprites[i]);
            g_AnmManager->ClearSprite();
        }
    }
    if (this->impl->msg.currentMsgIdx < 0 && this->bossPresent + this->impl->bossLifeBarState > 0)
    {
        healthBarRect.left = 64.0f;
        healthBarRect.top = 19.0f;
        healthBarRect.right = this->bossLifeBarSize * 320.0f + 64.0f;
        healthBarRect.bottom = 23.0f;
        color1 = this->bossUIOpacity << 24 | 0xffffff;
        color2 = this->bossUIOpacity << 24 | 0x202060;

        Float3 pos(48.0f, 16.0f, 0.0f);
        ScreenEffect::DrawSquareShaded(&healthBarRect, color1, color1, color2, color2);
        for (j = 0; j < MAX_BOSS_LIFEBAR_SEGMENTS; j++)
        {
            if (this->bossLifeBarSegmentStop[j] == 0.0f)
            {
                continue;
            }
            if (this->bossLifeBarSegmentStart[j] >= this->bossLifeBarSize)
            {
                continue;
            }
            segmentEndHealth = this->bossLifeBarSegmentStop[j];
            if (this->bossLifeBarSize < segmentEndHealth)
            {
                segmentEndHealth = this->bossLifeBarSize;
            }
            healthBarRect.left = this->bossLifeBarSegmentStart[j] * 320.0f + 64.0f;
            healthBarRect.top = 19.0f;
            healthBarRect.right = segmentEndHealth * 320.0f + 64.0f;
            healthBarRect.bottom = 23.0f;
            color1 = this->bossUIOpacity << 24 | this->bossLifeBarSegmentColor[j] & 0xffffff;
            color2 = this->bossUIOpacity << 24 | this->bossLifeBarSegmentColor[j] >> 2 & 0x3f3f3f;
            ScreenEffect::DrawSquareShaded(&healthBarRect, color1, color1, color2, color2);
        }
        g_AnmManager->DrawNoRotation(&this->impl->frontSprites[12]);
        healthBarRect.left = 33.0f;
        healthBarRect.top = 19.0f;
        healthBarRect.right = healthBarRect.left + 3.0f;
        healthBarRect.bottom = healthBarRect.top + 4.0f;
        tmp = this->eclSetLives;
        markerGap = (this->eclSetLives <= 5) + 1;
        for (j = 0; j < tmp; j++)
        {
            healthBarRect.left = ((f32)j * 26.0f) / (f32)tmp + 35.0f;
            healthBarRect.right = (((f32)(j + 1) * 26.0f) / (f32)tmp + 35.0f) - (f32)markerGap;
            color1 = this->bossUIOpacity << 24 | 0xffffff - (j * 255) / 9;
            color2 = this->bossUIOpacity << 24 | 0x202020;
            ScreenEffect::DrawSquareShaded(&healthBarRect, color1, color1, color2, color2);
        }
        pos = Float3(384.0f, 16.0f, 0.0f);
        if (this->spellcardSecondsRemaining >= 20)
        {
            timeColor = g_SpellcardTimeColors[0];
        }
        else if (this->spellcardSecondsRemaining >= 10)
        {
            timeColor = g_SpellcardTimeColors[1];
        }
        else if (this->spellcardSecondsRemaining >= 5)
        {
            timeColor = g_SpellcardTimeColors[2];
        }
        else
        {
            timeColor = g_SpellcardTimeColors[3];
        }
        g_AsciiManager.SetColor(this->bossUIOpacity << 24 | timeColor);
        tmp = this->spellcardSecondsRemaining > 99 ? 99 : this->spellcardSecondsRemaining;
        if (this->previousSpellcardSecondsRemaining != this->spellcardSecondsRemaining)
        {
            if (tmp < 3)
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_TIMEOUT_2, 0);
            }
            else if (tmp < 10)
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_TIMEOUT, 0);
            }
        }
        g_AsciiManager.AddFormatText(&pos, "%.2d", tmp);
        g_AsciiManager.SetColor(0xffffffff);
        this->previousSpellcardSecondsRemaining = this->spellcardSecondsRemaining;
        if (!g_GameManager.showPauseMenu && !g_GameManager.showRetryMenu && !g_GameManager.flags.unk10 &&
            g_EnemyManager.bossIds[0])
        {
            pos = Float3(2.0f, 29.0f, 0.0f);
            g_AsciiManager.SetScale(1.0f, 1.0f);
            g_AsciiManager.CreateFamiliarPopup(&pos, g_EnemyManager.bossIds[0]->GetFamiliarCount(),
                                               g_EnemyManager.bossIds[0]->field98_0x3380, 0xfff0f00f);
        }
    }
    g_AnmManager->DrawNoRotation(&this->impl->stageRankSprite);
}

ZunResult Gui::AddedCallback(Gui *gui)
{
    return gui->ActualAddedCallback();
}

ZunResult Gui::DeletedCallback(Gui *gui)
{
    if (!KeepStageResources())
    {
        g_AnmManager->ReleaseAnm(13);
    }
    gui->FreeMsgFile();
    if (ReleaseResourcesOnRestart())
    {
        g_AnmManager->ReleaseAnm(10);
        g_AnmManager->ReleaseAnm(12);
        g_AnmManager->ReleaseAnm(11);
        g_AnmManager->ReleaseAnm(14);
        ZUN_DELETE(gui->impl);
    }
    return ZUN_SUCCESS;
}

ZunResult Gui::RegisterChain()
{
    Gui *gui = &g_Gui;
    if (IsInitialStageLoad())
    {
        memset(gui, 0, sizeof(Gui));
        gui->impl = ZUN_NEW(GuiImpl, "FRScreenImplInf");
    }
    g_GuiCalcChain.SetCallback((ChainCallback)OnUpdate);
    g_GuiCalcChain.addedCallback = (ChainLifetimeCallback)AddedCallback;
    g_GuiCalcChain.deletedCallback = (ChainLifetimeCallback)DeletedCallback;
    g_GuiCalcChain.arg = gui;
    if (g_Chain.AddToCalcChain(&g_GuiCalcChain, 15) != 0)
    {
        return ZUN_ERROR;
    }

    g_GuiDrawChain.SetCallback((ChainCallback)OnDraw);
    g_GuiDrawChain.arg = gui;
    g_Chain.AddToDrawChain(&g_GuiDrawChain, 17);

    return ZUN_SUCCESS;
}

void Gui::CutChain()
{
    g_Chain.Cut(&g_GuiCalcChain);
    g_Chain.Cut(&g_GuiDrawChain);
}

ZunBool Gui::IsStageFinished()
{
    if (this->impl->loadingPortraitSprite.activeSpriteIndex >= 0 && this->impl->loadingPortraitSprite.IsStopped())
    {
        return TRUE;
    }

    return FALSE;
}

ZunBool Gui::IsDialogueSkippable()
{
    return this->impl->msg.dialogueSkippable;
}

void Gui::ShowBonusScore(i32 score)
{
    this->impl->bonusScore.pos = Float3(416.0f, 48.0f, 0.0f);
    this->impl->bonusScore.displayArg = GUI_DISPLAY_SHOWN;
    this->impl->bonusScore.timer = 0;
    this->impl->bonusScore.fmtArg = score;
    g_Supervisor.unk174 = 2;
}

void Gui::ShowPopupText(i32 fmtArg, i32 popupType)
{
    this->impl->popupText.pos = Float3(416.0f, 168.0f, 0.0f);
    this->impl->popupText.displayArg = popupType;
    this->impl->popupText.timer = 0;
    this->impl->popupText.fmtArg = fmtArg;
    g_Supervisor.unk174 = 2;
}

void Gui::ShowSpellcardBonus(i32 bonus)
{
    this->impl->spellcardBonus.pos = Float3(224.0f, 16.0f, 0.0f);
    this->impl->spellcardBonus.displayArg = GUI_DISPLAY_SHOWN;
    this->impl->spellcardBonus.timer = 0;
    this->impl->spellcardBonus.fmtArg = bonus;
    g_Supervisor.unk174 = 2;
}

// STUB: th08 0x437f5c
void Gui::CopyEnemyNameTexture(i32 param_1)
{
}

// STUB: th08 0x438046
void Gui::FUN_00438046()
{
    // Not actually sure this is Gui related, but it's located nearby other Gui functions
}

void Gui::DrawStageClearScreen()
{
    Float3 pos(120.0f, 96.0f, 0.0f);
    g_AsciiManager.SetColor(0xffffff40);
    if (g_GameManager.currentStage < STAGE6A)
    {
        g_AsciiManager.AddFormatText(&pos, "Stage Clear");
    }
    else
    {
        if (g_GameManager.currentStage >= STAGE6B)
        {
            pos.y -= 16.0f;
        }
        g_AsciiManager.AddFormatText(&pos, "All Clear!");
    }
    pos.y += 32.0f;
    g_AsciiManager.SetColor(0xffffffff);
    g_AsciiManager.AddFormatText(&pos, "Clear = %8d0", this->impl->clearBonusStage);
    pos.y += 16.0f;
    g_AsciiManager.SetColor(0xffe0e0ff);
    g_AsciiManager.AddFormatText(&pos, "Point = %8d0", this->impl->clearBonusPointItems * 5000);
    pos.y += 16.0f;
    g_AsciiManager.SetColor(0xffd0d0ff);
    g_AsciiManager.AddFormatText(&pos, "Graze = %8d0", this->impl->clearBonusGraze * 50);
    pos.y += 16.0f;
    g_AsciiManager.SetColor(0xffd0d0ff);
    g_AsciiManager.AddFormatText(&pos, "Time  = %8d0", this->impl->clearBonusTime * 100);
    pos.y += 16.0f;
    g_AsciiManager.SetColor(0xffd0d0ff);
    pos.y += 16.0f;
    g_AsciiManager.AddFormatText(&pos, "over-80%% = %3d.%.2d%%", g_GameManager.unk3de24 * 100 / g_GameManager.unk3de14,
                                 (g_GameManager.unk3de24 * 10000 / g_GameManager.unk3de14) % 100);
    pos.y += 16.0f;
    g_AsciiManager.AddFormatText(&pos, "over 80%% = %3d.%.2d%%", g_GameManager.unk3de20 * 100 / g_GameManager.unk3de14,
                                 (g_GameManager.unk3de20 * 10000 / g_GameManager.unk3de14) % 100);
    if (g_GameManager.currentStage >= 6 && !g_GameManager.IsPracticeMode() && !g_GameManager.IsReplayPractice())
    {
        pos.y += 16.0f;
        g_AsciiManager.SetColor(0xffffff80);
        g_AsciiManager.AddFormatText(&pos, "Player =%8d0", g_GameManager.GetLives() * 2500000);
        pos.y += 16.0f;
        g_AsciiManager.SetColor(0xffffff80);
        g_AsciiManager.AddFormatText(&pos, "Bomb   = %7d0", g_GameManager.GetBombsRemaining() * 500000);
        if (g_GameManager.currentStage == 7 && !g_GameManager.IsPracticeMode() && !g_GameManager.IsReplayPractice())
        {
            pos.y += 16.0f;
            g_AsciiManager.SetColor(0xffffff80);
            g_AsciiManager.AddFormatText(&pos, "Last Time = %2d:%.2d", (this->impl->clearScreenClockTime / 60) % 12,
                                         this->impl->clearScreenClockTime % 60);
            pos.y += 16.0f;
            g_AsciiManager.AddFormatText(&pos, "Night Bonus");
            pos.y += 16.0f;
            g_AsciiManager.AddFormatText(&pos, "        %8d0", (12 - g_GameManager.GetClockTime()) * 2000000);
        }
    }
    pos.y += 32.0f;
    switch (g_GameManager.difficulty)
    {
    case EASY:
        g_AsciiManager.SetColor(0xffff8080);
        g_AsciiManager.AddFormatText(&pos, "Easy Rank    *0.5");
        break;
    case NORMAL:
        g_AsciiManager.SetColor(0xffff8080);
        g_AsciiManager.AddFormatText(&pos, "Normal Rank  *1.0");
        break;
    case HARD:
        g_AsciiManager.SetColor(0xffff8080);
        g_AsciiManager.AddFormatText(&pos, "Hard Rank    *1.2");
        break;
    case LUNATIC:
        g_AsciiManager.SetColor(0xffff8080);
        g_AsciiManager.AddFormatText(&pos, "Lunatic Rank *1.5");
        break;
    case EXTRA:
        g_AsciiManager.SetColor(0xffff8080);
        g_AsciiManager.AddFormatText(&pos, "Extra Rank   *2.0");
        break;
    case 5:
        // Leftover from PCB.
        g_AsciiManager.SetColor(0xffff8080);
        g_AsciiManager.AddFormatText(&pos, "Phantasm Rank*2.0");
        break;
    }
    if (g_GameManager.difficulty < EXTRA && !g_GameManager.flags.isPracticeMode)
    {
        pos.y += 16.0f;
        switch (g_GameManager.cfg->lifeCount)
        {
        case 3:
            g_AsciiManager.SetColor(0xffff8080);
            g_AsciiManager.AddFormatText(&pos, "Player Penalty*0.5");
            pos.y += 16.0f;
            break;
        case 4:
            g_AsciiManager.SetColor(0xffff8080);
            g_AsciiManager.AddFormatText(&pos, "Player Penalty*0.2");
            pos.y += 16.0f;
            break;
        case 5:
            g_AsciiManager.SetColor(0xffff8080);
            g_AsciiManager.AddFormatText(&pos, "Player Penalty*0.1");
            pos.y += 16.0f;
            break;
        case 6:
            g_AsciiManager.SetColor(0xffff8080);
            g_AsciiManager.AddFormatText(&pos, "Player Penalty*0.05");
            pos.y += 16.0f;
            break;
        }
    }
    pos.y += 16.0f;
    g_AsciiManager.SetColor(0xffffffff);
    g_AsciiManager.AddFormatText(&pos, "Total = %8d0", this->impl->clearBonusTotal);
    g_AsciiManager.SetColor(0xffffffff);
    if (g_GameManager.currentStage <= STAGE5)
    {
        pos.y += 40.0f;
        pos.x = 120.0f;
        g_AsciiManager.SetColor(0xffdfdfdf);
        g_AsciiManager.AddFormatText(&pos, "%s%2d:%.2d", g_AmPmStrings[this->impl->clearScreenClockTimeOld / 60 < 12],
                                     (this->impl->clearScreenClockTimeOld / 60) % 12,
                                     this->impl->clearScreenClockTimeOld % 60);
        pos.x += 99.0f;
        g_AsciiManager.SetColor(0xffafafaf);
        g_AsciiManager.AddFormatText(&pos, ">>");
        pos.x += 34.0f;
        g_AsciiManager.SetColor(0xffff8f8f);
        g_AsciiManager.AddFormatText(
            &pos, "%s%2d:%.2d", g_AmPmStrings[this->impl->clearScreenDisplayedClockTime / 60 < 12],
            (this->impl->clearScreenDisplayedClockTime / 60) % 12, this->impl->clearScreenDisplayedClockTime % 60);
        g_AsciiManager.SetColor(0xffffffff);
    }
}

void Gui::DrawAsciiText()
{
    char fmtArg[32];

    g_AsciiManager.SetIsGuiMode(TRUE);
    if (this->impl->bonusScore.displayArg != GUI_DISPLAY_HIDDEN)
    {
        g_AsciiManager.SetColor(0xffffff80);
        g_AsciiManager.AddFormatText(&this->impl->bonusScore.pos, " BONUS %8d", this->impl->bonusScore.fmtArg);
        g_AsciiManager.SetColor(0xffffffff);
    }
    switch (this->impl->popupText.displayArg)
    {
    case GUI_DISPLAY_FULL_POWER:
        g_AsciiManager.SetColor(0xffc0b0ff);
        g_AsciiManager.AddFormatText(&this->impl->popupText.pos, "Full Power Mode!");
        g_AsciiManager.SetColor(0xffffffff);
        break;
    case GUI_DISPLAY_BORDER:
        // Leftover from PCB.
        g_AsciiManager.SetScale(0.9f, 1.0f);
        g_AsciiManager.SetSpaceWidth(11);
        g_AsciiManager.SetColor(0xffe0b0ff);
        g_AsciiManager.AddFormatText(&this->impl->popupText.pos, "Supernatural Border!!");
        g_AsciiManager.SetColor(0xffffffff);
        g_AsciiManager.SetScale(1.0f, 1.0f);
        g_AsciiManager.SetSpaceWidth(13);
        break;
    case GUI_DISPLAY_CHERRY_MAX:
        g_AsciiManager.SetColor(0xffc0b0ff);
        g_AsciiManager.AddFormatText(&this->impl->popupText.pos, "CherryPoint Max!");
        g_AsciiManager.SetColor(0xffffffff);
        break;
    case GUI_DISPLAY_BORDER_BONUS:
        g_AsciiManager.SetScale(0.9f, 1.0f);
        g_AsciiManager.SetSpaceWidth(11);
        g_AsciiManager.SetColor(0xffe0b0ff);
        g_AsciiManager.AddFormatText(&this->impl->popupText.pos, "Border Bonus %7d", this->impl->popupText.fmtArg);
        g_AsciiManager.SetColor(0xffffffff);
        g_AsciiManager.SetScale(1.0f, 1.0f);
        g_AsciiManager.SetSpaceWidth(13);
        break;
    case GUI_DISPLAY_SPELL_BONUS_FAILED:
        g_AsciiManager.SetScale(0.9f, 1.0f);
        g_AsciiManager.SetSpaceWidth(11);
        g_AsciiManager.SetColor(0xffe0b0ff);
        g_AsciiManager.AddFormatText(&this->impl->popupText.pos, "Spell Bonus Failed");
        g_AsciiManager.SetColor(0xffffffff);
        g_AsciiManager.SetScale(1.0f, 1.0f);
        g_AsciiManager.SetSpaceWidth(13);
        break;
    case GUI_DISPLAY_LAST_SPELL_FAILED:
        g_AsciiManager.SetScale(0.9f, 1.0f);
        g_AsciiManager.SetSpaceWidth(11);
        g_AsciiManager.SetColor(0xffe0b0ff);
        g_AsciiManager.AddFormatText(&this->impl->popupText.pos, "Last Spell Failed");
        g_AsciiManager.SetColor(0xffffffff);
        g_AsciiManager.SetScale(1.0f, 1.0f);
        g_AsciiManager.SetSpaceWidth(13);
    }
    if (this->impl->spellcardBonus.displayArg != GUI_DISPLAY_HIDDEN)
    {
        g_AsciiManager.SetColor(0xffff0000);
        this->impl->spellcardBonus.pos.x = (384.0f - strlen("Spell Card Bonus!") * 14.0f) / 2.0f + 32.0f;
        this->impl->spellcardBonus.pos.y = 80.0f;
        g_AsciiManager.AddFormatText(&this->impl->spellcardBonus.pos, "Spell Card Bonus!");
        this->impl->spellcardBonus.pos.y += 16.0f;
        sprintf(fmtArg, "+%d", this->impl->spellcardBonus.fmtArg);
        this->impl->spellcardBonus.pos.x = (384.0f - strlen(fmtArg) * 28.0f) / 2.0f + 32.0f;
        g_AsciiManager.SetScale(2.0f, 2.0f);
        g_AsciiManager.SetColor(0xffff8080);
        g_AsciiManager.AddString(&this->impl->spellcardBonus.pos, fmtArg);
        g_AsciiManager.SetScale(1.0f, 1.0f);
        g_AsciiManager.SetColor(0xffffffff);
    }
    g_AsciiManager.SetIsGuiMode(FALSE);
}

void Gui::CaptureArcade()
{
    g_AsciiManager.captureAnm->SetAndExecuteScriptIdx(&this->impl->arcadeZoneSprite, 1);
    g_AnmManager->SetTextureCaptureParams(
        3, 32, 16, 384, 448, this->impl->arcadeZoneSprite.loadedSprite->startPixelInclusive.x,
        this->impl->arcadeZoneSprite.loadedSprite->startPixelInclusive.y,
        this->impl->arcadeZoneSprite.loadedSprite->widthPx, this->impl->arcadeZoneSprite.loadedSprite->heightPx);
}

i32 Gui::ShowClockTime()
{
    this->timesAnm->ExecuteAnmIdx(&this->impl->clockTimeSprite, 2);
    this->timesAnm->SetSprite(&this->impl->clockTimeSprite, g_GameManager.GetClockTime());
    return 0;
}

i32 Gui::FlashClockTimeSlow()
{
    this->timesAnm->SetSprite(&this->impl->clockTimeSprite, g_GameManager.GetClockTime());
    this->impl->clockTimeSprite.SetInterrupt(1);
    return 0;
}

i32 Gui::FlashClockTimeFast()
{
    this->timesAnm->SetSprite(&this->impl->clockTimeSprite, g_GameManager.GetClockTime());
    this->impl->clockTimeSprite.SetInterrupt(2);
    return 0;
}

i32 Gui::HideClockTime()
{
    this->impl->clockTimeSprite.color1.a = 0;
    return 0;
}

ZunResult Gui::ActualAddedCallback()
{
    i32 i;
    i32 j;

    if (IsInitialStageLoad())
    {
        memset(this->impl, 0, sizeof(GuiImpl));
        this->frontAnm = g_AnmManager->PreloadAnm(10, "front.anm");
        if (!this->frontAnm)
        {
            return ZUN_ERROR;
        }
        InitStageClearScreen();
        this->timesAnm = g_AnmManager->PreloadAnm(14, "times.anm");
        if (!this->timesAnm)
        {
            return ZUN_ERROR;
        }
        this->loadingPortraitAnm = g_AnmManager->PreloadAnm(12, g_LoadingAnms[g_GameManager.shotType]);
        if (!this->loadingPortraitAnm)
        {
            return ZUN_ERROR;
        }
        g_AsciiManager.asciiAnm->SetAndExecuteScriptIdx(&this->impl->nullifySprite, 26);
        g_AsciiManager.asciiAnm->SetAndExecuteScriptIdx(&this->impl->difficultySprite, 25);
        if (g_GameManager.IsSpellPractice() && g_GameManager.currentSpellCardNumber >= 205)
        {
            g_AsciiManager.asciiAnm->SetSprite(&this->impl->difficultySprite, 288);
        }
        else
        {
            g_AsciiManager.asciiAnm->SetSprite(&this->impl->difficultySprite, g_GameManager.difficulty + 283);
        }
    }
    else
    {
        InitStageClearScreen();
        g_AsciiManager.captureAnm->SetAndExecuteScriptIdx(&this->impl->arcadeZoneSprite, 1);
        this->impl->arcadeZoneSprite.pendingInterrupt = 1;
        for (i = 0; i < TRANSITION_SPRITE_ROWS; i++)
        {
            for (j = 0; j < TRANSITION_SPRITE_COLS; j++)
            {
                g_AsciiManager.captureAnm->SetAndExecuteScriptIdx(
                    &this->impl->stageTransitionSprites[i * TRANSITION_SPRITE_COLS + j], (i + j & 1U) + 3);
                this->impl->stageTransitionSprites[i * TRANSITION_SPRITE_COLS + j].counterVar0 = i + j * 2;
                this->impl->stageTransitionSprites[i * TRANSITION_SPRITE_COLS + j].pos.x =
                    ((f32)j * 32.0f - 0.5f) + 16.0f;
                this->impl->stageTransitionSprites[i * TRANSITION_SPRITE_COLS + j].pos.y =
                    ((f32)i * 32.0f - 0.5f) + 16.0f;
                this->impl->stageTransitionSprites[i * TRANSITION_SPRITE_COLS + j].pos.z = 0.0f;
                this->impl->stageTransitionSprites[i * TRANSITION_SPRITE_COLS + j].uvScrollPos.x =
                    (f32)j * 32.0f / 512.0f;
                this->impl->stageTransitionSprites[i * TRANSITION_SPRITE_COLS + j].uvScrollPos.y =
                    (f32)i * 32.0f / 512.0f;
            }
        }
        this->impl->stageTransitionActiveScriptCount = TRANSITION_SPRITE_ROWS * TRANSITION_SPRITE_COLS;
    }
    g_Gui.HideClockTime();
    this->timesAnm->ExecuteAnmIdx(&this->impl->clockTimeIntroSprite, 0);
    this->timesAnm->SetSprite(&this->impl->clockTimeIntroSprite, g_GameManager.GetClockTime());
    if (!g_GameManager.IsSpellPractice() &&
        LoadMsg(g_MsgFiles[g_GameManager.currentStage][g_GameManager.shotType]) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }
    if (!KeepStageResources())
    {
        if (!g_GameManager.flags.isSpellPractice || g_GameManager.currentSpellCardNumber < 205)
        {
            this->stageTextAnm = g_AnmManager->PreloadAnm(13, g_StageTextAnms[g_GameManager.currentStage]);
            if (!this->stageTextAnm)
            {
                return ZUN_ERROR;
            }
        }
        else
        {
            this->stageTextAnm = g_AnmManager->PreloadAnm(13, g_StageTextAnms[8]);
            if (!this->stageTextAnm)
            {
                return ZUN_ERROR;
            }
        }
    }
    if (IsInitialStageLoad())
    {
        for (i32 i = 0; i < ARRAY_SIZE(this->impl->frontSprites); i++)
        {
            this->frontAnm->SetAndExecuteScriptIdx(this->impl->frontSprites + i, i);
        }
    }
    this->frameCounter = 0;
    this->bossPresent = false;
    this->impl->bossLifeBarState = 0;
    this->bossLifeBarMaxSize = 0.0f;
    this->bossLifeBarSize = 0.0f;
    if (!g_GameManager.flags.isSpellPractice)
    {
        this->stageTextAnm->ExecuteAnmIdxArray(this->impl->stageTextSprites, 0,
                                               ARRAY_SIZE(this->impl->stageTextSprites));
    }
    else
    {
        if (!KeepStageResources() ||
            GameManager::ShouldPauseMusicInSpellPractice(g_GameManager.currentSpellCardNumber) != 0)
        {
            this->stageTextAnm->ExecuteAnmIdxArray(this->impl->stageTextSprites, 3, 1);
            this->stageTextAnm->SetSprite(this->impl->stageTextSprites,
                                          GameManager::GetSongNameSpriteIdx(g_GameManager.currentSpellCardNumber) + 3);
        }
    }
    this->impl->msg.currentMsgIdx = -1;
    this->impl->stageClearScreenCounter = 0;
    this->impl->bonusScore.displayArg = GUI_DISPLAY_HIDDEN;
    this->impl->popupText.displayArg = GUI_DISPLAY_HIDDEN;
    this->impl->spellcardBonus.displayArg = GUI_DISPLAY_HIDDEN;
    this->flags.lifeDisplayUpdateFrames = 2;
    this->flags.bombDisplayUpdateFrames = 2;
    this->flags.grazeDisplayUpdateFrames = 2;
    this->flags.pointDisplayUpdateFrames = 2;
    this->flags.powerDisplayUpdateFrames = 2;
    this->flags.timeDisplayUpdateFrames = 2;
    g_AsciiManager.asciiAnm->SetAndExecuteScriptIdx(&this->impl->stageRankSprite, 3);
    g_Supervisor.unk174 = 16;
    this->impl->clearScreenDisplayedClockTime = 0;
    return ZUN_SUCCESS;
}

void Gui::InitStageClearScreen()
{
    this->impl->loadingPortraitSprite.activeSpriteIndex = -1;
    this->impl->unk_3a1c.activeSpriteIndex = -1;
    this->impl->arcadeZoneSprite.activeSpriteIndex = -1;
    this->impl->stageTransitionActiveScriptCount = 0;
}

ZunResult Gui::LoadMsg(const char *path)
{
    FreeMsgFile();
    this->impl->msg.msgFile = (MsgRawHeader *)FileSystem::OpenFile(path, NULL, 0);
    if (!this->impl->msg.msgFile)
    {
        g_GameErrorContext.Log(TH_ERR_GUI_FAILED_TO_LOAD_MSG, path);
        return ZUN_ERROR;
    }

    this->impl->msg.currentMsgIdx = -1;
    this->impl->msg.currentInstr = NULL;
    for (i32 i = 0; i < this->impl->msg.msgFile->numInstrs; i++)
    {
        (&this->impl->msg.msgFile->instrs)[i] =
            (MsgRawInstr *)((i32)(&this->impl->msg.msgFile->instrs)[i] + (i32)this->impl->msg.msgFile);
    }
    return ZUN_SUCCESS;
}

void Gui::FreeMsgFile(void)
{
    if (this->impl->msg.msgFile)
    {
        ZUN_FREE(this->impl->msg.msgFile);
        this->impl->msg.msgFile = NULL;
    }
}

void Gui::MsgRead(i32 msgIdx)
{
    this->impl->MsgRead(msgIdx);
}

} /* namespace th08 */
