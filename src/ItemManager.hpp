#pragma once

#include "AnmManager.hpp"
#include "Supervisor.hpp"

namespace th08
{

struct Item
{
    AnmVm sprite;

    D3DXVECTOR3 currentPosition;
    D3DXVECTOR3 startPosition;
    D3DXVECTOR3 targetPosition;

    ZunTimer timer;

    i8 itemType;
    i8 isInUse;
    i8 isOnscreen;
    i8 state;
    i8 isMaxValue;

    Item *next;
    Item *prev;

    void CollectPowerSmall();
    void CollectPoint();
    void CollectPointSmall();
    void CollectPowerBig();
    void CollectTimeOrb();

    void Delete();
};

C_ASSERT(sizeof(Item) == 0x2e4);

struct ItemManager
{
    Item items[2097];

    i32 nextIndex;
    u32 itemCount;

    Item itemListHead;
    Item *listListTail;

    Item *SpawnItem(D3DXVECTOR3 *position, int itemType, int state);
    void UpdatePointItemExtendThreshold();
    void OnUpdate();
    void AutoCollectAllItems();
    void ConventAllPowerItemsToTimeOrbs(Item *item);
    void CancelAutoCollect();
    void OnDraw();
    i32 GetTimeOrbCount();
};

C_ASSERT(sizeof(ItemManager) == 0x17b094);

DIFFABLE_EXTERN(ItemManager, g_ItemManager);

}; // namespace th08