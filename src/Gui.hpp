#pragma once

#include "Global.hpp"

namespace th08
{

struct GuiImpl
{
};

struct Gui
{
    static ChainCallbackResult OnUpdate(Gui *gui);
    static ChainCallbackResult OnDraw(Gui *gui);

    static ZunResult AddedCallback(Gui *gui);
    static ZunResult DeletedCallback(Gui *gui);

    static ZunResult RegisterChain();
    static void CutChain();

    ZunResult ActualAddedCallback();
    ZunResult LoadMsg(const char *path);
    void FreeMsgFile();
};

DIFFABLE_EXTERN(Gui, g_Gui);

} /* namespace th08 */