#include "TitleScreen.hpp"

namespace th08
{

ChainCallbackResult TitleScreen::OnUpdate(TitleScreen *titleScreen)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}
ChainCallbackResult TitleScreen::OnDraw(TitleScreen *titleScreen)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult TitleScreen::ActualAddedCallback()
{
    return ZUN_SUCCESS;
}

void TitleScreen::TitleSetupThread(void)
{
}

void TitleScreen::DisplayInfoImage(const char *path)
{
}

TitleScreen::~TitleScreen()
{
}

ZunResult TitleScreen::RegisterChain(int param)
{
    return ZUN_SUCCESS;
}

TitleScreen::TitleScreen()
{
    memset(this, 0, sizeof(TitleScreen));
}

ZunResult TitleScreen::AddedCallback(TitleScreen *titleScreen)
{
    return ZUN_SUCCESS;
}

ZunResult TitleScreen::DeletedCallback(TitleScreen *titleScreen)
{
    return ZUN_SUCCESS;
}

} /* namespace th08 */