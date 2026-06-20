
#pragma once 

// 디지몬 유저 관련
#define DIGIMON_MAX_COUNT									3
#define DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT		200.0f
#define	DIGIMONUSER_FOLLOW_RANGE							2600.0f

#ifdef KEYBOARD_MOVE
#define TAMERUSER_DIGIMON_REMAIN_MOVE_LENGTH_CONSTANT		700.0f
#endif

#define NPC_FT_MASK							90000

enum eRTTI_CLIENT
{
	RTTI_ITEM = RTTI_LIB_MAXCOUNT,		// 9
	RTTI_DIGIMON_USER,
	RTTI_DIGIMON,
	RTTI_TAMER_USER,
	RTTI_TAMER,
	RTTI_MONSTER,
	RTTI_NPC,
	RTTI_EMPLOYMENT,
	RTTI_PAT,
	RTTI_TUTORIAL_TAMER,
	RTTI_TUTORIAL_DIGIMON,
	RTTI_TUTORIAL_MONSTER,
	//RTTI_CARD,
};

#define MAX_DIGIMONUSER_COUNT		3

#include "cSort.h"
#include "DataMng.h"

#include "EffectMng.h"
#include "CharOption.h"

#include "Object_Header.h"
#include "Pat_AI_Mng.h"
#include "Pat.h"
#include "Employment.h"
#include "Digimon.h"
#include "DigimonUser.h"
#include "Tamer.h"
#include "TamerUser.h"
#include "Monster.h"
#include "Npc.h"
#include "NpcMng.h"
#include "TacticsObject.h"
#include "TutorialTamerUser.h"
#include "TutorialDigimonUser.h"
#include "TutorialMonster.h"
#include "CharMng.h"

#include "GameEvent.h"
#include "EventScene.h"
#include "DatsCenter.h"
#ifdef CROSSWARS_SYSTEM
#include "CrossEvent.h"
#endif