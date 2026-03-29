#pragma once

#include "Global.hpp"
#include "ZunResult.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"

namespace th08
{

struct TrackDescriptor
{
    TrackDescriptor()
    {
        memset(this, 0, sizeof(TrackDescriptor));
    }
};

struct MusicRoom
{
    MusicRoom()
    {
        memset(this, 0, sizeof(MusicRoom));
    }

    ZunResult CheckInputEnable();
    ZunResult ProcessInput();

    static ZunResult RegisterChain();
    static ChainCallbackResult OnUpdate(MusicRoom *musicRoom);
    static ChainCallbackResult OnDraw(MusicRoom *musicRoom);
    static ZunResult AddedCallback(MusicRoom *musicRoom);
    static ZunResult DeletedCallback(MusicRoom *musicRoom);
};

} // namespace th08