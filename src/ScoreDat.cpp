#include "ScoreDat.hpp"

namespace th08
{

i32 ScoreDat::LinkScore(ScoreListNode *prevNode, Hscr *newScore)
{
    return 0;
}

void ScoreDat::FreeAllScores(ScoreDat *score)
{
}

ScoreDat *ScoreDat::OpenScore(const char *filename)
{
    return NULL;
}

u32 ScoreDat::GetHighScore(ScoreDat *score, ScoreListNode *node, u32 character, u32 difficulty, u8 *continuesUsed)
{
    return 0;
}

i32 ScoreDat::ParseCATK(ScoreDat *score, Catk *outCatk)
{
    return 0;
}

i32 ScoreDat::ParseLSNM(ScoreDat *score, Lsnm *outLsnm)
{
    return 0;
}

i32 ScoreDat::ParseFLSP(ScoreDat *score, Flsp *outFlsp)
{
    return 0;
}

i32 ScoreDat::ParseCLRD(ScoreDat *score, Clrd *outClrd)
{
    return 0;
}

i32 ScoreDat::ParsePSCR(ScoreDat *score, Pscr *outPscr)
{
    return 0;
}

i32 ScoreDat::ParsePLST(ScoreDat *score, Plst *outPlst)
{
    return 0;
}

void ScoreDat::ReleaseScore(ScoreDat *score)
{
}

} /* namespace th08 */