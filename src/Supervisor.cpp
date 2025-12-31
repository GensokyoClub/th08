#include "Ending.hpp"
#include "GameManager.hpp"
#include "Global.hpp"
#include "i18n.hpp"
#include "MusicRoom.hpp"
#include "ReplayManager.hpp"
#include "ResultSysInf.hpp"
#include "SoundPlayer.hpp"
#include "SprtCtrl.hpp"
#include "Supervisor.hpp"
#include "Title.hpp"
#include "utils.hpp"
#include <WinBase.h>

namespace th08
{
DIFFABLE_STATIC(Supervisor, g_Supervisor);
DIFFABLE_STATIC_ARRAY(Sprt, 3, g_SupervisorSprites);


#pragma var_order(elem, result, supervisor)
ZunResult Supervisor::RegisterChain()
{
    Supervisor *supervisor = &g_Supervisor;

    supervisor->m_WantedState = 0;
    supervisor->m_CurState = -1;
    supervisor->m_CalcCount = 0;

    ChainElem *elem = g_Chain.CreateElem((ChainCallback)Supervisor::OnUpdate);

    elem->m_Arg = supervisor;
    elem->m_AddedCallback = (ChainLifetimeCallback) Supervisor::AddedCallback;
    elem->m_DeletedCallback = (ChainLifetimeCallback) Supervisor::DeletedCallback;

    ZunResult result = (ZunResult) g_Chain.AddToCalcChain(elem, 0);

    if (result != ZUN_SUCCESS)
    {
        return result;
    }

    elem = g_Chain.CreateElem((ChainCallback) Supervisor::DrawFpsCounter);
    elem->m_Arg = supervisor;
    g_Chain.AddToDrawChain(elem, 16);

    elem = g_Chain.CreateElem((ChainCallback) Supervisor::OnDraw2);
    elem->m_Arg = supervisor;
    g_Chain.AddToDrawChain(elem, 0);

    elem = g_Chain.CreateElem((ChainCallback) Supervisor::OnDraw3);
    elem->m_Arg = supervisor;
    g_Chain.AddToDrawChain(elem, 2);

    return ZUN_SUCCESS;
}

ChainCallbackResult Supervisor::OnUpdate(Supervisor *s)
{
    if (s->m_Flags.receivedCloseMsg && !s->IsSubthreadRunning())
    {
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
    }

    g_SprtCtrl->ClearVertexShader();
    g_SprtCtrl->ClearSprite();
    g_SprtCtrl->ClearTexture();
    g_SprtCtrl->ClearColorOp();
    g_SprtCtrl->ClearBlendMode();
    g_SprtCtrl->ClearZWrite();

    g_SprtCtrl->ResetSomeStuff();
    g_SprtCtrl->ClearCameraSettings();
    g_SprtCtrl->ResetMoreStuff();
    g_SprtCtrl->unk0x1c.x = g_SprtCtrl->unk0x1c.y = 0.0f;

    g_SprtCtrl->ExecuteAnms(g_SupervisorSprites, ARRAY_SIZE(g_SupervisorSprites));

    if (g_SprtCtrl->ServicePreloadedAnims() != ZUN_SUCCESS)
    {
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
    }

    if (s->m_Unk294 != 0)
    {
        if (s->m_Unk294 == 2)
        {
            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
        }
        if (s->m_Unk290 == 0)
        {
            s->m_Unk294 = 0;
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

    if (s->m_WantedState != s->m_CurState)
    {
        s->m_WantedState2 = s->m_WantedState;
        utils::GuiDebugPrint("scene %d -> %d\r\n", s->m_WantedState, s->m_CurState);
        switch (s->m_WantedState)
        {
            case 0:
init_titlescreen:
                s->m_CurState = 1;
                g_Supervisor.m_D3dDevice->ResourceManagerDiscardBytes(0);
                if (Title::RegisterChain(0) != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                }
                break;
            case 1:
                switch (s->m_CurState)
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
                        if (ResultSysInf::RegisterChain(0) != ZUN_SUCCESS)
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
                        if (EndingInf::RegisterChain() != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }
                        break;
                }
                break;
            case 5:
                switch (s->m_CurState)
                {
                    case -1:
                        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                    case 1:
                        s->m_CurState = 0;
                        goto init_titlescreen;
                }
                break;
            case 2:
                switch (s->m_CurState)
                {
                    case -1:
                        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                    case 1:
                        GameManager::CutChain();
                        s->m_CurState = 0;
                        ReplayManager::SaveReplay(NULL, NULL);

                        goto init_titlescreen;
                    case 6:
                        GameManager::CutChain();
                        if (ResultSysInf::RegisterChain(1) != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }
                        break;
                    case 10:
                        GameManager::CutChain();
                        if ((g_GameManager.m_Flags & 1) == 0 && g_GameManager.m_Difficulty < 4)
                        {
                            g_GameManager.m_CurrentStage = 0;
                        }
                        ReplayManager::SaveReplay(NULL, NULL);
                        if (GameManager::RegisterChain() != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }
                        s->m_CurState = 2;
                        break;
                    case 11:
                        g_Supervisor.m_CurState = 3;
                        g_Supervisor.m_Unk16c = 1;

                        GameManager::CutChain();

                        if (GameManager::RegisterChain() != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }
                        s->m_CurState = 2;
                        break;
                    case 12:
                        g_Supervisor.m_CurState = 3;
                        GameManager::CutChain();
                        g_GameManager.AdvanceToNextStage();

                        if (GameManager::RegisterChain() != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }
                        s->m_CurState = 2;
                        break;
                    case 3:
                        GameManager::CutChain();

                        if (GameManager::RegisterChain() != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }

                        s->m_CurState = 2;
                        break;
                    case 7:
                        GameManager::CutChain();

                        s->m_CurState = 0;
                        ReplayManager::SaveReplay(NULL, NULL);
                        s->m_CurState = 1;

                        g_Supervisor.m_D3dDevice->ResourceManagerDiscardBytes(0);

                        if (Title::RegisterChain(1) != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }
                        break;
                    case 9:
                        GameManager::CutChain();
                        if (EndingInf::RegisterChain() != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }
                        break;
                }
                break;
            case 6:
                switch (s->m_CurState)
                {
                    case -1:
                        ReplayManager::SaveReplay(NULL, NULL);
                        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                    case 1:
                        s->m_CurState = 0;

                        ReplayManager::SaveReplay(NULL, NULL);

                        goto init_titlescreen;
                }
                break;
            case 8:
                switch (s->m_CurState)
                {
                    case -1:
                        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                    case 1:
                        s->m_CurState = 0;

                        goto init_titlescreen;
                }
                break;
            case 9:
                switch (s->m_CurState)
                {
                    case -1:
                        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                    case 1:
                        s->m_CurState = 0;

                        goto init_titlescreen;
                    case 6:
                        if (ResultSysInf::RegisterChain(1) != ZUN_SUCCESS)
                        {
                            return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
                        }
                        break;
                }
                break;
        }
        g_LastFrameInput = g_CurFrameInput = g_IsEighthFrameOfHeldInput = 0;
    }

    s->m_WantedState = s->m_CurState;
    s->m_CalcCount++;

    if ((s->m_CalcCount % 4000) == 3999
        && g_Supervisor.VerifyExeIntegrity("100d", g_Supervisor.m_ExeSize, g_Supervisor.m_ExeChecksum) != ZUN_SUCCESS)
    {
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
    }

    if (g_UnknownCounter != 0)
    {
        g_UnknownCounter--;
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
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

    memset(&g_Supervisor.m_Cfg, 0, sizeof(GameConfiguration));
    configFileBuffer = FileSystem::OpenFile(configFile, &fileSize, true);
    if (configFileBuffer == NULL)
    {
        g_GameErrorContext.Log(TH_ERR_CONFIG_NOT_FOUND);
    SET_DEFAULT:
        g_Supervisor.m_Cfg.lifeCount = 2;
        g_Supervisor.m_Cfg.bombCount = 3;
        g_Supervisor.m_Cfg.colorMode16bit = 0;
        g_Supervisor.m_Cfg.version = GAME_VERSION;
        g_Supervisor.m_Cfg.padXAxis = 600;
        g_Supervisor.m_Cfg.padYAxis = 600;
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
            g_Supervisor.m_Cfg.musicMode = WAV;
        }
        else
        {
            g_Supervisor.m_Cfg.musicMode = MIDI;
            utils::GuiDebugPrint(TH_ERR_NO_WAVE_FILE);
        }
        g_Supervisor.m_Cfg.playSounds = 1;
        g_Supervisor.m_Cfg.defaultDifficulty = NORMAL;
        g_Supervisor.m_Cfg.windowed = false;
        g_Supervisor.m_Cfg.frameskipConfig = false;
        g_Supervisor.m_Cfg.controllerMapping = g_ControllerMapping;
        g_Supervisor.m_Cfg.effectQuality = MAXIMUM;
        g_Supervisor.m_Cfg.slowMode = 0;
        g_Supervisor.m_Cfg.shotSlow = 0;
        g_Supervisor.m_Cfg.musicVolume = 100;
        g_Supervisor.m_Cfg.sfxVolume = 80;
    }
    else
    {
        g_Supervisor.m_Cfg = *(GameConfiguration *)configFileBuffer;
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
        if (g_Supervisor.m_Cfg.lifeCount >= 7 || g_Supervisor.m_Cfg.bombCount >= 4 ||
            g_Supervisor.m_Cfg.colorMode16bit >= 2 || g_Supervisor.m_Cfg.musicMode >= 3 ||
            g_Supervisor.m_Cfg.defaultDifficulty >= 6 || g_Supervisor.m_Cfg.playSounds >= 2 ||
            g_Supervisor.m_Cfg.windowed >= 2 || g_Supervisor.m_Cfg.frameskipConfig >= 3 ||
            g_Supervisor.m_Cfg.effectQuality >= 3 || g_Supervisor.m_Cfg.slowMode >= 2 || g_Supervisor.m_Cfg.shotSlow >= 2 ||
            g_Supervisor.m_Cfg.version != GAME_VERSION || fileSize != 60)
        {

            g_GameErrorContext.Log(TH_ERR_CONFIG_ABNORMAL);
            memset(&g_Supervisor.m_Cfg, 0, sizeof(GameConfiguration));
            goto SET_DEFAULT;
        }
        g_ControllerMapping = g_Supervisor.m_Cfg.controllerMapping;
    }

    g_Supervisor.m_Cfg.opts.useSwTextureBlending = true; // Bit ignored from PCB onwards (HW blending always used)
    if (this->m_Cfg.opts.dontUseVertexBuf != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_VERTEX_BUFFER);
    }
    if (this->m_Cfg.opts.disableFog != 0)
    {
        g_GameErrorContext.Log(TH_ERR_NO_FOG);
    }
    if (this->m_Cfg.opts.force16bitTextures != false)
    {
        g_GameErrorContext.Log(TH_ERR_USE_16BIT_TEXTURES);
    }
    // This should be inlined
    if (this->ShouldForceBackbufferClear())
    {
        g_GameErrorContext.Log(TH_ERR_FORCE_BACKBUFFER_CLEAR);
    }
    if (this->m_Cfg.opts.displayMinimumGraphics != false)
    {
        g_GameErrorContext.Log(TH_ERR_DONT_RENDER_ITEMS);
    }
    if (this->m_Cfg.opts.suppressUseOfGoroudShading != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_GOURAUD_SHADING);
    }
    if (this->m_Cfg.opts.disableDepthTest != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_DEPTH_TESTING);
    }
    this->m_DisableVsync = false;
    this->m_Cfg.opts.force60Fps = false;

    if (this->m_Cfg.opts.disableColorCompositing != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_TEXTURE_COLOR_COMPOSITING);
    }
    if (this->m_Cfg.windowed != false)
    {
        g_GameErrorContext.Log(TH_ERR_LAUNCH_WINDOWED);
    }
    if (this->m_Cfg.opts.referenceRasterizerMode != false)
    {
        g_GameErrorContext.Log(TH_ERR_FORCE_REFERENCE_RASTERIZER);
    }
    if (this->m_Cfg.opts.dontUseDirectInput != false)
    {
        g_GameErrorContext.Log(TH_ERR_DO_NOT_USE_DIRECTINPUT);
    }
    if (this->m_Cfg.opts.redrawHUDEveryFrame != false)
    {
        g_GameErrorContext.Log(TH_ERR_REDRAW_HUD_EVERY_FRAME);
    }
    if (this->m_Cfg.opts.preloadMusic != false)
    {
        g_GameErrorContext.Log(TH_ERR_PRELOAD_BGM);
    }
    if (this->m_Cfg.opts.disableVsync != false)
    {
        g_GameErrorContext.Log(TH_ERR_NO_VSYNC);
        g_Supervisor.m_DisableVsync = true;
    }
    if (this->m_Cfg.opts.dontDetectTextDrawingBackground != false)
    {
        g_GameErrorContext.Log(TH_ERR_DONT_DETECT_TEXT_BG);
    }

    if (FileSystem::WriteDataToFile(configFile, &g_Supervisor.m_Cfg, sizeof(GameConfiguration)) != 0)
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

    this->m_runningSubthreadHandle = CreateThread(NULL,
                                                  0,
                                                  startFunction,
                                                  startParam,
                                                  0,
                                                  &m_runningSubthreadID);

    this->m_Unk290 = TRUE;

    return (this->m_runningSubthreadHandle != NULL) ? ZUN_SUCCESS : ZUN_ERROR;
}

void Supervisor::ThreadClose()
{
    if (m_runningSubthreadHandle != NULL)
    {
        utils::GuiDebugPrint("info : Sub Thread Close Request\n");
        m_Unk28c = TRUE;

        while (WaitForSingleObject(m_runningSubthreadHandle, 1000) == WAIT_TIMEOUT)
            Sleep(1);

        CloseHandle(m_runningSubthreadHandle);
        m_runningSubthreadHandle = NULL;
        m_Unk28c = FALSE;
    }
}

void Supervisor::InitializeCriticalSections() {
    for (u32 i = 0; i < ARRAY_SIZE_SIGNED(m_CriticalSections); i++)
    {
        InitializeCriticalSection(&m_CriticalSections[i]);
    }
}

i32 Supervisor::EnableFog()
{
    g_SprtCtrl->FlushVertexBuffer();

    if (this->m_FogState != FOG_ENABLED)
    {
        this->m_FogState = FOG_ENABLED;

        return this->m_D3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    }

    return 0;
}

i32 Supervisor::DisableFog()
{
    g_SprtCtrl->FlushVertexBuffer();

    if (this->m_FogState != FOG_DISABLED)
    {
        this->m_FogState = FOG_DISABLED;

        return this->m_D3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    }

    return 0;
}

void Supervisor::SetRenderState(D3DRENDERSTATETYPE renderStateType, int value)
{
    g_SprtCtrl->FlushVertexBuffer();

    this->m_D3dDevice->SetRenderState(renderStateType, value);
}

#pragma var_order(gameTime, difference)
void Supervisor::UpdateGameTime(Supervisor *s)
{
    DWORD gameTime = timeGetTime();

    if (gameTime < s->m_SystemTime)
    {
        s->m_SystemTime = 0;
    }

    DWORD difference = gameTime - s->m_SystemTime;

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

    s->m_SystemTime = gameTime;
}

#pragma var_order(playTime, difference)
void Supervisor::UpdatePlayTime(Supervisor *s)
{
    DWORD playTime = timeGetTime();

    if (playTime < s->m_TotalPlayTime)
    {
        s->m_TotalPlayTime = playTime;
    }

    DWORD difference = playTime - s->m_TotalPlayTime;

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

    s->m_TotalPlayTime = playTime;
}

}; // namespace th08
