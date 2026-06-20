//---------------------------------------------------------------------------
//
// 파일명 : FlowFactory.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "FlowFactory.h"

#include "IntroFlow.h"
#include "LogoFlow.h"
#include "LoginFlow.h"
#include "SecondPasswordFlow.h"
#include "ServerSelectFlow.h"
#include "CharacterSelectFlow.h"
#include "CharacterCreateFlow.h"
#include "DigimonCreateFlow.h"
#include "LoadingFlow.h"
#include "InGameFlow.h"
#include "DatsCenterFlow.h"
#include "DataTradeCenterFlow.h"
#include "BattleFlow.h"
#include "BattleRegisterFlow.h"
#include "BattleResultFlow.h"
#include "CharacterServerRelocateFlow.h"
//---------------------------------------------------------------------------

namespace Flow
{
	CFlowFactory::CFlowFactory()
	{
	}
	//---------------------------------------------------------------------------
	CFlowFactory::~CFlowFactory()
	{
	}
	//---------------------------------------------------------------------------
	CFlow* CFlowFactory::CreateFlow(int p_iID)
	{
		CFlow* pkFlow = NULL;

		switch (p_iID)
		{
		case CFlow::FLW_INTRO:
			pkFlow = NiNew CIntroFlow(p_iID);
			break;

		case CFlow::FLW_LOGO:
			pkFlow = NiNew CLogoFlow(p_iID);
			break;

		case CFlow::FLW_LOGIN:
			pkFlow = NiNew CLoginFlow(p_iID);
			break;

		case CFlow::FLW_SECURITY:
			pkFlow = NiNew CSecondPasswordFlow(p_iID);
			break;

		case CFlow::FLW_SERVERSEL:
			pkFlow = NiNew CServerSelectFlow(p_iID);
			break;

		case CFlow::FLW_CHARSEL:
			pkFlow = NiNew CCharacterSelectFlow(p_iID);
			break;

		case CFlow::FLW_CHARCREATE:
			pkFlow = NiNew CCharacterCreateFlow(p_iID);
			break;

		case CFlow::FLW_DIGIMON_CREATE:
			pkFlow = NiNew CDigimonCreateFlow(p_iID);
			break;

		case CFlow::FLW_LOADING:
			pkFlow = NiNew CLoadingFlow(p_iID);
			break;

		case CFlow::FLW_MAINGAME:
			pkFlow = NiNew CInGameFlow(p_iID);
			break;

		case CFlow::FLW_DATSCENTER:
			pkFlow = NiNew CDatsCenterFlow(p_iID);
			break;

		case CFlow::FLW_DATATRADECENTER:
			pkFlow = NiNew CDataTradeCenterFlow(p_iID);
			break;

		case CFlow::FLW_BATTLE:
			pkFlow = NiNew CBattleFlow(p_iID);
			break;

		case CFlow::FLW_BATTLE_REGISTER:
			pkFlow = NiNew CBattleRegisterFlow(p_iID);
			break;

		case CFlow::FLW_BATTLE_RESULT:
			pkFlow = NiNew CBattleResultFlow(p_iID);
			break;

		case CFlow::FLW_SERVER_RELOCATE:
			pkFlow = NiNew CCharacterServerRelocateFlow(p_iID);
			break;

		default:
#ifdef DEBUG_FLOW
			DBG("CFlowFactory::CreateFlow failed. Unknown flow id %d\n", p_iID);
#endif
			break;
		}

		return pkFlow;
	}
}
//---------------------------------------------------------------------------