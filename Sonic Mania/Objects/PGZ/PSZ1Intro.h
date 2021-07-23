#ifndef OBJ_PSZ1INTRO_H
#define OBJ_PSZ1INTRO_H

#include "SonicMania.h"

#if RETRO_USE_PLUS
// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectPSZ1Intro;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityPSZ1Intro;

// Object Struct
extern ObjectPSZ1Intro *PSZ1Intro;

// Standard Entity Events
void PSZ1Intro_Update(void);
void PSZ1Intro_LateUpdate(void);
void PSZ1Intro_StaticUpdate(void);
void PSZ1Intro_Draw(void);
void PSZ1Intro_Create(void* data);
void PSZ1Intro_StageLoad(void);
void PSZ1Intro_EditorDraw(void);
void PSZ1Intro_EditorLoad(void);
void PSZ1Intro_Serialize(void);

// Extra Entity Functions
#endif

#endif //!OBJ_PSZ1INTRO_H