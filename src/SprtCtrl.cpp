#include "SprtCtrl.hpp"

namespace th08
{
DIFFABLE_STATIC(SprtCtrl *, g_SprtCtrl);

void SprtCtrl::ExecuteAnms(Sprt *sprite, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (sprite->scriptIndex > -1)
        {
            g_SprtCtrl->ExecuteScript(sprite);
        }
        sprite++;
    }
}

}; // Namespace th08
