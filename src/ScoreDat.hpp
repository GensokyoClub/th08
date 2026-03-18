#pragma once

#include "GameManager.hpp"
#include "utils.hpp"

namespace th08
{

struct ScoreDat
{
    static ScoreDat *OpenScore(const char *filename);
    static ZunResult ParsePLST(ScoreDat *score, Plst *outPlst);
    static ZunResult ParseCLRD(ScoreDat *score, Clrd *outClrd);
    static ZunResult ParsePSCR(ScoreDat *score, Pscr *outPscr);
    static ZunResult ParseCATK(ScoreDat *score, Catk *outCatk);
    static BOOL ParseFLSP(ScoreDat *score, Flsp *outFlsp);
    static void ReleaseScore(ScoreDat *score);
};

} /* namespace th08 */
