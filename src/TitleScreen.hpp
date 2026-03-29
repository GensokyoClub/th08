#pragma once

#include "Global.hpp"
#include "ZunResult.hpp"

namespace th08
{

struct TitleScreen
{
    TitleScreen();
    ~TitleScreen();

    static ChainCallbackResult OnUpdate(TitleScreen *titleScreen);
    static ChainCallbackResult OnDraw(TitleScreen *titleScreen);
    ZunResult ActualAddedCallback();
    static void TitleSetupThread(void);
    static void DisplayInfoImage(const char *path);
    static ZunResult RegisterChain(int param);

    static ZunResult AddedCallback(TitleScreen *titleScreen);
    static ZunResult DeletedCallback(TitleScreen *titleScreen);
};

} // namespace th08