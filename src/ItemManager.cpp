#include "ItemManager.hpp"

namespace th08
{

DIFFABLE_STATIC(ItemManager, g_ItemManager);

Item *ItemManager::SpawnItem(D3DXVECTOR3 *position, int itemType, int state)
{
    return NULL;
}

void ItemManager::UpdatePointItemExtendThreshold()
{
}

void ItemManager::OnUpdate()
{
}

void Item::CollectPowerSmall()
{
}

void Item::CollectPoint()
{
}

void Item::CollectPointSmall()
{
}

void Item::CollectPowerBig()
{
}

void Item::CollectTimeOrb()
{
}

void ItemManager::AutoCollectAllItems()
{
}

void ItemManager::ConventAllPowerItemsToTimeOrbs(Item *item)
{
}

void ItemManager::CancelAutoCollect()
{
}

void ItemManager::OnDraw()
{
}

void Item::Delete()
{
}

i32 ItemManager::GetTimeOrbCount()
{
    return 0;
}

} /* namespace th08 */