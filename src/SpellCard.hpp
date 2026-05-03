#pragma once

#include "ZunBool.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"

/* TODO: this should be an enum with all the spell cards... */

#define SPELL_CARD_LAST_WORD_START 205

#define SPELL_CARD_COUNT_IN_GAME_SPELLCARDS 205
#define SPELL_CARD_COUNT_SPELLCARDS 222
#define SPELL_CARD_COUNT_LAST_WORD_SPELL_CARDS (SPELL_CARD_COUNT_SPELLCARDS - SPELL_CARD_COUNT_IN_GAME_SPELLCARDS)

namespace th08
{

struct SpellCard
{
    static ZunBool IsSpellLastSpell(i32 spellCardNumber);
    static i32 GetDifficultyFromSpellCard(i32 spellCardNumber);
};

DIFFABLE_EXTERN_ARRAY(i32 *, 6, g_SpellCardNumbersPerDifficulty);
DIFFABLE_EXTERN_ARRAY(i32, 6, g_SpellCardCountsPerDifficulty);
DIFFABLE_EXTERN_ARRAY(i32, 43, g_LastSpellNumbers);
DIFFABLE_EXTERN(i32, g_LastSpellCount);
DIFFABLE_EXTERN_ARRAY(i32 *, 10, g_SpellCardNumbersPerStage)
DIFFABLE_EXTERN_ARRAY(i32, 10, g_SpellCardCountPerStage)

} /* namespace th08 */