#include "Supervisor.hpp"
#include "AnmManager.hpp"
#include "Ending.hpp"
#include "GameManager.hpp"
#include "Global.hpp"
#include "MusicRoom.hpp"
#include "ReplayManager.hpp"
#include "ResultScreen.hpp"
#include "ScoreDat.hpp"
#include "SoundPlayer.hpp"
#include "TextHelper.hpp"
#include "Title.hpp"
#include "i18n.hpp"
#include "utils.hpp"
#include <WinBase.h>
#include <d3dx8.h>
#include <direct.h>
#include <stdio.h>
#include <time.h>

namespace th08
{
DIFFABLE_STATIC(Supervisor, g_Supervisor);
DIFFABLE_STATIC_ARRAY(AnmVm, 3, g_SupervisorLoadingVms);

Supervisor::Supervisor()
{
    memset(this, 0, sizeof(Supervisor));

    this->flags.unk6 = true;
    this->flags.unk8 = true;
}

ChainCallbackResult Supervisor::DrawFpsCounter(Supervisor *s)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult Supervisor::OnDraw2(Supervisor *s)
{
    if (s->loadingVmsHaveBeenSetup == 0)
    {
        /* ZUN bloat: no need to check because ReleaseSurface does that already. */
        if (g_AnmManager->surfaces[8] != NULL)
        {
            g_AnmManager->ReleaseSurface(8);
        }
    }
    else
    {
        g_AnmManager->CopySurfaceToBackbuffer(8, 0, 0, 0, 0);
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult Supervisor::OnDraw3(Supervisor *s)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(g_SupervisorLoadingVms); i++)
    {
        g_SupervisorLoadingVms[i].pos += g_SupervisorLoadingVms[i].pos2;

        g_AnmManager->Draw2D(&g_SupervisorLoadingVms[i]);

        g_SupervisorLoadingVms[i].pos -= g_SupervisorLoadingVms[i].pos2;
    }

    if (s->unk294 != 0)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

#pragma var_order(elem, result, supervisor)
ZunResult Supervisor::RegisterChain()
{
    Supervisor *supervisor = &g_Supervisor;

    supervisor->wantedState = 0;
    supervisor->curState = -1;
    supervisor->calcCount = 0;

    ChainElem *elem = g_Chain.CreateElem((ChainCallback)Supervisor::OnUpdate);

    elem->arg = supervisor;
    elem->addedCallback = (ChainLifetimeCallback)Supervisor::AddedCallback;
    elem->deletedCallback = (ChainLifetimeCallback)Supervisor::DeletedCallback;

    ZunResult result = (ZunResult)g_Chain.AddToCalcChain(elem, 0);

    if (result != ZUN_SUCCESS)
    {
        return result;
    }

    elem = g_Chain.CreateElem((ChainCallback)Supervisor::DrawFpsCounter);
    elem->arg = supervisor;
    g_Chain.AddToDrawChain(elem, 16);

    elem = g_Chain.CreateElem((ChainCallback)Supervisor::OnDraw2);
    elem->arg = supervisor;
    g_Chain.AddToDrawChain(elem, 0);

    elem = g_Chain.CreateElem((ChainCallback)Supervisor::OnDraw3);
    elem->arg = supervisor;
    g_Chain.AddToDrawChain(elem, 2);

    return ZUN_SUCCESS;
}

#pragma var_order(position, score, s)
int Supervisor::AddedCallback(Supervisor *s)
{
    g_Supervisor.framerateMultiplier = 1.0f;

    ScoreDat *score = ScoreDat::OpenScore("score.dat");

    memset(&g_GameManager.plst, 0, sizeof(g_GameManager.plst));
    g_GameManager.plst.base.unkLen = g_GameManager.plst.base.th8kLen = sizeof(Plst);
    g_GameManager.plst.base.magic = 'TSLP';
    g_GameManager.plst.base.version = 2;

    ScoreDat::ParsePLST(score, &g_GameManager.plst);
    ScoreDat::ParseCLRD(score, g_GameManager.clrdData);
    ScoreDat::ParsePSCR(score, g_GameManager.pscrData);
    ScoreDat::ParseCATK(score, g_GameManager.catkData);
    ScoreDat::ParseFLSP(score, &g_GameManager.flsp);

    ScoreDat::ReleaseScore(score);

    g_GameManager.flags.finalBClearedWithAnyTeam = g_GameManager.FinalBClearedWithAnyTeam();
    g_GameManager.flags.finalAClearedWithAnyTeam = g_GameManager.FinalAClearedWithAnyTeam();
    g_GameManager.flags.finalBClearedWithAllTeams = g_GameManager.FinalBClearedWithAllTeams();

    if (Supervisor::LoadDat() != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    g_AnmManager->LoadSurface(8, "title/th08logo.jpg");
    s->loadingAnm = g_AnmManager->LoadAnm(2, "nowloading.anm");
    if (s->loadingAnm == NULL)
    {
        g_AnmManager->ReleaseSurface(0);
        return ZUN_ERROR;
    }

    g_Supervisor.unk178 = 1;

    if (!g_Supervisor.disableVsync && Supervisor::CheckFps() != ZUN_SUCCESS)
    {
        g_AnmManager->ReleaseSurface(0);
        return -2;
    }

    g_AnmManager->SetupVertexBuffer();
    TextHelper::CreateTextBuffer();

    D3DXVECTOR3 position(500.0, 440.0f, 0.0f);

    g_Supervisor.SetupLoadingVms(&position);

    g_Supervisor.unk294 = 1;
    g_Supervisor.ThreadStart((LPTHREAD_START_ROUTINE)Supervisor::StartupThread, s);

    return ZUN_SUCCESS;
}

ZunResult Supervisor::LoadDat()
{
    if (g_PbgArchive.Load("th08.dat"))
    {
#pragma var_order(fileSize, versionFileName)
        i32 fileSize;
        char versionFileName[128];

        sprintf(versionFileName, "th08_%.4x%c.ver", 0x100, 'd');

        g_Supervisor.versionData = FileSystem::OpenFile(versionFileName, &fileSize, 0);
        g_Supervisor.versionDataSize = fileSize;
        if (g_Supervisor.versionData == NULL)
        {
            g_GameErrorContext.Fatal(TH_ERR_DAT_WRONG_VERSION);
            return ZUN_ERROR;
        }
    }
    else
    {
        g_GameErrorContext.Fatal(TH_ERR_DAT_NOT_FOUND);
        return ZUN_ERROR;
    }

    return ZUN_SUCCESS;
}

// STUB: th08 0x446232
i32 Supervisor::CheckFps()
{
    return -1;
}

#pragma var_order(bgmVolume, scoreFileSize, scoreFile, findFile, i, fileNameBuffer, scoreBackupFileName, findData,     \
                  currentLocalTime, currentTime)
void Supervisor::StartupThread(Supervisor *s)
{
    float bgmVolume;
    u8 *scoreFile;
    i32 scoreFileSize;
    HANDLE findFile;
    int i;
    char fileNameBuffer[256]; /* yes I know the buffer might be too small, but it would not match otherwise. */
    const char *scoreBackupFileName;
    WIN32_FIND_DATAA findData;
    time_t currentTime;
    tm *currentLocalTime;

    g_Supervisor.unk178 = 0;
    g_Supervisor.unk174 = 0;
    g_Supervisor.totalPlayTime = timeGetTime();

    g_Rng.SetSeed(g_Supervisor.totalPlayTime);

    g_Supervisor.SetupDInput();

    if (g_Supervisor.midiOutput == NULL)
    {
        g_Supervisor.midiOutput = new MidiOutput();
    }
    if (g_Supervisor.midiOutput != NULL)
    {
        g_Supervisor.midiOutput->ReadFileData(30, "bgm/init.mid");
    }
    g_SoundPlayer.InitSoundBuffers();
    g_Supervisor.textAnm = g_AnmManager->PreloadAnm(0, "text.anm");
    if (g_Supervisor.textAnm == NULL)
    {
        goto err;
    }

    if (AsciiManager::RegisterChain() != ZUN_SUCCESS)
    {
        if (g_Supervisor.subthreadCloseRequestActive)
        {
            return;
        }
        g_GameErrorContext.Log(TH_ERR_FAILED_TO_INIT_ASCII);
        goto err;
    }

    if (g_SoundPlayer.LoadFmt("bgm/thbgm.fmt") != ZUN_SUCCESS)
    {
        if (g_Supervisor.subthreadCloseRequestActive)
        {
            return;
        }
        g_GameErrorContext.Log(TH_ERR_FAILED_TO_INIT_BGM);
        goto err;
    }

    g_SoundPlayer.bgmVolume = g_Supervisor.cfg.musicVolume;
    g_SoundPlayer.sfxVolume = g_Supervisor.cfg.sfxVolume;

    bgmVolume = g_SoundPlayer.bgmVolume / 100.0f;

    if (g_SoundPlayer.sfxVolume != 0)
    {
        /* Strangely, the parentheses affect code generation, even
         * though they don't seem do anything?
         */

        bgmVolume = (1.0f - bgmVolume);
        bgmVolume *= bgmVolume;
        bgmVolume *= bgmVolume;
        bgmVolume = (1.0f - bgmVolume);

        g_SoundPlayer.unkVolume = ((int)(SOUNDPLAYER_VOLUME_RANGE * bgmVolume)) - SOUNDPLAYER_VOLUME_RANGE;
    }
    else
    {
        g_SoundPlayer.unkVolume = SOUNDPLAYER_SILENT_VOLUME;
    }

    if (g_SoundPlayer.unusedBgmSeekOffset == 0)
    {
        if (!g_Supervisor.IsMusicPreloadEnabled())
        {
            g_SoundPlayer.StartBGM("thbgm.dat");
        }
        else
        {
            strcpy(g_SoundPlayer.currentBgmFileName, "thbgm.dat");
        }
    }
    else if (!g_Supervisor.IsMusicPreloadEnabled())
    {
        g_SoundPlayer.StartBGM("th08.dat");
    }
    else
    {
        strcpy(g_SoundPlayer.currentBgmFileName, "th08.dat");
    }

    if (g_Supervisor.flags.unk8 && ((scoreFile = FileSystem::OpenFile("score.dat", &scoreFileSize, TRUE)) != NULL))
    {
        scoreBackupFileName = "score_4.??????.bak";

        i = 0;

        _mkdir("./backup");
        _chdir("./backup");

        findFile = FindFirstFileA("score_5.??????.bak", &findData);

        while (findFile != INVALID_HANDLE_VALUE)
        {
            DeleteFileA(findData.cFileName);

            if (!FindNextFileA(findFile, &findData))
            {
                break;
            }
        }

        FindClose(findFile);

        for (i = 4; i > 0; i--)
        {
            strcpy(fileNameBuffer, scoreBackupFileName);

            fileNameBuffer[6] = i + '0';

            findFile = FindFirstFileA(fileNameBuffer, &findData);

            while (findFile != INVALID_HANDLE_VALUE)
            {
                strcpy(fileNameBuffer, findData.cFileName);

                fileNameBuffer[6] = i + '1';

                rename(findData.cFileName, fileNameBuffer);

                if (!FindNextFileA(findFile, &findData))
                {
                    break;
                }
            }

            FindClose(findFile);
        }

        time(&currentTime);
        currentLocalTime = localtime(&currentTime);
        strftime(fileNameBuffer, 128, "score_1.%y%m%d.bak", currentLocalTime);

        FileSystem::WriteDataToFile(fileNameBuffer, scoreFile, scoreFileSize);
        free(scoreFile);
        _chdir("../");
    }

    if (g_Supervisor.flags.unk6)
    {
        g_Supervisor.dummyMidiTimer = new DummyMidiTimer();
        if (g_Supervisor.dummyMidiTimer != NULL)
        {
            g_Supervisor.dummyMidiTimer->StartTimer();
        }
    }

    g_Supervisor.runningSubthreadHandle = NULL;
    g_Supervisor.subthreadCloseRequestActive = FALSE;
    g_Supervisor.unk290 = 0;
    g_Supervisor.unk294 = 0;
    g_Supervisor.flags.unk8 = false;

    return;

err:
    g_Supervisor.runningSubthreadHandle = NULL;
    g_Supervisor.subthreadCloseRequestActive = FALSE;
    g_Supervisor.unk290 = 0;
    g_Supervisor.unk294 = 2;
    g_Supervisor.flags.receivedCloseMsg = true;
}

// STUB: th08 0x446a37
ZunResult Supervisor::SetupDInput()
{
    return ZUN_ERROR;
}

ZunResult Supervisor::DeletedCallback(Supervisor *s)
{
    return ZUN_SUCCESS;
}

ChainCallbackResult Supervisor::OnUpdate(Supervisor *s)
{
    if (s->flags.receivedCloseMsg && !s->IsSubthreadRunning())
    {
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
    }

    g_AnmManager->ClearVertexShader();
    g_AnmManager->ClearSprite();
    g_AnmManager->ClearTexture();
    g_AnmManager->ClearColorOp();
    g_AnmManager->ClearBlendMode();
    g_AnmManager->ClearZWrite();

    g_AnmManager->ResetFrameDebugInfo();
    g_AnmManager->ClearCameraSettings();
    g_AnmManager->ResetMoreStuff();
    g_AnmManager->screenShakeOffset.x = g_AnmManager->screenShakeOffset.y = 0.0f;

    g_AnmManager->ExecuteScriptOnVmArray(g_SupervisorLoadingVms, ARRAY_SIZE(g_SupervisorLoadingVms));

    if (g_AnmManager->ServicePreloadedAnims() != ZUN_SUCCESS)
    {
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
    }

    if (s->unk294 != 0)
    {
        if (s->unk294 == 2)
        {
            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
        }
        if (s->unk290 == 0)
        {
            s->unk294 = 0;
        }
        else
        {
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
    }

    g_Supervisor.ClearFogState();
    g_SoundPlayer.UpdateFades();

    if (!g_GameManager.IsUnknown())
    {
        g_LastFrameInput = g_CurFrameInput;
        g_CurFrameInput = Controller::GetInput();

        g_IsEighthFrameOfHeldInput = 0;
        if (g_LastFrameInput == g_CurFrameInput)
        {
            if (g_NumOfFramesInputsWereHeld >= 0x1e)
            {
                if (g_NumOfFramesInputsWereHeld % 8 == 0)
                {
                    g_IsEighthFrameOfHeldInput = 1;
                }
                if (0x26 <= g_NumOfFramesInputsWereHeld)
                {
                    g_NumOfFramesInputsWereHeld = 0x1e;
                }
            }

            g_NumOfFramesInputsWereHeld++;
        }
        else
        {
            g_NumOfFramesInputsWereHeld = 0;
        }
    }
    else
    {
        g_CurFrameInput |= Controller::GetInput();
    }

    if (s->wantedState != s->curState)
    {
        s->wantedState2 = s->wantedState;
        utils::GuiDebugPrint("scene %d -> %d\r\n", s->wantedState, s->curState);
        switch (s->wantedState)
        {
        case 0:
        init_titlescreen:
            s->curState = 1;
            g_Supervisor.d3dDevice->ResourceManagerDiscardBytes(0);
            if (Title::RegisterChain(0) != ZUN_SUCCESS)
            {
                return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
            }
            break;
        case 1:
            switch (s->curState)
            {
            case -1:
                return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
            case 2:
                if (GameManager::RegisterChain() != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            case 4:
                return CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR;
            case 5:
                if (ResultScreen::RegisterChain(0) != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            case 8:
                if (MusicRoom::RegisterChain() != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            case 9:
                GameManager::CutChain();
                if (Ending::RegisterChain() != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            }
            break;
        case 5:
            switch (s->curState)
            {
            case -1:
                return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
            case 1:
                s->curState = 0;
                goto init_titlescreen;
            }
            break;
        case 2:
            switch (s->curState)
            {
            case -1:
                return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
            case 1:
                GameManager::CutChain();
                s->curState = 0;
                ReplayManager::SaveReplay(NULL, NULL);

                goto init_titlescreen;
            case 6:
                GameManager::CutChain();
                if (ResultScreen::RegisterChain(1) != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            case 10:
                GameManager::CutChain();
                if ((g_GameManager.flags.unk0) == 0 && g_GameManager.difficulty < 4)
                {
                    g_GameManager.currentStage = 0;
                }
                ReplayManager::SaveReplay(NULL, NULL);
                if (GameManager::RegisterChain() != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                s->curState = 2;
                break;
            case 11:
                g_Supervisor.curState = 3;
                g_Supervisor.unk16c = 1;

                GameManager::CutChain();

                if (GameManager::RegisterChain() != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                s->curState = 2;
                break;
            case 12:
                g_Supervisor.curState = 3;
                GameManager::CutChain();
                g_GameManager.AdvanceToNextStage();

                if (GameManager::RegisterChain() != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                s->curState = 2;
                break;
            case 3:
                GameManager::CutChain();

                if (GameManager::RegisterChain() != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }

                s->curState = 2;
                break;
            case 7:
                GameManager::CutChain();

                s->curState = 0;
                ReplayManager::SaveReplay(NULL, NULL);
                s->curState = 1;

                g_Supervisor.d3dDevice->ResourceManagerDiscardBytes(0);

                if (Title::RegisterChain(1) != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            case 9:
                GameManager::CutChain();
                if (Ending::RegisterChain() != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            }
            break;
        case 6:
            switch (s->curState)
            {
            case -1:
                ReplayManager::SaveReplay(NULL, NULL);
                return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
            case 1:
                s->curState = 0;

                ReplayManager::SaveReplay(NULL, NULL);

                goto init_titlescreen;
            }
            break;
        case 8:
            switch (s->curState)
            {
            case -1:
                return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
            case 1:
                s->curState = 0;

                goto init_titlescreen;
            }
            break;
        case 9:
            switch (s->curState)
            {
            case -1:
                return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
            case 1:
                s->curState = 0;

                goto init_titlescreen;
            case 6:
                if (ResultScreen::RegisterChain(1) != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            }
            break;
        }
        g_CurFrameInput = g_LastFrameInput = g_IsEighthFrameOfHeldInput = 0;
    }

    s->wantedState = s->curState;
    s->calcCount++;

    if ((s->calcCount % 4000) == 3999 &&
        g_Supervisor.VerifyExeIntegrity("0100d", g_Supervisor.exeSize, g_Supervisor.exeChecksum) != ZUN_SUCCESS)
    {
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
    }

    if (g_UnknownCounter != 0)
    {
        g_UnknownCounter--;
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

void ZunTimer::Increment(int value)
{
    if (g_Supervisor.flags.unk5 != 0)
    {
        this->current++;
        this->subFrame = 0.0f;
        this->previous = -999.0f;
    }

    if (g_Supervisor.framerateMultiplier > 0.99f)
    {
        this->current += value;
        return;
    }

    if (value < 0)
    {
        this->Decrement(-value);
        return;
    }

    this->previous = this->current;
    this->subFrame += value * g_Supervisor.framerateMultiplier;

    while (this->subFrame >= 1.0f)
    {
        this->current++;
        this->subFrame -= 1.0f;
    }
}

void ZunTimer::Decrement(int value)
{
    if (g_Supervisor.flags.unk5 != 0)
    {
        this->current--;
        this->subFrame = 0.0f;
        this->previous = -999.0f;
    }

    if (g_Supervisor.framerateMultiplier > 0.99f)
    {
        this->current -= value;
        return;
    }

    if (value < 0)
    {
        this->Increment(-value);
        return;
    }

    this->previous = this->current;
    this->subFrame -= value * g_Supervisor.framerateMultiplier;

    while (this->subFrame < 0.0f)
    {
        this->current--;
        this->subFrame += 1.0f;
    }
}

void Supervisor::TickTimer(int *frames, float *subframes)
{
    if (this->framerateMultiplier <= 0.99f)
    {
        *subframes += this->framerateMultiplier;
        if (*subframes >= 1.0f)
        {
            *frames = *frames + 1;
            *subframes -= 1.0f;
        }
    }
    else
    {
        *frames = *frames + 1;
    }
}

ZunBool Supervisor::TakeSnapshot(const char *filePath)
{
    return FALSE;
}

#pragma var_order(fileSize, configFileBuffer, bgmHandle, bytesRead, bgmBuffer, bgmHandle2, bytesRead2, bgmBuffer2)
ZunResult Supervisor::LoadConfig(char *configFile)
{
    i32 bgmBuffer[4];
    i32 bgmBuffer2[4];

    HANDLE bgmHandle;
    HANDLE bgmHandle2;

    DWORD bytesRead;
    DWORD bytesRead2;

    u8 *configFileBuffer;
    i32 fileSize;

    memset(&g_Supervisor.cfg, 0, sizeof(GameConfiguration));
    configFileBuffer = FileSystem::OpenFile(configFile, &fileSize, true);
    if (configFileBuffer == NULL)
    {
        g_GameErrorContext.Log(TH_ERR_CONFIG_NOT_FOUND);
    SET_DEFAULT:
        g_Supervisor.cfg.lifeCount = 2;
        g_Supervisor.cfg.bombCount = 3;
        g_Supervisor.cfg.colorMode16bit = 0;
        g_Supervisor.cfg.version = GAME_VERSION;
        g_Supervisor.cfg.padXAxis = 600;
        g_Supervisor.cfg.padYAxis = 600;
        bgmHandle = CreateFileA("./thbgm.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (bgmHandle != INVALID_HANDLE_VALUE)
        {
            ReadFile(bgmHandle, bgmBuffer, 16, &bytesRead, NULL);
            CloseHandle(bgmHandle);
            if (bgmBuffer[0] != ZWAV_MAGIC || bgmBuffer[1] != 1 || bgmBuffer[2] != 0x800)
            {
                g_GameErrorContext.Fatal(TH_ERR_BGM_VERSION_MISMATCH);
                return ZUN_ERROR;
            }
            g_Supervisor.cfg.musicMode = WAV;
        }
        else
        {
            g_Supervisor.cfg.musicMode = MIDI;
            utils::GuiDebugPrint(TH_ERR_NO_WAVE_FILE);
        }
        g_Supervisor.cfg.playSounds = 1;
        g_Supervisor.cfg.defaultDifficulty = NORMAL;
        g_Supervisor.cfg.windowed = false;
        g_Supervisor.cfg.frameskipConfig = false;
        g_Supervisor.cfg.controllerMapping = g_ControllerMapping;
        g_Supervisor.cfg.effectQuality = MAXIMUM;
        g_Supervisor.cfg.slowMode = 0;
        g_Supervisor.cfg.shotSlow = 0;
        g_Supervisor.cfg.musicVolume = 100;
        g_Supervisor.cfg.sfxVolume = 80;
    }
    else
    {
        g_Supervisor.cfg = *(GameConfiguration *)configFileBuffer;
        free(configFileBuffer);
        bgmHandle2 = CreateFileA("./thbgm.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (bgmHandle2 != INVALID_HANDLE_VALUE)
        {
            ReadFile(bgmHandle2, bgmBuffer2, 16, &bytesRead2, NULL);
            CloseHandle(bgmHandle2);
            if (bgmBuffer2[0] != ZWAV_MAGIC || bgmBuffer2[1] != 1 || bgmBuffer2[2] != 0x800)
            {
                g_GameErrorContext.Fatal(TH_ERR_BGM_VERSION_MISMATCH);
                return ZUN_ERROR;
            }
        }
        if (g_Supervisor.cfg.lifeCount >= 7 || g_Supervisor.cfg.bombCount >= 4 ||
            g_Supervisor.cfg.colorMode16bit >= 2 || g_Supervisor.cfg.musicMode >= 3 ||
            g_Supervisor.cfg.defaultDifficulty >= 6 || g_Supervisor.cfg.playSounds >= 2 ||
            g_Supervisor.cfg.windowed >= 2 || g_Supervisor.cfg.frameskipConfig >= 3 ||
            g_Supervisor.cfg.effectQuality >= 3 || g_Supervisor.cfg.slowMode >= 2 || g_Supervisor.cfg.shotSlow >= 2 ||
            g_Supervisor.cfg.version != GAME_VERSION || fileSize != 60)
        {

            g_GameErrorContext.Log(TH_ERR_CONFIG_ABNORMAL);
            memset(&g_Supervisor.cfg, 0, sizeof(GameConfiguration));
            goto SET_DEFAULT;
        }
        g_ControllerMapping = g_Supervisor.cfg.controllerMapping;
    }

    g_Supervisor.cfg.opts.useSwTextureBlending = true; // Bit ignored from PCB onwards (HW blending always used)
    if (this->cfg.opts.dontUseVertexBuf != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_VERTEX_BUFFER);
    }
    if (this->cfg.opts.disableFog != 0)
    {
        g_GameErrorContext.Log(TH_ERR_NO_FOG);
    }
    if (this->cfg.opts.force16bitTextures != false)
    {
        g_GameErrorContext.Log(TH_ERR_USE_16BIT_TEXTURES);
    }
    // This should be inlined
    if (this->ShouldForceBackbufferClear())
    {
        g_GameErrorContext.Log(TH_ERR_FORCE_BACKBUFFER_CLEAR);
    }
    if (this->cfg.opts.displayMinimumGraphics != false)
    {
        g_GameErrorContext.Log(TH_ERR_DONT_RENDER_ITEMS);
    }
    if (this->cfg.opts.suppressUseOfGoroudShading != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_GOURAUD_SHADING);
    }
    if (this->cfg.opts.disableDepthTest != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_DEPTH_TESTING);
    }
    this->disableVsync = false;
    this->cfg.opts.force60Fps = false;

    if (this->cfg.opts.disableColorCompositing != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_TEXTURE_COLOR_COMPOSITING);
    }
    if (this->cfg.windowed != false)
    {
        g_GameErrorContext.Log(TH_ERR_LAUNCH_WINDOWED);
    }
    if (this->cfg.opts.referenceRasterizerMode != false)
    {
        g_GameErrorContext.Log(TH_ERR_FORCE_REFERENCE_RASTERIZER);
    }
    if (this->cfg.opts.dontUseDirectInput != false)
    {
        g_GameErrorContext.Log(TH_ERR_DO_NOT_USE_DIRECTINPUT);
    }
    if (this->cfg.opts.redrawHUDEveryFrame != false)
    {
        g_GameErrorContext.Log(TH_ERR_REDRAW_HUD_EVERY_FRAME);
    }
    if (this->cfg.opts.preloadMusic != false)
    {
        g_GameErrorContext.Log(TH_ERR_PRELOAD_BGM);
    }
    if (this->cfg.opts.disableVsync != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_VSYNC);
        g_Supervisor.disableVsync = true;
    }
    if (this->cfg.opts.dontDetectTextDrawingBackground != false)
    {
        g_GameErrorContext.Log(TH_ERR_DONT_DETECT_TEXT_BG);
    }

    if (FileSystem::WriteDataToFile(configFile, &g_Supervisor.cfg, sizeof(GameConfiguration)) != 0)
    {
        g_GameErrorContext.Fatal(TH_ERR_FILE_CANNOT_BE_EXPORTED, configFile);
        g_GameErrorContext.Fatal(TH_ERR_FOLDER_HAS_WRITE_PROTECT_OR_DISK_FULL);
        return ZUN_ERROR;
    }

    return ZUN_SUCCESS;
}

ZunResult Supervisor::ThreadStart(LPTHREAD_START_ROUTINE startFunction, void *startParam)
{
    this->ThreadClose();

    utils::GuiDebugPrint("info : Sub Thread Start Request\n");

    this->runningSubthreadHandle = CreateThread(NULL, 0, startFunction, startParam, 0, &this->runningSubthreadID);

    this->unk290 = TRUE;

    return (this->runningSubthreadHandle != NULL) ? ZUN_SUCCESS : ZUN_ERROR;
}

void Supervisor::ThreadClose()
{
    if (this->runningSubthreadHandle != NULL)
    {
        utils::GuiDebugPrint("info : Sub Thread Close Request\n");
        this->subthreadCloseRequestActive = TRUE;

        while (WaitForSingleObject(this->runningSubthreadHandle, 1000) == WAIT_TIMEOUT)
            Sleep(1);

        CloseHandle(this->runningSubthreadHandle);
        this->runningSubthreadHandle = NULL;
        this->subthreadCloseRequestActive = FALSE;
    }
}

void Supervisor::SetupLoadingVms(D3DXVECTOR3 *position)
{
    /* there's some weird stack stuff going on here. In the original code,
     * 0x4c is subtracted from ESP for no apparent. In an earlier version
     * it subtracted 0x58, and now it doesn't do that at all?? And it seems
     * to do with ExecuteAnmIdx?? WTF???
     */
    if (!this->loadingVmsHaveBeenSetup)
    {
        this->loadingAnm->ExecuteAnmIdx(&g_SupervisorLoadingVms[0], 0);
        this->loadingAnm->ExecuteAnmIdx(&g_SupervisorLoadingVms[1], 1);
        this->loadingAnm->ExecuteAnmIdx(&g_SupervisorLoadingVms[2], 2);

        this->loadingVmsHaveBeenSetup = TRUE;

        g_SupervisorLoadingVms[0].pos = *position;
        g_SupervisorLoadingVms[1].pos = *position;
        g_SupervisorLoadingVms[2].pos = *position;
    }
}

void Supervisor::InitializeCriticalSections()
{
    for (u32 i = 0; i < ARRAY_SIZE_SIGNED(this->criticalSections); i++)
    {
        InitializeCriticalSection(&this->criticalSections[i]);
    }
}

void Supervisor::DeleteCriticalSections()
{
}

i32 Supervisor::EnableFog()
{
    g_AnmManager->FlushVertexBuffer();

    if (this->fogState != FOG_ENABLED)
    {
        this->fogState = FOG_ENABLED;

        return this->d3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    }

    return 0;
}

i32 Supervisor::DisableFog()
{
    g_AnmManager->FlushVertexBuffer();

    if (this->fogState != FOG_DISABLED)
    {
        this->fogState = FOG_DISABLED;

        return this->d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    }

    return 0;
}

void Supervisor::SetRenderState(D3DRENDERSTATETYPE renderStateType, int value)
{
    g_AnmManager->FlushVertexBuffer();

    this->d3dDevice->SetRenderState(renderStateType, value);
}

#pragma var_order(gameTime, difference)
void Supervisor::UpdateGameTime(Supervisor *s)
{
    DWORD gameTime = timeGetTime();

    if (gameTime < s->systemTime)
    {
        s->systemTime = 0;
    }

    DWORD difference = gameTime - s->systemTime;

    g_GameManager.plst.gameHours += (difference / 3600000);
    difference %= 3600000;

    g_GameManager.plst.gameMinutes += (difference / 60000);
    difference %= 60000;

    g_GameManager.plst.gameSeconds += (difference / 1000);
    difference %= 1000;

    g_GameManager.plst.gameMilliseconds += difference;

    if (g_GameManager.plst.gameMilliseconds >= 1000)
    {
        g_GameManager.plst.gameSeconds += (g_GameManager.plst.gameMilliseconds / 1000);
        g_GameManager.plst.gameMilliseconds = (g_GameManager.plst.gameMilliseconds % 1000);
    }
    if (g_GameManager.plst.gameSeconds >= 60)
    {
        g_GameManager.plst.gameMinutes += (g_GameManager.plst.gameMilliseconds / 60);
        g_GameManager.plst.gameSeconds = (g_GameManager.plst.gameMilliseconds % 60);
    }
    if (g_GameManager.plst.gameMinutes >= 60)
    {
        g_GameManager.plst.gameHours += (g_GameManager.plst.gameMinutes / 60);
        g_GameManager.plst.gameMinutes = (g_GameManager.plst.gameMinutes % 60);
    }

    s->systemTime = gameTime;
}

#pragma var_order(playTime, difference)
void Supervisor::UpdatePlayTime(Supervisor *s)
{
    DWORD playTime = timeGetTime();

    if (playTime < s->totalPlayTime)
    {
        s->totalPlayTime = playTime;
    }

    DWORD difference = playTime - s->totalPlayTime;

    g_GameManager.plst.totalHours += (difference / 3600000);
    difference %= 3600000;

    g_GameManager.plst.totalMinutes += (difference / 60000);
    difference %= 60000;

    g_GameManager.plst.totalSeconds += (difference / 1000);
    difference %= 1000;

    g_GameManager.plst.totalMilliseconds += difference;

    if (g_GameManager.plst.totalMilliseconds >= 1000)
    {
        g_GameManager.plst.totalSeconds += (g_GameManager.plst.totalMilliseconds / 1000);
        g_GameManager.plst.totalMilliseconds = (g_GameManager.plst.totalMilliseconds % 1000);
    }
    if (g_GameManager.plst.totalSeconds >= 60)
    {
        g_GameManager.plst.totalMinutes += (g_GameManager.plst.totalMilliseconds / 60);
        g_GameManager.plst.totalSeconds = (g_GameManager.plst.totalMilliseconds % 60);
    }
    if (g_GameManager.plst.totalMinutes >= 60)
    {
        g_GameManager.plst.totalHours += (g_GameManager.plst.totalMinutes / 60);
        g_GameManager.plst.totalMinutes = (g_GameManager.plst.totalMinutes % 60);
    }

    s->totalPlayTime = playTime;
}

ZunResult Supervisor::VerifyExeIntegrity(const char *version, i32 exeSize, i32 checksum)
{
    return ZUN_ERROR;
}

}; // namespace th08
