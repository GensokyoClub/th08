#include "ScreenEffect.hpp"

namespace th08
{

DIFFABLE_STATIC(i32, g_ScreenEffectCounter);
DIFFABLE_STATIC(ScreenEffect, g_ScreenEffect);

void ScreenEffect::Clear(D3DCOLOR color)
{
}

void ScreenEffect::SetViewport(D3DCOLOR clearColor)
{
}

ChainCallbackResult ScreenEffect::CalcFadeIn(ScreenEffect *screenEffect)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}
void ScreenEffect::DrawSquare(ZunRect *rectDimensions, D3DCOLOR color)
{
}

void ScreenEffect::DrawSquareShaded(ZunRect *rect, D3DCOLOR topLeft, D3DCOLOR topRight, D3DCOLOR bottomLeft,
                                    D3DCOLOR bottomRight)
{
}

ChainCallbackResult ScreenEffect::CalcFadeOut(ScreenEffect *screenEffect)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ScreenEffect *ScreenEffect::RegisterChain(ScreenEffectType effect, i32 ticks, i32 param_3, i32 param_4, i32 param_5,
                                          i32 param_6)
{
    return NULL;
}

ChainCallbackResult ScreenEffect::DrawFullFade(ScreenEffect *screenEffect)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult ScreenEffect::DrawArcadeFade(ScreenEffect *screenEffect)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult ScreenEffect::CalcShake(ScreenEffect *screenEffect)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult ScreenEffect::AddedCallback(ScreenEffect *screenEffect)
{
    return ZUN_SUCCESS;
}

ZunResult ScreenEffect::DeletedCallback(ScreenEffect *screenEffect)
{
    return ZUN_SUCCESS;
}

} /* namespace th08 */