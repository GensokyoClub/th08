#pragma once

#include "AnmManager.hpp"
#include "Global.hpp"

namespace th08
{

struct BulletTypeSprites
{
    AnmVm spriteBullet;
    AnmVm spriteSpawnEffectFast;
    AnmVm spriteSpawnEffectNormal;
    AnmVm spriteSpawnEffectSlow;
    AnmVm spriteDespawnEffect;
    Float3 hitboxSize;
    u8 unk_d40;
    u8 bulletHeight;
};

C_ASSERT(sizeof(BulletTypeSprites) == 0xd44);

struct BulletEx
{
    float unk0x0;
    float unk0x4;
    i32 unk0x8;
    i32 unk0xc;
    i32 unk0x10;
    i32 unk0x14;
};

C_ASSERT(sizeof(BulletEx) == 0x18);

struct BulletExState
{
    ZunTimer timer;
    f32 unk0xc;
    f32 unk0x10;
    Float3 unk0x14;
    i32 unk0x20;
    i32 unk0x24;
    i32 unk0x28;
};

C_ASSERT(sizeof(BulletExState) == 0x2c);

struct Bullet
{
    BulletTypeSprites sprites;
    Float3 pos;
    Float3 velocity;
    Float3 ex4Acceleration; // Leftover from EoSD
    float speed;
    float ex5Float0;      // Leftover from EoSD
    float dirChangeSpeed; // Leftover from EoSD
    float angle;
    float ex5Float1;         // Leftover from EoSD
    float dirChangeRotation; // Leftover from EoSD
    ZunTimer timeSinceBulletFired;
    ZunTimer timeActive;
    i32 ex5Int0;           // Leftover from EoSD
    i32 dirChangeInterval; // Leftover from EoSD
    i32 dirChangeNumTimes; // Leftover from EoSD
    i32 dirChangeMaxTimes; // Leftover from EoSD
    i32 despawnProtectionFrames;
    u32 exFlags;
    u32 flags;
    i16 spriteOffset;
    u16 unk_db6;
    u16 state;
    u16 despawnCounter;
    u8 unk_dbc;
    u8 isGrazed;
    u8 unk_dbe;

    Bullet *nextInLayer;
    i32 reimuBarrierCooldownFrames;
    SoundIdx transformSfx;
    i32 currentExIndex;
    BulletEx ex[18];
    BulletExState exState[7];
    u8 isUnderReisenIllusion;
};

struct Laser
{
    AnmVm vm0;
    AnmVm vm1;
    Float3 position;
    float angle;
    float startOffset;
    float endOffset;
    float length;
    float width;
    float width2;
    float speed;
    i32 startTime;
    i32 startHitboxTime;
    i32 duration;
    i32 stopTime;
    i32 stopHitboxTime;
    ZunBool isInUse;
    ZunTimer tmier;
    u16 flags;
    i16 color;
    u8 state;
    i8 unk_599;
};

C_ASSERT(sizeof(Laser) == 0x59c);

#define NUM_ENEMY_BULLET_TYPES 32
#define MAX_ENEMY_BULLETS 0x600
#define MAX_ENEMY_LASERS 0x100
#define NUM_BULLET_LAYERS 6

struct BulletManager
{
    void Initialize();

    // STUB: th08 0x415c60
    void FUN_00415c60()
    {
    }

    static ZunResult RegisterChain(const char *filename);
    static ChainCallbackResult OnUpdate(BulletManager *bulletManager);
    static ChainCallbackResult OnDraw(BulletManager *bulletManager);
    static ZunResult AddedCallback(BulletManager *bulletManager);
    static ZunResult DeletedCallback(BulletManager *bulletManager);
    static void CutChain();

    BulletTypeSprites bulletTypeTemplates[NUM_ENEMY_BULLET_TYPES];
    Bullet bullets[MAX_ENEMY_BULLETS + 1];
    Laser lasers[MAX_ENEMY_LASERS];
    i32 numActiveBullets;
    i32 cancelFramesRemaining;
    ZunTimer timer;
    i32 unkCounter;
    char *bulletAnmFilename;
    Bullet *layerListHeads[NUM_BULLET_LAYERS];
    Bullet *nextBulletSlot;
    i32 bonusItemType;
    AnmLoaded *bulletAnm;
};

C_ASSERT(sizeof(BulletManager) == 0x6ba578);

DIFFABLE_EXTERN(BulletManager, g_BulletManager);

} /* namespace th08 */
