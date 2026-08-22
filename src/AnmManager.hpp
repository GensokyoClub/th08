#pragma once
#include "AnmVm.hpp"
#include "Supervisor.hpp"
#include "ZunColor.hpp"
#include "ZunResult.hpp"
#include "diffbuild.hpp"
#include "dxutil.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include <d3d8.h>

#define GAME_WINDOW_WIDTH 640
#define GAME_WINDOW_HEIGHT 480

namespace th08
{
struct VertexDiffuseXyzrhw
{
    Float3 pos;
    f32 w;
    D3DCOLOR diffuse;
};

struct VertexTex1DiffuseXyzrhw
{
    Float3 pos;
    float w;
    D3DCOLOR diffuse;
    Float2 textureUV;
};

// Touhou 8 uses DirectX 8.1, but evidently Zun used some mismatched DirectX 8 headers as well
// D3DXIMAGE_INFO changed from 20 to 28 bytes between DX8 and DX8.1, but somehow IN uses the the DX8 version
// This struct is a redefinition of the DX8 D3DXIMAGE_INFO for that
// The only reason this ABI mismatch doesn't cause issues is because no surface indices are ever loaded other than 0
struct ZunImageInfo
{
    u32 Width;
    u32 Height;
    u32 Depth;
    u32 MipLevels;
    D3DFORMAT Format;
};
C_ASSERT(sizeof(ZunImageInfo) == 0x14);

enum AnmBlendMode
{
    AnmBlendMode_Unset = -1,
    AnmBlendMode_Normal,
    AnmBlendMode_Additive
};

enum AnmColorOp
{
    AnmColorOp_Unset = -1
};

enum AnmVertexShader
{
    AnmVertexShader_Unset = -1
};

enum AnmCameraMode
{
    AnmCameraMode_Unset = -1
};

enum AnmZWriteMode
{
    AnmZWriteMode_Unset = -1
};

struct AnmEntry
{
    IDirect3DTexture8 *texture;
    u8 *rawData;
    i32 size;
};

C_ASSERT(sizeof(AnmEntry) == 0xc);

struct AnmRawEntry
{
    i32 numSprites;
    i32 numScripts;
    u32 textureIdx;
    i32 width;
    i32 height;
    u32 format;
    u32 colorKey;
    u32 nameOffset;
    u32 spriteIdxOffset;
    u32 mipmapNameOffset;
    u32 version;
    u32 priority;
    u32 textureOffset;
    u8 hasData;
    /* 3 bytes pad for alignment */
    u32 nextOffset;
    u32 unk2;
};

C_ASSERT(sizeof(AnmRawEntry) == 0x40);

struct AnmTextureHeader
{
    char magic[4]; /* THTX */
    u16 unk0x4;
    i16 format;
    i16 width;
    i16 height;
    u16 unk0x14;
    u16 unk0x18;
};

struct AnmLoaded
{
    i32 anmIdx;
    AnmRawEntry *rawData;
    i32 totalEntries;
    AnmLoadedSprite *sprites;
    AnmRawInstr **scripts;
    AnmEntry *textures;
    int numberEntriesToBeLoaded;

    void LoadSprite(i32 spriteIdx, AnmLoadedSprite *loadedSprite);
    ZunResult SetSprite(AnmVm *vm, i32 spriteIdx);

    void ExecuteAnmIdx(AnmVm *vm, i32 scriptIdx)
    {
        vm->scriptIndex = scriptIdx;

        vm->pos = Float3(0, 0, 0);
        vm->pos2 = Float3(0, 0, 0);

        vm->fontHeight = 15;
        vm->fontWidth = 15;

        this->SetAndExecuteScript(vm, this->scripts[scriptIdx]);
    }

    AnmLoadedSprite *GetSprite(u32 sprite)
    {
        return &this->sprites[sprite];
    }

    void InitializeAndSetSprite(AnmVm *vm, i32 sprite)
    {
        vm->Initialize();
        vm->anmFile = this;
        this->SetSprite(vm, sprite);
    }

    void SetAndExecuteScriptIdx(AnmVm *vm, i32 scriptIdx)
    {
        vm->anmFile = this;
        vm->scriptIndex = scriptIdx;
        this->SetAndExecuteScript(vm, this->scripts[scriptIdx]);
    }

    void ExecuteAnmIdxArray(AnmVm *vm, i32 scriptIdx, i32 count);
    void SetAndExecuteScript(AnmVm *vm, AnmRawInstr *beginningOfScript);
};

C_ASSERT(sizeof(AnmLoaded) == 0x1c);

struct AnmRawSprite
{
    u32 id;
    float x;
    float y;
    float width;
    float height;
};

struct AnmRawScript
{
    u32 id;
    u32 offset;
};

enum
{
    ANM_FILE_TEXT = 0,
    ANM_FILE_ASCII = 1,
    ANM_FILE_NOWLOADING = 2,
    ANM_FILE_CAPTURE = 3,
    ANM_FILE_BACKGROUND = 4,
    ANM_FILE_PLAYER = 5,
    ANM_FILE_BULLET = 6,
    ANM_FILE_ENEMY = 7,
    ANM_FILE_STAGE_ENEMY = 8,
    ANM_FILE_EFFECT = 9,
    ANM_FILE_FRONT = 10,
    ANM_FILE_LOADING = 12,
    ANM_FILE_STAGE_TEXT = 13,
    ANM_FILE_TIMES = 14,
    ANM_FILE_SPELLCARD_BACKGROUND = 15,
    ANM_FILE_SPELLCARD_PLAYER_FACE1 = 16,
    ANM_FILE_SPELLCARD_PLAYER_FACE2 = 17,
    ANM_FILE_SPELLCARD_ENEMY_FACE1 = 18,
    ANM_FILE_SPELLCARD_ENEMY_FACE2 = 19,
    ANM_FILE_TITLE = 20,
    ANM_FILE_RESULT = 21,
    ANM_FILE_RESULTTEXT = 22,
    ANM_FILE_MUSIC = 23,
    ANM_FILE_STAFF = 24,
    ANM_FILE_MAX
};

struct AnmManager
{
    AnmManager();

    // // FUNCTION: th08 0x43ef40 FOLDED
    ~AnmManager()
    {
    }
    void SetupVertexBuffer();

    ZunBool ExecuteScript(AnmVm *vm);
    void ExecuteScriptArray(AnmVm *sprites, int count);
    void SetRenderStateForVm(AnmVm *vm);
    ZunResult DrawInner(AnmVm *vm, i32 flags);
    ZunResult AddSpriteToDrawBuffer(VertexTex1DiffuseXyzrhw *vertices);
    ZunResult DrawNoRotation(AnmVm *vm);
    void TranslateRotation(VertexTex1DiffuseXyzrhw *vertex, float x, float y, float sine, float cosine, float xOffset,
                           float yOffset);
    ZunResult Draw2D(AnmVm *vm);
    ZunResult Draw2DNoRound(AnmVm *vm);
    ZunResult DrawNoRotationNoRound(AnmVm *vm);
    void TransformVerticesWorld(AnmVm *vm);
    ZunResult DrawWorld(AnmVm *vm);
    ZunResult DrawTriangleFan(AnmVm *vm, VertexDiffuseXyzrhw *vertices, i32 vertexCount);
    ZunResult CreateTextureFromFile(IDirect3DTexture8 **outTexture, i32 format, i32 colorKey);
    ZunResult CreateTextureFromAnm(IDirect3DTexture8 **outTexture, AnmTextureHeader *textureData, i32 format);
    ZunResult CreateEmptyTexture(IDirect3DTexture8 **outTexture, i32 width, i32 height, i32 format);
    AnmLoaded *LoadAnm(i32 anmIdx, const char *filename);
    AnmLoaded *ReadAnmEntries(i32 anmIdx, const char *filename);
    AnmLoaded *PreloadAnm(i32 anmIdx, const char *filename);
    i32 LoadExternalTextureData(AnmLoaded *anmLoaded, i32 entryNumber, i32 *sprites, i32 *scripts,
                                AnmRawEntry *rawEntry);
    AnmLoaded *PostloadAnmEntry(AnmLoaded *anm);
    BOOL LoadTextureData(AnmLoaded *anmLoaded, i32 entryNumber, i32 sprites, i32 scripts, AnmRawEntry *rawEntry);
    ZunResult ServicePreloadedAnims();
    void ReleaseAnm(i32 anmIdx);
    void ReleaseAnmEntry(AnmEntry *anmEntry);

    void DrawTextInner(IDirect3DTexture8 *outTexture, i32 x, i32 y, i32 width, i32 height, i32 fontWidth,
                       i32 fontHeight, COLORREF textColor, COLORREF outlineColor, const char *buffer,
                       float scaleFactorX, float scaleFactorY);
    void DrawTextLeft(AnmVm *vm, COLORREF textColor, COLORREF shadowColor, const char *fmt, ...);
    void DrawTextCentered(AnmVm *vm, COLORREF textColor, COLORREF shadowColor, const char *fmt, ...);
    ZunResult LoadSurface(i32 surfaceIdx, const char *path);
    ZunResult PreloadSurface(i32 surfaceIdx, const char *path);
    void ReleaseSurface(i32 surfaceIdx);
    void CopySurfaceToBackbuffer(int surfaceIdx, int left, int top, int x, int y);
    void CopySurfaceToBackbuffer2(i32 surfaceIdx, i32 destX, i32 destY, i32 srcX, i32 srcY, i32 width, i32 height);

    void ReleaseVertexBuffer()
    {
        SAFE_RELEASE(this->quadVertexBuffer);
    }

    void ClearBlendMode()
    {
        this->currentBlendMode = 3;
    }

    void ClearColorOp()
    {
        this->currentColorOp = AnmColorOp_Unset;
    }

    void ClearSprite()
    {
        this->currentSprite = NULL;
    }

    void ClearVertexShader()
    {
        this->currentVertexShader = AnmVertexShader_Unset;
    }

    void ClearTexture()
    {
        this->currentTexture = NULL;
    }

    void ClearCameraSettings()
    {
        this->cameraMode = AnmCameraMode_Unset;
    }

    void ClearZWrite()
    {
        this->disableZWrite = AnmZWriteMode_Unset;
    }

    void ResetFrameDebugInfo()
    {
        this->scriptsExecutedThisFrame = 0;
        this->renderStateChangesThisFrame = 0;
        this->unk0xc = 0;
        this->flushesThisFrame = 0;
    }

    void SetInterruptArray(AnmVm *vm, int count, i16 interrupt);

    ZunBool SpriteHasTexture(AnmVm *vm)
    {
        if (vm->loadedSprite == NULL)
        {
            return FALSE;
        }

        if (vm->loadedSprite->anmIdx < 0)
        {
            return FALSE;
        }

        return this->anmFiles[vm->loadedSprite->anmIdx].textures != NULL;
    }

    void ReleaseSurfaces()
    {
        i32 i;

        for (i = 0; i < ARRAY_SIZE_SIGNED(this->surfaces); i++)
        {
            if (this->surfaces[i] != NULL)
            {
                this->surfaces[i]->Release();
                this->surfaces[i] = NULL;
            }
        }
    }

    void TakeScreencaptures()
    {
        if (this->captureAnmIdx >= 0)
        {
            CaptureToTexture(this->captureAnmIdx, this->textureCaptureSrcX, this->textureCaptureSrcY,
                             this->textureCaptureSrcW, this->textureCaptureSrcH, this->textureCaptureDstX,
                             this->textureCaptureDstY, this->textureCaptureDstW, this->textureCaptureDstH);
            this->captureAnmIdx = -1;
        }

        if (this->captureSurfaceIdx >= 0)
        {
            CaptureToSurface(this->captureSurfaceIdx, this->surfaceCaptureSrcX, this->surfaceCaptureSrcY,
                             this->surfaceCaptureSrcW, this->surfaceCaptureSrcH, this->surfaceCaptureDstX,
                             this->surfaceCaptureDstY, this->surfaceCaptureDstW, this->surfaceCaptureDstH);
            this->captureSurfaceIdx = -1;
        }
    }

    void SetMixColorDefault()
    {
        this->useMixColor = FALSE;
        this->color.d3dColor = 0x80808080;
    }

    void SetMixColor(D3DCOLOR color)
    {
        this->useMixColor = TRUE;
        this->color.d3dColor = color;
    }

    // This function exists according to PoFV
    IDirect3DSurface8 *GetSurface(i32 surfaceIdx)
    {
        return this->surfaces[surfaceIdx];
    }

    ZunResult SetSurfaceCaptureParams(i32 captureSurfaceIdx, i32 srcX, i32 srcY, i32 srcW, i32 srcH, i32 dstX, i32 dstY,
                                      i32 dstW, i32 dstH)
    {
        if (this->captureSurfaceIdx >= 0)
        {
            return ZUN_ERROR;
        }

        this->captureSurfaceIdx = captureSurfaceIdx;
        this->surfaceCaptureSrcX = srcX;
        this->surfaceCaptureSrcY = srcY;
        this->surfaceCaptureSrcW = srcW;
        this->surfaceCaptureSrcH = srcH;
        this->surfaceCaptureDstX = dstX;
        this->surfaceCaptureDstY = dstY;
        this->surfaceCaptureDstW = dstW;
        this->surfaceCaptureDstH = dstH;

        return ZUN_SUCCESS;
    }

    ZunResult SetTextureCaptureParams(i32 idx, i32 srcX, i32 srcY, i32 srcW, i32 srcH, i32 dstX, i32 dstY, i32 dstW,
                                      i32 dstH)
    {
        if (this->captureAnmIdx >= 0)
        {
            return ZUN_ERROR;
        }

        this->captureAnmIdx = idx;
        this->textureCaptureSrcX = srcX;
        this->textureCaptureSrcY = srcY;
        this->textureCaptureSrcW = srcW;
        this->textureCaptureSrcH = srcH;
        this->textureCaptureDstX = dstX;
        this->textureCaptureDstY = dstY;
        this->textureCaptureDstW = dstW;
        this->textureCaptureDstH = dstH;

        return ZUN_SUCCESS;
    }

    void ReplaceSurface(i32 destIndex, i32 srcIndex)
    {
        if (this->surfaces[srcIndex] != NULL)
        {
            this->ReleaseSurface(destIndex);

            this->surfaces[destIndex] = this->surfaces[srcIndex];
            this->surfacesBis[destIndex] = this->surfacesBis[srcIndex];

            this->surfaces[srcIndex] = NULL;
            this->surfacesBis[srcIndex] = NULL;

            this->surfaceInfo[destIndex] = this->surfaceInfo[srcIndex];
        }
    }

    void CaptureToTexture(i32 captureAnmIdx, i32 srcX, i32 srcY, i32 srcW, i32 srcH, i32 dstX, i32 dstY, i32 dstW,
                          i32 dstH);
    void CaptureToSurface(i32 captureSurfaceIdx, i32 srcX, i32 srcY, i32 srcW, i32 srcH, i32 dstX, i32 dstY, i32 dstW,
                          i32 dstH);

    void ClearVertexBuffer();
    void FlushVertexBuffer();

    ZunColor color;
    ZunBool useMixColor;
    i32 captureSurfaceIdx;
    u32 unk0xc;
    u32 scriptsExecutedThisFrame;
    u32 renderStateChangesThisFrame;
    u32 flushesThisFrame;
    Float2 screenShakeOffset;
    AnmLoaded anmFiles[256];
    Float3 unk0x1c24;
    unknown_fields(0x1c30, 0x34);
    AnmVm unk0x1c64;
    unknown_fields(0x1f08, 0x130);

    IDirect3DSurface8 *surfaces[32];
    IDirect3DSurface8 *surfacesBis[32];
    u8 *surfaceData[32];
    u32 surfaceDataSizes[32];
    ZunImageInfo surfaceInfo[32];

    unknown_fields(0x24b8, 0x4);

    IDirect3DTexture8 *currentTexture;
    u8 currentBlendMode;
    u8 currentColorOp;
    u8 currentVertexShader;
    u8 disableZWrite;
    u8 cameraMode;
    unknown_fields(0x24c5, 3); // Padding?
    void *currentSprite;
    IDirect3DVertexBuffer8 *quadVertexBuffer;
    VertexDiffuseXyzrhw untexturedVector[4];
    u32 spritesToDraw;
    VertexTex1DiffuseXyzrhw vertexBuffer[0x18000];
    VertexTex1DiffuseXyzrhw *vertexBufferEndPtr;
    VertexTex1DiffuseXyzrhw *vertexBufferStartPtr;
    i32 captureAnmIdx;
    i32 textureCaptureSrcX;
    i32 textureCaptureSrcY;
    i32 textureCaptureSrcW;
    i32 textureCaptureSrcH;
    i32 textureCaptureDstX;
    i32 textureCaptureDstY;
    i32 textureCaptureDstW;
    i32 textureCaptureDstH;
    i32 surfaceCaptureSrcX;
    i32 surfaceCaptureSrcY;
    i32 surfaceCaptureSrcW;
    i32 surfaceCaptureSrcH;
    i32 surfaceCaptureDstX;
    i32 surfaceCaptureDstY;
    i32 surfaceCaptureDstW;
    i32 surfaceCaptureDstH;
};
C_ASSERT(sizeof(AnmManager) == 0x2a2570);

DIFFABLE_EXTERN(AnmManager *, g_AnmManager);

}; // namespace th08
