#include "ResultScreen.hpp"

namespace th08
{

const char *ResultScreen::GetStageName(i32 stage)
{
    return NULL;
}

const char *ResultScreen::GetCharacterName(i32 character)
{
    return NULL;
}

void ResultScreen::WriteScore(ResultScreen *resultScreen)
{
}

void ResultScreen::LogScoreDataToFile(ResultScreen *resultScreen)
{
}

void ResultScreen::LinkScoreEx(void *out, int difficulty, i32 character)
{
}

void ResultScreen::FreeScore(i32 difficulty, i32 character)
{
}

void ResultScreen::HandleCategorySelectScreen()
{
}

void ResultScreen::SetState(ResultScreenState state)
{
}

i32 ResultScreen::HandleHighScoreDifficultySelect()
{
    return 0;
}

i32 ResultScreen::HandleHighScoreCharacterSelect()
{
    return 0;
}

i32 ResultScreen::HandleHighScoreScreen()
{
    return 0;
}

i32 ResultScreen::HandleSpellCardDifficultySelect()
{
    return 0;
}

i32 ResultScreen::HandleSpellCardCharacterSelect()
{
    return 0;
}

i32 ResultScreen::HandleSpellCardScreen()
{
    return 0;
}

i32 ResultScreen::HandleResultKeyboard()
{
    return 0;
}

void ResultScreen::FormatDate(char *buffer)
{
}

i32 ResultScreen::HandleReplaySaveKeyboard()
{
    return 0;
}

ZunResult ResultScreen::CheckConfirmButton()
{
    return ZUN_SUCCESS;
}

i32 ResultScreen::HandleOtherStatsScreen()
{
    return 0;
}

i32 ResultScreen::DrawFinalStats()
{
    return 0;
}

ZunResult ResultScreen::RegisterChain(u32 unk)
{
    return ZUN_SUCCESS;
}

ChainCallbackResult ResultScreen::OnUpdate(ResultScreen *resultScreen)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult ResultScreen::OnDraw(ResultScreen *resultScreen)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult ResultScreen::AddedCallback(ResultScreen *resultScreen)
{
    return ZUN_SUCCESS;
}

ZunResult ResultScreen::DeletedCallback(ResultScreen *resultScreen)
{
    return ZUN_SUCCESS;
}

i32 ResultScreen::MoveCursor(ResultScreen *resultScreen, i32 length)
{
    return 0;
}
i32 ResultScreen::MoveShotTypeCursor(ResultScreen *resultScreen, i32 length)
{
    return 0;
}

i32 ResultScreen::MoveCursorHorizontally(ResultScreen *resultScreen, int length)
{
    return 0;
}

} /* namespace th08 */