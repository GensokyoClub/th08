#include "ScoreDat.hpp"

namespace th08
{

ScoreDat *ScoreDat::OpenScore(const char *path)
{
    return NULL;
}

ZunResult ScoreDat::ParseCATK(ScoreDat *score, Catk *outCatk)
{
    return ZUN_ERROR;
}

BOOL ScoreDat::ParseFLSP(ScoreDat *score, Flsp *outFlsp)
{
    return FALSE;
}

ZunResult ScoreDat::ParseCLRD(ScoreDat *score, Clrd *outClrd)
{
    return ZUN_ERROR;
}

ZunResult ScoreDat::ParsePSCR(ScoreDat *score, Pscr *outPscr)
{
    return ZUN_ERROR;
}

ZunResult ScoreDat::ParsePLST(ScoreDat *score, Plst *outPlst)
{
    return ZUN_ERROR;
}

void ScoreDat::ReleaseScore(ScoreDat *score)
{
}

} /* namespace th08 */