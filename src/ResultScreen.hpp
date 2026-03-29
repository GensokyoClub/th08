#pragma once

#include "Global.hpp"
#include "ZunResult.hpp"
#include "inttypes.hpp"
#include "utils.hpp"

namespace th08
{

enum ResultScreenState
{
};

struct ResultScreen
{
    ResultScreen()
    {
        memset(this, 0, sizeof(ResultScreen));
    }

    ~ResultScreen()
    {
    }

    static const char *GetStageName(i32 stage);
    static const char *GetCharacterName(i32 character);
    static void WriteScore(ResultScreen *resultScreen);
    static void LogScoreDataToFile(ResultScreen *resultScreen);
    void LinkScoreEx(void *out, int difficulty, i32 character);
    void FreeScore(i32 difficulty, i32 character);
    void HandleCategorySelectScreen();
    void SetState(ResultScreenState state);

    i32 HandleHighScoreDifficultySelect();
    i32 HandleHighScoreCharacterSelect();
    i32 HandleHighScoreScreen();
    i32 HandleSpellCardDifficultySelect();
    i32 HandleSpellCardCharacterSelect();
    i32 HandleSpellCardScreen();
    i32 HandleResultKeyboard();

    static void FormatDate(char *buffer);

    i32 HandleReplaySaveKeyboard();
    ZunResult CheckConfirmButton();
    i32 HandleOtherStatsScreen();
    i32 DrawFinalStats();

    static ZunResult RegisterChain(u32 unk);
    static ChainCallbackResult OnUpdate(ResultScreen *resultScreen);
    static ChainCallbackResult OnDraw(ResultScreen *resultScreen);
    static ZunResult AddedCallback(ResultScreen *resultScreen);
    static ZunResult DeletedCallback(ResultScreen *resultScreen);

    static i32 MoveCursor(ResultScreen *resultScreen, i32 length);
    static i32 MoveShotTypeCursor(ResultScreen *resultScreen, i32 length);
    static i32 MoveCursorHorizontally(ResultScreen *resultScreen, int length);

    unknown_fields(0x0, 0x477b0);
};
C_ASSERT(sizeof(ResultScreen) == 0x477b0);
}; // namespace th08
