#include "StdAfx.h"

#include "ContentsSystem.h"
#include "ContentsSystemDef.h"

#include "../_Interface/00.Contents/ResourceIntegrityChecker.h"
#include "../_Interface/00.Contents/LogoContents.h"
#include "../_Interface/00.Contents/LoginContents.h"
#include "../_Interface/00.Contents/SecondPasswordContents.h"
#include "../_Interface/00.Contents/ServerSelectContents.h"
#include "../_Interface/00.Contents/CharacterSelectContents.h"
#include "../_Interface/00.Contents/CharacterCreateContents.h"
#include "../_Interface/00.Contents/LoadingContents.h"
#include "../_Interface/00.Contents/Mail_Contents.h"
#include "../_Interface/00.Contents/DamageMeter_Contents.h"
#include "../_Interface/00.Contents/BattleSystemContent.h"

#ifdef USE_SHUTDOWN
#include "../_Interface/00.Contents/ShutDownContents.h"
#endif

#include "../_Interface/Game/ItemProduction_Contents.h"

#include "../_Interface/Game/ChatContents.h"

#include "../_Interface/00.Contents/NewCashshopContents.h"

#include "../_Interface/Game/WareHouseContents.h"

#include "../_Interface/Game/CardEventContents.h"

#include "../_Interface/Game/MapContents.h"

#include "../_Interface/Game/DataTradeContents.h"

#include "../_Interface/Game/ItemRewardContents.h"

#include "../DatsCenterContents.h"
#include "../_Interface/Game/EventContents.h"

#include "../_Interface/00.Contents/Trade_Contents.h"

#include "../_Interface/Game/SealMasterContents.h"

#include "../_Interface/00.Contents/CommunityContent.h"
#include "../_Interface/00.Contents/MacroProtectContents.h"

#include "../_Interface/00.Contents/ServerRelocateContents.h"
#include "../_Interface/00.Contents/TutorialContents.h"
#include "../_Interface/00.Contents/InfiniteWar_Contents.h"
#include "../_Interface/00.Contents/InfiniteWar_RankReward_Contents.h"
#include "../_Interface/00.Contents/DigimonArchiveContents.h"

#include "../_Interface/00.Contents/MainFrame_Contents.h"
#include "../_Interface/00.Contents/NeonDisplayContents.h"
#include "../_Interface/00.Contents/EvolutionStore_Contents.h"

#include "../_Interface/00.Contents/VipMemberShip_Contents.h"
#include "../_Interface/Game/InventoryContents.h"
#include "../_Interface/00.Contents/GotchaMachineContents.h"
#include "../_Interface/00.Contents/EquipSystemContents.h"
#include "../_Interface/00.Contents/EnchantOptionContents.h"

IUIContentBase* ContentsSystem::UIContentsContructor(int const& iContentsType)
{
	switch( iContentsType )
	{
	case E_CT_RESOURCEINTEGRITYCHECKER:	return new cResourectIntegrityChecker();
	case E_CT_LOGO:						return new LogoContents();
	case E_CT_LOGIN:					return new LoginContents();
	case E_CT_SECOND_PASSWORD:			return new SecondPasswordContents();
	case E_CT_SERVER_SELECT:			return new ServerSelectContents();
	case E_CT_CHARACTER_SELECT:			return new CharacterSelectContents();
	case E_CT_CHARACTER_CREATE:			return new CharacterCreateContents();
	case E_CT_LOADING:					return new LoadingContents();
#ifdef DAMAGE_METER
	case E_CT_DAMAGE_METER:				return new DamageMeterContents();
#endif
#ifdef USE_SHUTDOWN
	case E_CT_SHUTDOWN_SYSTEM:			return new CShutDownContents();
#endif
	case E_CT_ITEMPRODUCTION:			return new CItemProductionContents();
	case E_CT_CHATTING_STANDARDIZATION:	return new ChatContents();
	case E_CT_NEW_CASHSHOP:			return new NewCashshopContents();
	case E_CT_WAREHOUSE:				return new WareHouseContents();
	case E_CT_MAIL_SYSTEM:				return new MailContents();
	case E_CT_CARD_EVENT:				return new CardEventContents();
	case E_CT_MAP_INTERFACE:			return new CMapContents();
	case E_CT_DATA_TRADE_SYSTEM:		return new DataTradeContents();
	case E_CT_ITEM_REWARD:				return new ItemRewardContents();
	case E_CT_FRIENDRECOMMEND_EVENT:	return new CRecommendEvent_Contents();
	case E_CT_STORE_SYSTEM:				return new CStoreContents();
	case E_CT_DATS_CENTER:				return new CDatsCenterContents();
	case E_CT_EVENT_CONTENTS:			return new EventContents();
	case E_CT_TRADE:					return new CTradeContents();
	case E_CT_CHANELLSELECT:			return new ChannelContents();
	case E_CT_FRIENDLIST:				return new cFriendListContents();
	case E_CT_DIGITAMA_CONTENTS:		return new DigitamaContents();		//2017-03-29-nova
	case E_CT_MAKETACTICS_CONTENTS:		return new MakeTacticsContents();	//2017-04-12-nova
	case E_CT_PERSONSTORE_CONTENTS:		return new PersonStoreContents();	//2017-04-27-nova
	case E_CT_UNIONSTORE_CONTENTS:		return new UnionStoreContents();	//2017-04-20-nova
	case E_CT_ENCYCLOPEDIA_CONTENTS:	return new EncyclopediaContents();	//2017-05-10-nova
	case E_CT_ACHIEVE_CONTENTS:			return new AchieveContents();		//2017-06-08-nova

	case E_CT_PARTYSYSTEM:				return new CPartyContents();
	case E_CT_SEALMASTER:			return new cSealMasterContents();
	case E_CT_COMMUNITY:				return new cCommunityContent();
	case E_CT_MACROPROTECT:				return new CMacroProtectContents();
	case E_CT_BATTLE_SYSTEM:			return new CBattleSystemContents();
	case E_CT_SERVERCHANGE_CONTENTS:	return new CServerRelocateContents();
	case E_CT_TUTORIAL_CONTENTS:		return new CTutorialContents();
	case E_CT_INFINITEWAR_CONTENTS:		return new CInfiniteWarContents();
	case E_CT_INFINITEWAR_RANKREWARD_CONTENTS:		return new CInfiniteWarRankRewardContents();
	case E_CT_DIGIMON_ARCHIVE_CONTENTS:	return new CDigimonArchiveContents();
	case E_CT_MAINFRAME_CONTENTS:		return new CMainFrameContents();
	case E_CT_NEON_DISPLAY_CONTENTS:	return new CNeonDisplayContents();
	case E_CT_EVOLUTION_STORE_CONTENTS:	return new cEvolutionStoreContents();
#ifdef SDM_VIP_SYSTEM_20181105
	case E_CT_VIPMEMBERSHIP_CONTENTS:	return new CVipMemberShipContents();
#endif
	case E_CT_INVENTORY_CONTENTS:		return new cInventoryContents();
	case E_CT_GOTCHAMACHINE_CONTENTS:	return new cGotchaMachineContents();
	case E_CT_ELEMENTITEM_CONTENTS:		return new cEquipSystemContents();
	case E_CT_ENCHANT_OPTION_CONTENTS:	return new cEnchantOptionContents();
	default:
		break;
	}
	return 0;
}