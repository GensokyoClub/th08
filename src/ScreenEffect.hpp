#pragma once

#include "Global.hpp"
#include "inttypes.hpp"
#include "utils.hpp"
#include <d3dx8.h>

namespace th08
{

enum ScreenEffectType
{
};

struct ScreenEffect
{
    static void Clear(D3DCOLOR color);
    static void SetViewport(D3DCOLOR clearColor);

    static ChainCallbackResult CalcFadeIn(ScreenEffect *screenEffect);
    static void DrawSquare(ZunRect *rectDimensions, D3DCOLOR color);
    static void DrawSquareShaded(ZunRect *rect, D3DCOLOR topLeft, D3DCOLOR topRight, D3DCOLOR bottomLeft,
                                 D3DCOLOR bottomRight);
    static ChainCallbackResult CalcFadeOut(ScreenEffect *screenEffect);

    static ScreenEffect *RegisterChain(ScreenEffectType effect, i32 ticks, i32 param_3, i32 param_4, i32 param_5,
                                       i32 param_6);

    static ChainCallbackResult DrawFullFade(ScreenEffect *screenEffect);

    static ChainCallbackResult DrawArcadeFade(ScreenEffect *screenEffect);
    static ChainCallbackResult CalcShake(ScreenEffect *screenEffect);

    static ZunResult AddedCallback(ScreenEffect *screenEffect);
    static ZunResult DeletedCallback(ScreenEffect *screenEffect);

    unknown_fields(0x0, 0x34);
};
}; // Namespace th08
