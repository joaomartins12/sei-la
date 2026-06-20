
#include "stdafx.h"
#include "MngCollector.h"

CsGBTerrainMng* g_pTerrainMng = NULL;
CMngCollector* g_pMngCollector = NULL;

void CMngCollector::ShotDown()
{
	CPat_AI_Mng::GlobalShotDown();
	CNumber3DMng::GlobalShotDown();
	CSoundMgr::GlobalShotDown();
	cDataMng::GlobalShotdown();
	//cSecondPassword::GlobalShotDown();
	cTacticsAni::GlobalShotDown();
	cSystemMsg::GlobalShotDown();
	cHelpArrow::GlobalShotDown();
	// 	if( g_pCardMng != NULL )
	// 	{
	// 		g_pCardMng->Delete();
	// 		SAFE_DELETE( g_pCardMng );
	// 	}
		// 아이템 매니져
	if (g_pItemMng != NULL)
	{
		g_pItemMng->Delete();
		SAFE_DELETE(g_pItemMng);
	}

	// Npc 매니져
	if (g_pNpcMng != NULL)
	{
		g_pNpcMng->Delete();
		SAFE_NIDELETE(g_pNpcMng);
	}

	// 케릭터 매니져
	if (g_pCharMng != NULL)
	{
		g_pCharMng->Delete();
		SAFE_DELETE(g_pCharMng);
	}

	//이펙트 사운드 매니져 제거
	if (g_EffectSoundMng != NULL)
	{
		g_EffectSoundMng->Delete();
		SAFE_DELETE(g_EffectSoundMng);
	}

	//이벤트 매니져 제거
	if (g_pGameEventMng != NULL)
	{
		g_pGameEventMng->Delete();
		SAFE_DELETE(g_pGameEventMng);
	}

	if (g_pEventScene != NULL)
	{
		g_pEventScene->Delete();
		SAFE_DELETE(g_pEventScene);
	}

#ifdef CROSSWARS_SYSTEM
	if (g_pCrossEvent != NULL)
	{
		g_pCrossEvent->Delete();
		SAFE_DELETE(g_pCrossEvent);
	}
#endif
	if (g_pDatsCenter != NULL)
	{
		g_pDatsCenter->Delete();
		SAFE_NIDELETE(g_pDatsCenter);
	}

	//=============================================================================================================
	// 가장 마지막에 제거 = 아래 순서 지켜주자
	//=============================================================================================================

	// 쓰레드 리소스 매니져의 외부포인터들 없애 준다
	if (g_pThread)
	{
		g_pThread->GetResMng()->SetDefaultLight(NULL);
		g_pThread->GetResMng()->ReleaseConnetTerrain();
	}

	// 리소스 매니져
	if (g_pCharResMng != NULL)
	{
		g_pCharResMng->Destroy();
		SAFE_DELETE(g_pCharResMng);
	}

	//저장된 이벤트 데이터 삭제
	if (g_pEventDataMng != NULL)
	{
		g_pEventDataMng->Delete();
		SAFE_NIDELETE(g_pEventDataMng);
	}

	// 	if( g_bUseFilePack )
	// 		CsFilePackMng::GlobalShotDown();

		//=============================================================================================================
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 주 의 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//
		// 리소스 매니져에서 쓰레드 제거 끝났다. - 차후에 쓰레드 로딩 에서 받아오는 리소스는 없어야 한다.
		//
		//
		//=============================================================================================================

		// 지형 매니져 제거
	if (g_pTerrainMng != NULL)
	{
		g_pTerrainMng->Delete();
		SAFE_DELETE(g_pTerrainMng);
	}
	if (g_pMngCollector != NULL)
	{
		g_pMngCollector->Delete();
		SAFE_DELETE(g_pMngCollector);
	}


	// 메모리 풀 제거
	THREAD_MEMPOOL_DELETE(CItem);
	THREAD_MEMPOOL_DELETE(CMonster);
	THREAD_MEMPOOL_DELETE(CTamer);
	THREAD_MEMPOOL_DELETE(CDigimon);
	THREAD_MEMPOOL_DELETE(CNpc);
	THREAD_MEMPOOL_DELETE(CPat);
	THREAD_MEMPOOL_DELETE(CEmployment);
}

void CMngCollector::ShotInit()
{
	CPat_AI_Mng::GlobalInit();
	cStringAnalysis::GlobalInit();
	cString::GlobalInit();
	CNumber3DMng::GlobalInit();
	CSoundMgr::GlobalInit();
	cWindow::GlobalInit();
	cSystemMsg::GlobalInit();
	cHelpArrow::GlobalInit();
	//	cSecondPassword::GlobalInit();
	cTacticsAni::GlobalInit();

	// cDataMng::GlobalInit(); 외부로 따로 빼겟다

	assert_cs(g_pMngCollector == NULL);
	g_pMngCollector = NiNew CMngCollector;
	g_pMngCollector->Init();

	// 리소스 매니져
	g_pCharResMng = NiNew CCharResMng;
	g_pCharResMng->Init();

	// 케릭터 매니져
	g_pCharMng = NiNew CCharMng;
	g_pCharMng->Init();

	// Npc 매니져
	g_pNpcMng = NiNew CNpcMng;
	g_pNpcMng->Init();

	// 아이템 매니져
	assert_cs(g_pItemMng == NULL);
	g_pItemMng = NiNew CItemMng;
	g_pItemMng->Init();

	// 	assert_cs( g_pCardMng == NULL );
	// 	g_pCardMng = NiNew CCardMng;	
	// 	g_pCardMng->Init();

		// 지형 매니져
	assert_cs(g_pTerrainMng == NULL);
	g_pResist->m_Global.s_bUseMyShader = true;
	g_pTerrainMng = NiNew CsGBTerrainMng;
	g_pTerrainMng->Init(g_pEngine->m_spRenderer,
		g_pEngine->m_DeviceCaps.s_nMaxSimultaneousTextures,
		g_pEngine->m_DeviceCaps.s_bEnablePS_2_0,
		g_pResist->m_Global.s_bUseMyShader,
		15000.0f, 56000.0f);
	//15.0f, 50000.0f );

//이펙트 사운드 매니져
	g_EffectSoundMng = NiNew CsEffectSoundMng;
	g_EffectSoundMng->Init();

	//전체 이벤트 데이터 관리
	g_pEventDataMng = NiNew CEventDataMng;
	g_pEventDataMng->Init();

	//이벤트 매니져
	assert_cs(g_pGameEventMng == NULL);
	g_pGameEventMng = NiNew CGameEventMng;

	if (IsNetOff() == true)
		g_pGameEventMng->Init();

	g_pEventCall = g_pGameEventMng;

	g_pEventScene = NiNew CEventScene;
	g_pEventScene->Init();
#ifdef CROSSWARS_SYSTEM
	g_pCrossEvent = NiNew CCrossEvent;
	g_pCrossEvent->Init();
#endif
}

// 로딩 시작부에 지형에서 라이트랑 이런거 제거 하고 난후 호출 된다
void CMngCollector::ResetMap()
{
	// 날씨효과 리셋
	g_pWeather->Off();

	// Data
	g_pDataMng->Reset();

	// 리소스 매니져
	g_pCharResMng->Reset();

	// 케릭터 매니져
	g_pCharMng->Reset();
	// Npc 매니져
	g_pNpcMng->Reset();
	// 아이템 매니져
	g_pItemMng->Reset();
	//g_pCardMng->Reset();
	//시스템 메시지 제거
	g_pSystemMsg->ResetMap();
	g_pSkillMsg->ResetMap();

	m_AutoSaveResistTime.OffReset();
	//m_AutoProtect.OffReset();

	m_sSearched.Reset();
}

void CMngCollector::Delete()
{

}

void CMngCollector::Init()
{
	m_eSceneState = eSCENE_NONE;
	//m_bClientPortal = false;

#ifndef VERSION_USA
	m_LongTimeNotice.SetDeltaTime(1000 * 60 * 60);
	m_LongTimeNotice.OffReset();
	m_LongTimeNotice2.SetDeltaTime(5000);
#endif

	m_TimeNotice.SetDeltaTime(1000 * 60 * 83);
	m_TimeNotice.OffReset();
	m_TimeNotice2.SetDeltaTime(5000);

	/*m_AutoProtect.SetDeltaTime( 1000*60*60 );
	m_AutoProtect.OffReset();*/

	//m_EventNotice.SetDeltaTime( 1000*60*10 );
	//m_EventNotice.Reset();

	// 15분 마다 한번씩 저장
	m_AutoSaveResistTime.SetDeltaTime(1000 * 60 * 15);
	m_AutoSaveResistTime.OffReset();

	m_bRenderText = false;

	cText::sTEXTINFO ti;
	ti.Init();
	m_Text.Init(NULL, CsPoint::ZERO, &ti, false);

	m_sSearched.Reset();
}

void CMngCollector::LoadTerrain(DWORD dwMapID)
{
	SetWindowTitle(-1, dwMapID);
	int nValue = 62;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::LOADING_PROGRESS_VALUE, &nValue);

	//영역체크 이벤트 셋팅
	g_pEventSceneMng->ResetScene();

	nValue = 64;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::LOADING_PROGRESS_VALUE, &nValue);

	g_pGameEventMng->ReLoadScene(dwMapID);
	nValue = 66;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::LOADING_PROGRESS_VALUE, &nValue);

	g_pEventScene->LoadScene(dwMapID);
	nValue = 68;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::LOADING_PROGRESS_VALUE, &nValue);

	bool bSuccess = g_pTerrainMng->CreateRoot(dwMapID);
	assert_cs(bSuccess == true);

	nValue = 70;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::LOADING_PROGRESS_VALUE, &nValue);

	// 레지스트리에 저장된 오브젝트 클립 비율 곱하자
	float fRate = g_pResist->m_Global.s_fObjectSightRate;
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fClipObject_Far = fRate * nsCsGBTerrain::g_pCurRoot->GetOrgClipObject_Far();
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogStart = fRate * nsCsGBTerrain::g_pCurRoot->GetOrgFogObject_Start();
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogEndObject = fRate * nsCsGBTerrain::g_pCurRoot->GetOrgFogObject_End();
	nsCsGBTerrain::g_pCurRoot->CalFogDateValue();
}

void CMngCollector::LoadChar(DWORD dwMapID)
{
	// 엔피씨 재배치
	if (g_pNpcMng)
		g_pNpcMng->LoadMap(dwMapID);
}

void CMngCollector::DeleteChar(DWORD dwMapID)
{
	// 엔피씨 제거
	if (g_pNpcMng)
		g_pNpcMng->DeleteMapNpc(dwMapID);
}

void CMngCollector::LoadCompleate(DWORD dwMapID)
{
	g_pCharResMng->PostLoadMap();

	if (nsCsFileTable::g_pWorldMapMng->IsArea(dwMapID))
	{
		CsAreaMap::sINFO* pAreaInfo = nsCsFileTable::g_pWorldMapMng->GetArea(dwMapID)->GetInfo();
		D3DXVECTOR4* pVec = g_pEngine->GetGaussianBlurVal();
		pVec->x = pAreaInfo->s_fGaussianBlur[0];
		pVec->y = pAreaInfo->s_fGaussianBlur[1];
		pVec->z = pAreaInfo->s_fGaussianBlur[2];
		NiD3DShaderFactory::UpdateGlobalShaderConstant("g_GaussianVal", sizeof(float) * 4, pVec);
	}

	if (g_pDataMng->GetPCbang()->GetPcBang() == true)
	{
		g_pGameIF->GetAlime()->SetAlime(310, cAlime::PCBANG, 0);
	}
}

void CMngCollector::LoadDatsCenter()
{
	if (g_pDatsCenter == NULL)
	{
		g_pDatsCenter = NiNew CDatsCenter;
		g_pDatsCenter->Init();
	}
}

void CMngCollector::EndDatsCenter()
{
	if (g_pDatsCenter != NULL)
	{
		g_pDatsCenter->End();
		g_pDatsCenter->Delete();
		SAFE_NIDELETE(g_pDatsCenter);
	}
}

void CMngCollector::Update(cGlobalInput::eMOUSE_INPUT mi)
{
#ifndef VERSION_USA
#ifndef VERSION_TW
#ifndef VERSION_HK
	if (m_LongTimeNotice.IsEnable())
	{
		//cUserGrade::BeginGrade( false );

		cPrintMsg::PrintMsg(12000, const_cast<wchar_t*>(UISTRING_TEXT("TIME_NOTICE_CAUTION_LONGTIME").c_str()));
		m_LongTimeNotice2.Reset();
	}
	if (m_LongTimeNotice2.IsEnable())
	{
		cPrintMsg::PrintMsg(12000, const_cast<wchar_t*>(UISTRING_TEXT("TIME_NOTICE_CAUTION_LONGTIME_ADVISE").c_str()));
	}
#endif
#endif
#endif

	if (m_TimeNotice.IsEnable())
	{
		cPrintMsg::PrintMsg(12000, const_cast<wchar_t*>(UISTRING_TEXT("TIME_NOTICE_CAUTION_SHOUT").c_str()));
		m_TimeNotice2.Reset();
	}
	if (m_TimeNotice2.IsEnable())
	{
		cPrintMsg::PrintMsg(12000, const_cast<wchar_t*>(UISTRING_TEXT("TIME_NOTICE_CAUTION_SHOUT_APPRECIATE").c_str()));
	}

	if (m_AutoSaveResistTime.IsEnable())
	{
		g_pResist->SaveGlobal();
		g_pResist->m_bLoadChar = true;
	}

#ifdef _GIVE
	/*DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsFileTable::g_pBaseMng->IsEnableCheckMacro( dwMapID ) && m_AutoProtect.IsEnable() )
	{
		if( g_pGameIF && ( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PROTECT ) == false ) )
		{
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_PROTECT );
		}
	}*/
#endif

	if (g_pDataMng)
		g_pDataMng->Update();

	if (g_pSEffectMgr)
		g_pSEffectMgr->Update(g_fDeltaTime);

	switch (m_eSceneState)
	{
#ifdef CROSSWARS_SYSTEM
	case eSCENE_CROSS:
	{
		g_pTerrainMng->Update(g_fDeltaTime);
		//g_pCharMng->Update();		
		g_pCharMng->GetTamerUser()->Update(g_fDeltaTime);
		g_pCharMng->GetDigimonUser(0)->Update(g_fDeltaTime);
		g_pEffectMng->Update(g_fDeltaTime);
		g_pCrossEvent->Update(g_fDeltaTime);
	}
	break;
#endif
	case eSCENE_EVENT:
	{
		g_pTerrainMng->Update(g_fDeltaTime);
		g_pCharMng->Update();
		g_pNpcMng->Update();
		g_pEventScene->Update(g_fDeltaTime);
	}
	break;
	case eSCENE_DATS:
	{
		assert_cs(g_pDatsCenter != NULL);
		g_pDatsCenter->Update(g_fDeltaTime, mi);
		g_pCharMng->GetTamerUser()->Update(g_fDeltaTime);
		g_pCharMng->GetDigimonUser(0)->Update(g_fDeltaTime);
	}
	break;
	case eSCENE_TACTICSANI:
	{
		g_pTerrainMng->Update(g_fDeltaTime);
		g_pTacticsAni->Update();
		g_pNpcMng->Update();
		g_pEffectMng->Update(g_fDeltaTime);
	}
	break;
	case eSCENE_TUTORIAL_EVENT:
	{
		g_pTerrainMng->Update(g_fDeltaTime);
		g_pCharMng->Update_Tutorial();
		g_pCharResMng->Update_Tutorial(mi);
		g_pNumber3DMng->Update();
	}
	break;
	default:
	{
		// 카메라 업데이트 선	
		g_pTerrainMng->Update(g_fDeltaTime);
		g_pCharMng->Update();
		g_pNpcMng->Update();
		g_pItemMng->Update();
		//g_pCardMng->Update();
		g_pCharResMng->Update(mi);
		g_pHelpArrow->Update();
		g_pNumber3DMng->Update();
		g_pGameEventMng->Update(g_fDeltaTime);

		CheckNearObject();
	}
	break;
	}

}

void CMngCollector::Render(bool bClearGetterTerrain)
{
	switch (m_eSceneState)
	{
	case eSCENE_DATS:
	{
		assert_cs(g_pDatsCenter != NULL);
		g_pDatsCenter->Render();
	}
	break;
#ifdef CROSSWARS_SYSTEM
	case eSCENE_CROSS:
	{
		//g_pTerrainMng->BeginRender();	
		//g_pTerrainMng->Render( bClearGetterTerrain );
		//g_pTerrainMng->EndRender();	
		g_pEffectMng->Render();

		g_pCharMng->Render();
		//g_pCharResMng->Render();		


		g_pCharMng->Render_PostChar();
		//g_pCharResMng->Render_PostChar();	

		g_pCrossEvent->Render();
	}
	break;
#endif
	/*case eSCENE_EVENT:
		{

		}
		break;*/
	case eSCENE_TACTICSANI:
	{
		g_pTerrainMng->BeginRender();
		g_pTerrainMng->Render(bClearGetterTerrain);
		g_pTerrainMng->EndRender();

		g_pNpcMng->Render();
		g_pEffectMng->Render();

		g_pTacticsAni->Render();
	}
	break;
	case eSCENE_TUTORIAL_EVENT:
	{
		g_pTerrainMng->BeginRender();
		g_pTerrainMng->Render(bClearGetterTerrain);
		g_pTerrainMng->EndRender();

		g_pCharMng->Render_Tutorial();
		g_pCharMng->Render_PostChar_Tutorial();

		g_pCharResMng->Render();
		g_pCharResMng->Render_PostChar();
		g_pHelpArrow->Render();
		g_pNumber3DMng->Render();
	}
	break;
	default:
	{
		g_pTerrainMng->BeginRender();
		g_pTerrainMng->Render(bClearGetterTerrain);
		g_pTerrainMng->EndRender();
		g_pGameEventMng->Render();

		if (g_pEventSceneMng->IsCameraScene() == false)
		{
			g_pCharResMng->Render_PostTR();

			g_pCharMng->Render();
			g_pNpcMng->Render();
			g_pItemMng->Render();

			//g_pDataMng->Render();

			g_pCharResMng->Render();
			g_pHelpArrow->Render();

			g_pNpcMng->Render_PostChar();
			g_pCharMng->Render_PostChar();
			g_pCharResMng->Render_PostChar();

			g_pNumber3DMng->Render();
			g_pWeather->Render();
		}
	}
	break;
	}

	g_Sorting.Render(CAMERA_ST.GetCameraObj());
}

void CMngCollector::Render_Shadow(bool bClearGetterTerrain)
{
	switch (m_eSceneState)
	{
#ifdef CROSSWARS_SYSTEM
	case eSCENE_CROSS:
	{
		//g_pTerrainMng->BeginRender();	
		//g_pTerrainMng->Render( bClearGetterTerrain );
		//g_pTerrainMng->EndRender();	
		g_pEffectMng->Render();

		g_pCharMng->Render();
		//g_pCharResMng->Render();		


		g_pCharMng->Render_PostChar();
		//g_pCharResMng->Render_PostChar();	

		g_pCrossEvent->Render();
	}
	break;
#endif
	case eSCENE_DATS:
	{
		assert_cs(g_pDatsCenter != NULL);
		g_pDatsCenter->Render();
	}
	break;
	/*case eSCENE_EVENT:
		{

		}
		break;*/
	case eSCENE_TACTICSANI:
	{
		g_pTerrainMng->BeginRender();
		g_pTerrainMng->Render(bClearGetterTerrain);
		g_pTerrainMng->EndRender();

		g_pNpcMng->Render();
		g_pEffectMng->Render();

		g_pTacticsAni->Render();
	}
	break;
	case eSCENE_TUTORIAL_EVENT:
	{
		g_pTerrainMng->BeginRender();
		g_pTerrainMng->Render(bClearGetterTerrain);
		g_pTerrainMng->EndRender();

		g_pCharMng->Render_Tutorial();
		g_pCharMng->Render_PostChar_Tutorial();

		g_pCharResMng->Render();
		g_pCharResMng->Render_PostChar();
		g_pHelpArrow->Render();
		g_pNumber3DMng->Render();
	}
	break;
	default:
	{
		NiPoint3 vShadowCenter = g_pServerMoveOwner->GetParent()->GetPos();
		if (g_pEventSceneMng->IsCameraScene())
		{
			assert_cs(g_pEventSceneMng->GetScene());
			CEventMng* pScene = g_pEventSceneMng->GetScene();
			if (pScene)
				vShadowCenter = pScene->GetCameraTarget()->GetPos();
		}

		if (g_pShadow->BeginShadow(vShadowCenter) == true)
		{
			cObjectController::RenderShadow();
			g_pShadow->EndShadow(g_pEngine->GetCurrentRenderTargetGroup());
		}

		g_pTerrainMng->BeginRender();
		g_pTerrainMng->Render(bClearGetterTerrain);
		g_pTerrainMng->EndRender();
		g_pGameEventMng->Render();

		if (g_pEventSceneMng->IsCameraScene() == false)
		{
			g_pCharResMng->Render_PostTR();

			g_pCharMng->Render();
			g_pNpcMng->Render();
			g_pItemMng->Render();

			//g_pDataMng->Render();

			g_pCharResMng->Render();
			g_pHelpArrow->Render();


			g_pNpcMng->Render_PostChar();
			g_pCharMng->Render_PostChar();
			g_pCharResMng->Render_PostChar();

			g_pNumber3DMng->Render();
			g_pWeather->Render();
		}
	}
	break;
	}

	g_Sorting.Render(CAMERA_ST.GetCameraObj());
}


void CMngCollector::Render_DOF()
{
	g_pTerrainMng->BeginRender();
	g_pTerrainMng->Render_Shader(true, "Depth_Skin", "Depth_Object");
	g_pTerrainMng->EndRender();

	/*g_pGameEventMng->Render();

	if( g_pEventSceneMng->IsCameraScene() == false )
	{
		g_pCharResMng->Render_PostTR();

		g_pCharMng->Render();
		g_pNpcMng->Render();
		g_pItemMng->Render();
		g_pCharResMng->Render();
		g_pHelpArrow->Render();

		g_pNpcMng->Render_PostChar();
		g_pCharMng->Render_PostChar();
		g_pCharResMng->Render_PostChar();

		g_pNumber3DMng->Render();
		g_pWeather->Render();
	}

	g_Sorting.Render( CAMERA_ST.GetCameraObj() );*/
}

void CMngCollector::Render_PostIF()
{
	switch (m_eSceneState)
	{
	case eSCENE_TACTICSANI:	g_pTacticsAni->PostRender();	break;
#ifdef CROSSWARS_SYSTEM
	case eSCENE_CROSS:		g_pCrossEvent->PostRender();	break;
#endif		
	}

	g_Sorting.Render(CAMERA_ST.GetCameraObj());
}

CsC_AvObject* CMngCollector::PickObject(cType& type, POINT ptMouse, bool bIncludeSelf)
{
	//CsGBPick pick( false, NiPick::NO_SORT );
	CsGBPick pick(false, NiPick::SORT);
	float fLenth = FLT_MAX;
	NiPoint3 origin(0.0f, 0.0f, 0.0f);
	NiPoint3 dir(0.0f, 0.0f, 0.0f);

	CAMERA_ST.WindowPointToRay(ptMouse.x, ptMouse.y, origin, dir);

	CsC_AvObject* pReturn = NULL;

	// NoPick Object 검사
	MAP_GBTERRAIN_OBJECT* mapNoPick = nsCsGBTerrain::g_pCurRoot->GetMap_No_Pick();
	if (mapNoPick->empty() == false)
	{
		MAP_GBTERRAIN_OBJECT_IT it = mapNoPick->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = mapNoPick->end();
		CsGBObject* pObject = NULL;
		for (; it != itEnd; ++it)
		{
			pObject = it->second;
			pObject->ResetPicking();
			pObject->Picking(&pick, origin, dir);
			if (pick.GetRecord())
			{
				float fDist = pick.GetRecord()->GetDistance();
				if (fDist < fLenth)
					fLenth = fDist;
			}
		}
	}

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	CsMapObjectGroup const* pFTGroup = nsCsMapTable::g_pActObjectMng->GetGroup(dwMapID);
	if (pFTGroup)
	{
		std::list<DWORD> dwActorObjId;
		pFTGroup->GetActorObjectID(dwActorObjId);

		std::list<DWORD>::iterator it = dwActorObjId.begin();
		for (; it != dwActorObjId.end(); ++it)
		{
			CsGBObject* pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject((*it));
			if (NULL == pCsGBObject)
			{
				assert_csm1(false, "ActorObjectMng의 리스트에는 존재하나 오브젝트를 찾을 수 없다.(%d)", (*it));
				continue;
			}

			CsGBObjProp* pActorProp = pCsGBObject->GetProp(CsGBObjProp::PROP_ACTOR);
			if (NULL == pActorProp)
			{
				assert_csm1(false, "ActorObjectMng의 리스트에는 존재하나 오브젝트의 속성에 Prop_Actor 속성이 없다(%d)", (*it));
				continue;
			}

			SAFE_POINTER_CON(pActorProp);

			CsGBObjProp_Actor* pProp = static_cast<CsGBObjProp_Actor*>(pActorProp);
			if (_stricmp(pProp->GetKey(), "door") == 0)
			{
				DWORD dwCurAni = pProp->GetAnimation();
				if (dwCurAni == CsGBObjProp_Actor::ACT_ANIMATION1)	// 닫혀있는 상태면 
				{
					//오브젝트 픽
					pCsGBObject->ResetPicking();
					pCsGBObject->Picking(&pick, origin, dir);
					if (pick.GetRecord())
					{
						float fDist = pick.GetRecord()->GetDistance();
						if (fDist < fLenth)
						{
							fLenth = fDist;
						}
					}
				}
			}
		}
	}

	// 몬스터
	if ((pReturn = g_pCharMng->PickMonster(&pick, &origin, &dir, type, fLenth)) != NULL)
		return pReturn;

	// 엔피씨
	if ((pReturn = g_pNpcMng->PickNpc(&pick, &origin, &dir, type, fLenth)) != NULL)
		return pReturn;

	// 아이템
	if ((pReturn = g_pItemMng->PickItem(&pick, &origin, &dir, type, fLenth)) != NULL)
		return pReturn;

	// 개인상점
	if ((pReturn = g_pCharMng->PickEmployment(&pick, &origin, &dir, type, fLenth)) != NULL)
		return pReturn;

	// 디지몬
	if ((pReturn = g_pCharMng->PickDigimon(&pick, &origin, &dir, type, fLenth, bIncludeSelf)) != NULL)
		return pReturn;

	// 테이머
	if ((pReturn = g_pCharMng->PickTamer(&pick, &origin, &dir, type, fLenth, bIncludeSelf)) != NULL)
		return pReturn;

	type.m_nUID = 0;
	return NULL;

	// 자기 디지몬
	//bool bPick = false;
	//for( int i=0; i<DIGIMON_MAX_COUNT; ++i )
	//{
	//	if( g_PlayerUser.m_pDigimon[ i ] )
	//	{
	//		if( g_PlayerUser.m_pDigimon[ i ]->PickObject( &pick, &origin, &dir, eRtti, nUID, fLenth ) == true )
	//			bPick = true;
	//	}
	//}
	//if( bPick == true )
	//	return true;

	//// 자기 테이머
	//if( g_PlayerUser.m_pTamer->PickObject( &pick, &origin, &dir, eRtti, nUID, fLenth ) == true )
	//	return true;

	/*type.m_nUID = 0;
	return false;*/
}

CsC_AvObject* CMngCollector::GetObject(cType type)
{
	switch (type.m_nClass)
	{
	case nClass::Item:				return g_pItemMng->GetItem(type.m_nIDX);
	case nClass::Npc:				return g_pNpcMng->GetNpc(type.m_nIDX);
	case nClass::Monster:			return g_pCharMng->GetMonster(type.m_nIDX);
	case nClass::Digimon:			return g_pCharMng->GetDigimon(type.m_nIDX);
	case nClass::Tamer:				return g_pCharMng->GetTamer(type.m_nIDX);
	case nClass::CommissionShop:	return g_pCharMng->GetEmployment(type.m_nIDX);
	default:				assert_cs(false);
	}
	assert_cs(false);
	return NULL;
}

CsC_AvObject* CMngCollector::GetObject(eRTTI_CLIENT eRtti, TCHAR const* szName)
{
	switch (eRtti)
	{
	case RTTI_TAMER:
	{
		std::map< DWORD, CTamer* >* pVector = g_pCharMng->GetTamerMap();
		std::map< DWORD, CTamer* >::iterator it = pVector->begin();
		for (; it != pVector->end(); ++it)
		{
			CTamer* pTamer = it->second;
			SAFE_POINTER_CON(pTamer);
			if (_tcscmp(pTamer->GetName(), szName) == 0)
			{
				return pTamer;
			}
		}
	}
	break;
	case RTTI_DIGIMON:
	{
		std::map<DWORD, CDigimon*>* pVector = g_pCharMng->GetDigimonMap();
		std::map<DWORD, CDigimon*>::iterator it = pVector->begin();
		for (; it != pVector->end(); ++it)
		{
			CDigimon* pDigimon = it->second;
			SAFE_POINTER_CON(pDigimon);
			if (_tcscmp(pDigimon->GetName(), szName) == 0)
			{
				return pDigimon;
			}
		}
	}
	break;
	}

	return NULL;
}

void CMngCollector::ResetObjectShader()
{
	g_pCharMng->ResetShader();
	g_pNpcMng->ResetShader();
	g_pItemMng->ResetShader();
	//g_pCardMng->ResetShader();
}

void CMngCollector::RenderText()
{
	if (m_bRenderText == false)
		return;

	_RenderText(CsPoint(10, 250), L"%3.1f fps", g_pEngine->m_Frame.GetFrameRate());

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
	if (pDigimonUser != NULL)
	{
		CsC_AvObject* pTarget = pDigimonUser->GetProp_Attack()->GetFollowTarget();
		if (pTarget != NULL)
		{
			float fLen = (pDigimonUser->GetPos() - pTarget->GetPos()).Length();
			_RenderText(CsPoint(10, 265), L"Attack Target Dist = %.1f", fLen);
		}
	}

#ifdef NDEBUG
	//return;
#endif

	int nStep = 9;
	if (g_pCharMng->GetTamerUser())
	{
		CTamerUser* pTamer = g_pCharMng->GetTamerUser();
		CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser(0);
		CsC_AvObject::sBASE_STAT* pBaseStat = pTamer->GetBaseStat();

		_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"Tamer Pos X = %d", pDigimon->GetPos().x);
		_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"Tamer Pos Y = %d", pDigimon->GetPos().y);
		_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"Tamer Pos Z = %d", pDigimon->GetPos().z);
		_RenderText(CsPoint(30, 150 + (nStep++ * 15)), L"LV = %d", pBaseStat->GetLevel());
		_RenderText(CsPoint(30, 150 + (nStep++ * 15)), L"HP = %d / %d", pBaseStat->GetHP(), pBaseStat->GetMaxHP());
		_RenderText(CsPoint(30, 150 + (nStep++ * 15)), L"DS = %d / %d", pBaseStat->GetDS(), pBaseStat->GetMaxDS());
	}

	if (g_pCharResMng->GetTargetMark())
	{
		CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
		_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"TargetIDX = %d", pTarget->GetIDX());
		_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"T Pos X = %d", pTarget->GetPos().x);
		_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"T Pos Y = %d", pTarget->GetPos().y);
	}

	/*nStep += 2;
	if( g_pCharMng->GetDigimonUser( 0 ) )
	{
	CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
	CsC_AvObject::sBASE_STAT* pBaseStat = pDigimon->GetBaseStat();

	g_pEngine->DrawText( 10, 150+ (nStep++ * 15), 0xffffffff, DT_LEFT, _T( "Digimon Pos X = %d" ), (int)pDigimon->GetPos().x );
	g_pEngine->DrawText( 10, 150+ (nStep++ * 15), 0xffffffff, DT_LEFT, _T( "Digimon Pos Y = %d" ), (int)pDigimon->GetPos().y );
	g_pEngine->DrawText( 10, 150+ (nStep++ * 15), 0xffffffff, DT_LEFT, _T( "Digimon Pos Z = %d" ), (int)pDigimon->GetPos().z );
	g_pEngine->DrawText( 30, 150+ (nStep++ * 15), 0xffffffff, DT_LEFT, _T( "LV = %d" ), pBaseStat->GetLevel() );
	g_pEngine->DrawText( 30, 150+ (nStep++ * 15), 0xffffffff, DT_LEFT, _T( "HP = %d / %d" ), pBaseStat->GetHP(), pBaseStat->GetMaxHP() );
	g_pEngine->DrawText( 30, 150+ (nStep++ * 15), 0xffffffff, DT_LEFT, _T( "DS = %d / %d" ), pBaseStat->GetDS(), pBaseStat->GetMaxDS() );
	}*/

	// 	nStep += 2;	
	if (g_pCharResMng->GetTargetMark())
	{
		CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser(0);
		CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();

		_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"T Dist %.1f", ((pDigimon->GetPos2D() - pTarget->GetPos2D()).Length()));
	}

	_RenderText(CsPoint(30, 150 + (nStep++ * 15)), L"OrgVal  = %d", (g_pEngine->GetGaussianBlurVal()->x * 1000.0f));
	_RenderText(CsPoint(30, 150 + (nStep++ * 15)), L"BlurVal = %d", (g_pEngine->GetGaussianBlurVal()->y * 1000.0f));
	_RenderText(CsPoint(30, 150 + (nStep++ * 15)), L"Filter  = %d", (g_pEngine->GetGaussianBlurVal()->z * 1000.0f));
#ifdef KEYBOARD_MOVE
	CsGBObject* pActor = nsCsGBTerrain::g_pCurRoot->GetActor_Object_NearPos("door", pDigimonUser->GetPos2D());
	if (pActor)
	{
		_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"ActObj Collusion = %d", (pActor->GetInfo()->s_Bound.InBoundCheck(pDigimonUser->GetPos2D().x, pDigimonUser->GetPos2D().y)));
	}
	_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"KeyState = %d", (g_pCharMng->GetDigimonUser(0)->GetUserServerMove()->GetKeyCheck()));
	_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"MousePosX = %d MousePosY = %d", (CURSOR_ST.GetPos().x), (CURSOR_ST.GetPos().y));
#endif
	_RenderText(CsPoint(10, 150 + (nStep++ * 15)), L"CameraDist = %d", (CAMERA_ST.GetDist()));
}

void CMngCollector::_RenderText(CsPoint const& pos, dm_str_t cptr szformat, ...)
{
	va_list		args;
	dm_sint_t	iLength = 0;

	dm_string_t stmp;
	stmp.resize(1024);

	va_start(args, szformat);
	iLength = ::vswprintf_s(&stmp.at(0), 1024, szformat, args);
	va_end(args);

	m_Text.SetText(stmp.c_str());
	m_Text.Render(pos);
}

void CMngCollector::CheckNearObject()
{
	SAFE_POINTER_RET(g_pServerMoveOwner);
	CsC_AvObject* pUser = g_pServerMoveOwner->GetParent();
	SAFE_POINTER_RET(pUser);
	NiPoint2 niPos = pUser->GetPos2D();

	sSearchObj pred;
	pred.SetTargetPos(niPos);
	pred.SetLength(500.0f);

	CNpcMng::NpcCont* pVector = g_pNpcMng->GetNpcVector();
	pred = std::for_each(pVector->begin(), pVector->end(), pred);

	_ChangeNearObject(pred);
}

void CMngCollector::_ChangeNearObject(sSearchObj const& sChangeObj)
{
	if (g_pGameIF->IsOpenNpcWindow())
		return;

	if (sChangeObj == m_sSearched)
		return;

	m_sSearched = sChangeObj;

	ContentsStream kTmp;
	kTmp << m_sSearched.s_nFindFTID << m_sSearched.s_nObjType;

	GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_ACTIONKEY_NPC, &kTmp);
}

void CMngCollector::ResetSearched()
{
	m_sSearched.Reset();
}
