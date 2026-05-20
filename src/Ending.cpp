#include "Ending.hpp"

namespace th08
{

// STUB: th08 0x428890
ZunResult Ending::ReadEndFileParameter()
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x428910
void Ending::FadingEffect()
{
}

// STUB: th08 0x428b80
ZunResult Ending::ParseEndFile()
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x4295e0
ZunResult Ending::LoadEnding(const char *path)
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x4296a0
ZunResult Ending::RegisterChain()
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x429860
ChainCallbackResult Ending::OnUpdate(Ending *ending)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

// STUB: th08 0x4298f0
ChainCallbackResult Ending::OnDraw(Ending *ending)
{
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

// STUB: th08 0x429980
ZunResult Ending::AddedCallback(Ending *ending)
{
    return ZUN_SUCCESS;
}

// STUB: th08 0x429d40
ZunResult Ending::DeletedCallback(Ending *ending)
{
    return ZUN_SUCCESS;
}

} /* namespace th08 */