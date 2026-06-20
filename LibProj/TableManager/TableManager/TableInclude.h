
#pragma once

#define  BEGIN_NAME_TABLE		namespace Tb{
#define  END_NAME_TABLE			}

typedef size_t (*PackDataLoadFun)( int, LPCSTR, dm_bytevec& );

#include "Singleton.h"
#include "TableDefs.h"
#include "TableTmpl.h"
#include "TableMgr.h"

//////////////////////////////////////////////////////////////////////////
// Data Struct
//////////////////////////////////////////////////////////////////////////
#include ".\TableStruct\TableStruct_Achieve.h"
#include ".\TableStruct\TableStruct_Buff.h"
#include ".\TableStruct\TableStruct_Cashshop.h"
#include ".\TableStruct\TableStruct_CoolTime.h"
#include ".\TableStruct\TableStruct_CreateCharacter.h"
#include ".\TableStruct\TableStruct_DBook.h"
#include ".\TableStruct\TableStruct_DEvolution.h"
#include ".\TableStruct\TableStruct_Digimon.h"
#include ".\TableStruct\TableStruct_DSkillLevelup.h"
#include ".\TableStruct\TableStruct_InfiniteWar.h"
#include ".\TableStruct\TableStruct_Talk.h"
#include ".\TableStruct\TableStruct_ItemCreation.h"
#include ".\TableStruct\TableStruct_Item.h"
#include ".\TableStruct\TableStruct_ItemRank.h"
#include ".\TableStruct\TableStruct_ItemExchange.h"
#include ".\TableStruct\TableStruct_BaseStats.h"
#include ".\TableStruct\TableStruct_MapsInfo.h"
#include ".\TableStruct\TableStruct_Guild.h"

//////////////////////////////////////////////////////////////////////////
// Data Class
//////////////////////////////////////////////////////////////////////////
#include ".\TableClass\TableClass_Achieve.h"
#include ".\TableClass\TableClass_BaseStats.h"
#include ".\TableClass\TableClass_Buff.h"
#include ".\TableClass\TableClass_Cashshop.h"
#include ".\TableClass\TableClass_CoolTime.h"
#include ".\TableClass\TableClass_CreateCharacter.h"
#include ".\TableClass\TableClass_DBook.h"
#include ".\TableClass\TableClass_DEvolution.h"
#include ".\TableClass\TableClass_Digimon.h"
#include ".\TableClass\TableClass_DSkillLevelUp.h"
#include ".\TableClass\TableClass_Exp.h"
#include ".\TableClass\TableClass_Guild.h"
#include ".\TableClass\TableClass_InfiniteWar.h"
#include ".\TableClass\TableClass_Item.h"
#include ".\TableClass\TableClass_ItemCreation.h"
#include ".\TableClass\TableClass_ItemExchange.h"
#include ".\TableClass\TableClass_ItemRank.h"
#include ".\TableClass\TableClass_JumpBuster.h"
#include ".\TableClass\TableClass_MapsInfo.h"
#include ".\TableClass\TableClass_Monster.h"
#include ".\TableClass\TableClass_Tactics.h"
#include ".\TableClass\TableClass_Talk.h"
#include ".\TableClass\TableClass_UIText.h"

//////////////////////////////////////////////////////////////////////////
// Language Pack Class
//////////////////////////////////////////////////////////////////////////
#include ".\TableString\TableString_Achieve.h"
#include ".\TableString\TableString_Buff.h"
#include ".\TableString\TableString_Cashshop.h"
#include ".\TableString\TableString_DigiEvo.h"
#include ".\TableString\TableString_Digimon.h"
#include ".\TableString\TableString_DigiTalk.h"
#include ".\TableString\TableString_DigiTip.h"
#include ".\TableString\TableString_Emotion.h"
#include ".\TableString\TableString_EventTalk.h"
#include ".\TableString\TableString_Guild.h"
#include ".\TableString\TableString_Item.h"
#include ".\TableString\TableString_ItemCreation.h"
#include ".\TableString\TableString_Map.h"
#include ".\TableString\TableString_MapRegion.h"
#include ".\TableString\TableString_Message.h"
#include ".\TableString\TableString_Monster.h"
#include ".\TableString\TableString_Npc.h"
#include ".\TableString\TableString_Quest.h"
#include ".\TableString\TableString_Riding.h"
#include ".\TableString\TableString_Scene.h"
#include ".\TableString\TableString_Seal.h"
#include ".\TableString\TableString_Skill.h"
#include ".\TableString\TableString_Tamer.h"
#include ".\TableString\TableString_Tip.h"
#include ".\TableString\TableString_Tooltip.h"
#include ".\TableString\TableString_UIText.h"