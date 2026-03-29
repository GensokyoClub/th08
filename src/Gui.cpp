#include "Gui.hpp"

namespace th08
{

DIFFABLE_STATIC(Gui, g_Gui);
DIFFABLE_STATIC(ChainElem, g_GuiCalcChain);
DIFFABLE_STATIC(ChainElem, g_GuiDrawChain);

ChainCallbackResult Gui::OnUpdate(Gui *gui)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult Gui::OnDraw(Gui *gui)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult Gui::AddedCallback(Gui *gui)
{
    return ZUN_SUCCESS;
}

ZunResult Gui::DeletedCallback(Gui *gui)
{
    return ZUN_SUCCESS;
}

ZunResult Gui::RegisterChain()
{
    return ZUN_SUCCESS;
}

void Gui::CutChain()
{
}

ZunResult Gui::ActualAddedCallback()
{
    return ZUN_SUCCESS;
}

ZunResult Gui::LoadMsg(const char *path)
{
    return ZUN_SUCCESS;
}

void Gui::FreeMsgFile(void)
{
}

} /* namespace th08 */