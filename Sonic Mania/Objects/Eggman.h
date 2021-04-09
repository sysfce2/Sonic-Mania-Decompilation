#ifndef OBJ_EGGMAN_H
#define OBJ_EGGMAN_H

#include "../SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    ushort spriteIndex;
} ObjectEggman;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    StateMachine(stateStore);
    int timer;
    ushort animID;
    Entity* parent;
    Vector2 offset;
    Hitbox hitbox;
    Animator data;
} EntityEggman;

// Object Struct
extern ObjectEggman *Eggman;

// Standard Entity Events
void Eggman_Update(void);
void Eggman_LateUpdate(void);
void Eggman_StaticUpdate(void);
void Eggman_Draw(void);
void Eggman_Create(void* data);
void Eggman_StageLoad(void);
void Eggman_EditorDraw(void);
void Eggman_EditorLoad(void);
void Eggman_Serialize(void);

// Extra Entity Functions
void Eggman_Unknown1(void);
void Eggman_Unknown2(void);
void Eggman_Unknown3(void);
void Eggman_Unknown4(void);
void Eggman_Unknown5(void);
void Eggman_Unknown6(void);
void Eggman_Unknown7(void);

#endif //!OBJ_EGGMAN_H
