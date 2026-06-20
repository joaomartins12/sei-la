//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "TableDefs.h"
#include "TableFactory.h"

BEGIN_NAME_TABLE

namespace TFactory
{
	IDataTableEvent* TFactory::CreateITable(int const& TableType)
	{
		switch( TableType )
		{
		case ETABLE_ACHIEVE:			return GNew CAchieveTable;
		case ETABLE_BASESTATS:			return GNew CBaseStatsTable;
		case ETABLE_BUFF:				return GNew CBuffTable;
		case ETABLE_CASH_SHOP:			return GNew CCashshopTable;
		case ETABLE_COOLTIME:			return GNew CCoolTimeTable;
		case ETABLE_CREATE_CHARACTER:	return GNew CCreateCharacterTable;
		case ETABLE_DBOOK:				return GNew CDBookTable;
		case ETALBE_DEVOLUTION:			return GNew CDEvolutionTable;
		case ETABLE_DIGIMONS:			return GNew CDigimonTable;
		case ETABLE_DSKILL_LEVELUP:		return GNew CDSkillLvUpTable;
		case ETABLE_EXP:				return GNew CExpTable;
		case ETABLE_GUILD:				return GNew CGuildTable;
		case ETABLE_INFINITEWAR:		return GNew CInfiniteWarTable;
		case ETABLE_ITEMS:				return GNew CItemTable;
		case ETABLE_ITEMS_RANK:			return GNew CItemRankTable;
		case ETABLE_ITEM_CREATION:		return GNew CItemCreationTable;
		case ETABLE_ITEMS_EXCHANGE:		return GNew CItemExchangeTable;
		case ETABLE_JUMPBUSTER:			return GNew CJumpBusterTable;
		case ETABLE_MAPSINFO:			return GNew CMapsTable;
		case ETABLE_MONSTER:			return GNew CMonsterTable;
		//case ETABLE_TACTICS:			return GNew CTacticsTable;
		//case ETABLE_TALK:				return GNew CTalk;
		case ETABLE_UITEXT:				return GNew CUITextTable;
		default:
			return NULL;
		}

		return NULL;
	}

	ITableEvent* TFactory::CreateISTable(int const& TableType)
	{
		switch( TableType )
		{
		case ESTRING_TABLE_ACHIEVE:			return GNew CSTable_Achieve;
		case ESTRING_TABLE_BUFF:			return GNew CSTable_Buff;
		case ESTRING_TABLE_CASHSHOP:		return GNew CSTable_Cashshop;
		case ESTRING_TABLE_DIGIEVO:			return GNew CSTable_DigiEvo;
		case ESTRING_TABLE_DIGIMON:			return GNew CSTable_Digimon;
		case ESTRING_TABLE_DIGIMON_TIP:		return GNew CSTable_DigiTip;
		case ESTRING_TABLE_DIGIMON_TALK:	return GNew CSTable_DigiTalk;
		case ESTRING_TABLE_EMOTION:			return GNew CSTable_Emotion;
		case ESTRING_TABLE_EVENT_TALK:		return GNew CSTable_EventTalk;
		case ESTRING_TABLE_GUILD:			return GNew CSTable_Guild;
		case ESTRING_TABLE_ITEM:			return GNew CSTable_Item;
		case ESTRING_TABLE_ITEM_CREATION:	return GNew CSTable_ItemCreation;
		case ESTRING_TABLE_MAP:				return GNew CSTable_Map;
		case ESTRING_TABLE_MAP_REGION:		return GNew CSTable_MapRegion;
		case ESTRING_TABLE_MESSAGE:			return GNew CSTable_Message;
		case ESTRING_TABLE_MONSTER:			return GNew CSTable_Monster;
		case ESTRING_TABLE_NPC:				return GNew CSTable_Npc;
		case ESTRING_TABLE_QUEST:			return GNew CSTable_Quest;
		case ESTRING_TABLE_RIDING:			return GNew CSTable_Riding;
		case ESTRING_TABLE_SCENE:			return GNew CSTable_Scene;
		case ESTRING_TABLE_SEAL:			return GNew CSTable_Seal;
		case ESTRING_TABLE_SKILL:			return GNew CSTable_Skill;
		case ESTRING_TABLE_TAMER:			return GNew CSTable_Tamer;
		case ESTRING_TABLE_TIP:				return GNew CSTable_Tip;
		case ESTRING_TABLE_TOOLTIP:			return GNew CSTable_ToolTip;
		case ESTRING_TABLE_UITEXT:			return GNew CSTable_UIText;
		default:
			return NULL;
		}

		return NULL;
	}

}
END_NAME_TABLE