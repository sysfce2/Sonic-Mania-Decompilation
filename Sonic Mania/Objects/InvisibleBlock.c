#include "../SonicMania.h"

ObjectInvisibleBlock *InvisibleBlock;

void InvisibleBlock_Update(void)
{
    RSDK_THIS(InvisibleBlock);
    foreach_active(Player, player)
    {
        if ((entity->planeFilter <= 0 || player->collisionPlane == (((byte)entity->planeFilter - 1) & 1)) && (!entity->noChibi || !player->isChibi)) {
            switch (Player_CheckCollisionBox(player, entity, &entity->hitbox)) {
                case 1:
                    if (!entity->noCrush)
                        player->killFlagB |= 1;
                    break;
                case 2:
                    if (!entity->noCrush)
                        player->killFlagA |= 1;
                    break;
                case 3:
                    if (!entity->noCrush)
                        player->killFlagA |= 2;
                    break;
                case 4:
                    if (!entity->noCrush)
                        player->killFlagB |= 2;
                    break;
                default: break;
            }
        }
    }
    entity->visible = DebugMode->debugActive;
}

void InvisibleBlock_LateUpdate(void) {}

void InvisibleBlock_StaticUpdate(void) {}

void InvisibleBlock_Draw(void) { InvisibleBlock_DrawSprites(); }

void InvisibleBlock_Create(void *data)
{
    RSDK_THIS(InvisibleBlock);
    if (!RSDK_sceneInfo->inEditor) {
        if (entity->timeAttackOnly) {
            if (globals->gameMode < MODE_TIMEATTACK)
                RSDK.ResetEntityPtr(entity, TYPE_BLANK, NULL);
        }
        entity->visible = false;
        entity->active  = 2 * (entity->activeNormal == 0) + 2;

        entity->updateRange.x = (entity->width + 5) << 19;
        entity->updateRange.y = (entity->height + 5) << 19;

        entity->hitbox.right  = 8 * entity->width + 8;
        entity->hitbox.left   = -entity->hitbox.right;
        entity->hitbox.bottom = 8 * entity->height + 8;
        entity->hitbox.top    = -(entity->hitbox.top);
        entity->drawOrder     = Zone->drawOrderHigh;
    }
}

void InvisibleBlock_StageLoad(void)
{
    InvisibleBlock->spriteIndex = RSDK.LoadSpriteAnimation("Global/ItemBox.bin", SCOPE_STAGE);
    RSDK.SetSpriteAnimation(InvisibleBlock->spriteIndex, 2, &InvisibleBlock->data, true, 0);
    InvisibleBlock->data.frameID = 10;
}

void InvisibleBlock_DrawSprites(void)
{
    Vector2 drawPos;
    RSDK_THIS(InvisibleBlock);
    drawPos.x                    = RSDK_sceneInfo->entity->position.x;
    drawPos.y                    = entity->position.y;
    drawPos.x -= entity->width << 19;
    drawPos.y -= entity->height << 19;

    for (int y = 0; y <= entity->height; ++y) {
        for (int x = 0; x <= entity->width; ++x) {
            RSDK.DrawSprite(&InvisibleBlock->data, &drawPos, 0);
            drawPos.x += 0x100000;
        }
        drawPos.y += 0x100000;
        drawPos.x = 0xFFF00000 - (entity->width << 20);
    }
}

void InvisibleBlock_EditorDraw(void) {}

void InvisibleBlock_EditorLoad(void) {}

void InvisibleBlock_Serialize(void)
{
    RSDK_EDITABLE_VAR(InvisibleBlock, VAR_UINT8, width);
    RSDK_EDITABLE_VAR(InvisibleBlock, VAR_UINT8, height);
    RSDK_EDITABLE_VAR(InvisibleBlock, VAR_ENUM, planeFilter);
    RSDK_EDITABLE_VAR(InvisibleBlock, VAR_BOOL, noCrush);
    RSDK_EDITABLE_VAR(InvisibleBlock, VAR_BOOL, activeNormal);
    RSDK_EDITABLE_VAR(InvisibleBlock, VAR_BOOL, timeAttackOnly);
    RSDK_EDITABLE_VAR(InvisibleBlock, VAR_BOOL, noChibi);
}
