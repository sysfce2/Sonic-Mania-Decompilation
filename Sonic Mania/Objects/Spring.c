#include "../SonicMania.h"

ObjectSpring *Spring;

void Spring_Update(void)
{
    RSDK_THIS(Spring);

    if (entity->timer > 0)
        entity->timer--;
    StateMachine_Run(entity->state);
    RSDK.ProcessAnimation(&entity->data);
    if (entity->data.frameID == 8)
        entity->data.animationSpeed = 0;
}

void Spring_LateUpdate(void) {}

void Spring_StaticUpdate(void) {}

void Spring_Draw(void)
{
    RSDK_THIS(Spring);
    RSDK.DrawSprite(&entity->data, NULL, false);
}

void Spring_Create(void *data)
{
    RSDK_THIS(Spring);
    entity->drawFX       = FX_FLIP;
    if (!RSDK_sceneInfo->inEditor) {
        entity->type %= 6;
        if (data) {
            int *pv          = (int *)data;
            entity->type     = (*pv >> 0) & 0xFF;
            entity->flipFlag = (*pv >> 8) & 0xFF;
        }
        RSDK.SetSpriteAnimation(Spring->spriteIndex, entity->type, &entity->data, true, 0);
        entity->active              = ACTIVE_BOUNDS;
        entity->data.animationSpeed = 0;
        entity->updateRange.x       = 0x600000;
        entity->updateRange.y       = 0x600000;
        entity->visible             = true;
        if (entity->planeFilter && ((byte)entity->planeFilter - 1) & 2)
            entity->drawOrder = Zone->drawOrderHigh;
        else
            entity->drawOrder = Zone->drawOrderLow;

        if (entity->type >> 1) {
            if (entity->type >> 1 == 1) {
                entity->direction = entity->flipFlag;
                if (entity->type & 1)
                    entity->velocity.x = 0x100000;
                else
                    entity->velocity.x = 0xA0000;
                if (entity->flipFlag)
                    entity->velocity.x = -entity->velocity.x;
                entity->hitbox.left   = -0x8;
                entity->hitbox.top    = -0xF;
                entity->hitbox.right  = 0x10;
                entity->hitbox.bottom = 0x8;
                entity->state         = Spring_State_Horizontal;
            }
            else if (entity->type >> 1 == 2) {
                entity->direction = entity->flipFlag;
                if (entity->type & 1) {
                    entity->velocity.x = 0xB4000;
                    entity->velocity.y = 0xB4000;
                }
                else {
                    entity->velocity.x = 0x74000;
                    entity->velocity.y = 0x74000;
                }
                if (entity->flipFlag < FLIP_Y)
                    entity->velocity.y = -entity->velocity.y;
                if (entity->flipFlag & FLIP_X)
                    entity->velocity.x = -entity->velocity.x;
                entity->hitbox.left   = -0xC;
                entity->hitbox.top    = -0xB;
                entity->hitbox.right  = 0xC;
                entity->hitbox.bottom = 0xC;
                entity->state         = Spring_State_Diagonal;
            }
        }
        else {
            entity->direction = entity->flipFlag;
            if (entity->type & 1)
                entity->velocity.y = 0x100000;
            else
                entity->velocity.y = 0xA0000;
            if (!entity->flipFlag)
                entity->velocity.y = -entity->velocity.y;
            entity->hitbox.left   = -0x10;
            entity->hitbox.top    = -0x7;
            entity->hitbox.right  = 0x10;
            entity->hitbox.bottom = 0x8;
            entity->state         = Spring_State_Vertical;
        }
    }
}

void Spring_StageLoad(void)
{
    Spring->spriteIndex = RSDK.LoadSpriteAnimation("Global/Springs.bin", SCOPE_STAGE);
    Spring->sfx_Spring  = RSDK.GetSFX("Global/Spring.wav");
}

void Spring_State_Vertical(void)
{
    RSDK_THIS(Spring);
    if (RSDK_sceneInfo->entity->direction == FLIP_NONE) {
        foreach_active(Player, player) {
            if (!entity->planeFilter || player->collisionPlane == ((byte)(entity->planeFilter - 1) & 1)) {
                int col = (entity->type != 0xFF || player->velocity.y >= -0x50000) ? Player_CheckCollisionBox(player, entity, &entity->hitbox)
                                                                                 : Player_CheckCollisionPlatform(player, entity, &entity->hitbox);
                if (col == 1) {
                    int anim = player->playerAnimData.animationID;
                    if (anim == ANI_WALK || (anim > ANI_AIRWALK && anim <= ANI_DASH))
                        player->storedAnim = player->playerAnimData.animationID;
                    else
                        player->storedAnim = ANI_WALK;

                    //if (player->state != Ice_State_FrozenPlayer) {
                        if (player->state == Player_State_RollLock || player->state == Player_State_ForceRoll) {
                            player->state = Player_State_RollLock;
                        }
                        else {
                            RSDK.SetSpriteAnimation(player->spriteIndex, ANI_SPRINGTWIRL, &player->playerAnimData, true, 0);
                            player->state = Player_State_Air;
                        }
                    //}
                    player->onGround                        = 0;
                    player->velocity.y                      = entity->velocity.y;
                    player->tileCollisions                  = 1;
                    entity->data.animationSpeed             = 0x80;
                    entity->data.animationTimer             = 0;
                    entity->data.frameID                    = 1;
                    if (entity->timer == 0) {
                        RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                        entity->timer = 8;
                    }
                }
            }
        }
    }
    else {
        foreach_active(Player, player) {
            if ((!entity->planeFilter || player->collisionPlane == ((byte)(entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 4) {
                //if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state == Player_State_RollLock || player->state == Player_State_ForceRoll)
                        player->state = Player_State_RollLock;
                    else
                        player->state = Player_State_Air;
                //}
                player->onGround                        = 0;
                player->velocity.y                      = entity->velocity.y;
                player->tileCollisions                  = 1;
                entity->data.animationSpeed             = 0x80;
                entity->data.animationTimer             = 0;
                entity->data.frameID                    = 1;
                if (!entity->timer) {
                    RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
}
void Spring_State_Horizontal(void)
{
    RSDK_THIS(Spring);
    if (entity->direction == FLIP_NONE) {
        foreach_active(Player, player) {
            if ((!entity->planeFilter || player->collisionPlane == ((byte)(entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 3 && (!entity->onGround || player->onGround)) {
                if (player->collisionMode == CMODE_ROOF) {
                    player->velocity.x = -entity->velocity.x;
                    player->groundVel  = -entity->velocity.x;
                }
                else {
                    player->velocity.x = entity->velocity.x;
                    player->groundVel  = player->velocity.x;
                }

                //if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state != Player_State_Roll && player->state != Player_State_RollLock && player->state != Player_State_ForceRoll) {
                        if (player->onGround == 1)
                            player->state = Player_State_Ground;
                        else
                            player->state = Player_State_Air;
                    }
                    int anim = player->playerAnimData.animationID;
                    if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                        player->playerAnimData.animationID = ANI_WALK;
                //}
                player->glideTimer          = 16;
                player->skidding            = 0;
                player->pushing             = 0;
                player->direction           = 0;
                player->tileCollisions      = 1;
                entity->data.animationSpeed = 0x80;
                entity->data.animationTimer = 0;
                entity->data.frameID        = 1;
                if (entity->timer == 0) {
                    RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
    else {
        foreach_active(Player, player) {
            if ((!entity->planeFilter || player->collisionPlane == ((byte)(entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 2 && (entity->onGround == false || player->onGround == 1)) {
                if (player->collisionMode == CMODE_ROOF) {
                    player->velocity.x = -entity->velocity.x;
                    player->groundVel  = -entity->velocity.x;
                }
                else {
                    player->velocity.x = entity->velocity.x;
                    player->groundVel  = player->velocity.x;
                }

                //if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state != Player_State_Roll && player->state != Player_State_RollLock && player->state != Player_State_ForceRoll) {
                        if (player->onGround)
                            player->state = Player_State_Ground;
                        else
                            player->state = Player_State_Air;
                    }
                    int anim = player->playerAnimData.animationID;
                    if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                        player->playerAnimData.animationID = ANI_WALK;
                //}
                player->glideTimer          = 16;
                player->skidding            = 0;
                player->pushing             = 0;
                player->direction           = 1;
                player->tileCollisions      = 1;
                entity->data.animationSpeed = 0x80;
                entity->data.animationTimer = 0;
                entity->data.frameID        = 1;
                if (entity->timer == 0) {
                    RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
}
void Spring_State_Diagonal(void)
{
    RSDK_THIS(Spring);
    foreach_active(Player, player) {
        if ((!entity->planeFilter || player->collisionPlane == ((byte)(entity->planeFilter - 1) & 1))) {
            if (Player_CheckCollisionTouch(player, entity, &entity->hitbox)) {
                bool32 flag = false;
                if (player->onGround) {
                    flag = true;
                }
                else {
                    int y = player->velocity.y;
                    if (y >= 0) {
                        flag = true;
                    }
                    else {
                        int x = player->velocity.x;
                        if (x < 0)
                            x = -x;
                        flag = x > -y;
                    }
                }
                if (flag) {
                    // if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state == Player_State_RollLock || player->state == Player_State_ForceRoll) {
                        player->state = Player_State_RollLock;
                    }
                    else {
                        player->state = Player_State_Air;
                        int anim      = player->playerAnimData.animationID;
                        if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                            player->playerAnimData.animationID = ANI_WALK;
                    }
                    //}
                    if (entity->direction < FLIP_Y) {
                        if (player->state != Player_State_RollLock && player->state != Player_State_ForceRoll) {
                            int anim = player->playerAnimData.animationID;
                            if (anim == ANI_WALK || (anim > ANI_AIRWALK && anim <= ANI_DASH))
                                player->storedAnim = player->playerAnimData.animationID;
                            else
                                player->storedAnim = ANI_WALK;
                            RSDK.SetSpriteAnimation(player->spriteIndex, ANI_SPRINGDIAGONAL, &player->playerAnimData, true, 0);
                        }
                    }
                    player->direction           = entity->direction & 1;
                    player->onGround            = 0;
                    player->velocity.x          = entity->velocity.x;
                    player->velocity.y          = entity->velocity.y;
                    player->tileCollisions      = 1;
                    entity->data.animationSpeed = 0x80;
                    entity->data.animationTimer = 0;
                    entity->data.frameID        = 1;
                    if (entity->timer == 0) {
                        RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                        entity->timer = 8;
                    }
                }
            }
        }
    }
}

void Spring_EditorDraw(void) {}

void Spring_EditorLoad(void) {}

void Spring_Serialize(void)
{
    RSDK_EDITABLE_VAR(Spring, VAR_ENUM, type);
    RSDK_EDITABLE_VAR(Spring, VAR_ENUM, flipFlag);
    RSDK_EDITABLE_VAR(Spring, VAR_BOOL, onGround);
    RSDK_EDITABLE_VAR(Spring, VAR_UINT8, planeFilter);
}
