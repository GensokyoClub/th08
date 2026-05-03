#include "SpellCard.hpp"
#include "utils.hpp"

namespace th08
{

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 42, g_SpellCardNumbersEasy) =
{
    2,   6,   13,  17,  21,  25,  32,  39,  43,  47,  54,  58,  62,  66,  70,
    77,  81,  85,  89,  93,  100, 104, 108, 112, 119, 123, 127, 131, 135, 139,
    143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 49, g_SpellCardNumbersNormal) =
{
    3,   7,   10,  14,  18,  22,  26,  29,  33,  36,  40,  44,  48,  51,  55,
    59,  63,  67,  71,  74,  78,  82,  86,  90,  94,  97,  101, 105, 109, 113,
    116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172,
    176, 180, 184, 188
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 50, g_SpellCardNumbersHard) =
{
    0,   4,   8,   11,  15,  19,  23,  27,  30,  34,  37,  41,  45,  49,  52,
    56,  60,  64,  68,  72,  75,  79,  83,  87,  91,  95,  98,  102, 106, 110,
    114, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169,
    173, 177, 181, 185, 189
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 50, g_SpellCardNumbersLunatic) =
{
    1,   5,   9,   12,  16,  20,  24,  28,  31,  35,  38,  42,  46,  50,  53,
    57,  61,  65,  69,  73,  76,  80,  84,  88,  92,  96,  99,  103, 107, 111,
    115, 118, 122, 126, 130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170,
    174, 178, 182, 186, 190
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 14, g_SpellCardNumbersExtra) =
{
    191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, SPELL_CARD_COUNT_SPELLCARDS, g_SpellCardNumbersAllDifficulties) =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
    45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
    60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,
    75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
    90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32 *, 6, g_SpellCardNumbersPerDifficulty) =
{
    g_SpellCardNumbersEasy,
    g_SpellCardNumbersNormal,
    g_SpellCardNumbersHard,
    g_SpellCardNumbersLunatic,
    g_SpellCardNumbersExtra,
    g_SpellCardNumbersAllDifficulties
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 6, g_SpellCardCountsPerDifficulty) =
{
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersEasy),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersNormal),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersHard),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersLunatic),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersExtra),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersAllDifficulties)
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 13, g_SpellCardNumbersStage1) =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 19, g_SpellCardNumbersStage2) =
{
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 22, g_SpellCardNumbersStage3) =
{
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 23, g_SpellCardNumbersStage4A) =
{
    54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72,
    73, 74, 75, 76
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 23, g_SpellCardNumbersStage4B) =
{
    77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 19, g_SpellCardNumbersStage5) =
{
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
    115, 116, 117, 118
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 28, g_SpellCardNumbersStage6A) =
{
    119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133,
    134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146,
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 44, g_SpellCardNumbersStage6B) =
{
    147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161,
    162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
    177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 14, g_SpellCardNumbersStageExtra) =
{
    191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 17, g_SpellCardNumbersLastWord) =
{
    SPELL_CARD_LAST_WORD_START, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
    220, 221
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 43, g_LastSpellNumbers) =
{
    10,  11,  12,       /* Wriggle Nightbug          */
    29,  30,  31,       /* Mystia Lorelei            */
    51,  52,  53,       /* Keine Kamishirasawa       */
    74,  75,  76,       /* Reimu Hakurei             */
    97,  98,  99,       /* Marisa Kirisame           */
    116, 117, 118,      /* Reisen U. Inaba           */
    143, 144, 145, 146, /* Eirin Yagokoro            */

    171, 172, 173, 174, /* Kaguya Houraisan, Spell 1 */
    175, 176, 177, 178, /* Kaguya Houraisan, Spell 2 */
    179, 180, 181, 182, /* Kaguya Houraisan, Spell 3 */
    183, 184, 185, 186, /* Kaguya Houraisan, Spell 4 */
    187, 188, 189, 190, /* Kaguya Houraisan, Spell 5 */

    204                 /* Fujiwara no Mokou   */
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32 *, 10, g_SpellCardNumbersPerStage) =
{
    g_SpellCardNumbersStage1,
    g_SpellCardNumbersStage2,
    g_SpellCardNumbersStage3,
    g_SpellCardNumbersStage4A,
    g_SpellCardNumbersStage4B,
    g_SpellCardNumbersStage5,
    g_SpellCardNumbersStage6A,
    g_SpellCardNumbersStage6B,
    g_SpellCardNumbersStageExtra,
    g_SpellCardNumbersLastWord
};

DIFFABLE_STATIC_ARRAY_ASSIGN(i32, 10, g_SpellCardCountPerStage) =
{
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersStage1),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersStage2),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersStage3),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersStage4A),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersStage4B),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersStage5),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersStage6A),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersStage6B),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersExtra),
    ARRAY_SIZE_SIGNED(g_SpellCardNumbersLastWord)
};

ZunBool SpellCard::IsSpellLastSpell(i32 spellCardNumber)
{
    return 0;
}

i32 SpellCard::GetDifficultyFromSpellCard(i32 spellCardNumber)
{
    return 0;
}

} /* namespace th08 */