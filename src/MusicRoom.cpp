#include "MusicRoom.hpp"

namespace th08
{

ZunResult MusicRoom::CheckInputEnable()
{
    return ZUN_SUCCESS;
}

ZunResult MusicRoom::ProcessInput()
{
    return ZUN_SUCCESS;
}

ZunResult MusicRoom::RegisterChain()
{
    return ZUN_SUCCESS;
}

ChainCallbackResult MusicRoom::OnUpdate(MusicRoom *musicRoom)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult MusicRoom::OnDraw(MusicRoom *musicRoom)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult MusicRoom::AddedCallback(MusicRoom *musicRoom)
{
    return ZUN_SUCCESS;
}

ZunResult MusicRoom::DeletedCallback(MusicRoom *musicRoom)
{
    return ZUN_SUCCESS;
}

} /* namespace th08 */