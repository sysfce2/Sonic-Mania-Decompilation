#ifndef OBJ_SPIKEFLAIL_H
#define OBJ_SPIKEFLAIL_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
    uint16 sfxFlail;
} ObjectSpikeFlail;

// Entity Class
typedef struct {
    RSDK_ENTITY
    uint8 chainLength;
    uint16 phase512;
    Animator animator1;
    Animator animator2;
    Animator animator3;
    Vector2 origin;
    bool32 playingFlailSfx;
    Hitbox hitbox;
} EntitySpikeFlail;

// Object Struct
extern ObjectSpikeFlail *SpikeFlail;

// Standard Entity Events
void SpikeFlail_Update(void);
void SpikeFlail_LateUpdate(void);
void SpikeFlail_StaticUpdate(void);
void SpikeFlail_Draw(void);
void SpikeFlail_Create(void* data);
void SpikeFlail_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void SpikeFlail_EditorDraw(void);
void SpikeFlail_EditorLoad(void);
#endif
void SpikeFlail_Serialize(void);

// Extra Entity Functions
void SpikeFlail_SetupHitbox(void);
void SpikeFlail_CheckPlayerCollisions(void);
Vector2 SpikeFlail_GetScale(int a1, int angle);

#endif //!OBJ_SPIKEFLAIL_H
