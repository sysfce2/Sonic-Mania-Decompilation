#include "../SonicMania.h"

ObjectCollapsingPlatform *CollapsingPlatform;

void CollapsingPlatform_Update(void)
{
    EntityCollapsingPlatform *entity = (EntityCollapsingPlatform *)RSDK_sceneInfo->entity;
    entity->visible                  = false;
    if (DebugMode)
        entity->visible = DebugMode->debugActive;

    if (entity->collapseDelay) {
#if RETRO_USE_PLUS
        if (Player) {
            foreach_active(Player, player)
            {
                if (Player_CheckCollisionTouch(player, entity, &entity->hitbox) && player->characterID == ID_MIGHTY && player->jumpAbilityTimer > 1) {
                    StateMachine_Run(entity->state);
                    RSDK.PlaySFX(CollapsingPlatform->sfx_Crumble, 0, 255);
                    if (entity->respawn) {
                        entity->collapseDelay = 0;
                        entity->playerPos.x   = 0;
                    }
                    else {
                        RSDK.ResetEntityPtr(entity, TYPE_BLANK, NULL);
                    }
                    foreach_return;
                }
            }
        }
#endif
        if (--entity->collapseDelay == 0) {
            StateMachine_Run(entity->state);
            RSDK.PlaySFX(CollapsingPlatform->sfx_Crumble, 0, 255);
            if (entity->respawn) {
                entity->collapseDelay = 0;
                entity->playerPos.x   = 0;
            }
            else {
                RSDK.ResetEntityPtr(entity, TYPE_BLANK, NULL);
            }
        }
    }
    else if (Player) {
        entity->direction = FLIP_NONE;
        foreach_active(Player, player)
        {
            if (Player_CheckCollisionTouch(player, entity, &entity->hitbox)
#if RETRO_USE_PLUS
                && (!entity->mightyOnly || (player->characterID == ID_MIGHTY && player->state == Player_State_MightyHammerDrop)) 
#endif
                && !player->sidekick
                && player->onGround && !player->collisionMode && !entity->eventOnly && entity->delay < 0xFFFF) {
                entity->playerPos.x = player->position.x;
#if RETRO_USE_PLUS
                if (player->characterID == ID_MIGHTY && player->jumpAbilityTimer > 1) {
                    StateMachine_Run(entity->state);
                    RSDK.PlaySFX(CollapsingPlatform->sfx_Crumble, 0, 255);
                    if (entity->respawn) {
                        entity->collapseDelay = 0;
                        entity->playerPos.x   = 0;
                    }
                    else {
                        RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
                    }
                    foreach_break;
                }
#endif
            }
        }
        if (entity->playerPos.x) {
            entity->collapseDelay = entity->delay;
            if (!entity->delay) {
                StateMachine_Run(entity->state);
                RSDK.PlaySFX(CollapsingPlatform->sfx_Crumble, 0, 255);
                if (entity->respawn) {
                    entity->collapseDelay = 0;
                    entity->playerPos.x   = 0;
                }
                else {
                    RSDK.ResetEntityPtr(entity, TYPE_BLANK, NULL);
                }
            }
        }
    }
}

void CollapsingPlatform_LateUpdate(void) {}

void CollapsingPlatform_StaticUpdate(void) {}

void CollapsingPlatform_Draw(void)
{
    EntityCollapsingPlatform *entity = (EntityCollapsingPlatform *)RSDK_sceneInfo->entity;
    Vector2 drawPos;
    drawPos.x = RSDK_sceneInfo->entity->position.x;
    drawPos.y = entity->position.y;
    drawPos.x -= entity->size.x >> 1;
    drawPos.y -= entity->size.y >> 1;
    RSDK.DrawLine(drawPos.x - 0x10000, drawPos.y - 0x10000, drawPos.x + entity->size.x, drawPos.y - 0x10000, 0xE0E0E0, 0, INK_NONE, 0);
    RSDK.DrawLine(drawPos.x - 0x10000, entity->size.y + drawPos.y, drawPos.x + entity->size.x, entity->size.y + drawPos.y, 0xE0E0E0, 0, INK_NONE, 0);
    RSDK.DrawLine(drawPos.x - 0x10000, drawPos.y - 0x10000, drawPos.x - 0x10000, drawPos.y + entity->size.y, 0xE0E0E0, 0, INK_NONE, 0);
    RSDK.DrawLine(drawPos.x + entity->size.x, drawPos.y - 0x10000, drawPos.x + entity->size.x, drawPos.y + entity->size.y, 0xE0E0E0, 0, INK_NONE, 0);
    entity->direction = FLIP_NONE;
    RSDK.DrawSprite(&CollapsingPlatform->animData, &drawPos, false);
    drawPos.x += entity->size.x;
    entity->direction = FLIP_X;
    RSDK.DrawSprite(&CollapsingPlatform->animData, &drawPos, false);
    drawPos.y += entity->size.y;
    entity->direction = FLIP_XY;
    RSDK.DrawSprite(&CollapsingPlatform->animData, &drawPos, false);
    drawPos.x -= entity->size.x;
    entity->direction = FLIP_Y;
    RSDK.DrawSprite(&CollapsingPlatform->animData, &drawPos, false);
}

void CollapsingPlatform_Create(void *data)
{
    EntityCollapsingPlatform *entity = (EntityCollapsingPlatform *)RSDK_sceneInfo->entity;
    entity->visible                  = true;
    entity->position.x &= 0xFFF80000;
    entity->position.y &= 0xFFF80000;
    entity->drawFX |= FX_FLIP;
    entity->drawOrder = Zone->drawOrderLow;
    if (entity->targetLayer == 0) {
        entity->targetLayer = Zone->fgLow;
        entity->drawOrder   = Zone->drawOrderLow;
    }
    else {
        entity->targetLayer = Zone->fgHigh;
        entity->drawOrder   = Zone->drawOrderHigh;
    }

    if (!RSDK_sceneInfo->inEditor) {
        entity->active        = ACTIVE_BOUNDS;
        bool32 checkA         = (entity->size.y & 0xFFF00000) == 0;
        bool32 checkB         = (entity->size.y & 0xFFF00000 & 0x80000000) != 0;
        entity->updateRange.x = 0x800000;
        entity->updateRange.y = 0x800000;
        int xOff              = (entity->position.x >> 20) - (entity->size.x >> 21);
        int yOff              = (entity->position.y >> 20) - (entity->size.y >> 21);

        if (!checkA && !checkB) {
            int sx = entity->size.x >> 20;
            int sy = entity->size.y >> 20;
            for (int y = 0; y < sy; ++y) {
                for (int x = 0; x < sx; ++x) {
                    entity->storedTiles[x + y * (entity->size.x >> 20)] = RSDK.GetTileInfo(entity->targetLayer, x + xOff, y + yOff);
                }
            }
        }

        entity->hitbox.right  = entity->size.x >> 17;
        entity->hitbox.left   = -(entity->size.x >> 17);
        entity->hitbox.bottom = entity->size.y >> 17;
        entity->hitbox.top    = -16 - entity->hitbox.bottom;
    }
    switch (entity->type) {
        default:
        case 0: entity->state = CollapsingPlatform_State_Left; break;
        case 1: entity->state = CollapsingPlatform_State_Right; break;
        case 2: entity->state = CollapsingPlatform_State_Center; break;
        case 3: entity->state = CollapsingPlatform_State_LeftOrRight; break;
        case 4: entity->state = CollapsingPlatform_State_PlayerPos; break;
    }
}

void CollapsingPlatform_StageLoad(void)
{
    CollapsingPlatform->spriteIndex = RSDK.LoadSpriteAnimation("Global/TicMark.bin", SCOPE_STAGE);
    RSDK.SetSpriteAnimation(CollapsingPlatform->spriteIndex, 0, &CollapsingPlatform->animData, true, 0);
    if (RSDK.CheckStageFolder("OOZ1") || RSDK.CheckStageFolder("OOZ2"))
        CollapsingPlatform->shift = 1;

    if (RSDK.CheckStageFolder("FBZ") || RSDK.CheckStageFolder("HCZ") || RSDK.CheckStageFolder("LRZ1") || RSDK.CheckStageFolder("LRZ2")
        || RSDK.CheckStageFolder("AIZ")) {
        CollapsingPlatform->sfx_Crumble = RSDK.GetSFX("Stage/LedgeBreak3.wav");
    }
    else {
        CollapsingPlatform->sfx_Crumble = RSDK.GetSFX("Stage/LedgeBreak.wav");
    }
}

void CollapsingPlatform_State_Left(void)
{
    EntityCollapsingPlatform *entity = (EntityCollapsingPlatform *)RSDK_sceneInfo->entity;

    ushort *tiles = entity->storedTiles;
    int startTX   = (RSDK_sceneInfo->entity->position.x >> 20) - (entity->size.x >> 21);
    int startTY   = (RSDK_sceneInfo->entity->position.y >> 20) - (entity->size.y >> 21);
    int tx        = RSDK_sceneInfo->entity->position.x - (entity->size.x >> 1) + 0x80000;
    int ty        = (RSDK_sceneInfo->entity->position.y - (entity->size.y >> 1)) + 0x80000;

    int sx = entity->size.x >> 20;
    int sy = entity->size.y >> 20;

    for (int y = 0; y < sy; ++y) {
        for (int x = 0; x < sx; ++x) {
            EntityBreakableWall *tileChunk = (EntityBreakableWall *)RSDK.CreateEntity(BreakableWall->objectID, (void *)2, tx, ty);
            tx += 0x100000;
            tileChunk->layerID  = entity->targetLayer;
            tileChunk->tileInfo = *tiles;
            tileChunk->drawOrder = entity->drawOrder;
            tileChunk->tilePos.y = y + startTY;
            tileChunk->tilePos.x = x + startTX;
            int timerX           = x >> CollapsingPlatform->shift;
            int timerY           = y >> CollapsingPlatform->shift;
            tileChunk->timer     = 3 * (sy + 2 * timerX - timerY);
            ++tiles;
        }
        tx -= entity->size.x;
        ty += 0x100000;
    }
}
void CollapsingPlatform_State_Right(void)
{
    EntityCollapsingPlatform *entity = (EntityCollapsingPlatform *)RSDK_sceneInfo->entity;

    ushort *tiles = entity->storedTiles;
    int startTX   = (RSDK_sceneInfo->entity->position.x >> 20) - (entity->size.x >> 21);
    int startTY   = (RSDK_sceneInfo->entity->position.y >> 20) - (entity->size.y >> 21);
    int tx        = RSDK_sceneInfo->entity->position.x - (entity->size.x >> 1) + 0x80000;
    int ty        = (RSDK_sceneInfo->entity->position.y - (entity->size.y >> 1)) + 0x80000;

    int sx = entity->size.x >> 20;
    int sy = entity->size.y >> 20;
    for (int y = 0; y < sy; ++y) {
        for (int x = 0; x < sx; ++x) {
            EntityBreakableWall *tileChunk = (EntityBreakableWall *)RSDK.CreateEntity(BreakableWall->objectID, (void *)2, tx, ty);
            tx += 0x100000;
            tileChunk->layerID   = entity->targetLayer;
            tileChunk->tileInfo  = *tiles;
            tileChunk->drawOrder = entity->drawOrder;
            tileChunk->tilePos.y = y + startTY;
            tileChunk->tilePos.x = x + startTX;
            int timerX           = x >> CollapsingPlatform->shift;
            int timerY           = y >> CollapsingPlatform->shift;
            int timerSX          = entity->size.x >> CollapsingPlatform->shift >> 20;
            tileChunk->timer     = 3 * (sy + 2 * (timerSX - timerX) - timerY);
            ++tiles;
        }
        tx -= entity->size.x;
        ty += 0x100000;
    }
}
void CollapsingPlatform_State_Center(void)
{
    EntityCollapsingPlatform *entity = (EntityCollapsingPlatform *)RSDK_sceneInfo->entity;

    ushort *tiles = entity->storedTiles;
    int startTX   = (RSDK_sceneInfo->entity->position.x >> 20) - (entity->size.x >> 21);
    int startTY   = (RSDK_sceneInfo->entity->position.y >> 20) - (entity->size.y >> 21);
    int tx        = RSDK_sceneInfo->entity->position.x - (entity->size.x >> 1) + 0x80000;
    int ty        = (RSDK_sceneInfo->entity->position.y - (entity->size.y >> 1)) + 0x80000;

    int tCountX = entity->size.x >> CollapsingPlatform->shift >> 20;

    int sx = entity->size.x >> 20;
    int sy = entity->size.y >> 20;
    for (int y = 0; y < sy; ++y) {
        for (int x = 0; x < sx; ++x) {
            EntityBreakableWall *tileChunk = (EntityBreakableWall *)RSDK.CreateEntity(BreakableWall->objectID, (void *)2, tx, ty);
            tx += 0x100000;
            tileChunk->layerID   = entity->targetLayer;
            tileChunk->tileInfo  = *tiles;
            tileChunk->drawOrder = entity->drawOrder;
            tileChunk->tilePos.x = x + startTX;
            tileChunk->tilePos.y = y + startTY;
            int timerX           = abs((tCountX >> 1) - (x >> CollapsingPlatform->shift));
            int timerY           = y >> CollapsingPlatform->shift;
            int timerSY          = y >> CollapsingPlatform->shift >> 20;
            tileChunk->timer     = 3 * (timerSY + 2 * timerX - timerY);
            if (!(tCountX & 1) && x >> CollapsingPlatform->shift < (tCountX >> 1))
                tileChunk->timer -= 6;
            ++tiles;
        }
        tx -= entity->size.x;
        ty += 0x100000;
    }
}
void CollapsingPlatform_State_LeftOrRight(void)
{
    EntityCollapsingPlatform *entity = (EntityCollapsingPlatform *)RSDK_sceneInfo->entity;
    int px                           = entity->playerPos.x;
    int x                            = entity->position.x;

    if (px < x)
        CollapsingPlatform_State_Left();
    else
        CollapsingPlatform_State_Right();
}
void CollapsingPlatform_State_PlayerPos(void)
{
    EntityCollapsingPlatform *entity = (EntityCollapsingPlatform *)RSDK_sceneInfo->entity;
    int px = entity->playerPos.x;
    int x = entity->position.x;

    if (abs(px - x) < entity->size.x / 6) {
        CollapsingPlatform_State_Center();
    }
    else {
        if (px < x)
            CollapsingPlatform_State_Left();
        else
            CollapsingPlatform_State_Right();
    }
}

void CollapsingPlatform_EditorDraw(void) {}

void CollapsingPlatform_EditorLoad(void) {}

void CollapsingPlatform_Serialize(void)
{
    RSDK_EDITABLE_VAR(CollapsingPlatform, VAR_VECTOR2, size);
    RSDK_EDITABLE_VAR(CollapsingPlatform, VAR_BOOL, respawn);
    RSDK_EDITABLE_VAR(CollapsingPlatform, VAR_UINT16, targetLayer);
    RSDK_EDITABLE_VAR(CollapsingPlatform, VAR_UINT8, type);
    RSDK_EDITABLE_VAR(CollapsingPlatform, VAR_ENUM, delay);
    RSDK_EDITABLE_VAR(CollapsingPlatform, VAR_BOOL, eventOnly);
    RSDK_EDITABLE_VAR(CollapsingPlatform, VAR_BOOL, mightyOnly);
}
