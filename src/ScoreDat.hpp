#pragma once

#include "GameManager.hpp"
#include "utils.hpp"

namespace th08
{

struct ScoreListNode
{
};

struct ScoreDat
{
    static i32 LinkScore(ScoreListNode *prevNode, Hscr *newScore);
    static void FreeAllScores(ScoreDat *score);
    static ScoreDat *OpenScore(const char *filename);
    static u32 GetHighScore(ScoreDat *score, ScoreListNode *node, u32 character, u32 difficulty, u8 *continuesUsed);
    static i32 ParseCATK(ScoreDat *score, Catk *outCatk);
    static i32 ParseLSNM(ScoreDat *score, Lsnm *outLsnm);
    static i32 ParseFLSP(ScoreDat *score, Flsp *outFlsp);
    static i32 ParseCLRD(ScoreDat *score, Clrd *outClrd);
    static i32 ParsePSCR(ScoreDat *score, Pscr *outPscr);
    static i32 ParsePLST(ScoreDat *score, Plst *outPlst);
    static void ReleaseScore(ScoreDat *score);
};

} /* namespace th08 */
