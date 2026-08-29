#include "th_pch.h"

#include "Background.hpp"

namespace th08
{
DIFFABLE_STATIC(Background, g_Background);

Background::Background()
{
    memset(this, 0, sizeof(Background));

    this->camera.unk0x0 = D3DXVECTOR3(0.0f, 0.0f, 1000.0f);
    this->camera.unk0xc = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    this->camera.unk0x18 = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    this->camera.fov = D3DXToRadian(30);

    this->cameraInterpFinal = this->camera;
    this->cameraInterpInitial = this->camera;
}

// STUB: th08 0x407400
ChainCallbackResult Background::OnUpdate(Background *background)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

// STUB: th08 0x409200
ChainCallbackResult Background::OnDrawHighPrio(Background *background)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

// STUB: th08 0x409640
ChainCallbackResult Background::OnDrawLowPrio(Background *background)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

// STUB: th08 0x409850
ZunResult Background::AddedCallback(Background *background)
{
    return ZUN_ERROR;
}

// STUB: th08 0x409b20
ZunResult Background::RegisterChain(u32 stage)
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x409c20
ZunResult Background::DeletedCallback()
{
    return ZUN_ERROR;
}

// STUB: th08 0x409ca0
void Background::CutChain()
{
}

// STUB: th08 0x409ce0
ZunResult Background::LoadStageData()
{
    return ZUN_ERROR;
}

}; // Namespace th08
