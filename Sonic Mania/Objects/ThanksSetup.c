#include "../SonicMania.h"

ObjectThanksSetup *ThanksSetup;

void ThanksSetup_Update()
{
    RSDK_THIS(ThanksSetup);
    if (entity->state)
        entity->state();
    RSDK_screens->position.x = 0x100 - RSDK_screens->centerX;
}

void ThanksSetup_LateUpdate() {}

void ThanksSetup_StaticUpdate() {}

void ThanksSetup_Draw()
{
    RSDK_THIS(ThanksSetup);
    if (entity->stateDraw)
        entity->stateDraw();
}

void ThanksSetup_Create(void *data)
{
    RSDK_THIS(ThanksSetup);
    if (!RSDK_sceneInfo->inEditor) {
        entity->active           = ACTIVE_ALWAYS;
        entity->visible          = true;
        entity->drawOrder        = 12;
        entity->drawFX           = FX_FLIP;
        entity->state            = ThanksSetup_Unknown2;
        entity->stateDraw        = ThanksSetup_Unknown6;
        entity->timer            = 768;
        entity->offset           = 0x10000;
        entity->scale.x          = 0x200;
        entity->scale.y          = 0x200;
        EntityUIPicture *picture = RSDK_GET_ENTITY(80, UIPicture);
        picture->scale.x         = 0x200;
        picture->scale.y         = 0x200;
        entity->picture          = (Entity*)picture;
    }
}

void ThanksSetup_StageLoad()
{
    ThanksSetup->sfx_Sega = RSDK.GetSFX("Stage/Sega.wav");
    RSDK.ResetEntitySlot(0, ThanksSetup->objectID, 0);
    UIPicture->spriteIndex = RSDK.LoadSpriteAnimation("Thanks/Decorations.bin", SCOPE_STAGE);
    for (int i = 0; i < 16; ++i) {
        RSDK.SoundUnknown1(i);
    }
}

void ThanksSetup_Unknown1()
{
    RSDK_THIS(ThanksSetup);
    entity->angle = (entity->angle - 3) & 0x1FF;

    int angle = entity->angle;
    foreach_active(UIPicture, picture)
    {
        if (!picture->data.animationID) {
            picture->position.x = 0x1000000 + entity->offset * RSDK.Sin512(angle);
            picture->position.y = 0x780000 + entity->offset * RSDK.Cos512(angle);
            angle += 32;
        }
    }
}

void ThanksSetup_Unknown2()
{
    RSDK_THIS(ThanksSetup);

    if (entity->timer <= 0) {
        entity->timer = 0;
        entity->offset += (0x3000 - entity->offset) >> 4;
        entity->state     = ThanksSetup_Unknown3;
        entity->stateDraw = 0;
    }
    else {
        entity->offset += (0x3000 - entity->offset) >> 4;
        entity->timer -= 16;
    }
    ThanksSetup_Unknown1();
}

void ThanksSetup_Unknown3()
{
    RSDK_THIS(ThanksSetup);
    if (++entity->timer > 120) {
        entity->rotation += 4;
        EntityUIPicture *picture = (EntityUIPicture *)entity->picture;
        picture->drawFX          = FX_SCALE;
        picture->scale.x         = RSDK.Cos512(entity->rotation);
        if (entity->rotation == 128) {
            RSDK.SetSpriteAnimation(UIPicture->spriteIndex, 2, &picture->data, true, 0);
            entity->timer = 0;
            entity->state = ThanksSetup_Unknown4;
        }
    }
    entity->offset += (0x3000 - entity->offset) >> 4;
    ThanksSetup_Unknown1();
}

void ThanksSetup_Unknown4()
{
    RSDK_THIS(ThanksSetup);

    if (entity->rotation <= 0) {
        if (++entity->timer > 120) {
            entity->timer     = 0;
            entity->state     = ThanksSetup_Unknown5;
            entity->stateDraw = ThanksSetup_Unknown6;
        }
    }
    else {
        EntityUIPicture *picture = (EntityUIPicture *)entity->picture;
        entity->rotation -= 4;
        picture->scale.x = RSDK.Cos512(entity->rotation);
        if (!entity->rotation) {
            picture->drawFX = FX_NONE;
        }
    }
    ThanksSetup_Unknown1();
}

void ThanksSetup_Unknown5()
{
    RSDK_THIS(ThanksSetup);

    if (entity->timer >= 1024) {
        RSDK_sceneInfo->listPos = 0;
        RSDK.InitSceneLoad();
    }
    else {
        entity->timer += 16;
    }
    entity->offset += (entity->offset - 0x2000) >> 4;
    ThanksSetup_Unknown1();
}

void ThanksSetup_Unknown6()
{
    RSDK_THIS(ThanksSetup);
    RSDK.FillScreen(0, entity->timer, entity->timer - 128, entity->timer - 256);
}

void ThanksSetup_EditorDraw() {}

void ThanksSetup_EditorLoad() {}

void ThanksSetup_Serialize() {}
