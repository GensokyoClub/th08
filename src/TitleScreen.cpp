#include "AsciiManager.hpp"
#include "GameManager.hpp"
#include "i18n.hpp"
#include "ScreenEffect.hpp"
#include "ScoreDat.hpp"
#include "SoundPlayer.hpp"
#include "SpellCard.hpp"
#include "TitleScreen.hpp"
#include "ZunMath.hpp"

#define TITLE_SPRITE_OPTION_START                       10

#define TITLE_SPRITE_OPTION_PLAYER_START                20
#define TITLE_SPRITE_OPTION_PLAYER_END                  26

#define TITLE_SPRITE_OPTION_GRAPHICS_MODE_START         27
#define TITLE_SPRITE_OPTION_GRAPHICS_MODE_END           28

#define TITLE_SPRITE_OPTION_MUSIC_MODE_START            29
#define TITLE_SPRITE_OPTION_MUSIC_MODE_END              31

#define TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3         32
#define TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2         33
#define TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT1         34

#define TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3           36
#define TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2           37
#define TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT1           38

#define TITLE_SPRITE_OPTION_WINDOWED_START              40
#define TITLE_SPRITE_OPTION_WINDOWED_END                41

#define TITLE_SPRITE_OPTION_SLOWMODE_START              42
#define TITLE_SPRITE_OPTION_SLOWMODE_END                43

#define TITLE_SPRITE_KEYCONFIG_SLOWSHOT_START           75
#define TITLE_SPRITE_KEYCONFIG_SLOWSHOT_END             76

#define TITLE_SPRITE_CHARACTER_START                    111
#define TITLE_SPRITE_CHARACTER_END                      130
#define TITLE_SPRITE_DIFFICULTY_START                   131
#define TITLE_SPRITE_DIFFICULTY_EXTRA                   (TITLE_SPRITE_DIFFICULTY_START + 4)

namespace th08
{


DIFFABLE_STATIC(TitleScreen *, g_TitleScreen);

i32 g_TitleCharacterSpriteIndices[12][4] =
{
    /* Team character sprites */
    { 0x77, 0x6f, 0x70, -1, },
    { 0x78, 0x72, 0x71, -1, },
    { 0x79, 0x73, 0x74, -1, },
    { 0x7a, 0x76, 0x75, -1, },

    /* Solo character sprites */
    { 0x7b, 0x6f, -1, 0x70 },
    { 0x7c, 0x70, -1, 0x6f },
    { 0x7d, 0x72, -1, 0x71 },
    { 0x7e, 0x71, -1, 0x72 },
    { 0x7f, 0x73, -1, 0x74 },
    { 0x80, 0x74, -1, 0x73 },
    { 0x81, 0x76, -1, 0x75 },
    { 0x82, 0x75, -1, 0x76 },
};

DIFFABLE_STATIC_ASSIGN(const char *, g_DemoReplayFiles[]) =
{
    "demo/demorpy0.rpy",
    "demo/demorpy1.rpy",
    "demo/demorpy2.rpy",
    "demo/demorpy3.rpy",
};

DIFFABLE_STATIC_ASSIGN(const char *, g_StartMenuHelpText[]) =
{
    TH_TITLE_STARTMENU_HELPTEXT0,
    TH_TITLE_STARTMENU_HELPTEXT1,
    TH_TITLE_STARTMENU_HELPTEXT2,
    TH_TITLE_STARTMENU_HELPTEXT3,
    TH_TITLE_STARTMENU_HELPTEXT4,
    TH_TITLE_STARTMENU_HELPTEXT5,
    TH_TITLE_STARTMENU_HELPTEXT6,
    TH_TITLE_STARTMENU_HELPTEXT7,
    TH_TITLE_STARTMENU_HELPTEXT8,
};

DIFFABLE_STATIC_ASSIGN(const char *, g_OptionsHelpText[]) =
{
    TH_TITLE_OPTIONS_HELPTEXT0,
    TH_TITLE_OPTIONS_HELPTEXT1,
    TH_TITLE_OPTIONS_HELPTEXT2,
    TH_TITLE_OPTIONS_HELPTEXT3,
    TH_TITLE_OPTIONS_HELPTEXT4,
    TH_TITLE_OPTIONS_HELPTEXT5,
    TH_TITLE_OPTIONS_HELPTEXT6,
    TH_TITLE_OPTIONS_HELPTEXT7,
    TH_TITLE_OPTIONS_HELPTEXT8,
    TH_TITLE_OPTIONS_HELPTEXT9,
};

DIFFABLE_STATIC_ASSIGN(i16, g_LastKeyChanged) = 32;

DIFFABLE_STATIC_ASSIGN(const char *, g_KeyConfigHelpText[]) =
{
    TH_TITLE_KEYCONFIG_HELPTEXT0,
    TH_TITLE_KEYCONFIG_HELPTEXT1,
    TH_TITLE_KEYCONFIG_HELPTEXT2,
    TH_TITLE_KEYCONFIG_HELPTEXT3,
    TH_TITLE_KEYCONFIG_HELPTEXT4,
    TH_TITLE_KEYCONFIG_HELPTEXT5,
    TH_TITLE_KEYCONFIG_HELPTEXT6,
    TH_TITLE_KEYCONFIG_HELPTEXT7,
    TH_TITLE_KEYCONFIG_HELPTEXT8,
    TH_TITLE_KEYCONFIG_HELPTEXT9,
    TH_TITLE_KEYCONFIG_HELPTEXT10,
    TH_TITLE_KEYCONFIG_HELPTEXT11,
};

DIFFABLE_STATIC_ASSIGN(const char *, g_StageNames[]) =
{
    "Stage1 ",
    "Stage2 ",
    "Stage3 ",
    "Stage4A",
    "Stage4B",
    "Stage5 ",
    "Stage6A",
    "Stage6B",
    "StageEX",
};

DIFFABLE_STATIC_ASSIGN(const char *, g_StageNamesSpellPractice[]) =
{
    "Stage1   ",
    "Stage2   ",
    "Stage3   ",
    "Stage4A  ",
    "Stage4B  ",
    "Stage5   ",
    "Stage6A  ",
    "Stage6B  ",
    "Extra    ",
    "Last Word"
};

DIFFABLE_STATIC_ASSIGN(const char *, g_FullWidthDigits[]) =
{
    TH_TITLE_FULLWIDTH_DIGIT_0,
    TH_TITLE_FULLWIDTH_DIGIT_1,
    TH_TITLE_FULLWIDTH_DIGIT_2,
    TH_TITLE_FULLWIDTH_DIGIT_3,
    TH_TITLE_FULLWIDTH_DIGIT_4,
    TH_TITLE_FULLWIDTH_DIGIT_5,
    TH_TITLE_FULLWIDTH_DIGIT_6,
    TH_TITLE_FULLWIDTH_DIGIT_7,
    TH_TITLE_FULLWIDTH_DIGIT_8,
    TH_TITLE_FULLWIDTH_DIGIT_9,
};

void DrawPieChart(D3DXVECTOR3 *position, D3DCOLOR color, float param_3, float param_4);

ChainCallbackResult TitleScreen::OnUpdate(TitleScreen *titleScreen)
{
    ChainCallbackResult result;

    if (titleScreen->state != TitleScreenState_Ready)
    {
        if (titleScreen->state == TitleScreenState_Close)
        {
            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
        }
        else
        {
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
    }

    switch (titleScreen->currentScreen)
    {
    case TitleCurrentScreen_StartMenu:
        result = titleScreen->OnUpdateStartMenu();
        break;
    case TitleCurrentScreen_Replay:
        result = titleScreen->OnUpdateReplayMenu();
        break;
    case TitleCurrentScreen_Option:
        result = titleScreen->OnUpdateOptions();
        break;
    case TitleCurrentScreen_KeyConfig:
        result = titleScreen->OnUpdateKeyConfig();
        break;
    case TitleCurrentScreen_DifficultySelect:
    case TitleCurrentScreen_DifficultySelectPractice:
    case TitleCurrentScreen_DifficultySelectExtra:
        result = titleScreen->OnUpdateDifficultySelect();
        break;
    case TitleCurrentScreen_CharacterSelect:
    case TitleCurrentScreen_CharacterSelectPractice:
    case TitleCurrentScreen_CharacterSelectExtra:
    case TitleCurrentScreen_CharacterSelectSpell:
        result = titleScreen->OnUpdateCharacterSelect();
        break;
    case TitleCurrentScreen_PracticeStageSelect:
        result = titleScreen->OnUpdatePracticeStageSelect();
        break;
    case TitleCurrentScreen_SpellStageSelect:
        result = titleScreen->OnUpdateSpellStageSelect();
        break;
    case TitleCurrentScreen_SpellCardSelect:
        result = titleScreen->OnUpdateSpellCardSelect();
        break;
    }

    g_AnmManager->ExecuteScriptArray(titleScreen->vms, titleScreen->vmCount);
    if (titleScreen->currentHelpTextVm != NULL)
    {
        g_AnmManager->ExecuteScript(titleScreen->currentHelpTextVm);
    }

    return result;
}

ChainCallbackResult TitleScreen::OnUpdateStartMenu()
{
    i32 i;

    switch (this->currentScreenState)
    {
    case 0:
        if (this->stateTimer2 == 0)
        {
            if (this->previousScreen == TitleCurrentScreen_StartMenu
                && g_Supervisor.wantedState2 != SupervisorState_ResultScreen)
            {
                g_Supervisor.PlayMusic(8, 0);
            }

            if (this->previousScreen == TitleCurrentScreen_StartMenu
                || this->previousScreen == TitleCurrentScreen_DifficultySelect
                || this->previousScreen == TitleCurrentScreen_Replay
                || this->previousScreen == TitleCurrentScreen_DifficultySelectPractice
                || this->previousScreen == TitleCurrentScreen_DifficultySelectExtra
                || this->previousScreen == TitleCurrentScreen_CharacterSelectSpell
                || this->previousScreen == TitleCurrentScreen_SpellStageSelect)
            {
                if (g_AnmManager->LoadSurface(0, "title/title00.png") != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
                }
            }

            if (this->vmCount == 0)
            {
                this->vmCount = 142;
                this->vms = new AnmVm[this->vmCount];
                this->titleAnm->ExecuteAnmIdxArray(this->vms, 0, this->vmCount);
            }

            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 2);

            if (g_GameManager.IsReplay())
            {
                this->ChangeCurrentScreen(TitleCurrentScreen_Replay);
                g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 13);
                this->currentHelpTextVm->SetInterrupt(2);
                g_GameManager.SetIsReplayWeird(FALSE);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            }

            if (this->practiceState != 0)
            {
                if (this->practiceState == 2)
                {
                    g_GameManager.flags.isSpellPractice = TRUE;
                }

                this->ChangeCurrentScreen(g_GameManager.IsSpellPractice() ? TitleCurrentScreen_CharacterSelectSpell : TitleCurrentScreen_CharacterSelectPractice);

                g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 5);
                this->currentHelpTextVm->SetInterrupt(2);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            }

            /* Set each menu item's sprite. */
            for (i = 0; i < 9; i++)
            {
                this->titleAnm->SetSprite(&this->vms[1 + i], this->vms[1 + i].baseSpriteIndex + 1);
            }

            /* Mark the selected menu item. */
            this->titleAnm->SetSprite(&this->vms[1 + this->cursor], this->vms[1 + this->cursor].baseSpriteIndex);

            /* Mark the "Spell Practice" button as grayed out. */
            if (!g_GameManager.IsSpellPracticeUnlocked())
            {
                this->vms[3].prefix.color1.d3dColor = 0xff404040;
            }

            /* Mark the "Extra Start" button as grayed out. */
            if (!g_GameManager.IsExtraUnlocked())
            {
                this->vms[2].prefix.color1.d3dColor = 0xff404040;
            }
        }

        if (this->stateTimer2 < ARRAY_SIZE(g_StartMenuHelpText))
        {
            g_AnmManager->DrawTextCentered(&this->helpTextVms[this->stateTimer2], 0xfff0e0, 0x300000, g_StartMenuHelpText[this->stateTimer2]);
            this->stateTimer2++;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        this->stateTimer2 = 0;
        this->stateTimer = 0;
        this->cursor2 = -1;
        this->currentScreenState = 1;
        this->startMenuIdleFrames = 0;
    case 1:
        i = this->MoveCursorVertical(9);
        if (i != 0)
        {
            /* ... Just why, ZUN */
back:
            if (!g_GameManager.IsSpellPracticeUnlocked())
            {
                if (this->cursor == 2)
                {
                    this->cursor += i;
                    goto back;
                }
            }

            if (!g_GameManager.IsExtraUnlocked())
            {
                if (this->cursor == 1)
                {
                    this->cursor += i;
                    goto back;
                }
            }

            /* Set each menu item's sprite. */
            for (i = 0; i < 9; i++)
            {
                this->titleAnm->SetSprite(&this->vms[1 + i], this->vms[1 + i].baseSpriteIndex + 1);
            }

            /* Mark the selected menu item. */
            this->titleAnm->SetSprite(&this->vms[1 + this->cursor], this->vms[1 + this->cursor].baseSpriteIndex);

            /* Mark the "Spell Practice" button as grayed out. */
            if (!g_GameManager.IsSpellPracticeUnlocked())
            {
                this->vms[3].prefix.color1.d3dColor = 0xff404040;
            }

            /* Mark the "Extra Start" button as grayed out. */
            if (!g_GameManager.IsExtraUnlocked())
            {
                this->vms[2].prefix.color1.d3dColor = 0xff404040;
            }
        }

        this->startMenuIdleFrames++;
        if (g_CurFrameInput != 0)
        {
            this->startMenuIdleFrames = 0;
        }

        if (this->startMenuIdleFrames > 1500)
        {
            /* TODO: needs ReplayManager work */

            g_GameManager.currentDemoReplay = (g_GameManager.currentDemoReplay + 1) % ARRAY_SIZE_SIGNED(g_DemoReplayFiles);
            strcpy(g_GameManager.replayFilename, g_DemoReplayFiles[g_GameManager.currentDemoReplay]);

            ZUN_FREE(this->currentReplay);

            utils::GuiDebugPrint("error : Demo Play is not ready\r\n");
        }

        if (this->cursor != this->cursor2)
        {
            this->currentHelpTextVm = this->helpTextVms + this->cursor;
            this->currentHelpTextVm->SetInterrupt(1);
        }

        this->cursor2 = this->cursor;

        if (this->stateTimer2 < 10)
        {
            break;
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
            g_SoundPlayer.ProcessQueues();

            switch (this->cursor)
            {
            case 0:         /* Start */
                g_GameManager.flags.isPracticeMode = FALSE;
                g_GameManager.flags.isSpellPractice = FALSE;
                this->cursor = g_Supervisor.cfg.defaultDifficulty;
                if(this->cursor >= EXTRA)
                {
                    this->cursor = HARD;
                }

                this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelect);
                g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 5);
                this->currentHelpTextVm->SetInterrupt(2);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            case 3:         /* Practice Start */
                g_GameManager.flags.isPracticeMode = TRUE;
                g_GameManager.flags.isSpellPractice = FALSE;

                this->cursor = g_Supervisor.cfg.defaultDifficulty;
                if (this->cursor >= EXTRA)
                {
                    this->cursor = HARD;
                }

                this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelectPractice);
                g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 5);

                this->currentHelpTextVm->SetInterrupt(2);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            case 1:         /* Extra Start */
                if (g_GameManager.IsExtraUnlocked())
                {
                    g_GameManager.flags.isPracticeMode = FALSE;
                    g_GameManager.flags.isSpellPractice = FALSE;

                    this->cursor = 0;
                    this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelectExtra);

                    g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 5);
                    this->currentHelpTextVm->SetInterrupt(2);

                    return CHAIN_CALLBACK_RESULT_CONTINUE;
                }
            case 2:         /* Spell Practice */
                if (g_GameManager.IsSpellPracticeUnlocked())
                {
                    g_GameManager.flags.isPracticeMode = TRUE;
                    g_GameManager.flags.isSpellPractice = TRUE;

                    this->cursor = g_GameManager.shotType;
                    this->ChangeCurrentScreen(TitleCurrentScreen_SpellStageSelect);

                    g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 5);
                    this->currentHelpTextVm->SetInterrupt(2);

                    return CHAIN_CALLBACK_RESULT_CONTINUE;
                }
            case 4:         /* Replay */
                g_GameManager.flags.isPracticeMode = FALSE;
                g_GameManager.flags.isSpellPractice = FALSE;

                this->ChangeCurrentScreen(TitleCurrentScreen_Replay);

                g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 13);
                this->currentHelpTextVm->SetInterrupt(2);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            case 6:         /* Music Room */
                g_Supervisor.curState = SupervisorState_MusicRoom;
                this->currentHelpTextVm->SetInterrupt(2);
                return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
            case 5:         /* Result */
                g_Supervisor.curState = SupervisorState_ResultScreen;
                this->currentHelpTextVm->SetInterrupt(2);
                return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
            case 7:         /* Option */
                /* ??? */
                this->currentScreenState = 0;
                this->cursor = 0;
                this->stateTimer2 = 0;
                this->stateTimer = 0;
                this->currentScreenState = 3;
                this->stateTimer = 0;
                this->OnUpdateOptions();
                this->cursor = 0;
                break;
            case 8:         /* Quit */
                this->currentScreenState = 2;
                this->stateTimer = 0;
                g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 1);
                if (g_Supervisor.cfg.musicMode == MIDI)
                {
                    g_Supervisor.midiOutput->PlayFile(30);
                }
                break;
            }
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            this->titleAnm->SetSprite(&this->vms[this->cursor + 1], this->vms[this->cursor + 1].baseSpriteIndex + 1);
            this->cursor = 8;
            this->titleAnm->SetSprite(&this->vms[this->cursor + 1], this->vms[this->cursor + 1].baseSpriteIndex + 1);
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
            g_SoundPlayer.ProcessQueues();
        }
        break;
    case 2:
        if (stateTimer >= 60)
        {
            ZUN_DELETE2(this->vms);
            this->vms = NULL;
            /* Yes, this->vms is set to NULL twice. */
            this->vms = NULL;
            this->vmCount = 0;
            this->stateTimer2 = 0;
            g_Supervisor.curState = SupervisorState_ExitGame;

            return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
        }
        break;
    case 3:
        if (stateTimer >= 30)
        {
            this->ChangeCurrentScreen(TitleCurrentScreen_Option);
            this->currentGameConfig = g_Supervisor.cfg;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        break;
    }

    this->stateTimer++;
    this->stateTimer2++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult TitleScreen::OnUpdateOptions()
{
    i32 i;

    switch (this->currentScreenState)
    {
    case 0:
        if (this->stateTimer2 == 0)
        {
            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 3);

            for (i = 0; i < 10; i++)
            {
                this->titleAnm->SetSprite(&this->vms[i + TITLE_SPRITE_OPTION_START], this->vms[i + TITLE_SPRITE_OPTION_START].baseSpriteIndex + 1);
            }
            this->titleAnm->SetSprite(&this->vms[this->cursor + TITLE_SPRITE_OPTION_START],
                                      this->vms[this->cursor + TITLE_SPRITE_OPTION_START].baseSpriteIndex);
            this->currentScreenState = 0;
            this->stateTimer = 0;
            this->cursor2 = -1;
        }
        this->currentScreenState = 1;

        for (i = 0; i < ARRAY_SIZE(g_OptionsHelpText); i++)
        {
            g_AnmManager->DrawTextCentered(&this->helpTextVms[i], 0xfff0e0, 0x300000, g_OptionsHelpText[i]);
        }
    case 1:
        if (this->MoveCursorVertical(10) != 0)
        {
            for (i = 0; i < 10; i++)
            {
                this->titleAnm->SetSprite(&this->vms[i + TITLE_SPRITE_OPTION_START], this->vms[i + TITLE_SPRITE_OPTION_START].baseSpriteIndex + 1);
            }
            this->titleAnm->SetSprite(&this->vms[this->cursor + TITLE_SPRITE_OPTION_START], this->vms[this->cursor + TITLE_SPRITE_OPTION_START].baseSpriteIndex);
        }

        if (this->cursor2 != this->cursor)
        {
            this->currentHelpTextVm = &this->helpTextVms[this->cursor];
            this->currentHelpTextVm->SetInterrupt(1);
        }

        this->cursor2 = this->cursor;

        /* Player life option */

        for (i = TITLE_SPRITE_OPTION_PLAYER_START; i <= TITLE_SPRITE_OPTION_PLAYER_END; i++)
        {
            this->titleAnm->SetSprite(&this->vms[i], this->vms[i].baseSpriteIndex + 1);
        }

        if (g_GameManager.plst.playDataTotals.attemptsTotal < 30)
        {
            this->vms[25].prefix.flag1 = FALSE;
        }

        if (g_GameManager.plst.playDataTotals.attemptsTotal < 60)
        {
            this->vms[26].prefix.flag1 = FALSE;
        }

        i = TITLE_SPRITE_OPTION_PLAYER_START + g_Supervisor.cfg.lifeCount;
        this->titleAnm->SetSprite(&this->vms[i],
                                  this->vms[i].baseSpriteIndex);

        /* Graphics mode */

        for (i = TITLE_SPRITE_OPTION_GRAPHICS_MODE_START; i <= TITLE_SPRITE_OPTION_GRAPHICS_MODE_END; i++)
        {
            this->titleAnm->SetSprite(&this->vms[i], this->vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_OPTION_GRAPHICS_MODE_START + g_Supervisor.cfg.colorMode16bit;
        this->titleAnm->SetSprite(&this->vms[i],
                                  this->vms[i].baseSpriteIndex);

        /* Music mode */

        for (i = TITLE_SPRITE_OPTION_MUSIC_MODE_START; i <= TITLE_SPRITE_OPTION_MUSIC_MODE_END; i++)
        {
            this->titleAnm->SetSprite(&this->vms[i], this->vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_OPTION_MUSIC_MODE_START + g_Supervisor.cfg.musicMode;
        this->titleAnm->SetSprite(&this->vms[i],
                                  this->vms[i].baseSpriteIndex);

        /* Music volume */

        /* 3rd digit */
        if (g_Supervisor.cfg.musicVolume >= 100)
        {
            this->titleAnm->SetSprite(&this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3],
                                      this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3].baseSpriteIndex);
            this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3].prefix.color1.a = 255;
        }
        else
        {
            this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3].prefix.color1.a = 0;
        }

        /* 2nd digit */
        if (g_Supervisor.cfg.musicVolume >= 10)
        {
            this->titleAnm->SetSprite(&this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2],
                                      this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2].baseSpriteIndex
                                      + ((g_Supervisor.cfg.musicVolume / 10) % 10) * 2);
            this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2].prefix.color1.a = 255;
        }
        else
        {
            this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2].prefix.color1.a = 0;
        }

        this->titleAnm->SetSprite(&this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT1], this->vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT1].baseSpriteIndex + ((g_Supervisor.cfg.musicVolume) % 10) * 2);

        this->titleAnm->SetSprite(&this->vms[35], this->vms[35].baseSpriteIndex);

        /* SFX volume */

        /* 3rd digit */
        if (g_Supervisor.cfg.sfxVolume >= 100)
        {
            this->titleAnm->SetSprite(&this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3],
                                      this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3].baseSpriteIndex);
            this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3].prefix.color1.a = 255;
        }
        else
        {
            this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3].prefix.color1.a = 0;
        }

        /* 2nd digit */
        if (g_Supervisor.cfg.sfxVolume >= 10)
        {
            this->titleAnm->SetSprite(&this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2],
                                      this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2].baseSpriteIndex
                                      + ((g_Supervisor.cfg.sfxVolume / 10) % 10) * 2);
            this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2].prefix.color1.a = 255;
        }
        else
        {
            this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2].prefix.color1.a = 0;
        }

        this->titleAnm->SetSprite(&this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT1],
                                  this->vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT1].baseSpriteIndex + ((g_Supervisor.cfg.sfxVolume % 10) * 2));

        /* Display mode */

        this->titleAnm->SetSprite(&this->vms[39], this->vms[39].baseSpriteIndex);
        for (i = TITLE_SPRITE_OPTION_WINDOWED_START; i <= TITLE_SPRITE_OPTION_WINDOWED_END; i++)
        {
            this->titleAnm->SetSprite(&this->vms[i], this->vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_OPTION_WINDOWED_START + g_Supervisor.cfg.windowed;
        this->titleAnm->SetSprite(&this->vms[i],
                                  this->vms[i].baseSpriteIndex);

        /* Slow mode */

        for (i = TITLE_SPRITE_OPTION_SLOWMODE_START; i <= TITLE_SPRITE_OPTION_SLOWMODE_END; i++)
        {
            this->titleAnm->SetSprite(&this->vms[i], this->vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_OPTION_SLOWMODE_START + g_Supervisor.cfg.slowMode;
        this->titleAnm->SetSprite(&this->vms[i],
                                  this->vms[i].baseSpriteIndex);

        if (this->stateTimer2 < 4)
        {
            break;
        }

        if (WAS_PRESSED_SCROLLING(TH_BUTTON_LEFT))
        {
            switch (this->cursor)
            {
            case 0:         /* Player */
                i = 7;
                if (g_GameManager.plst.playDataTotals.attemptsTotal < 30)
                {
                    i--;
                }
                if (g_GameManager.plst.playDataTotals.attemptsTotal < 60)
                {
                    i--;
                }
                if (g_Supervisor.cfg.lifeCount == 0)
                {
                    g_Supervisor.cfg.lifeCount = i - 1;
                }
                else
                {
                    g_Supervisor.cfg.lifeCount--;
                }
                break;
            case 1:         /* Graphic */
                if (g_Supervisor.cfg.colorMode16bit == 0)
                {
                    g_Supervisor.cfg.colorMode16bit = 1;
                }
                else
                {
                    g_Supervisor.cfg.colorMode16bit--;
                }
                break;
            case 2:         /* BGM */
                g_Supervisor.StopAudio();
                if (g_Supervisor.cfg.musicMode == MIDI)
                {
                    g_Supervisor.midiOutput->PlayFile(30);
                }
                if (g_Supervisor.cfg.musicMode == OFF)
                {
                    g_Supervisor.cfg.musicMode = MIDI;
                }
                else
                {
                    g_Supervisor.cfg.musicMode--;
                }
                g_Supervisor.LoadMusic(8, "bgm/th08_01.mid");
                g_Supervisor.PlayMusic(8, 0);
                break;
            case 5:         /* Mode */
                if (g_Supervisor.cfg.windowed == 0)
                {
                    g_Supervisor.cfg.windowed = 1;
                }
                else
                {
                    g_Supervisor.cfg.windowed--;
                }
                break;
            case 6:         /* SlowMode */
                if (g_Supervisor.cfg.slowMode == 0)
                {
                    g_Supervisor.cfg.slowMode = 1;
                }
                else
                {
                    g_Supervisor.cfg.slowMode--;
                }
                break;
            default:
                goto left_button_check;
            }

            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
            g_SoundPlayer.ProcessQueues();
        }
left_button_check:

        if (WAS_PRESSED(TH_BUTTON_LEFT))
        {
            switch (this->cursor)
            {
            case 3:     /* Vol */
                g_Supervisor.cfg.musicVolume -= 4;
                if (g_Supervisor.cfg.musicVolume < 0)
                {
                    g_Supervisor.cfg.musicVolume = 0;
                }
                g_SoundPlayer.QueueSetVolumeCommand();
                break;
            case 4:     /* S.E. Vol */
                g_Supervisor.cfg.sfxVolume -= 4;
                if (g_Supervisor.cfg.sfxVolume < 0)
                {
                    g_Supervisor.cfg.sfxVolume = 0;
                }
                g_SoundPlayer.QueueSetVolumeCommand();
                break;
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RIGHT))
        {
            switch (this->cursor)
            {
            case 3:     /* Vol */
                g_Supervisor.cfg.musicVolume += 4;
                if (g_Supervisor.cfg.musicVolume > 100)
                {
                    g_Supervisor.cfg.musicVolume = 100;
                }
                g_SoundPlayer.QueueSetVolumeCommand();
                break;
            case 4:     /* S.E. Vol */
                g_Supervisor.cfg.sfxVolume += 4;
                if (g_Supervisor.cfg.sfxVolume > 100)
                {
                    g_Supervisor.cfg.sfxVolume = 100;
                }
                g_SoundPlayer.QueueSetVolumeCommand();
                break;
            }
        }

        /* If you press left for more than 30 frames, counter goes brrr */
        if (WAS_PRESSED(TH_BUTTON_LEFT)
            || g_NumOfFramesInputsWereHeld >= 30 && (g_CurFrameInput & TH_BUTTON_LEFT))
        {
            switch (this->cursor)
            {
            case 3:     /* Vol */
                if (g_Supervisor.cfg.musicVolume > 0)
                {
                    g_Supervisor.cfg.musicVolume--;
                }
                g_SoundPlayer.QueueSetVolumeCommand();
                break;
            case 4:     /* S.E. Vol */
                if (g_Supervisor.cfg.sfxVolume > 0)
                {
                    g_Supervisor.cfg.sfxVolume--;
                }
                g_SoundPlayer.QueueSetVolumeCommand();
                break;
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RIGHT)
            || g_NumOfFramesInputsWereHeld >= 30 && (g_CurFrameInput & TH_BUTTON_RIGHT))
        {
            switch (this->cursor)
            {
            case 3:     /* Vol */
                if (g_Supervisor.cfg.musicVolume < 100)
                {
                    g_Supervisor.cfg.musicVolume++;
                }
                g_SoundPlayer.QueueSetVolumeCommand();
                break;
            case 4:     /* S.E. Vol */
                if (g_Supervisor.cfg.sfxVolume < 100)
                {
                    g_Supervisor.cfg.sfxVolume++;
                }
                g_SoundPlayer.QueueSetVolumeCommand();
                break;
            }
        }

        g_SoundPlayer.bgmVolume = g_Supervisor.cfg.musicVolume;
        g_SoundPlayer.sfxVolume = g_Supervisor.cfg.sfxVolume;

        if (WAS_PRESSED_SCROLLING(TH_BUTTON_RIGHT))
        {
            switch (this->cursor)
            {
            case 0:         /* Player */
                i = 7;
                if (g_GameManager.plst.playDataTotals.attemptsTotal < 30)
                {
                    i--;
                }
                if (g_GameManager.plst.playDataTotals.attemptsTotal < 60)
                {
                    i--;
                }
                if (g_Supervisor.cfg.lifeCount >= i - 1)
                {
                    g_Supervisor.cfg.lifeCount = 0;
                }
                else
                {
                    g_Supervisor.cfg.lifeCount++;
                }
                break;
            case 1:         /* Graphic */
                if (g_Supervisor.cfg.colorMode16bit >= 1)
                {
                    g_Supervisor.cfg.colorMode16bit = 0;
                }
                else
                {
                    g_Supervisor.cfg.colorMode16bit++;
                }
                break;
            case 2:         /* BGM */
                g_Supervisor.StopAudio();
                if (g_Supervisor.cfg.musicMode >= MIDI)
                {
                    g_Supervisor.cfg.musicMode = OFF;
                }
                else
                {
                    g_Supervisor.cfg.musicMode++;
                }
                g_Supervisor.LoadMusic(8, "bgm/th08_01.mid");
                g_Supervisor.PlayMusic(8, 0);
                break;
            case 5:         /* Mode */
                if (g_Supervisor.cfg.windowed >= 1)
                {
                    g_Supervisor.cfg.windowed = 0;
                }
                else
                {
                    g_Supervisor.cfg.windowed++;
                }
                break;
            case 6:         /* SlowMode */
                if (g_Supervisor.cfg.slowMode >= 1)
                {
                    g_Supervisor.cfg.slowMode = 0;
                }
                else
                {
                    g_Supervisor.cfg.slowMode++;
                }
                break;
            default:
                goto sfx_play;
            }

            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
            g_SoundPlayer.ProcessQueues();
        }

sfx_play:
        /* Play sound effect when hovering over Vol or S.E. Vol menu items */
        switch (this->cursor)
        {
        case 3:
        case 4:
            if ((this->stateTimer2 % 50) == 0)
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_TIMEOUT, 0);
            }
            break;
        default:
            break;
        }

        if (g_CurFrameInput != 0)
        {
            this->idleFrames = 0;
        }

        if (this->idleFrames >= 3600)
        {
            goto exit_options;
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            switch (cursor)
            {
            case 7:         /* Reset */
                g_Supervisor.cfg.lifeCount = 2;
                g_Supervisor.cfg.bombCount = 3;
                g_Supervisor.cfg.musicMode = WAV;
                g_Supervisor.cfg.playSounds = 1;
                g_Supervisor.cfg.slowMode = 0;

                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
                g_SoundPlayer.ProcessQueues();
                break;
            case 8:         /* KeyConfig */
                this->cursor = 0;
                this->ChangeCurrentScreen(TitleCurrentScreen_KeyConfig);
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
                g_SoundPlayer.ProcessQueues();
                return CHAIN_CALLBACK_RESULT_CONTINUE;
            case 9:         /* Exit */
exit_options:
                this->cursor = 7;
                this->ChangeCurrentScreen(TitleCurrentScreen_StartMenu);
                g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
                g_SoundPlayer.ProcessQueues();

                if (this->currentGameConfig.colorMode16bit != g_Supervisor.cfg.colorMode16bit
                    || this->currentGameConfig.windowed != g_Supervisor.cfg.windowed)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR;
                }
                else
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            if (this->cursor == 9)
            {
                goto exit_options;
            }

            this->titleAnm->SetSprite(&this->vms[this->cursor + TITLE_SPRITE_OPTION_START], this->vms[this->cursor + TITLE_SPRITE_OPTION_START].baseSpriteIndex + 1);
            this->cursor = 9;
            this->titleAnm->SetSprite(&this->vms[this->cursor + TITLE_SPRITE_OPTION_START], this->vms[this->cursor + TITLE_SPRITE_OPTION_START].baseSpriteIndex);

            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
            g_SoundPlayer.ProcessQueues();
        }
    }

    this->idleFrames++;
    this->stateTimer++;
    this->stateTimer2++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(vmPair, i, keyToChange, controllerState)
ChainCallbackResult TitleScreen::OnUpdateKeyConfig()
{
    AnmVm *vmPair;
    i32 i;
    u8 *controllerState;
    i16 keyToChange;

    switch (this->currentScreenState)
    {
    case 0:
        if (this->stateTimer2 == 0)
        {
            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 4);

            for (i = 0; i < 12; i++)
            {
                this->titleAnm->SetSprite(&this->vms[i + 45], this->vms[i + 45].baseSpriteIndex + 1);
            }

            this->titleAnm->SetSprite(&this->vms[this->cursor + 45], this->vms[this->cursor + 45].baseSpriteIndex);
            this->currentScreenState = 0;
            this->stateTimer = 0;

            this->controllerMapping = g_Supervisor.cfg.controllerMapping;

            g_Supervisor.cfg.controllerMapping.upButton = -1;
            g_Supervisor.cfg.controllerMapping.downButton = -1;

            /* Yes, ZUN really did write this. */
            vmPair = &this->vms[57];
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.shotButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.bombButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.focusButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.skipButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.menuButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.upButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.downButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.leftButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, this->controllerMapping.rightButton);

            this->cursor2 = -1;
        }

        this->currentScreenState = 1;

        for (i = 0; i < ARRAY_SIZE(g_KeyConfigHelpText); i++)
        {
            g_AnmManager->DrawTextCentered(&this->helpTextVms[i], 0xfff0e0, 0x300000, g_KeyConfigHelpText[i]);
        }

    case 1:
        if (this->MoveCursorVertical(12) != 0)
        {
            for (i = 0; i < 12; i++)
            {
                this->titleAnm->SetSprite(&this->vms[i + 45], this->vms[i + 45].baseSpriteIndex + 1);
            }

            this->titleAnm->SetSprite(&this->vms[this->cursor + 45], this->vms[this->cursor + 45].baseSpriteIndex);
        }
        if (this->cursor2 != this->cursor)
        {
            this->currentHelpTextVm = &this->helpTextVms[this->cursor];
            this->currentHelpTextVm->SetInterrupt(1);
        }
        this->cursor2 = this->cursor;

        vmPair = &this->vms[57];
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.shotButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.bombButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.focusButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.skipButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.menuButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.upButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.downButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.leftButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, this->controllerMapping.rightButton);

        for (i = TITLE_SPRITE_KEYCONFIG_SLOWSHOT_START; i <= TITLE_SPRITE_KEYCONFIG_SLOWSHOT_END; i++)
        {
            this->titleAnm->SetSprite(&this->vms[i], this->vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_KEYCONFIG_SLOWSHOT_START + g_Supervisor.cfg.shotSlow;
        this->titleAnm->SetSprite(&this->vms[i], this->vms[i].baseSpriteIndex);

        controllerState = Controller::GetControllerState();

        for (keyToChange = 0; keyToChange < 32; keyToChange++)
        {
            if ((controllerState[keyToChange] & TH_BUTTON_RIGHT) != 0)
            {
                break;
            }
        }

        if (keyToChange < 32 && g_LastKeyChanged != keyToChange)
        {
            switch (this->cursor)
            {
            case 0:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.shotButton, 1);
                this->controllerMapping.shotButton = keyToChange;
                break;
            case 1:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.bombButton, 0);
                this->controllerMapping.bombButton = keyToChange;
                break;
            case 2:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.focusButton, 1);
                this->controllerMapping.focusButton = keyToChange;
                break;
            case 4:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.menuButton, 0);
                this->controllerMapping.menuButton = keyToChange;
                break;
            case 5:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.upButton, 0);
                this->controllerMapping.upButton = keyToChange;
                break;
            case 6:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.downButton, 0);
                this->controllerMapping.downButton = keyToChange;
                break;
            case 7:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.leftButton, 0);
                this->controllerMapping.leftButton = keyToChange;
                break;
            case 8:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.rightButton, 0);
                this->controllerMapping.rightButton = keyToChange;
                break;
            case 3:
                this->SetKeyConfigKey(keyToChange, this->controllerMapping.skipButton, 0);
                this->controllerMapping.skipButton = keyToChange;
                break;
            default:
                goto out;
            }

            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
            g_SoundPlayer.ProcessQueues();
        }

out:

        g_LastKeyChanged = keyToChange;

        /* Slow shot */
        if (WAS_PRESSED(TH_BUTTON_LEFT))
        {
            switch (this->cursor)
            {
            case 9:
                g_Supervisor.cfg.shotSlow = 1 - g_Supervisor.cfg.shotSlow;
                break;
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RIGHT))
        {
            switch (this->cursor)
            {
            case 9:
                g_Supervisor.cfg.shotSlow = 1 - g_Supervisor.cfg.shotSlow;
                break;
            }
        }

        if (g_CurFrameInput != 0)
        {
            this->idleFrames = 0;
        }

        if (this->idleFrames >= 3600)
        {
            goto exit_keyconfig;
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            switch (this->cursor)
            {
            case 10:
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
                g_SoundPlayer.ProcessQueues();

                this->controllerMapping = g_ControllerMapping;
                g_Supervisor.cfg.shotSlow = TRUE;
                break;
            case 11:
exit_keyconfig:
                g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
                g_SoundPlayer.ProcessQueues();

                this->ChangeCurrentScreen(TitleCurrentScreen_Option);

                g_Supervisor.cfg.controllerMapping = this->controllerMapping;
                this->cursor = 8;
                return CHAIN_CALLBACK_RESULT_CONTINUE;
            }
        }

        break;
    }

    this->idleFrames++;
    this->stateTimer++;
    this->stateTimer2++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(menuLength, i, oldScreen)
ChainCallbackResult TitleScreen::OnUpdateDifficultySelect()
{
    int i;
    TitleCurrentScreen oldScreen;
    int menuLength;

    switch (this->currentScreenState)
    {
    case 0:
        if (this->stateTimer2 == 0)
        {
            if (this->previousScreen != TitleCurrentScreen_CharacterSelect
                && this->previousScreen != TitleCurrentScreen_CharacterSelectPractice
                && this->previousScreen != TitleCurrentScreen_CharacterSelectExtra)
            {
                if (g_AnmManager->LoadSurface(0, "title/select00.png") != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
                }
            }

            this->cursor = g_Supervisor.cfg.defaultDifficulty;

            if (this->currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 7);
            }
            else
            {
                /* Hold over from PCB because IN doesn't have Phantasm */
                if (!g_GameManager.IsPhantasmUnlocked())
                {
                    g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 12);
                    this->cursor = 4;
                }
            }

            if (this->currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                if (this->cursor >= EXTRA)
                {
                    this->cursor = NORMAL;
                }

                for (i = 0; i < 4; i++)
                {
                    this->titleAnm->SetSprite(&this->vms[i + TITLE_SPRITE_DIFFICULTY_START],
                                              this->vms[i + TITLE_SPRITE_DIFFICULTY_START].baseSpriteIndex + 1);
                    this->vms[i + TITLE_SPRITE_DIFFICULTY_START].SetInterrupt(25);
                }

                this->titleAnm->SetSprite(&this->vms[this->cursor + TITLE_SPRITE_DIFFICULTY_START],
                                          this->vms[this->cursor + TITLE_SPRITE_DIFFICULTY_START].baseSpriteIndex);
                this->vms[this->cursor + TITLE_SPRITE_DIFFICULTY_START].SetInterrupt(24);
            }
            else
            {
                this->cursor -= 4;
                if (this->cursor < 0)
                {
                    this->cursor = 0;
                }
                this->titleAnm->SetSprite(&this->vms[TITLE_SPRITE_DIFFICULTY_EXTRA],
                                          this->vms[TITLE_SPRITE_DIFFICULTY_EXTRA].baseSpriteIndex);
            }

            this->currentScreenState = 0;
            this->stateTimer = 0;
            this->currentHelpTextVm = NULL;
        }

        if (this->practiceState != 0)
        {
            this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);
            this->cursor = 0;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        if (this->stateTimer2 == 8)
        {
            this->currentScreenState = 1;
        }
        break;
    case 1:
        menuLength = (this->currentScreen != TitleCurrentScreen_DifficultySelectExtra) ? 4 : 1;

        if (this->MoveCursorVertical(menuLength) != 0
            && this->currentScreen != TitleCurrentScreen_DifficultySelectExtra)
        {
            for (i = 0; i < 4; i++)
            {
                this->titleAnm->SetSprite(&this->vms[i + TITLE_SPRITE_DIFFICULTY_START],
                                          this->vms[i + TITLE_SPRITE_DIFFICULTY_START].baseSpriteIndex + 1);
                this->vms[i + TITLE_SPRITE_DIFFICULTY_START].SetInterrupt(25);
            }

            this->titleAnm->SetSprite(&this->vms[this->cursor + TITLE_SPRITE_DIFFICULTY_START],
                                      this->vms[this->cursor + TITLE_SPRITE_DIFFICULTY_START].baseSpriteIndex);
            this->vms[this->cursor + TITLE_SPRITE_DIFFICULTY_START].SetInterrupt(24);
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            if (this->currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                g_Supervisor.cfg.defaultDifficulty = this->cursor;
            }
            else
            {
                g_Supervisor.cfg.defaultDifficulty = this->cursor + EXTRA;
            }

            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
            g_SoundPlayer.ProcessQueues();

            if (this->currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                if (!g_GameManager.IsPracticeMode())
                {
                    this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelect);
                }
                else
                {
                    this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);
                }
            }
            else
            {
                this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectExtra);
            }

            this-> cursor = 0;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            if (this->currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                g_Supervisor.cfg.defaultDifficulty = this->cursor;
            }
            else
            {
                g_Supervisor.cfg.defaultDifficulty = this->cursor + 4;
            }
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
            g_SoundPlayer.ProcessQueues();

            this->currentScreenState = 3;
            this->stateTimer = 0;

            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 6);
        }
        break;
    case 3:
        if (this->stateTimer >= 20)
        {
            /* ?! but what does TitleScreen::previousScreen store? */
            oldScreen = this->currentScreen;

            this->ChangeCurrentScreen(TitleCurrentScreen_StartMenu);

            if (oldScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                if (!g_GameManager.IsPracticeMode())
                {
                    this->cursor = 0;
                }
                else
                {
                    this->cursor = 3;
                }
            }
            else
            {
                this->cursor = 1;
            }

            g_GameManager.flags.isPracticeMode = FALSE;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        break;
    }

    this->stateTimer++;
    this->idleFrames++;
    this->stateTimer2++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(cursorMovement, menuLength, vmIdx1, i1, vmIdx2, i2, oldScreen, isReplay)
ChainCallbackResult TitleScreen::OnUpdateCharacterSelect()
{
    i32 menuLength;
    i32 vmIdx1;
    i32 vmIdx2;
    i32 i1;
    i32 i2;
    i32 cursorMovement;
    TitleCurrentScreen oldScreen;

    switch (this->currentScreenState)
    {
    case 0:
        if (this->stateTimer2 == 0)
        {
            this->cursor = g_GameManager.shotType;
            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 8);
            menuLength = g_GameManager.IsExtraUnlockedWithAllTeams() ? 12 : 4;
            if (this->currentScreen == TitleCurrentScreen_CharacterSelectSpell)
            {
                /* This is all dead code, by the way. */
                if (g_AnmManager->LoadSurface(0, "title/select00.png") != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
                }
                while (!g_GameManager.IsSpellPracticeUnlockedForCharacter(this->cursor))
                {
                    this->cursor++;
                    if (this->cursor >= menuLength)
                    {
                        this->cursor -= menuLength;
                    }
                }
            }
            else
            {
                /* Highlight the selected difficulty. */
                this->vms[TITLE_SPRITE_DIFFICULTY_START + g_Supervisor.cfg.defaultDifficulty].SetInterrupt(9);
                if (this->currentScreen == TitleCurrentScreen_CharacterSelectExtra)
                {
                    while (!g_GameManager.IsExtraUnlockedForCharacter(this->cursor))
                    {
                        this->cursor++;
                        if (this->cursor >= menuLength)
                        {
                            this->cursor -= menuLength;
                        }
                    }
                }
            }

            for (vmIdx1 = TITLE_SPRITE_CHARACTER_START; vmIdx1 <= TITLE_SPRITE_CHARACTER_END; vmIdx1++)
            {
                this->vms[vmIdx1].prefix.flag1 = FALSE;
                this->vms[vmIdx1].SetInterrupt(8);
                for (i1 = 0; i1 < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i1++)
                {
                    if (g_TitleCharacterSpriteIndices[this->cursor][i1] == vmIdx1)
                    {
                        this->vms[vmIdx1].prefix.flag1 = TRUE;
                        this->vms[vmIdx1].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[this->cursor][i1] == vmIdx1)
                {
                    this->vms[vmIdx1].prefix.flag1 = TRUE;
                    this->vms[vmIdx1].SetInterrupt(23);
                }
            }

            this->currentScreenState = 0;
            this->stateTimer = 0;
        }
        
        if (this->practiceState != 0)
        {
            if (this->currentScreen != TitleCurrentScreen_CharacterSelectSpell)
            {
                this->ChangeCurrentScreen(TitleCurrentScreen_PracticeStageSelect);
            }
            else
            {
                this->ChangeCurrentScreen(TitleCurrentScreen_SpellStageSelect);
            }

            this->cursor = g_GameManager.currentStage;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        if (this->stateTimer2 == 8)
        {
            this->currentScreenState = 1;
        }
        break;
    case 1:
        menuLength = g_GameManager.IsExtraUnlockedWithAllTeams() ? 12 : 4;
        if (this->cursor >= menuLength)
        {
            this->cursor = 0;
        }

        cursorMovement = this->MoveCursorHorizontal(menuLength);
        if (cursorMovement != 0)
        {
            if (this->currentScreen == TitleCurrentScreen_CharacterSelectSpell)
            {
                while (!g_GameManager.IsSpellPracticeUnlockedForCharacter(this->cursor))
                {
                    this->cursor += cursorMovement;

                    if (this->cursor >= menuLength)
                    {
                        this->cursor -= menuLength;
                    }
                    if (this->cursor < 0)
                    {
                        this->cursor += menuLength;
                    }
                }
            }
            else if (this->currentScreen == TitleCurrentScreen_CharacterSelectExtra)
            {
                while (!g_GameManager.IsExtraUnlockedForCharacter(this->cursor))
                {
                    this->cursor += cursorMovement;

                    if (this->cursor >= menuLength)
                    {
                        this->cursor -= menuLength;
                    }
                    if (this->cursor < 0)
                    {
                        this->cursor += menuLength;
                    }
                }
            }
            for (vmIdx2 = TITLE_SPRITE_CHARACTER_START; vmIdx2 <= TITLE_SPRITE_CHARACTER_END; vmIdx2++)
            {
                this->vms[vmIdx2].prefix.flag1 = TRUE;
                this->vms[vmIdx2].SetInterrupt(8);
                for (i2 = 0; i2 < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i2++)
                {
                    if (g_TitleCharacterSpriteIndices[this->cursor][i2] == vmIdx2)
                    {
                        this->vms[vmIdx2].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[this->cursor][i2] == vmIdx2)
                {
                    this->vms[vmIdx2].SetInterrupt(23);
                }
            }
        }
        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            g_GameManager.shotType = this->cursor;
            g_GameManager.fullShotType = 0;

            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
            g_SoundPlayer.ProcessQueues();

            if (this->currentScreen == TitleCurrentScreen_CharacterSelectPractice)
            {
                this->cursor = g_GameManager.currentStage;
                this->ChangeCurrentScreen(TitleCurrentScreen_PracticeStageSelect);
                return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
            }
            else if (this->currentScreen == TitleCurrentScreen_CharacterSelectSpell)
            {
                this->cursor = g_GameManager.currentStage;
                this->ChangeCurrentScreen(TitleCurrentScreen_SpellStageSelect);
                return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
            }

            g_GameManager.difficulty = g_Supervisor.cfg.defaultDifficulty;
            if (this->currentScreen != TitleCurrentScreen_CharacterSelectExtra)
            {
                g_GameManager.currentStage = STAGE1;
            }
            else
            {
                g_GameManager.currentStage = g_GameManager.difficulty + EXTRA;
            }

            g_Supervisor.curState = SupervisorState_GameManager;
            g_GameManager.SetIsReplayWeird(FALSE);
            g_Supervisor.StopAudio();

            return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
            g_SoundPlayer.ProcessQueues();

            g_GameManager.shotType = this->cursor;

            if (this->currentScreen == TitleCurrentScreen_CharacterSelectSpell)
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
                g_SoundPlayer.ProcessQueues();
                this->currentScreenState = 3;
                this->stateTimer = 0;

                g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 6);

                break;
            }
            else
            {
                if (this->currentScreen != TitleCurrentScreen_CharacterSelectExtra)
                {
                    if (!g_GameManager.IsPracticeMode())
                    {
                        this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelect);
                    }
                    else
                    {
                        this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelectPractice);
                    }
                }
                else
                {
                    this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelectExtra);
                }

                this->cursor = 0;

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            }
        }
        break;
    case 3:
        if (this->stateTimer >= 30)
        {
            oldScreen = this->currentScreen;
            this->ChangeCurrentScreen(TitleCurrentScreen_StartMenu);
            this->cursor = 2;

            g_GameManager.flags.isPracticeMode = FALSE;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        break;
    }

    this->idleFrames++;
    this->stateTimer++;
    this->stateTimer2++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(vmIdx, i, cursorMovement, clearInfo)
ChainCallbackResult TitleScreen::OnUpdatePracticeStageSelect()
{
    u16 clearInfo;
    i32 i;
    i32 vmIdx;
    i32 cursorMovement;

    switch (this->currentScreenState)
    {
    case 0:
        if (this->stateTimer2 == 0)
        {
            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 18);
            this->practiceState = 0;
            for (vmIdx = TITLE_SPRITE_CHARACTER_START; vmIdx <= TITLE_SPRITE_CHARACTER_END; vmIdx++)
            {
                this->vms[vmIdx].prefix.flag1 = FALSE;
                this->vms[vmIdx].SetInterrupt(8);
                for (i = 1; i < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i++)
                {
                    if (g_TitleCharacterSpriteIndices[g_GameManager.shotType][i] == vmIdx)
                    {
                        this->vms[vmIdx].prefix.flag1 = TRUE;
                        this->vms[vmIdx].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[g_GameManager.shotType][i] == vmIdx)
                {
                    this->vms[vmIdx].prefix.flag1 = TRUE;
                    this->vms[vmIdx].SetInterrupt(23);
                }
            }
            this->currentScreenState = 0;
            this->stateTimer = 0;
            g_GameManager.flags.isPracticeMode = TRUE;
        }
        if (this->stateTimer2 == 8)
        {
            this->currentScreenState = 1;
        }
        break;
    case 1:
        clearInfo = g_GameManager.clrdData[g_GameManager.shotType].difficultiesClearedWithRetries[g_Supervisor.cfg.defaultDifficulty];

        /* Make Stage 1 selectable in practice */
        if (clearInfo == 0)
        {
            clearInfo = 1;
        }

        /* Make Stage 4A and Stage 4B selectable when Final B is cleared */
        if (IS_STAGE_CLEARED(clearInfo, STAGE6B))
        {
            clearInfo |= ZUN_BIT(STAGE4A) | ZUN_BIT(STAGE4B);
        }

        if (!IS_STAGE_CLEARED(clearInfo, this->cursor))
        {
            this->cursor = 0;
        }

        cursorMovement = this->MoveCursorVertical(8);

        if (cursorMovement != 0)
        {
            while (!IS_STAGE_CLEARED(clearInfo, this->cursor))
            {
                this->cursor += cursorMovement;
                if (this->cursor >= 8)
                {
                    this->cursor = 0;
                }
                if (this->cursor < 0)
                {
                    this->cursor = 7;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
            g_GameManager.difficulty = g_Supervisor.cfg.defaultDifficulty;
            g_GameManager.currentStage = this->cursor;

            g_Supervisor.curState = SupervisorState_GameManager;

            g_GameManager.SetIsReplayWeird(FALSE);
            g_Supervisor.StopAudio();

            return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);

            this->cursor = g_GameManager.shotType;
            this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);

            return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
        }
        break;
    }

    this->idleFrames++;
    this->stateTimer++;
    this->stateTimer2++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(menuLength1, vmIdx1, i1, horizontalCursorMovement, oldCursorPos, menuLength2, vmIdx2, i2, oldScreen)
ChainCallbackResult TitleScreen::OnUpdateSpellStageSelect()
{
    i32 menuLength1;
    i32 menuLength2;
    i32 vmIdx1;
    i32 vmIdx2;
    i32 i1;
    i32 i2;
    i32 oldCursorPos;
    i32 horizontalCursorMovement;
    TitleCurrentScreen oldScreen;

    switch (this->currentScreenState)
    {
    case 0:
        if (stateTimer2 == 0)
        {
            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 18);

            this->vms[139].SetInterrupt(27);
            this->vms[138].SetInterrupt(27);

            if (this->previousScreen != TitleCurrentScreen_SpellCardSelect)
            {
                if (g_AnmManager->LoadSurface(0, "title/select00.png") != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
                }
            }

            this->cursor = g_GameManager.shotType;

            menuLength1 = g_GameManager.IsExtraUnlockedWithAllTeams() ? 12 : 4;
            while (!g_GameManager.IsSpellPracticeUnlockedForCharacter(this->cursor))
            {
                this->cursor++;
                if (this->cursor >= menuLength1)
                {
                    this->cursor -= menuLength1;
                }
            }

            g_GameManager.shotType = this->cursor;

            this->percentageCapturedSpellPracticePerShot = 0.0f;
            this->percentageCapturedInGamePerShot = 0.0f;
            this->percentageCapturedSpellPractice = 0.0f;
            this->percentageCapturedInGame = 0.0f;

            this->cursor = g_GameManager.currentStage;

            for (vmIdx1 = TITLE_SPRITE_CHARACTER_START; vmIdx1 <= TITLE_SPRITE_CHARACTER_END; vmIdx1++)
            {
                this->vms[vmIdx1].prefix.flag1 = FALSE;
                this->vms[vmIdx1].SetInterrupt(8);
                for (i1 = 1; i1 < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i1++)
                {
                    if (g_TitleCharacterSpriteIndices[g_GameManager.shotType][i1] == vmIdx1)
                    {
                        this->vms[vmIdx1].prefix.flag1 = TRUE;
                        this->vms[vmIdx1].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[g_GameManager.shotType][i1] == vmIdx1)
                {
                    this->vms[vmIdx1].prefix.flag1 = TRUE;
                    this->vms[vmIdx1].SetInterrupt(23);
                }
            }

            this->resultTextAnm->InitializeAndSetSprite(&this->spellCardNameVms[0], 2);
            this->spellCardNameVms[0].pos = D3DXVECTOR3(0, 0, 0);
            this->spellCardNameVms[0].prefix.anchor = 3;
            this->spellCardNameVms[0].fontWidth = 15;
            this->spellCardNameVms[0].fontHeight = 15;
            this->spellCardNameVms[0].prefix.color1.a = 255;
            this->spellCardNameVms[0].prefix.color1.r = 255;
            this->spellCardNameVms[0].prefix.color1.g = 255;
            this->spellCardNameVms[0].prefix.color1.b = 255;
            g_AnmManager->DrawTextLeft(&this->spellCardNameVms[0], 0xffffff, 0, TH_TITLE_SPELL_STAGE_INFO);

            this->resultTextAnm->InitializeAndSetSprite(&this->spellCardNameVms[1], 3);
            this->spellCardNameVms[1].pos = D3DXVECTOR3(0, 0, 0);
            this->spellCardNameVms[1].prefix.anchor = 3;
            this->spellCardNameVms[1].fontWidth = 15;
            this->spellCardNameVms[1].fontHeight = 15;
            this->spellCardNameVms[1].prefix.color1.a = 255;
            this->spellCardNameVms[1].prefix.color1.r = 255;
            this->spellCardNameVms[1].prefix.color1.g = 255;
            this->spellCardNameVms[1].prefix.color1.b = 255;
            g_AnmManager->DrawTextLeft(&this->spellCardNameVms[1], 0xffffff, 0, TH_TITLE_SPELL_CAPTURE_PERCENTAGE);

            /* ZUN bug: possible copy paste mistake? */
            this->titleAnm->InitializeAndSetSprite(&this->spellCardNameVms[2], 144);
            this->spellCardNameVms[1].prefix.anchor = 3;
            this->spellCardNameVms[1].prefix.color1.a = 255;
            this->spellCardNameVms[1].prefix.color1.r = 255;
            this->spellCardNameVms[1].prefix.color1.g = 255;
            this->spellCardNameVms[1].prefix.color1.b = 255;

            this->currentScreenState = 0;
            this->stateTimer = 0;

            g_GameManager.flags.isPracticeMode = TRUE;
            g_GameManager.flags.isSpellPractice = TRUE;
        }

        if (this->practiceState != 0)
        {
            this->cursor = 0;
            this->ChangeCurrentScreen(TitleCurrentScreen_SpellCardSelect);
            if (g_GameManager.currentSpellCardNumber >= SPELL_CARD_LAST_WORD_START)
            {
                g_GameManager.currentStage = STAGE_LAST_WORD;
            }
            return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
        }

        if (stateTimer2 == 8)
        {
            this->currentScreenState = 1;
        }
        break;
    case 1:
        this->MoveCursorVertical(10);
        oldCursorPos = this->cursor;
        this->cursor = g_GameManager.shotType;

        menuLength2 = g_GameManager.IsExtraUnlockedWithAllTeams() ? 12 : 4;

        horizontalCursorMovement = this->MoveCursorHorizontal(menuLength2);
        if (horizontalCursorMovement != 0)
        {
            while (!g_GameManager.IsSpellPracticeUnlockedForCharacter(this->cursor))
            {
                this->cursor += horizontalCursorMovement;
                if (this->cursor >= menuLength2)
                {
                    this->cursor -= menuLength2;
                }
                if (this->cursor < 0)
                {
                    this->cursor += menuLength2;
                }
            }

            g_GameManager.shotType = this->cursor;

            for (vmIdx2 = TITLE_SPRITE_CHARACTER_START; vmIdx2 <= TITLE_SPRITE_CHARACTER_END; vmIdx2++)
            {
                this->vms[vmIdx2].prefix.flag1 = FALSE;
                this->vms[vmIdx2].SetInterrupt(8);
                for (i2 = 1; i2 < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i2++)
                {
                    if (g_TitleCharacterSpriteIndices[this->cursor][i2] == vmIdx2)
                    {
                        this->vms[vmIdx2].prefix.flag1 = TRUE;
                        this->vms[vmIdx2].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[this->cursor][i2] == vmIdx2)
                {
                    this->vms[vmIdx2].prefix.flag1 = TRUE;
                    this->vms[vmIdx2].SetInterrupt(23);
                }
            }

            this->stateTimer2 = 0;
        }

        this->cursor = oldCursorPos;

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);

            g_GameManager.currentStage = this->cursor;

            /* ??? */
            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
            g_SoundPlayer.ProcessQueues();

            this->cursor = 0;

            this->ChangeCurrentScreen(TitleCurrentScreen_SpellCardSelect);

            return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
            g_SoundPlayer.ProcessQueues();

            this->currentScreenState = 3;
            this->stateTimer = 0;

            g_GameManager.currentStage = this->cursor;
            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 6);
            break;
        }
        break;
    case 3:
        if (this->stateTimer >= 20)
        {
            oldScreen = this->currentScreen;
            this->ChangeCurrentScreen(TitleCurrentScreen_StartMenu);
            this->cursor = 2;
            g_GameManager.flags.isPracticeMode = FALSE;
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        break;
    }

    this->idleFrames++;
    this->stateTimer++;
    this->stateTimer2++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(spellCardNumber, i, oldCursor, spellCardNumber2, i2, oldPageIdx, spellCardNumber3)
ChainCallbackResult TitleScreen::OnUpdateSpellCardSelect()
{
    i32 spellCardNumber;
    i32 spellCardNumber3;
    i32 spellCardNumber2;
    i32 i;
    i32 i2;
    i32 oldCursor;
    i32 oldPageIdx;

    switch (this->currentScreenState)
    {
    case 0:
        if (this->stateTimer2 == 0)
        {
            g_AnmManager->SetInterruptArray(this->vms, this->vmCount, 26);

            this->practiceState = 0;
            this->currentScreenState = 0;
            this->stateTimer = 0;

            g_GameManager.flags.isPracticeMode = TRUE;
            g_GameManager.flags.isSpellPractice = TRUE;

            this->currentNumberOfSpellCards = g_SpellCardCountPerStage[g_GameManager.currentStage];

            this->UnlockLastWordSpellCards();

            this->cursor = 0;
            for (i = 0; i < this->currentNumberOfSpellCards; i++)
            {
                if (g_SpellCardNumbersPerStage[g_GameManager.currentStage][i] == g_GameManager.currentSpellCardNumber)
                {
                    this->cursor = i;
                    break;
                }
            }

            this->currentPageSpellCardSelect = this->cursor / TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;

            for (i = 0; i < TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE; i++)
            {
                if ((i + this->currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE) >= this->currentNumberOfSpellCards)
                { 
                    break;
                }

                spellCardNumber = g_SpellCardNumbersPerStage[g_GameManager.currentStage][i + this->currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE];

                this->resultTextAnm->InitializeAndSetSprite(&this->spellCardNameVms[i], i + 2);
                this->spellCardNameVms[i].pos = D3DXVECTOR3(0, 0, 0);
                this->spellCardNameVms[i].prefix.anchor = 3;
                /* Copy paste mistake? */
                this->spellCardNameVms[0].fontWidth = 15;
                this->spellCardNameVms[i].fontHeight = 15;

                if (g_GameManager.catkData[spellCardNumber].inGameHistory.attempts[SHOT_ALL] == 0
                    && g_GameManager.catkData[spellCardNumber].spellPracticeHistory.attempts[SHOT_ALL] == 0)
                {
                    if (SpellCard::GetDifficultyFromSpellCard(spellCardNumber) <= EXTRA
                        || !g_GameManager.IsLastWordSpellCardAttempted(spellCardNumber))
                    {
                        g_AnmManager->DrawTextLeft(&this->spellCardNameVms[i],
                                                   0xffffff,
                                                   0,
                                                   TH_TITLE_SPELLCARD_NOT_UNLOCKED);
                    }
                    else
                    {
                        g_AnmManager->DrawTextLeft(&this->spellCardNameVms[i],
                                                   0xffffff,
                                                   0,
                                                   TH_TITLE_SPELLCARD_AVAILABLE);
                    }
                }
                else
                {
                    g_AnmManager->DrawTextLeft(&this->spellCardNameVms[i],
                                               0xffffff,
                                               0,
                                               g_GameManager.catkData[spellCardNumber].spellName);
                }

                this->spellCardNameVms[i].prefix.color1.a = 255;
                this->spellCardNameVms[i].prefix.color1.r = 96;
                this->spellCardNameVms[i].prefix.color1.g = 96;
                this->spellCardNameVms[i].prefix.color1.b = 96;
            }

            i = TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;
            this->resultTextAnm->InitializeAndSetSprite(&this->spellCardNameVms[i], i + 2);
            this->spellCardNameVms[i].pos = D3DXVECTOR3(0, 0, 0);
            this->spellCardNameVms[i].prefix.anchor = 3;
            this->spellCardNameVms[i].fontWidth = 15;
            this->spellCardNameVms[i].fontHeight = 15;

            g_AnmManager->DrawTextLeft(&this->spellCardNameVms[i],
                                               0xffffff,
                                               0,
                                               TH_TITLE_SPELL_CARD_INFO);

            this->spellCardNameVms[i].prefix.color1.a = 255;
            this->spellCardNameVms[i].prefix.color1.r = 255;
            this->spellCardNameVms[i].prefix.color1.g = 255;
            this->spellCardNameVms[i].prefix.color1.b = 255;

            i = this->cursor - (this->currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE);
            this->spellCardNameVms[i].prefix.color1.r = 255;
            this->spellCardNameVms[i].prefix.color1.g = 255;
            this->spellCardNameVms[i].prefix.color1.b = 255;

            for (i = 0; i < 7; i++)
            {
                g_Supervisor.textAnm->InitializeAndSetSprite(&this->spellCardInfoVms[i], i + 21);

                if (i < 4)
                {
                    this->spellCardInfoVms[i].pos = D3DXVECTOR3(64.0f, (i * 16) + 344.0f, 0.0f);
                }
                else
                {
                    this->spellCardInfoVms[i].pos = D3DXVECTOR3(64.0f, (i * 16) + 344.0f + 8.0f, 0.0f);
                }

                this->spellCardInfoVms[i].prefix.anchor = 3;
                this->spellCardInfoVms[i].fontWidth = 15;
                this->spellCardInfoVms[i].fontHeight = 15;
                this->spellCardInfoVms[i].prefix.color1.d3dColor = 0xffffffff;
            }

            this->FormatSpellCardInfo();
            this->unk0xc29c = 0;
        }

        if (this->stateTimer2 == 8)
        {
            this->currentScreenState = 1;
        }
        break;
    case 1:
        oldPageIdx = this->currentPageSpellCardSelect;
        oldCursor = this->cursor;

        if (this->currentNumberOfSpellCards > TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE)
        {
            if (WAS_PRESSED_SCROLLING(TH_BUTTON_LEFT))
            {
                this->cursor -= TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;

                g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
                g_SoundPlayer.ProcessQueues();

                if (this->cursor < 0)
                {
                    this->cursor = this->currentNumberOfSpellCards - 1;
                }
                if (this->currentNumberOfSpellCards >= this->cursor)
                {
                    this->cursor = 0;
                }
            }
            if (WAS_PRESSED_SCROLLING(TH_BUTTON_RIGHT))
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);

                if (this->currentNumberOfSpellCards - this->cursor <= this->currentNumberOfSpellCards % TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE)
                {
                    this->cursor %= TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;
                }
                else
                {
                    this->cursor += TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;
                    if (this->cursor < 0)
                    {
                        this->cursor = currentNumberOfSpellCards - 1;
                    }
                    if (this->currentNumberOfSpellCards >= this->cursor)
                    {
                        this->cursor = currentNumberOfSpellCards - 1;
                    }
                }
            }
        }

        this->MoveCursorVertical(this->currentNumberOfSpellCards);
        this->currentPageSpellCardSelect = this->cursor / TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;

        if (oldPageIdx != this->currentPageSpellCardSelect)
        {
            for (i2 = 0; i2 < TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE; i2++)
            {
                if ((i2 + this->currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE) >= this->currentNumberOfSpellCards)
                { 
                    break;
                }

                this->resultTextAnm->InitializeAndSetSprite(&this->spellCardNameVms[i2], i + 2);
                this->spellCardNameVms[i2].pos = D3DXVECTOR3(0, 0, 0);
                this->spellCardNameVms[i2].prefix.anchor = 3;
                /* Similar copy paste mistake as before? */
                this->spellCardInfoVms[0].fontWidth = 15;
                this->spellCardNameVms[i2].fontHeight = 15;

                spellCardNumber2 = g_SpellCardNumbersPerStage[g_GameManager.currentStage][i2 + this->currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE];

                /* Why does ZUN use this helper method here, and in the initialization , use direct access? */
                if (g_GameManager.HasSpellCardBeenEncountered(spellCardNumber2, SHOT_ALL))
                {
                    g_AnmManager->DrawTextLeft(&this->spellCardNameVms[i2],
                                               0xffffff,
                                               0,
                                               g_GameManager.catkData[spellCardNumber2].spellName);
                }
                else
                {
                    if (SpellCard::GetDifficultyFromSpellCard(spellCardNumber2) <= EXTRA
                        || !g_GameManager.IsLastWordSpellCardAttempted(spellCardNumber2))
                    {
                        g_AnmManager->DrawTextLeft(&this->spellCardNameVms[i2],
                                                   0xffffff,
                                                   0,
                                                   TH_TITLE_SPELLCARD_NOT_UNLOCKED);
                    }
                    else
                    {
                        g_AnmManager->DrawTextLeft(&this->spellCardNameVms[i2],
                                                   0xffffff,
                                                   0,
                                                   TH_TITLE_SPELLCARD_AVAILABLE);
                    }
                }

                this->spellCardNameVms[i2].prefix.color1.a = 255;
                this->spellCardNameVms[i2].prefix.color1.r = 96;
                this->spellCardNameVms[i2].prefix.color1.g = 96;
                this->spellCardNameVms[i2].prefix.color1.b = 96;
            }
        }

        if (oldCursor != this->cursor)
        {
            for (i2 = 0; i2 < TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE; i2++)
            {
                this->spellCardNameVms[i2].prefix.color1.r = 96;
                this->spellCardNameVms[i2].prefix.color1.g = 96;
                this->spellCardNameVms[i2].prefix.color1.b = 96;
            }

            i2 = this->cursor - (this->currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE);
            this->spellCardNameVms[i2].prefix.color1.r = 255;
            this->spellCardNameVms[i2].prefix.color1.g = 255;
            this->spellCardNameVms[i2].prefix.color1.b = 255;

            for (i2 = 0; i2 < 7; i2++)
            {
                this->spellCardInfoVms[i2].prefix.color1.a = 0;
            }

            this->unk0xc29c = 21;
        }

        this->FormatSpellCardInfo();

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            spellCardNumber3 = g_SpellCardNumbersPerStage[g_GameManager.currentStage][this->cursor];
            if (g_GameManager.catkData[spellCardNumber3].inGameHistory.attempts[SHOT_ALL] != 0
                || g_GameManager.catkData[spellCardNumber3].spellPracticeHistory.attempts[SHOT_ALL] != 0
                || (spellCardNumber3 >= SPELL_CARD_LAST_WORD_START
                    && g_GameManager.IsLastWordSpellCardAttempted(spellCardNumber3)))
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);

                g_GameManager.flags.isSpellPractice = TRUE;
                g_GameManager.currentSpellCardNumber = g_SpellCardNumbersPerStage[g_GameManager.currentStage][this->cursor];

                g_GameManager.difficulty = SpellCard::GetDifficultyFromSpellCard(g_GameManager.currentSpellCardNumber);

                if (g_GameManager.difficulty > EXTRA)
                {
                    /* Set the correct difficulty for each Last Word spell card. */
                    switch (g_GameManager.currentSpellCardNumber)
                    {
                    /* TODO: do not hardcode these values */
                    case 205:   /* Wriggle Nightbug */
                        g_GameManager.currentStage = STAGE1;
                        break;
                    case 206:   /* Mystia Lorelei */
                        g_GameManager.currentStage = STAGE2;
                        break;
                    case 207:   /* Keine Kamishirasawa */
                        g_GameManager.currentStage = STAGE3;
                        break;
                    case 208:   /* Reisen U. Inaba */
                        g_GameManager.currentStage = STAGE5;
                        break;
                    case 209:   /* Eirin Yagokoro */
                        g_GameManager.currentStage = STAGE6A;
                        break;
                    case 210:   /* Kaguya Houraisan */
                        g_GameManager.currentStage = STAGE6B;
                        break;
                    case 211:   /* Fujiwara no Mokou */
                        g_GameManager.currentStage = EXTRASTAGE;
                        break;
                    case 212:   /* Tewi Inaba */
                        g_GameManager.currentStage = STAGE5;
                        break;
                    case 213:   /* Keine Kamishirasawa */
                        g_GameManager.currentStage = EXTRASTAGE;
                        break;
                    case 214:   /* Reimu Hakurei */
                        g_GameManager.currentStage = STAGE4A;
                        break;
                    case 215:   /* Marisa Kirisame */
                        g_GameManager.currentStage = STAGE4B;
                        break;
                    default:    /* ... everyone else */
                        g_GameManager.currentStage = STAGE4A;
                        break;
                    }

                    g_GameManager.difficulty = NORMAL;
                }

                g_Supervisor.curState = SupervisorState_GameManager;
                g_GameManager.SetIsReplayWeird(FALSE);

                g_Supervisor.StopAudio();

                return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
            }
            else
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_INVALID_ACTION, 0);
            }
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            g_GameManager.currentSpellCardNumber = g_SpellCardNumbersPerStage[g_GameManager.currentStage][this->cursor];

            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK, 0);
            this->cursor = g_GameManager.currentStage;

            this->ChangeCurrentScreen(TitleCurrentScreen_SpellStageSelect);

            return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
        }

        break;
    }

    this->idleFrames++;
    this->stateTimer++;
    this->stateTimer2++;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

/* This function checks the conditions needed to unlock certain Last Word spell cards. */
// STUB: th08 0x46cbbb
void TitleScreen::UnlockLastWordSpellCards()
{
}

// STUB: th08 0x46ef8d
ChainCallbackResult TitleScreen::DrawReplayMenu()
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(vm, i, clearInfo, position)
ChainCallbackResult TitleScreen::DrawPracticeStageSelect()
{
    D3DXVECTOR3 position;
    u16 clearInfo;
    i32 i;
    AnmVm *vm;

    g_AsciiManager.SetColor(0xffffffff);
    g_AsciiManager.SetIsSelected(FALSE);

    vm = &this->vms[141];
    position = vm->pos;
    position.y -= 96.0f;

    g_AsciiManager.AddFormatText(&position, "Stage    HI-Score");

    position.y += 16.0f;

    clearInfo = g_GameManager.clrdData[g_GameManager.shotType].difficultiesClearedWithRetries[g_Supervisor.cfg.defaultDifficulty];
    if (clearInfo == 0)
    {
        clearInfo = 1;
    }

    if (IS_STAGE_CLEARED(clearInfo, STAGE6B))
    {
        clearInfo |= ZUN_BIT(STAGE4A) | ZUN_BIT(STAGE4B);
    }

    for (i = 0; i < ARRAY_SIZE(g_StageNames) - 1; i++)
    {
        g_AsciiManager.SetIsSelected(i == this->cursor);

        if (i == this->cursor)
        {
            g_AsciiManager.SetColor(0xffffffff);
        }
        else if (IS_STAGE_CLEARED(clearInfo, i))
        {
            g_AsciiManager.SetColor(0xffa0a0a0);
        }
        else
        {
            g_AsciiManager.SetColor(0xff404040);
        }

        g_AsciiManager.AddFormatText(&position, "%s %9d0 (%3d)", g_StageNames[i],
                                     g_GameManager.pscrData[g_GameManager.shotType].highScores[i][g_Supervisor.cfg.defaultDifficulty],
                                     g_GameManager.pscrData[g_GameManager.shotType].attempts[i][g_Supervisor.cfg.defaultDifficulty]);
        position.y += 16.0f;
    }

    g_AsciiManager.SetColor(0xffffffff);
    g_AsciiManager.SetIsSelected(FALSE);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(capturesPerStageSpellPractice, vm, spellCardNumber, i, attemptedLastWordPerShot, totalAttemptedLastWordPerShot, \
                  totalCapturesSpellPracticePerShot, capturesPerStageSpellPracticePerShot, \
                  capturesPerStageInGamePerShot, totalCapturesInGamePerShot, totalAttemptedLastWord, \
                  totalAttemptedLastWord, totalCapturesSpellPractice, attemptedLastWord, spellCardIdx, \
                  totalCapturesInGame, position, capturesPerStageInGame, percentageCapturedInGame, \
                  percentageCapturedSpellPractice, pieChartPosition)
ChainCallbackResult TitleScreen::DrawSpellStageSelect()
{
    i32 totalCapturesSpellPracticePerShot;
    i32 totalCapturesSpellPractice;
    i32 totalCapturesInGamePerShot;
    i32 totalCapturesInGame;
    i32 totalAttemptedLastWordPerShot;
    i32 totalAttemptedLastWord;
    i32 capturesPerStageInGamePerShot;
    i32 capturesPerStageInGame;
    i32 capturesPerStageSpellPractice;
    i32 capturesPerStageSpellPracticePerShot;
    i32 attemptedLastWord;
    i32 attemptedLastWordPerShot;
    float percentageCapturedSpellPractice;
    float percentageCapturedInGame;
    AnmVm *vm;
    i32 i;
    i32 spellCardIdx;
    i32 spellCardNumber;
    D3DXVECTOR3 position;
    D3DXVECTOR3 pieChartPosition;

    totalCapturesSpellPracticePerShot = 0;
    totalCapturesSpellPractice = 0;
    totalCapturesInGamePerShot = 0;
    totalCapturesInGame = 0;
    totalAttemptedLastWordPerShot = 0;
    totalAttemptedLastWord = 0;

    g_AsciiManager.SetColor(0xffffffff);
    g_AsciiManager.SetIsSelected(FALSE);

    vm = &this->vms[141];
    position = vm->pos;
    position.y -= 152.0f;
    position.x -= 32.0f;

    this->spellCardNameVms[0].pos = position;
    g_AnmManager->DrawNoRotation(&this->spellCardNameVms[0]);

    position.y += 16.0f;

    for (i = 0; i < ARRAY_SIZE(g_StageNamesSpellPractice); i++)
    {
        g_AsciiManager.SetIsSelected(i == this->cursor);
        if (i == this->cursor)
        {
            g_AsciiManager.SetColor(0xffffffff);
        }
        else
        {
            g_AsciiManager.SetColor(0xffa0a0a0);
        }

        capturesPerStageSpellPracticePerShot = 0;
        capturesPerStageInGamePerShot = 0;
        capturesPerStageInGame = 0;
        attemptedLastWordPerShot = 0;
        capturesPerStageSpellPractice = 0;
        attemptedLastWord = 0;

        for (spellCardIdx = 0; spellCardIdx < g_SpellCardCountPerStage[i]; spellCardIdx++)
        {
            spellCardNumber = g_SpellCardNumbersPerStage[i][spellCardIdx];

            if (g_GameManager.catkData[spellCardNumber].spellPracticeHistory.captures[g_GameManager.shotType] > 0)
            {
                capturesPerStageSpellPracticePerShot++;
            }
            if (g_GameManager.catkData[spellCardNumber].inGameHistory.captures[g_GameManager.shotType] > 0)
            {
                capturesPerStageInGamePerShot++;
            }
            if ((g_GameManager.catkData[spellCardNumber].inGameHistory.attempts[g_GameManager.shotType] > 0
                || g_GameManager.catkData[spellCardNumber].spellPracticeHistory.attempts[g_GameManager.shotType] > 0)
                || (i >= ARRAY_SIZE(g_SpellCardCountPerStage) - 1 && g_GameManager.IsLastWordSpellCardAttempted(spellCardNumber)))
            {
                attemptedLastWordPerShot++;
            }

            if (g_GameManager.catkData[spellCardNumber].spellPracticeHistory.captures[SHOT_ALL] > 0)
            {
                capturesPerStageSpellPractice++;
            }
            if (g_GameManager.catkData[spellCardNumber].inGameHistory.captures[SHOT_ALL] > 0)
            {
                capturesPerStageInGame++;
            }
            if ((g_GameManager.catkData[spellCardNumber].inGameHistory.attempts[SHOT_ALL] > 0
                || g_GameManager.catkData[spellCardNumber].spellPracticeHistory.attempts[SHOT_ALL] > 0)
                || (i >= ARRAY_SIZE(g_SpellCardCountPerStage) - 1 && g_GameManager.IsLastWordSpellCardAttempted(spellCardNumber)))
            {
                attemptedLastWord++;
            }
        }

        totalCapturesSpellPracticePerShot += capturesPerStageSpellPracticePerShot;
        totalCapturesSpellPractice += capturesPerStageSpellPractice;
        totalCapturesInGamePerShot += capturesPerStageInGamePerShot;
        totalCapturesInGame += capturesPerStageInGame;
        totalAttemptedLastWordPerShot += attemptedLastWordPerShot;
        totalAttemptedLastWord += attemptedLastWord;

        g_AsciiManager.AddFormatText(&position, "%s%s", (capturesPerStageSpellPracticePerShot >= g_SpellCardCountPerStage[i]
                                                        ? "@"
                                                        : (capturesPerStageSpellPractice >= g_SpellCardCountPerStage[i] ? "*" : " ")), g_StageNamesSpellPractice[i]);
        position.x += 182.0f;

        g_AsciiManager.SetScale(0.75f, 1.0f);

        g_AsciiManager.AddFormatText(&position,
                                     "%3d(%3d)/%3d/%3d",
                                     capturesPerStageSpellPracticePerShot,
                                     capturesPerStageSpellPractice,
                                     attemptedLastWord,
                                     g_SpellCardCountPerStage[i]);

        position.x -= 182.0f;
        position.y += 16.0f;

        g_AsciiManager.SetScale(1.0f, 1.0f);
    }

    position.y += 5.0f;

    g_AsciiManager.SetIsSelected(FALSE);
    g_AsciiManager.SetColor(0xffd06060);

    g_AsciiManager.AddFormatText(&position, "%sTotal", (totalCapturesSpellPracticePerShot >= SPELL_CARD_COUNT_SPELLCARDS
                                                    ? "@"
                                                    : (totalCapturesSpellPractice >= SPELL_CARD_COUNT_SPELLCARDS ? "*" : " ")));

    position.x += 182.0f;

    g_AsciiManager.SetScale(0.75f, 1.0f);

    g_AsciiManager.AddFormatText(&position,
                                 "%3d(%3d)/%3d/%3d",
                                 totalCapturesSpellPracticePerShot,
                                 totalCapturesSpellPractice,
                                 totalAttemptedLastWord,
                                 SPELL_CARD_COUNT_SPELLCARDS);

    position.x -= 182.0f;

    g_AsciiManager.SetScale(1.0f, 1.0f);
    g_AsciiManager.SetColor(0xffffffff);
    g_AsciiManager.SetIsSelected(FALSE);

    this->spellCardNameVms[1].pos.x = 400.0f;
    this->spellCardNameVms[1].pos.y = 376.0f;
    this->spellCardNameVms[1].pos.z = 0.0f;

    g_AnmManager->DrawNoRotation(&this->spellCardNameVms[1]);

    this->spellCardNameVms[2].pos.x = 552.0f;
    this->spellCardNameVms[2].pos.y = 426.0f;
    this->spellCardNameVms[2].pos.z = 0.0f;

    g_AnmManager->DrawNoRotation(&this->spellCardNameVms[2]);

    /* Draw per shot type statistics. */

    pieChartPosition.x = 530.0f;
    pieChartPosition.y = 420.0f;
    percentageCapturedSpellPractice = (float) totalCapturesSpellPracticePerShot / SPELL_CARD_COUNT_SPELLCARDS;
    percentageCapturedInGame = (float) totalCapturesInGamePerShot / SPELL_CARD_COUNT_IN_GAME_SPELLCARDS;

    if (this->stateTimer2 < 50)
    {
        if (this->percentageCapturedSpellPracticePerShot != percentageCapturedSpellPractice)
        {
            percentageCapturedSpellPractice = ((percentageCapturedSpellPractice - this->percentageCapturedSpellPracticePerShot) * this->stateTimer2) / 50.0f
                                               + this->percentageCapturedSpellPracticePerShot;
        }
        if (this->percentageCapturedInGamePerShot != percentageCapturedInGame)
        {
            percentageCapturedInGame = ((percentageCapturedInGame - this->percentageCapturedInGamePerShot) * this->stateTimer2) / 50.0f
                                               + this->percentageCapturedInGamePerShot;
        }
    }

    this->percentageCapturedSpellPracticePerShot = percentageCapturedSpellPractice;
    this->percentageCapturedInGamePerShot = percentageCapturedInGame;

    pieChartPosition.z = 0.02f;
    DrawPieChart(&pieChartPosition, 0x60c0c0f0, percentageCapturedSpellPractice, 48.0f);
    pieChartPosition.z = 0.01f;
    DrawPieChart(&pieChartPosition, 0x80c0c0f0, percentageCapturedInGame, 24.0f);

    pieChartPosition.x = 514.0f;
    pieChartPosition.y = 404.0f;

    g_AsciiManager.DrawPercentage(&pieChartPosition, percentageCapturedSpellPractice * 10000, 0xffffffff);
    pieChartPosition.y += 8.0f;
    g_AsciiManager.DrawPercentage(&pieChartPosition, percentageCapturedInGame * 10000, 0x80c0c080);

    /* Draw statistics for all shot types. */

    pieChartPosition.x = 530.0f;
    pieChartPosition.y = 420.0f;

    /* ??? */
    pieChartPosition.x += 32.0f;
    pieChartPosition.y += 18.0f;

    percentageCapturedSpellPractice = (float) totalCapturesSpellPractice / SPELL_CARD_COUNT_SPELLCARDS;
    percentageCapturedInGame = (float) totalCapturesInGame / SPELL_CARD_COUNT_IN_GAME_SPELLCARDS;

    if (this->stateTimer2 < 50)
    {
        if (this->percentageCapturedSpellPractice != percentageCapturedSpellPractice)
        {
            percentageCapturedSpellPractice = ((percentageCapturedSpellPractice - this->percentageCapturedSpellPractice) * this->stateTimer2) / 50.0f
                                               + this->percentageCapturedSpellPractice;
        }
        if (this->percentageCapturedInGame != percentageCapturedInGame)
        {
            percentageCapturedInGame = ((percentageCapturedInGame - this->percentageCapturedInGame) * this->stateTimer2) / 50.0f
                                               + this->percentageCapturedInGame;
        }
    }

    this->percentageCapturedSpellPractice = percentageCapturedSpellPractice;
    this->percentageCapturedInGame = percentageCapturedInGame;

    pieChartPosition.z = 0.02f;
    DrawPieChart(&pieChartPosition, 0x6080c0c0, percentageCapturedSpellPractice, 32.0f);
    pieChartPosition.z = 0.01f;
    DrawPieChart(&pieChartPosition, 0x8080c0c0, percentageCapturedInGame, 16.0f);

    pieChartPosition.x = 580.0f;
    pieChartPosition.y = 451.0f;

    g_AsciiManager.DrawPercentage(&pieChartPosition, percentageCapturedSpellPractice * 10000, 0xffffffff);
    pieChartPosition.y -= 8.0f;
    g_AsciiManager.DrawPercentage(&pieChartPosition, percentageCapturedInGame * 10000, 0x80c0c080);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

/* Is matching except for missing stack space. */
#pragma var_order(center, vm, vertices, i, angle)
void DrawPieChart(D3DXVECTOR3 *position, D3DCOLOR color, float param_3, float param_4)
{
    VertexDiffuseXyzrhw vertices[64];
    D3DXVECTOR3 center;
    AnmVm vm;
    float angle;
    i32 i;

    vm.prefix.blendMode = AnmBlendMode_Normal;
    vm.prefix.color1.d3dColor = 0xffffffff;
    vm.prefix.zWriteDisabled = TRUE;
    vm.prefix.flag15 = FALSE;

    vertices[0].diffuse = color;
    vertices[0].pos = *position;
    vertices[1].w = 1.0f;
    angle = -(ZUN_PI / 2.0f);

    center.x = param_4 / 2.0f;
    center.y = 0.0f;

    for (i = 1; i < ARRAY_SIZE_SIGNED(vertices); i++)
    {
        Rotate(&vertices[i].pos, &center, angle);

        vertices[i].pos.x += vertices[0].pos.x;
        vertices[i].pos.y += vertices[0].pos.y;
        vertices[i].pos.z = vertices[0].pos.z;
        vertices[i].diffuse = color;
        vertices[i].w = 1.0f;

        angle = AddNormalizeAngle(angle, (ZUN_PI / 31.0f) * param_3);
    }

    g_AnmManager->DrawTriangleFan(&vm, vertices, ARRAY_SIZE(vertices));
}


#pragma var_order(spellCardNumber, i, clearInfo, position)
ChainCallbackResult TitleScreen::DrawSpellCardSelect()
{
    i32 i;
    i32 spellCardNumber;
    u16 clearInfo;
    D3DXVECTOR3 position;

    g_AsciiManager.SetColor(0xffffffff);
    g_AsciiManager.SetIsSelected(FALSE);

    position = D3DXVECTOR3(16.0f, 78.0f, 0.0f);

    clearInfo = g_GameManager.clrdData[g_GameManager.shotType].difficultiesClearedWithRetries[g_Supervisor.cfg.defaultDifficulty];
    if (clearInfo == 0)
    {
        clearInfo = 1;
    }

    position.x += 318.0f;
    this->spellCardNameVms[TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE].pos = position;
    g_AnmManager->DrawNoRotation(&this->spellCardNameVms[TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE]);
    position.x -= 318.0f;
    position.y += 16.0f;

    for (i = 0; i < TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE; i++)
    {
        if ((i + this->currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE) >= this->currentNumberOfSpellCards)
        { 
            break;
        }

        g_AsciiManager.SetColor(this->spellCardNameVms[i].prefix.color1.d3dColor);

        spellCardNumber = g_SpellCardNumbersPerStage[g_GameManager.currentStage][i + this->currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE];

        g_AsciiManager.AddFormatText(&position, "%sNo.%.3d",
                                                g_GameManager.catkData[spellCardNumber].spellPracticeHistory.captures[g_GameManager.shotType] > 0
                                                ? "@"
                                                : (g_GameManager.catkData[spellCardNumber].spellPracticeHistory.captures[SHOT_ALL] > 0 ? "*" : " "), spellCardNumber + 1);

        position.x += 414.0f;

        g_AsciiManager.SetScale(0.8, 1.0f);

        g_AsciiManager.AddFormatText(&position, "%3d/%3d(%3d/%3d)", g_GameManager.catkData[spellCardNumber].spellPracticeHistory.captures[g_GameManager.shotType],
                                                                    g_GameManager.catkData[spellCardNumber].spellPracticeHistory.attempts[g_GameManager.shotType],
                                                                    g_GameManager.catkData[spellCardNumber].inGameHistory.captures[g_GameManager.shotType],
                                                                    g_GameManager.catkData[spellCardNumber].inGameHistory.attempts[g_GameManager.shotType]);

        g_AsciiManager.SetScale(1.0, 1.0f);

        position.x -= 414.0f;
        this->spellCardNameVms[i].pos = position;
        this->spellCardNameVms[i].pos.x += 102.0f;

        g_AnmManager->DrawNoRotation(&this->spellCardNameVms[i]);

        position.y += 16.0f;
    }

    g_AnmManager->DrawNoRotation(&this->spellCardInfoVms[0]);
    g_AnmManager->DrawNoRotation(&this->spellCardInfoVms[1]);
    g_AnmManager->DrawNoRotation(&this->spellCardInfoVms[2]);
    g_AnmManager->DrawNoRotation(&this->spellCardInfoVms[3]);
    g_AnmManager->DrawNoRotation(&this->spellCardInfoVms[4]);
    g_AnmManager->DrawNoRotation(&this->spellCardInfoVms[5]);
    g_AnmManager->DrawNoRotation(&this->spellCardInfoVms[6]);

    g_AsciiManager.SetColor(0xffffffff);
    g_AsciiManager.SetIsSelected(FALSE);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleScreen::MoveCursorVertical(i32 menuLength)
{
    if (menuLength == 0)
    {
        return 0;
    }

    if (WAS_PRESSED_SCROLLING(TH_BUTTON_UP))
    {
        this->cursor--;
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
        if (this->cursor < 0)
        {
            this->cursor = menuLength - 1;
        }
        if (this->cursor >= menuLength)
        {
            this->cursor = 0;
        }

        return -1;
    }
    
    if (WAS_PRESSED_SCROLLING(TH_BUTTON_DOWN))
    {
        this->cursor++;
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
        if (this->cursor < 0)
        {
            this->cursor = menuLength - 1;
        }
        if (this->cursor >= menuLength)
        {
            this->cursor = 0;
        }

        return 1;
    }

    return 0;
}

/* Why is this function slightly better written than MoveCursorVertical? */
i32 TitleScreen::MoveCursorHorizontal(i32 menuLength)
{
    if (menuLength == 0)
    {
        return 0;
    }

    if (WAS_PRESSED_SCROLLING(TH_BUTTON_LEFT))
    {
        this->cursor--;

        if (this->cursor < 0)
        {
            this->cursor = this->cursor + menuLength;
        }

        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
        return -1;
    }

    if (WAS_PRESSED_SCROLLING(TH_BUTTON_RIGHT))
    {
        this->cursor++;

        if (this->cursor >= menuLength)
        {
            this->cursor = this->cursor - menuLength;
        }

        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU, 0);
        return 1;
    }

    return 0;
}

// STUB: th08 0x46d7f9
void TitleScreen::FormatSpellCardInfo()
{
}

// STUB: th08 0x46e136
ChainCallbackResult TitleScreen::OnUpdateReplayMenu()
{
     return CHAIN_CALLBACK_RESULT_CONTINUE;
}

/* This is matching functionally but there is some unexplained stack nonsense
 * from AnmLoaded::InitializeAndSetSprite.
 */
ChainCallbackResult TitleScreen::DrawCompletionStatusText()
{
    ZunBool showVm = FALSE;

    if (this->stateTimer2 > 8)
    {
        if (g_GameManager.IsStageClearedWithoutRetries(STAGE6B, this->cursor, g_Supervisor.cfg.defaultDifficulty)
            && g_GameManager.IsStageClearedWithRetries(STAGE6A, this->cursor, g_Supervisor.cfg.defaultDifficulty))
        {
            showVm = TRUE;
            this->titleAnm->InitializeAndSetSprite(&this->spellCardNameVms[0], 146);
        }
        else if (g_GameManager.IsStageClearedWithoutRetries(STAGE6B, this->cursor, g_Supervisor.cfg.defaultDifficulty))
        {
            showVm = TRUE;
            this->titleAnm->InitializeAndSetSprite(&this->spellCardNameVms[0], 148);
        }
        else if (g_GameManager.IsStageClearedWithoutRetries(STAGE6B, this->cursor, EASY)
                || g_GameManager.IsStageClearedWithoutRetries(STAGE6B, this->cursor, NORMAL)
                || g_GameManager.IsStageClearedWithoutRetries(STAGE6B, this->cursor, HARD)
                || g_GameManager.IsStageClearedWithoutRetries(STAGE6B, this->cursor, LUNATIC)
                && this->cursor > 3)
        {
            showVm = TRUE;
            this->titleAnm->InitializeAndSetSprite(&this->spellCardNameVms[0], 147);
        }
        else if (g_GameManager.IsStageClearedWithRetries(STAGE6A, this->cursor, EASY)
                || g_GameManager.IsStageClearedWithRetries(STAGE6A, this->cursor, NORMAL)
                || g_GameManager.IsStageClearedWithRetries(STAGE6A, this->cursor, HARD)
                || g_GameManager.IsStageClearedWithRetries(STAGE6A, this->cursor, LUNATIC))
        {
            showVm = TRUE;
            this->titleAnm->InitializeAndSetSprite(&this->spellCardNameVms[0], 145);
        }
    }

    if (showVm)
    {
        this->spellCardNameVms[0].prefix.anchor = 3;

        this->spellCardNameVms[0].prefix.color1.a = 255;
        this->spellCardNameVms[0].prefix.color1.r = 255;
        this->spellCardNameVms[0].prefix.color1.g = 255;
        this->spellCardNameVms[0].prefix.color1.b = 255;

        this->spellCardNameVms[0].pos.x = 400.0f;
        this->spellCardNameVms[0].pos.y = 170.0f;
        this->spellCardNameVms[0].pos.z = 0.0f;

        g_AnmManager->DrawNoRotation(&this->spellCardNameVms[0]);
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
}

#pragma var_order(i, vm, position)
ChainCallbackResult TitleScreen::OnDraw(TitleScreen *titleScreen)
{
    D3DXVECTOR3 position;
    AnmVm *vm;
    int i;

    if (titleScreen->state != TitleScreenState_Ready)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

    g_AnmManager->ClearTexture();
    g_AnmManager->CopySurfaceToBackbuffer(0, 0, 0, 0, 0);

    for (vm = titleScreen->vms, i = 0; i < titleScreen->vmCount; i++, vm++)
    {
        if (g_AnmManager->SpriteHasTexture(vm))
        {
            position = vm->pos;
            vm->pos += vm->pos2;

            if (vm->prefix.rotation.z != 0.0f)
            {
                g_AnmManager->Draw2D(vm);
            }
            else
            {
                g_AnmManager->DrawNoRotation(vm);
            }

            vm->pos = position;
        }
    }

    if (titleScreen->currentHelpTextVm != NULL)
    {
        g_AnmManager->DrawNoRotation(titleScreen->currentHelpTextVm);
    }

    switch (titleScreen->currentScreen)
    {
    case TitleCurrentScreen_CharacterSelect:
        titleScreen->DrawCompletionStatusText();
        break;
    case TitleCurrentScreen_Replay:
        titleScreen->DrawReplayMenu();
        break;
    case TitleCurrentScreen_PracticeStageSelect:
        titleScreen->DrawPracticeStageSelect();
        break;
    case TitleCurrentScreen_SpellStageSelect:
        titleScreen->DrawSpellStageSelect();
        break;
    case TitleCurrentScreen_SpellCardSelect:
        titleScreen->DrawSpellCardSelect();
        break;
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult TitleScreen::ActualAddedCallback()
{
    ScoreDat *score;

    g_ScreenEffectCounter = 0;
    g_AsciiManager.Reset();
    g_AsciiManager.InitializeVms();

    if (g_GameManager.cfg != NULL)
    {
        delete g_GameManager.cfg;
        g_GameManager.cfg = NULL;
    }
    g_GameManager.cfg = new GameConfiguration();

    if (g_GameManager.globals != NULL)
    {
        delete g_GameManager.globals;
        g_GameManager.globals = NULL;
    }

    g_GameManager.globals = new ZunGlobals();
    g_Supervisor.framerateMultiplier = 1.0f;

    if (g_GameManager.IsReplay())
    {
        /* This seems to be a leftover from PCB where there were separate
         * characters and shot types.
         */
        g_GameManager.shotType = g_GameManager.shotType = 0;
    }

    if (g_GameManager.IsDemoMode())
    {
        g_GameManager.flags.isReplay = FALSE;
    }

    if (g_GameManager.IsExtraUnlocked() && !g_GameManager.flags.isExtraUnlocked)
    {
        this->DisplayInfoImage("title/info00.jpg");
    }
    if (g_GameManager.IsSpellPracticeUnlocked() && !g_GameManager.flags.isSpellPracticeUnlocked)
    {
        this->DisplayInfoImage("title/info02.jpg");
    }
    if (g_GameManager.IsExtraUnlockedWithAllTeams() && !g_GameManager.flags.isExtraUnlockedWithAllTeams)
    {
        this->DisplayInfoImage("title/info01.jpg");
    }

    score = ScoreDat::OpenScore("score.dat");

    ScoreDat::ParseCLRD(score, g_GameManager.clrdData);
    ScoreDat::ParsePSCR(score, g_GameManager.pscrData);
    ScoreDat::ParseCATK(score, g_GameManager.catkData);
    ScoreDat::ParseFLSP(score, &g_GameManager.flsp);

    ScoreDat::ReleaseScore(score);

    g_GameManager.flags.isExtraUnlocked = g_GameManager.IsExtraUnlocked();
    g_GameManager.flags.isSpellPracticeUnlocked = g_GameManager.IsSpellPracticeUnlocked();
    g_GameManager.flags.isExtraUnlockedWithAllTeams = g_GameManager.IsExtraUnlockedWithAllTeams();

    this->UnlockLastWordSpellCards();
    this->currentScreen = TitleCurrentScreen_StartMenu;

    g_Supervisor.ResetUnknownStuff();

    switch (g_Supervisor.wantedState2)
    {
    case SupervisorState_GameManager:
    case SupervisorState_GameManagerReInit:
    case SupervisorState_ResultScreenFromGame:
        this->cursor = (g_GameManager.difficulty >= EXTRA) ? 1 : 0;
        break;
    case SupervisorState_ResultScreen:
        this->cursor = 5;
        break;
    case SupervisorState_MusicRoom:
        this->cursor = 6;
        break;
    default:
        this->cursor = 0;
        break;
    }

    this->practiceState = 0;
    if (g_GameManager.IsPracticeMode())
    {
        this->cursor = g_GameManager.flags.isSpellPractice ? 2 : 3;
        this->practiceState = g_GameManager.IsSpellPractice() ? 2 : 1;
    }

    g_GameManager.flags.isPracticeMode = FALSE;
    g_GameManager.flags.isSpellPractice = FALSE;

    D3DXVECTOR3 loadingVmsPosition(500.0f, 440.0f, 0.0f);

    if (g_Supervisor.wantedState2 == SupervisorState_GameManager)
    {
        g_Supervisor.SetupLoadingVmsAndInitCapture(&loadingVmsPosition);
        g_Supervisor.StartEffect(0);
    }
    else if (g_Supervisor.wantedState2 != SupervisorState_Init)
    {
        g_Supervisor.SetupLoadingVms(&loadingVmsPosition);
    }

    g_GameManager.flags.isDemoMode = FALSE;
    g_GameManager.unk3DBB8 = 0;

    this->state = TitleScreenState_Loading;
    g_Supervisor.ThreadStart((LPTHREAD_START_ROUTINE) TitleScreen::TitleSetupThread, NULL);

    return ZUN_SUCCESS;
}

void TitleScreen::TitleSetupThread(TitleScreen *titleScreen)
{
    int i;

    while (g_AnmManager->captureSurfaceIdx >= 0)
    {
        Sleep(1);
    }

    g_TitleScreen->titleAnm = g_AnmManager->PreloadAnm(20, "title01.anm");
    if (g_TitleScreen->titleAnm == NULL)
    {
        g_TitleScreen->state = TitleScreenState_Close;
        return;
    }

    g_TitleScreen->resultTextAnm = g_AnmManager->PreloadAnm(22, "resulttext.anm");
    if (g_TitleScreen->resultTextAnm == NULL)
    {
        g_TitleScreen->state = TitleScreenState_Close;
        return;
    }

    if (g_Supervisor.subthreadCloseRequestActive)
    {
        return;
    }

    for (i = 0; i < ARRAY_SIZE_SIGNED(g_TitleScreen->helpTextVms); i++)
    {
        g_Supervisor.textAnm->SetAndExecuteScriptIdx(&g_TitleScreen->helpTextVms[i], 9);
        g_Supervisor.textAnm->SetSprite(&g_TitleScreen->helpTextVms[i], g_TitleScreen->helpTextVms[i].activeSpriteIndex + i);
        g_Supervisor.textAnm->SetSprite(&g_TitleScreen->spellCardInfoVms[i], i + 21);

        if (i <= 4)
        {
            g_TitleScreen->spellCardInfoVms[i].pos = D3DXVECTOR3(64.0f, (i * 16.0f) + 352.0f, 0.0f);
        }
        else
        {
            g_TitleScreen->spellCardInfoVms[i].pos = D3DXVECTOR3(64.0f, (i * 16.0f) + 352.0f + 10.0f, 0.0f);
        }

        g_TitleScreen->spellCardInfoVms[i].prefix.anchor = 3;
        g_TitleScreen->spellCardInfoVms[i].fontWidth = 15;
        g_TitleScreen->spellCardInfoVms[i].fontHeight = 15;
        g_TitleScreen->spellCardInfoVms[i].prefix.flag1 = TRUE;
    }

    if (g_Supervisor.subthreadCloseRequestActive)
    {
        return;
    }

    if (g_AnmManager->PreloadSurface(0, "title/title00.png") != ZUN_SUCCESS)
    {
        g_TitleScreen->state = TitleScreenState_Close;
        return;
    }

    if (!g_GameManager.IsDemoMode())
    {
        if (g_Supervisor.wantedState2 != SupervisorState_ResultScreen)
        {
            g_Supervisor.LoadMusic(8, "bgm/th08_01.mid");
        }
        if (g_Supervisor.totalPlayTime == 0)
        {
            ScreenEffect::RegisterChain(SCREEN_EFFECT_FULL_FADE_IN, 70, RGB(255, 255, 255), 0, 0, 21);
        }
        else
        {
            ScreenEffect::RegisterChain(SCREEN_EFFECT_FULL_FADE_IN, 70, RGB(255, 255, 255), 0, 0, 21);
        }
    }

    g_TitleScreen->currentHelpTextVm = &g_TitleScreen->helpTextVms[0];
    g_TitleScreen->state = TitleScreenState_Ready;
    g_Supervisor.HideLoadingVms();
    g_Supervisor.runningSubthreadHandle = NULL;
    g_Supervisor.subthreadCloseRequestActive = FALSE;
    g_Supervisor.unk290 = 0;
}

#define TITLE_IMAGE_INFO_MAX_FRAMES     6000
#define TITLE_IMAGE_FADE_IN_TIME        20

#pragma var_order(i, rect1, color1, color2, rect2)
void TitleScreen::DisplayInfoImage(const char *path)
{
    ZunColor color1;
    ZunColor color2;
    ZunRect rect1;
    ZunRect rect2;
    i32 i = 0;
    g_AnmManager->LoadSurface(0, path);

    while (i < TITLE_IMAGE_INFO_MAX_FRAMES)
    {
        g_AnmManager->ClearVertexShader();
        g_AnmManager->ClearSprite();
        g_AnmManager->ClearTexture();
        g_AnmManager->ClearColorOp();
        g_AnmManager->ClearBlendMode();
        g_AnmManager->ClearZWrite();

        g_AnmManager->ResetFrameDebugInfo();
        g_AnmManager->ClearCameraSettings();
        g_AnmManager->SetMixColorDefault();

        g_Supervisor.d3dDevice->BeginScene();
        g_AnmManager->CopySurfaceToBackbuffer(0, 0, 0, 0, 0);
        
        
        if (i < TITLE_IMAGE_FADE_IN_TIME)
        {
            rect1.left = 0.0f;
            rect1.top = 0.0f;
            rect1.right = 640.0f - 1.0f;
            rect1.bottom = 480.0f - 1.0f;

            color1.a = (((60 - i) * 255) / TITLE_IMAGE_FADE_IN_TIME);
            color1.r = color1.g = color1.b = 0;

            ScreenEffect::DrawSquare(&rect1, color1.d3dColor);
        }
        else if (i > TITLE_IMAGE_INFO_MAX_FRAMES - 60)
        {
            rect2.left = 0.0f;
            rect2.top = 0.0f;
            rect2.right = 640.0f - 1.0f;
            rect2.bottom = 480.0f - 1.0f;

            color2.a = (((i - (TITLE_IMAGE_INFO_MAX_FRAMES - TITLE_IMAGE_FADE_IN_TIME)) * 255) / TITLE_IMAGE_FADE_IN_TIME);
            color2.r = color2.g = color2.b = 0;

            ScreenEffect::DrawSquare(&rect2, color2.d3dColor);
        }

        g_CurFrameInput = Controller::GetInput();

        g_Supervisor.d3dDevice->EndScene();
        if (FAILED(g_Supervisor.d3dDevice->Present(NULL, NULL, NULL, NULL)))
        {
            g_Supervisor.d3dDevice->Reset(&g_Supervisor.presentParameters);
        }

        if (i >= 180
            && i < TITLE_IMAGE_INFO_MAX_FRAMES - TITLE_IMAGE_FADE_IN_TIME
            && WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_BOMB | TH_BUTTON_ENTER))
        {
            i = TITLE_IMAGE_INFO_MAX_FRAMES - TITLE_IMAGE_FADE_IN_TIME;
            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT, 0);
        }

        i++;

        g_SoundPlayer.ProcessQueues();
    }

    g_AnmManager->ReleaseSurface(0);
}

ZunResult TitleScreen::Release()
{
    if (this->currentReplay != NULL)
    {
        g_ZunMemory.Free(this->currentReplay);
        this->currentReplay = NULL;
    }
    
    if (this->vms != NULL)
    {
        ZUN_DELETE2(this->vms);
        /* Again, double NULL set. */
        this->vms = NULL;
    }

    return ZUN_SUCCESS;
}

ZunResult TitleScreen::RegisterChain(int param)
{
    TitleScreen *titleScreen = new TitleScreen();
    g_TitleScreen = titleScreen;

    memset(titleScreen, 0, sizeof(TitleScreen));
    g_GameManager.isInGameMenu = FALSE;

    titleScreen->calcChain = g_Chain.CreateElem((ChainCallback) TitleScreen::OnUpdate);
    titleScreen->calcChain->arg = titleScreen;
    titleScreen->calcChain->addedCallback = (ChainLifetimeCallback) TitleScreen::AddedCallback;
    titleScreen->calcChain->deletedCallback = (ChainLifetimeCallback) TitleScreen::DeletedCallback;

    if (g_Chain.AddToCalcChain(titleScreen->calcChain, 4) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    titleScreen->drawChain = g_Chain.CreateElem((ChainCallback) TitleScreen::OnDraw);
    titleScreen->drawChain->arg = titleScreen;
    g_Chain.AddToDrawChain(titleScreen->drawChain, 3);

    return ZUN_SUCCESS;
}

ZunResult TitleScreen::AddedCallback(TitleScreen *titleScreen)
{
    return titleScreen->ActualAddedCallback();
}

ZunResult TitleScreen::DeletedCallback(TitleScreen *titleScreen)
{
    g_Supervisor.d3dDevice->ResourceManagerDiscardBytes(0);

    g_AnmManager->ReleaseAnm(20);
    g_AnmManager->ReleaseAnm(22);
    g_AnmManager->ReleaseSurface(0);

    g_Chain.Cut(titleScreen->drawChain);

    titleScreen->drawChain = NULL;
    titleScreen->Release();
    ZUN_DELETE2(titleScreen);

    return ZUN_SUCCESS;
}

} /* namespace th08 */