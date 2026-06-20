
#include "stdafx.h"
#include "CharMng.h"

CCharMng*		g_pCharMng = NULL;

CCharMng::CCharMng()
{
	m_pTamerUser = NULL;
	for( int i=0; i<MAX_DIGIMONUSER_COUNT; ++i )
		m_pDigimonUser[ i ] = NULL;
}

void CCharMng::_DeleteContainer()
{
	std::map<DWORD,CTamer*>::iterator tamerit = m_mapTamer.begin();
	for( ; tamerit != m_mapTamer.end(); ++tamerit )
	{
		SAFE_POINTER_CON( tamerit->second );
		g_pCharResMng->ThreadDeleteChar( tamerit->second );
	}
	m_mapTamer.clear();

	std::map<DWORD, CDigimon*>::iterator digimonIT = m_mapDigimon.begin();
	for( ; digimonIT != m_mapDigimon.end(); ++digimonIT )
	{
		SAFE_POINTER_CON( digimonIT->second );
		g_pCharResMng->ThreadDeleteChar( digimonIT->second );
	}
	m_mapDigimon.clear();

	std::map<DWORD, CMonster*>::iterator monsterIT = m_mapMonster.begin();
	for( ; monsterIT != m_mapMonster.end(); ++monsterIT )
	{
		SAFE_POINTER_CON( monsterIT->second );
		g_pCharResMng->ThreadDeleteChar( monsterIT->second );
	}
	m_mapMonster.clear();

	std::map<DWORD, CEmployment*>::iterator empIT = m_mapEmployment.begin();
	for( ; empIT != m_mapEmployment.end(); ++empIT )
	{
		SAFE_POINTER_CON( empIT->second );
		g_pCharResMng->ThreadDeleteChar( empIT->second );
	}
	m_mapEmployment.clear();

	std::map<DWORD, CsC_AvObject*>::iterator tmpIT = m_mapTempChar.begin();
	for( ; tmpIT != m_mapTempChar.end(); ++tmpIT )
	{
		SAFE_POINTER_CON( tmpIT->second );
		g_pCharResMng->ThreadDeleteChar( tmpIT->second );
	}
	m_mapTempChar.clear();

	std::map<DWORD, CPat*>::iterator patIT = m_mapPat.begin();
	for( ; patIT != m_mapPat.end(); ++patIT )
	{
		SAFE_POINTER_CON( patIT->second );
		g_pCharResMng->ThreadDeleteChar( patIT->second );
	}
	m_mapPat.clear();

#ifdef DEBUG_SYNC_CHAR
	std::map<DWORD, CsC_AvObject*>::iterator dcIT = m_mapDebugChar.begin();
	for( ; dcIT != m_mapDebugChar.end(); ++dcIT )
	{
		SAFE_POINTER_CON( dcIT->second );
		g_pCharResMng->ThreadDeleteChar( dcIT->second );
	}
	m_mapDebugChar.clear();
#endif

	std::list< CsC_AvObject* >::iterator deleteit = m_listDeleteChar.begin();
	for( ; deleteit != m_listDeleteChar.end(); ++deleteit )
		g_pCharResMng->ThreadDeleteChar( *deleteit );
	m_listDeleteChar.clear();

	auto dcIT2 = m_mapTempObject.begin();
	for( ; dcIT2 != m_mapTempObject.end(); ++dcIT2)
		g_pCharResMng->ThreadDeleteChar(dcIT2->second.m_pObj );
	m_mapTempObject.clear();
}

void CCharMng::Delete()
{
	_UserReleasePointer();
	_DeleteContainer();

#ifdef SYNC_DATA_LIST
	_DeleteSync();
#endif
	m_listMonsterSE_HighLight.clear();
}

void CCharMng::Init()
{	
	_DeleteContainer();

#ifdef SYNC_DATA_LIST
	_InitSync();
#endif

#ifndef _GIVE
	m_bRenderChar = true;
	m_bRenderUser = true;
#endif
	m_listMonsterSE_HighLight.clear();
}

void CCharMng::Reset()
{
	_UserReleasePointer();
	_DeleteContainer();
#ifdef SYNC_DATA_LIST
	_DeleteSync();
#endif
}

void CCharMng::Update()
{
#ifdef SYNC_DATA_LIST
	_UpdateSync();
#endif

	CCharOption::CalNameScaleConstant();	

	_UpdateDigimon( g_pResist->m_Global.s_bFigureDigimon );
	_UpdateTamer( g_pResist->m_Global.s_bFigureTamer, g_pResist->m_Global.s_bFigureEmployment == TRUE );
	_UpdateMonster();
	_UpdateEmployment( g_pResist->m_Global.s_bFigureEmployment );
	_UpdatePat( g_pResist->m_Global.s_bFigureTamer );
#ifdef DEBUG_SYNC_CHAR
	_UpdateDebugChar();
#endif

	_UpdateDeleteChar();

	_UpdateTempObject();

	g_pServerMoveOwner->UpdateCamera();
}

void CCharMng::Render()
{
#ifndef _GIVE
	if( m_bRenderChar == false )
		return;
#endif

	_RenderTamer( g_pResist->m_Global.s_bFigureTamer, g_pResist->m_Global.s_bFigureEmployment == TRUE );
	_RenderDigimon( g_pResist->m_Global.s_bFigureDigimon );
	_RenderMonster();
	_RenderEmployment( g_pResist->m_Global.s_bFigureEmployment );
	_RenderPat( g_pResist->m_Global.s_bFigureTamer );
#ifdef DEBUG_SYNC_CHAR
	_RenderDebugChar();
#endif

	_RenderDeleteChar();
	_RenderTempObject();
}

void CCharMng::Render_PostChar()
{
	if( g_pEventSceneMng->IsCameraScene() == false )
	{
		_RenderPostChar_Tamer();
		_RenderPostChar_Digimon();
		_RenderPostChar_Monster();
		_RenderPostChar_Employment();
		_RenderPostChar_Pat();
		_RenderPostChar_DeleteChar();
	}	
}

void CCharMng::Update_Tutorial()
{
	CCharOption::CalNameScaleConstant();

	std::map<DWORD, CTamer*>::iterator it = m_mapTamer.begin();
	for( ; it != m_mapTamer.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( it->second->GetLeafRTTI() == RTTI_TUTORIAL_TAMER )
			it->second->Update( g_fDeltaTime );
	}

	std::map<DWORD, CDigimon* >::iterator itDigi = m_mapDigimon.begin();
	for( ; itDigi != m_mapDigimon.end(); ++itDigi )
	{
		SAFE_POINTER_CON( itDigi->second );
		if( itDigi->second->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			itDigi->second->Update( g_fDeltaTime );
	}

	std::map<DWORD, CMonster* >::iterator itMon = m_mapMonster.begin();
	for( ; itMon != m_mapMonster.end(); ++itMon )
	{
		SAFE_POINTER_CON( itMon->second );
		if( itMon->second->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER )
			itMon->second->Update( g_fDeltaTime );
	}

	_UpdateTempObject();
	if( g_pServerMoveOwner )
		g_pServerMoveOwner->UpdateCamera();
}

void CCharMng::Render_Tutorial()
{
	std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
	for( ; it != m_mapDigimon.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( it->second->GetLeafRTTI() != RTTI_TUTORIAL_DIGIMON )
			continue;
		it->second->Render();
	}

	if( m_pTamerUser )
		m_pTamerUser->Render();

	std::map<DWORD, CMonster* >::iterator itMon = m_mapMonster.begin();
	for( ; itMon != m_mapMonster.end(); ++itMon )
	{
		SAFE_POINTER_CON( itMon->second );
		if( itMon->second->GetLeafRTTI() != RTTI_TUTORIAL_MONSTER )
			continue;
		itMon->second->Render();
	}
}

void CCharMng::Render_PostChar_Tutorial()
{
	std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
	for( ; it != m_mapDigimon.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( it->second->GetLeafRTTI() != RTTI_TUTORIAL_DIGIMON )
			continue;
		it->second->Render_PostChar();
	}

	if( m_pTamerUser )
		m_pTamerUser->Render_PostChar();

	std::map<DWORD, CMonster* >::iterator itMon = m_mapMonster.begin();
	for( ; itMon != m_mapMonster.end(); itMon = ++itMon )
	{
		SAFE_POINTER_CON( itMon->second );
		if( itMon->second->GetLeafRTTI() != RTTI_TUTORIAL_MONSTER )
			continue;
		itMon->second->Render_PostChar();
	}
}

void CCharMng::ResetShader()
{
	std::map<DWORD,CTamer*>::iterator tamerIT = m_mapTamer.begin();
	for( ; tamerIT != m_mapTamer.end(); ++tamerIT )
	{
		CsC_AvObject* pAvObject = tamerIT->second;
		SAFE_POINTER_CON( pAvObject );
		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}

	std::map<DWORD,CDigimon*>::iterator digiIT = m_mapDigimon.begin();
	for( ; digiIT != m_mapDigimon.end(); ++digiIT )
	{
		CsC_AvObject* pAvObject = digiIT->second;
		SAFE_POINTER_CON( pAvObject );
		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}

	std::map<DWORD,CMonster*>::iterator monIT = m_mapMonster.begin();
	for( ; monIT != m_mapMonster.end(); ++monIT )
	{
		CsC_AvObject* pAvObject = monIT->second;
		SAFE_POINTER_CON( pAvObject );
		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}

	std::map<DWORD,CEmployment*>::iterator empIT = m_mapEmployment.begin();
	for( ; empIT != m_mapEmployment.end(); ++empIT )
	{
		CsC_AvObject* pAvObject = empIT->second;
		SAFE_POINTER_CON( pAvObject );
		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}

	std::map<DWORD,CPat*>::iterator patIT = m_mapPat.begin();
	for( ; patIT != m_mapPat.end(); ++patIT )
	{
		CsC_AvObject* pAvObject = patIT->second;
		SAFE_POINTER_CON( pAvObject );
		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}

	std::map<DWORD,CsC_AvObject*>::iterator tmpIT = m_mapTempChar.begin();
	for( ; tmpIT != m_mapTempChar.end(); ++tmpIT )
	{
		CsC_AvObject* pAvObject = tmpIT->second;
		SAFE_POINTER_CON( pAvObject );
		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}

#ifdef DEBUG_SYNC_CHAR
	std::map<DWORD,CsC_AvObject*>::iterator debIT = m_mapDebugChar.begin();
	for( ; debIT != m_mapDebugChar.end(); ++debIT )
	{
		CsC_AvObject* pAvObject = debIT->second;
		SAFE_POINTER_CON( pAvObject );
		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}
#endif

	std::list< CsC_AvObject* >::iterator delit = m_listDeleteChar.begin();
	for( ; delit != m_listDeleteChar.end(); ++delit )
	{
		CsC_AvObject* pAvObject = *delit;
		SAFE_POINTER_CON( pAvObject );
		if( pAvObject->IsLoad() == false )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}

	std::map<DWORD,sTempObj>::iterator tmpOIT = m_mapTempObject.begin();
	for( ; tmpOIT != m_mapTempObject.end(); ++tmpOIT )
	{
		CsC_AvObject* pAvObject = tmpOIT->second.m_pObj;
		SAFE_POINTER_CON( pAvObject );
		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}
}

void CCharMng::ResetNameTamer()
{
	std::map<DWORD,CTamer*>::iterator it = m_mapTamer.begin();
	for( ; it != m_mapTamer.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		CCharOption* pOption = it->second->GetCharOption();
		SAFE_POINTER_CON( pOption );
		pOption->ResetNameColor();
	}
}

void CCharMng::ResetNameDigimon()
{
	std::map<DWORD,CDigimon*>::iterator it = m_mapDigimon.begin();
	for( ; it != m_mapDigimon.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		CCharOption* pOption = it->second->GetCharOption();
		SAFE_POINTER_CON( pOption );
		pOption->ResetNameColor();
	}
}


//==============================================================================================================
//
// 유저
//
//==============================================================================================================

void CCharMng::_UserReleasePointer()
{
	m_pTamerUser = NULL;
	for( int i=0; i<MAX_DIGIMONUSER_COUNT; ++i )
		m_pDigimonUser[ i ] = NULL;
}

//==============================================================================================================
//
// 테이머
//
//==============================================================================================================

void CCharMng::_UpdateTamer( bool bFigure, bool bFigureEmployment )
{
	if( ( bFigure == false )&&( bFigureEmployment == false ) )
	{
		std::map<DWORD, CTamer*>::iterator it = m_mapTamer.begin();
		for( ; it != m_mapTamer.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			it->second->Update( g_fDeltaTime );
		}
	}
	else if( ( bFigure == true )&&( bFigureEmployment == false ) )
	{
		bool bBackup = CsC_AniProp::g_bUpdateActor;
		CsC_AniProp::g_bUpdateActor = false;
		std::map<DWORD, CTamer*>::iterator it = m_mapTamer.begin();
		for( ; it != m_mapTamer.end(); ++it )
		{
			CTamer* pTamer = it->second;
			SAFE_POINTER_CON( pTamer );
			if( pTamer->GetLeafRTTI() == RTTI_TAMER )
			{
				pTamer->Update( g_fDeltaTime, false );

				if( pTamer->GetViewSight() )
				{
#ifdef GM_CLOCKING
					if( pTamer->IsClocking() && pTamer->GetLeafRTTI() != RTTI_TAMER_USER )
						continue;
#endif
					CCharResMng::sFIGURE_INFO::eFIGURETYPE eFigureInfo = CCharResMng::sFIGURE_INFO::FT_TAMER;
					g_pCharResMng->FigureInsert( pTamer->GetPos(), pTamer->GetCurRot(), pTamer->GetAniTime(), eFigureInfo );
				}
			}
			else
			{
				pTamer->Update( g_fDeltaTime );
			}
		}
		
		CsC_AniProp::g_bUpdateActor = bBackup;
	}
	else if( ( bFigure == false )&&( bFigureEmployment == true ) )
	{
		bool bBackup = CsC_AniProp::g_bUpdateActor;
		CsC_AniProp::g_bUpdateActor = false;
		std::map<DWORD, CTamer*>::iterator it = m_mapTamer.begin();
		for( ; it != m_mapTamer.end(); ++it )
		{
			CTamer* pTamer = it->second;
			SAFE_POINTER_CON( pTamer );
			if( pTamer->GetLeafRTTI() == RTTI_TAMER )
			{
				if( pTamer->GetCondition()->IsCondition( nSync::Shop ) == true )
				{
					CsC_AniProp::g_bUpdateActor = false;
					pTamer->Update( g_fDeltaTime, false );
					if( pTamer->GetViewSight() )
					{
						g_pCharResMng->FigureInsert( pTamer->GetPos(), pTamer->GetCurRot(), pTamer->GetAniTime(), CCharResMng::sFIGURE_INFO::FT_EMPLOYMENT );
					}
				}
				else
				{
					CsC_AniProp::g_bUpdateActor = bBackup;
					pTamer->Update( g_fDeltaTime );					
				}				
			}
			else
			{
				pTamer->Update( g_fDeltaTime );
			}
		}
		CsC_AniProp::g_bUpdateActor = bBackup;
	}
	else
	{
		bool bBackup = CsC_AniProp::g_bUpdateActor;
		CsC_AniProp::g_bUpdateActor = false;
		std::map<DWORD, CTamer*>::iterator it = m_mapTamer.begin();
		for( ; it != m_mapTamer.end(); ++it )
		{
			CTamer* pTamer = it->second;
			SAFE_POINTER_CON( pTamer );
			if( pTamer->GetLeafRTTI() == RTTI_TAMER )
			{
				pTamer->Update( g_fDeltaTime, false );

				if( pTamer->GetViewSight() )
				{
#ifdef GM_CLOCKING
					if( pTamer->IsClocking() && pTamer->GetLeafRTTI() == RTTI_TAMER_USER )
						continue;
#endif

					CCharResMng::sFIGURE_INFO::eFIGURETYPE eFigureInfo = CCharResMng::sFIGURE_INFO::FT_TAMER;
					if( pTamer->GetCondition()->IsCondition( nSync::Shop ) == true )
						eFigureInfo = CCharResMng::sFIGURE_INFO::FT_EMPLOYMENT;
					g_pCharResMng->FigureInsert( pTamer->GetPos(), pTamer->GetCurRot(), pTamer->GetAniTime(), eFigureInfo );
				}
			}
			else
			{
				pTamer->Update( g_fDeltaTime );
			}
		}
		
		CsC_AniProp::g_bUpdateActor = bBackup;
	}	
}

void CCharMng::_RenderTamer( bool bFigure, bool bFigureEmployment )
{
	if( ( bFigure == false )&&( bFigureEmployment == false ) )
	{
		std::map<DWORD, CTamer*>::iterator it = m_mapTamer.begin();
		for( ; it != m_mapTamer.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
#ifndef CLIENT_MONSTER
			it->second->Render();
#else
			if( m_bRenderUser )
				it->second->Render();
			else if( it->second->GetLeafRTTI() == RTTI_TAMER )
				it->second->Render();
#endif
		}
	}
	// 상점이 아닐때만 그리자
	else if( ( bFigure == false )&&( bFigureEmployment == true ) )
	{
		std::map<DWORD, CTamer*>::iterator it = m_mapTamer.begin();
		for( ; it != m_mapTamer.end(); ++it )
		{
			CTamer* pTamer = it->second;
			SAFE_POINTER_CON( pTamer );
			if( pTamer->GetLeafRTTI() == RTTI_TAMER )
			{
				if( pTamer->GetCondition()->IsCondition( nSync::Shop ) == false )
				{
					it->second->Render();
				}
			}
			else
			{
				it->second->Render();
			}
		}		
	}
	else
	{
		g_pCharMng->GetTamerUser()->Render();
	}	
}

void CCharMng::_RenderPostChar_Tamer()
{
	std::map<DWORD, CTamer*>::iterator it = m_mapTamer.begin();
	for( ; it != m_mapTamer.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render_PostChar();
	}
}

CTamer* CCharMng::AddTamer( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, TCHAR const* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwSyncOption )
{
	CTamer* pTamer = GetTamer( nIDX );

	if (NULL != pTamer) {
		return pTamer;
	}
	

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
	prop.s_dwOptionPlag = _OP_ALL;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	pTamer = CTamer::NewInstance();



	pTamer->Init( nIDX, nFileTableID, sPos, fRot, szName, &prop, dwSyncOption );
	m_mapTamer.insert(std::make_pair(nIDX, pTamer));
	pTamer->ThreadLoad_Part( pPartCordArray );

	

	/*if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		pTamer->SetEnableObject( false );*/

	return pTamer;
}

CTamerUser* CCharMng::AddTamerUser( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, TCHAR const* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwSyncOption )
{
	assert_cs( m_pTamerUser == NULL );

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_USER;
	prop.s_dwOptionPlag = _OP_ALL;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	m_pTamerUser = NiNew CTamerUser;
	m_mapTamer.insert( std::make_pair( nIDX, m_pTamerUser ) );

	// 초기화
	m_pTamerUser->Init( nIDX, nFileTableID, sPos, fRot, szName, &prop, dwSyncOption );

	// 쓰레드 로드
	m_pTamerUser->ThreadLoad_Part( pPartCordArray );

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		m_pTamerUser->SetEnableObject( false );

	return m_pTamerUser;
}

CTamerUser* CCharMng::AddTamerUser_Tutorial(uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, TCHAR const* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwSyncOption)
{
	assert_cs( m_pTamerUser == NULL );

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_USER;
	prop.s_dwOptionPlag = _OP_ALL;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	m_pTamerUser = NiNew CTutorialTamerUser;
	m_mapTamer.insert( std::make_pair( nIDX, m_pTamerUser ) );

	// 초기화
	m_pTamerUser->Init( nIDX, nFileTableID, sPos, fRot, szName, &prop, dwSyncOption );

	// 쓰레드 로드
	m_pTamerUser->ThreadLoad_Part( pPartCordArray );

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		m_pTamerUser->SetEnableObject( false );

	return m_pTamerUser;
}

CTamer* CCharMng::GetTamer( uint const& nIDX )
{ 
	std::map<DWORD,CTamer*>::iterator it = m_mapTamer.find( nIDX );
	if( it == m_mapTamer.end() )
		return NULL;
	return it->second;
}

CTamerUser* CCharMng::GetTamerUser()
{ 
	return m_pTamerUser; 
}

void CCharMng::ChangeTamerKFM( uint nIDX, uint nFileTableID )
{
	CTamer* pTamer = GetTamer( nIDX );
	SAFE_POINTER_RET( pTamer );

	// 필수 체크
	assert_cs( pTamer->IsLoad() == true );

	// 가지고 있는 데이터 초기화
	if( nFileTableID == 0 )
	{
		pTamer->ToOriginalPart( g_pDataMng->GetPostLoad()->GetTamerData()->s_Type.m_nType );
	}
	else
	{
		pTamer->ChangeKFM( nFileTableID );
		// 로드시 필요한 초기화
		sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
		// 초기화 - Prop 초기화 포함
		pUint->s_Prop = *pTamer->GetCharProp();
		// 로드 설정
		pUint->s_pLoadedObject = (CsC_Object*)pTamer;
		std::string strPath = pTamer->GetModelPath();
		strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );	

		// 쓰레드 로드
		g_pCharResMng->LoadChar( pUint );
	}	
}

void CCharMng::DeleteTamer( uint const& nIDX )
{
	std::map<DWORD, CTamer*>::iterator it = m_mapTamer.find(nIDX);
	if( it == m_mapTamer.end() )
		return;

	SAFE_POINTER_RET( it->second );

	g_pCharResMng->DeleteChar( it->second );	
	it->second->SetDeleteState( CsC_AvObject::DS_FADEOUT );
	m_listDeleteChar.push_back( it->second );
	m_mapTamer.erase( it );
}

#ifdef ADVENTURE_TAMER_CHANGE
void CCharMng::DelTamer()
{
	_UserReleasePointer();

	std::map< DWORD, CTamer* >::iterator tamerIT = m_mapTamer.begin();
	for( ; tamerIT != m_mapTamer.end(); ++tamerIT )
	{
		SAFE_POINTER_CON( tamerIT->second );
		g_pCharResMng->ThreadDeleteChar( tamerIT->second );		
	}
	m_mapTamer.clear();

	std::map< DWORD, CDigimon* >::iterator digiIT = m_mapDigimon.begin();
	for( ; digiIT != m_mapDigimon.end(); ++digiIT )
	{
		SAFE_POINTER_CON( digiIT->second );
		g_pCharResMng->ThreadDeleteChar( digiIT->second );		
	}
	m_mapDigimon.clear();
}
#endif

void CCharMng::DeleteGuildName( TCHAR const* szGuildName )
{
	std::map< DWORD, CTamer* >::iterator it = m_mapTamer.begin();
	for( ; it != m_mapTamer.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( it->second->GetLeafRTTI() == RTTI_TAMER_USER )
			continue;		

		CCharOption* pOpt = it->second->GetCharOption();
		SAFE_POINTER_CON( pOpt );
		if( _tcscmp( pOpt->GetGuildName(), szGuildName ) == 0 )
		{
			pOpt->DeleteGuildName();
#ifdef GUILD_RENEWAL
			pOpt->SetNameColor( DEFAULT_NAMECOLOR );
#endif
		}
	}

	if( m_pTamerUser )
	{
		CCharOption* pOpt = m_pTamerUser->GetCharOption();
		if( pOpt )
		{
			if( _tcscmp( pOpt->GetGuildName(), szGuildName ) == 0 )
			{
				pOpt->DeleteGuildName();
#ifdef GUILD_RENEWAL
				pOpt->SetNameColor( DEFAULT_NAMECOLOR );
#endif
			}
		}
	}
}

CsC_AvObject* CCharMng::PickTamer( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength, bool bIncludeSelf )
{
	CsC_AvObject* pReturn = NULL;
	std::map< DWORD, CTamer* >::iterator it = m_mapTamer.begin();
	for( ; it != m_mapTamer.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( bIncludeSelf == false )
		{
			if( it->second->GetLeafRTTI() == RTTI_TAMER_USER )
				continue;
		}

		if( g_pMngCollector && g_pMngCollector->IsSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT ) )
			continue;

		if( it->second->PickObject( pCsGBPick, vOrigin, vDir, fLength ) == true )
		{
			type.m_nIDX = it->second->GetIDX();
			type.m_nClass = nClass::Tamer;
			pReturn = it->second;
		}
	}
	return pReturn;
}


//==============================================================================================================
//
// 디지몬
//
//==============================================================================================================

void CCharMng::_UpdateDigimon( bool bFigure )
{
	if( bFigure == false )
	{
		std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
		for( ; it != m_mapDigimon.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			it->second->Update( g_fDeltaTime );
		}
	}
	else
	{
		bool bBackup = CsC_AniProp::g_bUpdateActor;
		CsC_AniProp::g_bUpdateActor = false;

		std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
		for( ; it != m_mapDigimon.end(); ++it )
		{
			CDigimon* pDigimon = it->second;
			SAFE_POINTER_CON( pDigimon );
			if( pDigimon->GetLeafRTTI() == RTTI_DIGIMON )
			{
				pDigimon->Update( g_fDeltaTime, false );

				if( pDigimon->GetViewSight() )
				{
#ifdef GM_CLOCKING
					if( pDigimon->IsClocking() && pDigimon->GetLeafRTTI() == RTTI_DIGIMON_USER )
						continue;
#endif
					g_pCharResMng->FigureInsert( pDigimon->GetPos(), pDigimon->GetCurRot(), pDigimon->GetAniTime(), CCharResMng::sFIGURE_INFO::FT_DIGIMON );
				}
			}
			else
			{
				pDigimon->Update( g_fDeltaTime );
			}			
		}
		CsC_AniProp::g_bUpdateActor = bBackup;
	}	
}

void CCharMng::_RenderDigimon( bool bFigure )
{
	if( bFigure == false )
	{
		std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
		for( ; it != m_mapDigimon.end(); it = ++it )
		{
			SAFE_POINTER_CON( it->second );
			
#ifndef CLIENT_MONSTER
			it->second->Render();
#else
			if( m_bRenderUser )
				it->second->Render();
			else if( it->second->GetLeafRTTI() == RTTI_DIGIMON )
				it->second->Render();
#endif			
		}
	}
	else
	{
		// 간략화 사용 중일 때 범위 표시 / 징표 이펙트 보이도록 수정
		std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
		for( ; it != m_mapDigimon.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );

			if( it->second->GetLeafRTTI() == RTTI_DIGIMON_USER )
				continue;

			CsC_EffectProp* pEffect = it->second->GetProp_Effect();
			if( NULL == pEffect )
				continue;

			CsC_EffectProp::SKILL_EFF_MAP* pEffMap = pEffect->_GetLoopSkillEffectMap();
			if( NULL == pEffMap )
				continue;

			CsC_EffectProp::SKILL_EFF_MAP_IT ItEff = pEffMap->begin();
			for( ; ItEff != pEffMap->end(); ++ItEff )
			{
				if( ItEff->second )
					ItEff->second->Render();
			}
		}
		g_pCharMng->GetDigimonUser( 0 )->Render();		
	}
}

void CCharMng::_RenderPostChar_Digimon()
{
	std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
	for( ; it != m_mapDigimon.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render_PostChar();
	}
}

CDigimon* CCharMng::AddDigimon( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, nSync::Pos sPos, float fRot, TCHAR const* szName )
{
	CDigimon* pDigimon = GetDigimon( nIDX );
	if( pDigimon )
		return pDigimon;

	// 로드시 필요한 초기화
	sCREATEINFO s_Prop;
	s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
	s_Prop.s_dwOptionPlag = _OP_ALL;
	s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	// 객체 할당
	pDigimon = CDigimon::NewInstance();
	pDigimon->Init( nIDX, nFileTableID, sPos, fRot, szName, &s_Prop );

	m_mapDigimon.insert( std::make_pair( nIDX, pDigimon ) );

// 	// 로드 설정
// 	pUint->s_pLoadedObject = (CsC_Object*)pDigimon;
// 	std::string strPath = pDigimon->GetModelPath();
// 	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );
// 
// 	// 쓰레드 로드
// 	g_pCharResMng->LoadChar( pUint );
	pDigimon->ThreadLoad_Part(pParts, true);

	return pDigimon;
}

CDigimonUser* CCharMng::AddDigimonUser( int nUserIndex, uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, std::list<nsDPART::sDPART_INFO> const* pParts, nSync::Pos sPos, float fRot, TCHAR const* szName, bool bInitPos )
{
	// 로드시 필요한 초기화
	sCREATEINFO s_Prop;
	s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_USER;
	s_Prop.s_dwOptionPlag = _OP_ALL;
	s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	// 객체 할당
	CDigimonUser** ppUser = NULL;
	assert_cs( m_pDigimonUser[ nUserIndex ] == NULL );
	ppUser = &m_pDigimonUser[ nUserIndex ];	
	assert_cs( ppUser != NULL );

	*ppUser = NiNew CDigimonUser;
	(*ppUser)->Init( nIDX, nFileTableID, dwBaseDigimonID, sPos, fRot, szName, &s_Prop, nUserIndex, bInitPos );
	m_mapDigimon.insert( std::make_pair( nIDX, *ppUser ) );
	
	// 쓰레드 로드
	(*ppUser)->ThreadLoad_Part(pParts, true);
	return *ppUser;
}

CDigimonUser* CCharMng::AddDigimonUser_Tutorial(int nUserIndex, uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, std::list<nsDPART::sDPART_INFO> const* pParts, nSync::Pos sPos, float fRot, TCHAR const* szName, bool bInitPos)
{
	// 로드시 필요한 초기화
	sCREATEINFO s_Prop;
	s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_USER;
	s_Prop.s_dwOptionPlag = _OP_ALL;
	s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	// 객체 할당
	CDigimonUser** ppUser = NULL;
	assert_cs( m_pDigimonUser[ nUserIndex ] == NULL );
	ppUser = &m_pDigimonUser[ nUserIndex ];	
	assert_cs( ppUser != NULL );

	*ppUser = NiNew CTutorialDigimonUser;
	(*ppUser)->Init( nIDX, nFileTableID, dwBaseDigimonID, sPos, fRot, szName, &s_Prop, nUserIndex, bInitPos );
	m_mapDigimon.insert( std::make_pair( nIDX, *ppUser ) );

	// 로드 설정	
	(*ppUser)->ThreadLoad_Part(pParts, true);

	return *ppUser;
}

void CCharMng::ChangeDigimonKFM( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts)
{
	CDigimon* pDigimon = GetDigimon( nIDX );
	SAFE_POINTER_RET( pDigimon );

	// 필수 체크
	assert_cs( pDigimon->IsLoad() == true );	

#define CSLAST_ERROR
	g_CsLastError.m_nIntValue2 = pDigimon->IsLoad();	

	// 가지고 있는 데이터 초기화
	pDigimon->ChangeKFM( nFileTableID );

// 	// 로드시 필요한 초기화
// 	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
// 	// 초기화 - Prop 초기화 포함
// 	pUint->s_Prop = *pDigimon->GetCharProp();
// 	// 로드 설정
// 	g_CsLastError.m_nIntValue5 = pDigimon->GetModelID();
// 	g_CsLastError.m_nIntValue4 = g_pModelDataMng->IsData( pDigimon->GetModelID() );
// 
// 	pUint->s_pLoadedObject = (CsC_Object*)pDigimon;
// 	std::string strPath = pDigimon->GetModelPath();
// 	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

#define CSLAST_ERROR
	g_CsLastError.Release();

	// 쓰레드 로드
	pDigimon->ThreadLoad_Part(pParts, true);
	//g_pCharResMng->LoadChar(pUint);
}
#ifdef CROSSWARS_SYSTEM


void CCharMng::ChangeCrossDigimonKFM( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, bool bUser )
{
	CDigimon* pDigimon = GetDigimon( nIDX );
	SAFE_POINTER_RET( pDigimon );

	// 필수 체크
	assert_cs( pDigimon->IsLoad() == true );	

#define CSLAST_ERROR
	g_CsLastError.m_nIntValue2 = pDigimon->IsLoad();	

	// 가지고 있는 데이터 초기화
	pDigimon->ChangeCrossKFM( nFileTableID, bUser );

// 	// 로드시 필요한 초기화
// 	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
// 	// 초기화 - Prop 초기화 포함
// 	pUint->s_Prop = *pDigimon->GetCharProp();
// 	// 로드 설정
// 	g_CsLastError.m_nIntValue5 = pDigimon->GetModelID();
// 	g_CsLastError.m_nIntValue4 = g_pModelDataMng->IsData( pDigimon->GetModelID() );
// 
// 	pUint->s_pLoadedObject = (CsC_Object*)pDigimon;
// 	std::string strPath = pDigimon->GetModelPath();
// 	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

#define CSLAST_ERROR
	g_CsLastError.Release();

	// 쓰레드 로드
	//g_pCharResMng->LoadChar( pUint );
	pDigimon->ThreadLoad_Part(pParts, true);
}

void CCharMng::ChangeCrossDigimonModel( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, LPTSTR szName, float fNextScale, int nNextLevel, bool bUser )
{
	CDigimon* pDigimon = GetDigimon( nIDX );
	SAFE_POINTER_RET( pDigimon );

	// 필수 체크
	assert_cs( pDigimon->IsLoad() == true );

#define CSLAST_ERROR
	g_CsLastError.m_nIntValue2 = pDigimon->IsLoad();				

	// 가지고 있는 데이터 초기화
	pDigimon->ChangeCrossModel( nFileTableID, szName, bUser );
	
	if( pDigimon->GetLeafRTTI() != RTTI_DIGIMON_USER )
	{
		pDigimon->SetScale( fNextScale );
		pDigimon->GetBaseStat()->SetLevel( nNextLevel );
	}

// 	// 로드시 필요한 초기화
// 	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
// 	// 초기화 - Prop 초기화 포함
// 	pUint->s_Prop = *pDigimon->GetCharProp();
// 
// 	// 로드 설정
// 	g_CsLastError.m_nIntValue5 = pDigimon->GetModelID();
// 	g_CsLastError.m_nIntValue4 = g_pModelDataMng->IsData( pDigimon->GetModelID() );
// 
// 	// 로드 설정
// 	pUint->s_pLoadedObject = (CsC_Object*)pDigimon;
// 	std::string strPath = pDigimon->GetModelPath();
// 	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );	

#define CSLAST_ERROR
	g_CsLastError.Release();

	// 쓰레드 로드
	//g_pCharResMng->LoadChar( pUint );
	pDigimon->ThreadLoad_Part(pParts, true);

}
#endif
void CCharMng::ChangeDigimonModel( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, LPTSTR szName, float fNextScale, int nNextLevel )
{
	CDigimon* pDigimon = GetDigimon( nIDX );
	SAFE_POINTER_RET( pDigimon );

	// 필수 체크
	assert_cs( pDigimon->IsLoad() == true );

#define CSLAST_ERROR
	g_CsLastError.m_nIntValue2 = pDigimon->IsLoad();				

	// 가지고 있는 데이터 초기화
	pDigimon->ChangeModel( nFileTableID, szName );
	if( pDigimon->GetLeafRTTI() != RTTI_DIGIMON_USER )
	{
		pDigimon->SetScale( fNextScale );
		pDigimon->GetBaseStat()->SetLevel( nNextLevel );
	}

// 	// 로드시 필요한 초기화
// 	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
// 	// 초기화 - Prop 초기화 포함
// 	pUint->s_Prop = *pDigimon->GetCharProp();
// 
// 	// 로드 설정
// 	g_CsLastError.m_nIntValue5 = pDigimon->GetModelID();
// 	g_CsLastError.m_nIntValue4 = g_pModelDataMng->IsData( pDigimon->GetModelID() );
// 
// 	// 로드 설정
// 	pUint->s_pLoadedObject = (CsC_Object*)pDigimon;
// 	std::string strPath = pDigimon->GetModelPath();
// 	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

#define CSLAST_ERROR
	g_CsLastError.Release();

	// 쓰레드 로드
//	g_pCharResMng->LoadChar( pUint );
	pDigimon->ThreadLoad_Part(pParts, true);
}

CDigimon* CCharMng::GetDigimon( uint const& nIDX )
{ 
	std::map<DWORD, CDigimon*>::iterator it = m_mapDigimon.find( nIDX );
	if( it == m_mapDigimon.end() )
		return NULL;
	return it->second;
}

CDigimonUser* CCharMng::GetDigimonUser( int const& nIndex )
{ 
	return m_pDigimonUser[ nIndex ]; 
}

bool CCharMng::IsDigimonUser( uint nIDX )
{
	return ( GetDigimonUser( 0 ) )&&( GetDigimonUser( 0 )->GetIDX() == nIDX );
}

void CCharMng::DeleteDigimon( uint nIDX )
{
	if( IsDigimonUser( nIDX ) )
		return;

	std::map<DWORD,CDigimon*>::iterator it = m_mapDigimon.find( nIDX );
	if( it == m_mapDigimon.end() )
		return;

	SAFE_POINTER_RET( it->second );	

	// 셰이더 설정 이펙트에 걸려있는 녀석이 있는지 찾아서 지워준다.
	if( g_pSEffectMgr )
		g_pSEffectMgr->DeleteSEffect( it->second );

	g_pCharResMng->DeleteChar( it->second );
	it->second->SetDeleteState( CsC_AvObject::DS_FADEOUT );
	m_listDeleteChar.push_back( it->second );
	m_mapDigimon.erase( it );
}

void CCharMng::DeleteDigimonUser( uint nIDX )
{
	std::map<DWORD,CDigimon*>::iterator it = m_mapDigimon.find( nIDX );
	if( it == m_mapDigimon.end() )
		return;

	SAFE_POINTER_RET( it->second );
	//assert_cs( it->second->GetLeafRTTI() == RTTI_DIGIMON_USER );

	// 셰이더 설정 이펙트에 걸려있는 파트너몬을 지운다.
	if( g_pSEffectMgr )
		g_pSEffectMgr->DeleteSEffect(it->second);

	m_pDigimonUser[ ( (CDigimonUser*)it->second )->GetDigimonUserIndex() ] = NULL;
	g_pCharResMng->DeleteChar( it->second );
	it->second->SetDeleteState( CsC_AvObject::DS_FADEOUT );
	m_listDeleteChar.push_back( it->second );
	m_mapDigimon.erase( it );
}

CsC_AvObject* CCharMng::PickDigimon( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength, bool bIncludeSelf )
{
	CsC_AvObject* pReturn = NULL;
	std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
	for( ; it != m_mapDigimon.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( bIncludeSelf == false )
		{
			if( it->second->GetLeafRTTI() == RTTI_DIGIMON_USER )
				continue;
		}

		if( g_pMngCollector && g_pMngCollector->IsSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT ) )
			continue;

		if( it->second->PickObject( pCsGBPick, vOrigin, vDir, fLength ) == true )
		{
			type.SetIDX(it->second->GetIDX(), nClass::Digimon);
// 			type.m_nIDX = it->second->GetIDX();
// 			type.m_nClass = nClass::Digimon;
			pReturn = it->second;
		}
	}
	return pReturn;
}

//==============================================================================================================
//
//	몬스터
//
//==============================================================================================================

void CCharMng::_UpdateMonster()
{	
	std::map<DWORD, CMonster* >::iterator it = m_mapMonster.begin();
	for( ; it != m_mapMonster.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Update( g_fDeltaTime );
	}
}

void CCharMng::_RenderMonster()
{
	std::map<DWORD, CMonster* >::iterator it = m_mapMonster.begin();
	for( ; it != m_mapMonster.end(); it = ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render();
	}
}

void CCharMng::_RenderPostChar_Monster()
{
	std::map<DWORD, CMonster* >::iterator it = m_mapMonster.begin();
	for( ; it != m_mapMonster.end(); it = ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render_PostChar();
	}
}

CMonster* CCharMng::AddMonster( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, CMonster::eMONSTER_STATE MonsterState )
{
	CMonster* pMonster = GetMonster( nIDX );
	if( pMonster )
		return pMonster;

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
	pUint->s_Prop.s_dwOptionPlag = _OP_ALL;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	pMonster = CMonster::NewInstance();
	pMonster->Init( nIDX, nFileTableID, sPos, fRot, &pUint->s_Prop, MonsterState );

	m_mapMonster.insert( std::make_pair( nIDX, pMonster ) );

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pMonster;	

	std::string strPath = pMonster->GetModelPath();
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	if( IsSEHighLightMonster_TableID(nFileTableID) )
		pMonster->SetShaderEffect_Highlight();

	return pMonster;
}

CMonster* CCharMng::AddMonster_Tutorial(uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, CMonster::eMONSTER_STATE MonsterState)
{
	CMonster* pMonster = GetMonster( nIDX );
	if( pMonster )
		return pMonster;

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
	pUint->s_Prop.s_dwOptionPlag = _OP_ALL;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	pMonster = NiNew CTutorialMonster;
	pMonster->Init( nIDX, nFileTableID, sPos, fRot, &pUint->s_Prop, MonsterState );

	m_mapMonster.insert( std::make_pair( nIDX, pMonster ) );

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pMonster;	

	std::string strPath = pMonster->GetModelPath();
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	if( IsSEHighLightMonster_TableID(nFileTableID) )
		pMonster->SetShaderEffect_Highlight();

	return pMonster;
}

void CCharMng::DeleteMonster( uint nIDX, CsC_AvObject::eDELETE_STATE ds )
{
	std::map<DWORD, CMonster* >::iterator it = m_mapMonster.find(nIDX);
	if( it == m_mapMonster.end() )
		return;

	SAFE_POINTER_RET( it->second );

	// 바닥 이펙트 삭제
	DeleteTempObject( it->second->GetUniqID() );
	if( g_pSEffectMgr )
		g_pSEffectMgr->DeleteSEffect(it->second);
	if( g_pCharResMng )
		g_pCharResMng->DeleteChar( it->second );

	it->second->SetDeleteState( ds );
	m_listDeleteChar.push_back( it->second );

	m_mapMonster.erase( it );
}

void CCharMng::NextSelectMonster()
{
	assert_cs( m_pTamerUser );

	CsC_AvObject* pFind = NULL;

	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		{
			if( g_pMngCollector && g_pMngCollector->IsSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT ) )
				return;

			CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
			CsC_AvObject* pSelect = NULL;
			if( pTarget && ( pTarget->GetLeafRTTI() == RTTI_MONSTER ) )
				pSelect = pTarget;

			// 선택된거보다 먼넘을 찾는다
			pFind = _FindObject_FromDist( GetTamerUser(), pSelect, RTTI_MONSTER );
			if( pFind == NULL )
			{
				// 없다면 다시 앞에서 부터 찾고 - 그래도 없다면 리턴
				if( ( pFind = _FindObject_FromDist( GetTamerUser(), NULL, RTTI_MONSTER ) ) == NULL )
					return;
			}			
		}
		break;
	case nLIB::SVR_BATTLE:
		{
			CDigimonUser* pDigimonUser = GetDigimonUser( 0 );
			CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
			CsC_AvObject* pSelect = NULL;
			if( pTarget && ( pTarget->GetLeafRTTI() == RTTI_DIGIMON ) )
				pSelect = pTarget;

			// 선택된거보다 먼넘을 찾는다
			pFind = _FindObject_FromDist( pDigimonUser, pSelect, RTTI_DIGIMON, pDigimonUser->GetBattleTeam() );
			if( pFind == NULL )
			{
				// 없다면 다시 앞에서 부터 찾고 - 그래도 없다면 리턴
				if( ( pFind = _FindObject_FromDist( GetDigimonUser( 0 ), NULL, RTTI_DIGIMON, pDigimonUser->GetBattleTeam() ) ) == NULL )
					return;
			}
		}
		break;
	}

	// 찾은게 잇다면 설정	
	g_pCharMng->GetTamerUser()->SetArriveTarget( NULL );
	g_pCharResMng->SetTargetMark( pFind );
}


CsC_AvObject* CCharMng::_FindObject_FromDist( CsC_AvObject* pFinder, CsC_AvObject* pSelect, int nFindRTTI, int nExceptValue /*=0*/ )
{
	// 15 미터 내외만 찾겠다
#define SEARCH_MONSTER_DIST			1500.0f
#define SEARCH_DIGIMON_DIST			5000.0f

	NiPoint2 vFinderPos = pFinder->GetPos2D();

	float fCurDist;
	float fMinDist = pSelect == NULL ? 0.0f : ( vFinderPos - pSelect->GetPos2D() ).Length();
	float fMaxDist = FLT_MAX;

	CsC_AvObject* pFind = NULL;

	switch( nFindRTTI )
	{
	case RTTI_MONSTER:
		{
			std::map<DWORD, CMonster* >::iterator it = m_mapMonster.begin();
			for( ; it != m_mapMonster.end(); ++it )
			{
				CsC_AvObject* pCur = it->second; 
				SAFE_POINTER_CON( pCur );
				if( pCur == pSelect )
					continue;

				if( pCur->IsPickEnable() == false )
					continue;

				fCurDist = ( pCur->GetPos2D() - vFinderPos ).Length();
				if( fCurDist < fMinDist )
					continue;

				if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_DUNGEON )
				{
					if( fCurDist > 850 )
						continue;
				}
				else
				{
					if( fCurDist > SEARCH_MONSTER_DIST )
						continue;
				}

				if( fCurDist < fMaxDist )
				{
					fMaxDist = fCurDist;
					pFind = pCur;				
				}
			}
			return pFind;
		}
	case RTTI_DIGIMON:
		{
			std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
			for( ; it != m_mapDigimon.end(); it = ++it )
			{
				CsC_AvObject* pCur = it->second;
				SAFE_POINTER_CON( pCur );

				if( pCur == pSelect )
					continue;

				if( pCur->IsPickEnable() == false )
					continue;

				// 같은 팀이면 제외
				if( ( (CDigimon*)pCur )->GetBattleTeam() == nExceptValue )
					continue;

				fCurDist = ( pCur->GetPos2D() - vFinderPos ).Length();
				if( fCurDist < fMinDist )
					continue;
				if( fCurDist > SEARCH_DIGIMON_DIST )
					continue;		

				if( fCurDist < fMaxDist )
				{
					fMaxDist = fCurDist;
					pFind = pCur;
				}
			}
			return pFind;
		}
		break;
	default:
		assert_cs( false );
	}

	return pFind;
}

CsC_AvObject* CCharMng::PickMonster( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength )
{
	CsC_AvObject* pReturn = NULL;
	std::map<DWORD, CMonster* >::iterator it = m_mapMonster.begin();
	for( ; it != m_mapMonster.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( g_pMngCollector && g_pMngCollector->IsSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT ) )
		{
			if( it->second->GetLeafRTTI() != RTTI_TUTORIAL_MONSTER )
				continue;
		}
		if( it->second->PickObject( pCsGBPick, vOrigin, vDir, fLength ) == true )
		{
			type.SetIDX(it->second->GetIDX(), nClass::Monster);
			//type.m_nIDX = it->second->GetIDX();
			//type.m_nClass = nClass::Monster;
			pReturn = it->second;
		}
	}
	return pReturn;
}

int CCharMng::GetMonsterTempID()
{
	static int nIDX = 0;

	if( m_queueMonsterTempID.empty() == true )
	{
		return ++nIDX;
	}

	int nReturn = m_queueMonsterTempID.front();
	m_queueMonsterTempID.pop();
	return nReturn;
}

void CCharMng::PushMonsterTempID( int nIDX )
{
	m_queueMonsterTempID.push( nIDX );
}

int CCharMng::GetMyDigimonLv() const
{
	SAFE_POINTER_RETVAL( m_pDigimonUser[ 0 ], 0 );
	CsC_AvObject::sBASE_STAT* pBaseStat = m_pDigimonUser[ 0 ]->GetBaseStat();
	SAFE_POINTER_RETVAL( pBaseStat, 0 );
	return pBaseStat->GetLevel();
}
//==============================================================================================================
//
//	 펫
//
//==============================================================================================================


void CCharMng::_UpdatePat( BOOL bFigure )
{
	if( bFigure == FALSE )
	{
		std::map<DWORD,CPat*>::iterator it = m_mapPat.begin();
		for( ; it != m_mapPat.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			it->second->Update( g_fDeltaTime );
		}
	}
	else
	{
		std::map<DWORD,CPat*>::iterator it = m_mapPat.begin();
		for( ; it != m_mapPat.end(); ++it )
		{
			CPat* pPat = it->second;
			SAFE_POINTER_CON( pPat );
			if(pPat->GetIDX() == g_pCharMng->GetTamerUser()->GetIDX())
			{
				pPat->Update( g_fDeltaTime );
			}
			else
			{
				pPat->Update( g_fDeltaTime );
				if( pPat->GetViewSight() ){
					g_pCharResMng->FigureInsert( pPat->GetPos(), pPat->GetCurRot(), pPat->GetAniTime(), CCharResMng::sFIGURE_INFO::FT_PAT );
				}
			}
		}
		//CsC_AniProp::g_bUpdateActor = bBackup;
	}	
}
void CCharMng::_RenderPat( BOOL bFigure )
{	
	// 배틀 중에는 펫을 렌더하지 않음.
	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE) 
		return;

	if( bFigure == FALSE )
	{
		std::map<DWORD,CPat*>::iterator it = m_mapPat.begin();
		for( ; it != m_mapPat.end(); ++it )
		{		
			SAFE_POINTER_CON( it->second );
#ifndef CLIENT_MONSTER
			it->second->Render();
#else
			if( m_bRenderUser )
				it->second->Render();
			else if( it->second->GetIDX() != g_pCharMng->GetTamerUser()->GetIDX() )
				it->second->Render();
#endif	

		}
	}
	else
	{
		if(g_pCharMng->GetTamerUser()->GetPat())
			g_pCharMng->GetTamerUser()->GetPat()->Render();		
	}
}
void CCharMng::_RenderPostChar_Pat()
{
	std::map<DWORD,CPat*>::iterator it = m_mapPat.begin();
	for( ; it != m_mapPat.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if(it->second->GetIDX() == g_pCharMng->GetTamerUser()->GetIDX())
			it->second->Render();
	}
}

CPat* CCharMng::AddPat( CsC_AvObject* pParent, uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot )
{
	std::map<DWORD,CPat*>::iterator it = m_mapPat.find( nIDX );
	if( it != m_mapPat.end() )
		return it->second;

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_PATH_NONE | PR_ATTACK_NONE | PR_EFFECT_NONE | PR_MOVECON_NONE;
	pUint->s_Prop.s_dwOptionPlag = OP_FOG | OP_ABVWIRE | OP_LIGHT;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	CPat* pObject = CPat::NewInstance();
	pObject->Init( pParent, nIDX, nFileTableID, vPos, fRot, &pUint->s_Prop );
	// 여기서 테이블 참고하여 스케일 설정한다.
	//pObject->SetScale( pFTObj->s_fScale );

	m_mapPat.insert( std::make_pair( nIDX, pObject ) );

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pObject;	

	std::string strPath = pObject->GetModelPath();
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	return pObject;
}

CPat* CCharMng::AddPat( CsC_AvObject* pParent, uint nIDX, uint nFileTableID, sCREATEINFO* pProp, NiPoint3 vPos, float fRot )
{
	std::map<DWORD,CPat*>::iterator it = m_mapPat.find( nIDX );
	if( it != m_mapPat.end() )
		return it->second;

	// 로드시 필요한 초기화

	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
	pUint->s_Prop = *pProp;
// 	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_PATH_NONE | PR_ATTACK_NONE | PR_EFFECT_NONE | PR_MOVECON_NONE;
// 	pUint->s_Prop.s_dwOptionPlag = OP_FOG | OP_ABVWIRE | OP_LIGHT;
// 	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	CPat* pObject = CPat::NewInstance();
	pObject->Init( pParent, nIDX, nFileTableID, vPos, fRot, &pUint->s_Prop );
	// 여기서 테이블 참고하여 스케일 설정한다.
	//pObject->SetScale( pFTObj->s_fScale );

	m_mapPat.insert( std::make_pair( nIDX, pObject ) );

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pObject;	

	std::string strPath = pObject->GetModelPath();
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	return pObject;
}

void CCharMng::DeletePat( uint nIDX )
{
	std::map<DWORD,CPat*>::iterator it = m_mapPat.find( nIDX );
	if( it == m_mapPat.end() )
		return;

	SAFE_POINTER_RET(it->second);

	g_pCharResMng->DeleteChar( it->second );
	it->second->SetDeleteState( CsC_AvObject::DS_FADEOUT );
	m_listDeleteChar.push_back( it->second );
	m_mapPat.erase( it );
}



//==============================================================================================================
//
//		대리상점
//
//==============================================================================================================

void CCharMng::_UpdateEmployment( BOOL bFigure )
{
	if( bFigure == FALSE )
	{
		std::map<DWORD, CEmployment*>::iterator it = m_mapEmployment.begin();
		for( ; it != m_mapEmployment.end(); ++it )
		{
			SAFE_POINTER_CON(it->second);
			it->second->Update( g_fDeltaTime );
		}
	}
	else
	{
		bool bBackup = CsC_AniProp::g_bUpdateActor;
		CsC_AniProp::g_bUpdateActor = false;

		std::map<DWORD, CEmployment*>::iterator it = m_mapEmployment.begin();
		for( ; it != m_mapEmployment.end(); ++it )
		{
			CEmployment* pEmployment = it->second;
			SAFE_POINTER_CON(pEmployment);

			pEmployment->Update( g_fDeltaTime, false );
			if( pEmployment->GetViewSight() )
				g_pCharResMng->FigureInsert( pEmployment->GetPos(), pEmployment->GetCurRot(), pEmployment->GetAniTime(), CCharResMng::sFIGURE_INFO::FT_EMPLOYMENT );
		}
		CsC_AniProp::g_bUpdateActor = bBackup;
	}	
}

void CCharMng::_RenderEmployment( BOOL bFigure )
{
	if( bFigure )
		return;

	std::map<DWORD, CEmployment*>::iterator it = m_mapEmployment.begin();
	for( ; it != m_mapEmployment.end(); ++it )
	{
		SAFE_POINTER_CON(it->second);
		it->second->Render();
	}
}

void CCharMng::_RenderPostChar_Employment()
{
	std::map<DWORD, CEmployment*>::iterator it = m_mapEmployment.begin();
	for( ; it != m_mapEmployment.end(); ++it )
	{
		SAFE_POINTER_CON(it->second);
		it->second->Render_PostChar();
	}
}

bool CCharMng::IsEmployment( uint const& nIDX ) const
{ 
	std::map<DWORD,CEmployment*>::const_iterator it = m_mapEmployment.find( nIDX );
	if( it == m_mapEmployment.end() )
		return false;
	return true;
}

CEmployment* CCharMng::GetEmployment( uint const& nIDX )
{ 
	std::map<DWORD,CEmployment*>::iterator it = m_mapEmployment.find( nIDX );
	if( it == m_mapEmployment.end() )
		return NULL;

	return it->second;
}

CEmployment* CCharMng::AddEmployment( uint nIDX, uint nItemID, NiPoint3 vPos, float fRot )
{
	CEmployment* pObject = GetEmployment( nIDX );
	if( pObject )
		return pObject;

	assert_cs( nsCsFileTable::g_pBaseMng->GetPersonStore()->s_mapObject.find( nItemID ) != nsCsFileTable::g_pBaseMng->GetPersonStore()->s_mapObject.end() );
	CsBaseMng::sPERSON_STORE::sOBJECT_INFO* pFTObj = nsCsFileTable::g_pBaseMng->GetPersonStore()->s_mapObject[ nItemID ];

#ifdef COMPAT_487
	if (pFTObj == NULL) {
		return pObject;
	}
#endif

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, pFTObj->s_nDigimonID );
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_PATH_NONE | PR_ATTACK_NONE | PR_EFFECT_NONE | PR_MOVECON_NONE;
	pUint->s_Prop.s_dwOptionPlag = OP_FOG | OP_ABVWIRE | OP_LIGHT;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();


	pObject = CEmployment::NewInstance();
	pObject->Init( nIDX, pFTObj->s_nDigimonID, vPos, fRot, &pUint->s_Prop );
	pObject->SetScale( pFTObj->s_fScale );

	m_mapEmployment.insert( std::make_pair( nIDX, pObject ) );

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pObject;	

	std::string strPath = pObject->GetModelPath();
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	return pObject;
}

void CCharMng::DeleteEmployment( uint nIDX )
{
	std::map<DWORD,CEmployment*>::iterator it = m_mapEmployment.find( nIDX );
	if( it == m_mapEmployment.end() )
		return;

	SAFE_POINTER_RET( it->second );
	g_pCharResMng->DeleteChar( it->second );
	it->second->SetDeleteState( CsC_AvObject::DS_FADEOUT );
	m_listDeleteChar.push_back( it->second );
	m_mapEmployment.erase( it );
}

CsC_AvObject* CCharMng::PickEmployment( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength )
{
	CsC_AvObject* pReturn = NULL;
	std::map<DWORD, CEmployment*>::iterator it = m_mapEmployment.begin();
	for( ; it != m_mapEmployment.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( !it->second->PickObject( pCsGBPick, vOrigin, vDir, fLength ) )
			continue;
		type.m_nIDX = it->second->GetIDX();
		type.m_nClass = nClass::CommissionShop;
		pReturn = it->second;
	}
	return pReturn;
}


//==============================================================================================================
//
//	임시 케릭터 - 테이머
//
//==============================================================================================================

void CCharMng::_UpdateTempChar()
{
	std::map<DWORD, CsC_AvObject*>::iterator it = m_mapTempChar.begin();
	for( ; it != m_mapTempChar.end(); ++it )	
	{
		SAFE_POINTER_CON( it->second );
		it->second->Update( g_fDeltaTime );
	}
}

void CCharMng::_RenderTempChar()
{
	std::map<DWORD, CsC_AvObject*>::iterator it = m_mapTempChar.begin();
	for( ; it != m_mapTempChar.end(); ++it )	
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render();
	}
}

void CCharMng::DeleteTempChar( CsC_AvObject* pDeleteChar )
{
	SAFE_POINTER_RET( pDeleteChar );
	std::map<DWORD,CsC_AvObject*>::iterator it = m_mapTempChar.find( pDeleteChar->GetIDX() );
	if( it == m_mapTempChar.end() )
		return;

	PushTempCharID( pDeleteChar->GetIDX() );
	g_pCharResMng->DeleteChar( pDeleteChar );
	g_pCharResMng->ThreadDeleteChar( pDeleteChar );
	m_mapTempChar.erase( it );
}

CTamer* CCharMng::AddTempTamer( sCREATEINFO* pProp, uint nFileTableID, NiPoint3 vPos, float fRot, TCHAR* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwServerOption )
{
	UINT nIDX = PopTempCharID();

	CTamer* pTamer = CTamer::NewInstance();
	m_mapTempChar.insert( std::make_pair( nIDX, pTamer ) );

	pTamer->Init( nIDX, nFileTableID, vPos, fRot, szName, pProp, dwServerOption );
	pTamer->ThreadLoad_Part( pPartCordArray );
	return pTamer;
}

CDigimon* CCharMng::AddTempDigimon( uint nIDX, sCREATEINFO* pProp, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, NiPoint3 vPos, float fRot, TCHAR* szName )
{
//	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
// 
// 	// 초기화 - Prop 초기화 포함
// 	pUint->s_Prop = *pProp;
// 	assert_cs( pUint->s_Prop.s_dwPropPlag & PR_ANI_ENABLE );
// 	assert_cs( pUint->s_Prop.s_dwPropPlag & PR_EFFECT_ENABLE );

	CDigimon* pDigimon = CDigimon::NewInstance();
	m_mapTempChar.insert( std::make_pair( nIDX, pDigimon ) );

	pDigimon->Init( nIDX, nFileTableID, vPos, fRot, szName, pProp );
	// 로드 설정
// 	pUint->s_pLoadedObject = (CsC_Object*)pDigimon;
// 	std::string strPath = pDigimon->GetModelPath();
// 	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );
// 
// 	// 쓰레드 로드
// 	g_pCharResMng->LoadChar( pUint );
	pDigimon->ThreadLoad_Part(pParts, true);

	return pDigimon;
}

CDigimon* CCharMng::AddTempDigimon( sCREATEINFO* pProp, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, NiPoint3 vPos, float fRot, TCHAR* szName )
{
	UINT nIDX = PopTempCharID();
	//assert_cs( m_mapTempChar.IsElement( nIDX ) == false );

	return AddTempDigimon( nIDX, pProp, nFileTableID, pParts, vPos, fRot, szName );
}

CsC_AvObject* CCharMng::GetTempChar( uint nIDX )
{
	std::map<DWORD,CsC_AvObject*>::iterator it = m_mapTempChar.find( nIDX );
	if( it == m_mapTempChar.end() )
		return NULL;
	return it->second;
}

uint CCharMng::PopTempCharID()
{
	static uint nUID = 0;
	uint nReturn = 0;
	if( m_stackTempCharID.empty() == false )
	{
		nReturn = m_stackTempCharID.top();
		m_stackTempCharID.pop();
	}
	else
	{
		nReturn = ++nUID;
	}
	return nReturn;
}

void CCharMng::PushTempCharID( uint nUID )
{
	m_stackTempCharID.push( nUID );
}



bool CCharMng::AttachEffectFromTamerUIDX(uint nTamerUID, char* cEffectPath, float fScale, DWORD dwPlag, bool bPreLoad/* = false */)
{
	CTamer* pFindTamer = GetTamerFromUIDX( nTamerUID );
	if( pFindTamer )
	{
		return pFindTamer->AttachEffect( cEffectPath, fScale, dwPlag, bPreLoad );
	}
	else if( m_pTamerUser )
	{
		if( m_pTamerUser->GetUniqID() == nTamerUID )
			return m_pTamerUser->AttachEffect( cEffectPath, fScale, dwPlag, bPreLoad );
	}
	return false;
}

//==============================================================================================================
//
//	디버그 케릭터
//
//==============================================================================================================

#ifdef DEBUG_SYNC_CHAR

void CCharMng::_UpdateDebugChar()
{
	std::map<DWORD,CsC_AvObject*>::iterator it = m_mapDebugChar.begin();
	for( ; it != m_mapDebugChar.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Update( g_fDeltaTime );
	}
}

void CCharMng::_RenderDebugChar()
{
	std::map<DWORD,CsC_AvObject*>::iterator it = m_mapDebugChar.begin();
	for( ; it != m_mapDebugChar.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render();
	}
}

void CCharMng::DeleteDebugChar( CsC_AvObject* pDeleteChar )
{
	SAFE_POINTER_RET( pDeleteChar );
	std::map<DWORD,CsC_AvObject*>::iterator it = m_mapDebugChar.find( pDeleteChar->GetIDX() );
	if(it == m_mapDebugChar.end() )
		return;

	g_pCharResMng->DeleteChar( pDeleteChar );
	g_pCharResMng->ThreadDeleteChar( pDeleteChar );
	m_mapDebugChar.erase( it );
}

CTamer* CCharMng::AddDebugTamer( uint nIDX, sCREATEINFO* pProp, uint nFileTableID, NiPoint3 vPos, float fRot, TCHAR* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwServerOption )
{
	CTamer* pTamer = CTamer::NewInstance();
	m_mapDebugChar.insert( std::make_pair( nIDX, pTamer ) );

	pTamer->Init( nIDX, nFileTableID, vPos, fRot, szName, pProp, dwServerOption );
	pTamer->ThreadLoad_Part( pPartCordArray );
	return pTamer;
}

CDigimon* CCharMng::AddDebugDigimon( uint nIDX, sCREATEINFO* pProp, uint nFileTableID, NiPoint3 vPos, float fRot, TCHAR* szName )
{
	CDigimon* pDigimon = CDigimon::NewInstance();
	m_mapDebugChar.insert( std::make_pair( nIDX, pDigimon ) );

	pDigimon->Init( nIDX, nFileTableID, vPos, fRot, szName, pProp );
	pDigimon->ThreadLoad_Part(NULL, true);
	return pDigimon;

// 	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
// 
// 	// 초기화 - Prop 초기화 포함
// 	pUint->s_Prop = *pProp;
// 	assert_cs( pUint->s_Prop.s_dwPropPlag & PR_ANI_ENABLE );
// 	assert_cs( pUint->s_Prop.s_dwPropPlag & PR_EFFECT_ENABLE );
// 
// 	CDigimon* pDigimon = CDigimon::NewInstance();
// 	m_mapDebugChar.insert( std::make_pair( nIDX, pDigimon ) );
// 
// 	pDigimon->Init( nIDX, nFileTableID, vPos, fRot, szName, pProp );
// 	// 로드 설정
// 	pUint->s_pLoadedObject = (CsC_Object*)pDigimon;
// 	std::string strPath = pDigimon->GetModelPath();
// 	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );
// 
// 	// 쓰레드 로드
// 	g_pCharResMng->LoadChar( pUint );
//
//	return pDigimon;
}

CMonster* CCharMng::AddDebugMonster( uint nIDX, sCREATEINFO* pProp, uint nFileTableID, NiPoint3 vPos, float fRot, TCHAR* szName )
{
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );

	// 초기화 - Prop 초기화 포함
	pUint->s_Prop = *pProp;
	assert_cs( pUint->s_Prop.s_dwPropPlag & PR_ANI_ENABLE );
	assert_cs( pUint->s_Prop.s_dwPropPlag & PR_EFFECT_ENABLE );

	CMonster* pMonster = CMonster::NewInstance();
	m_mapDebugChar.insert( std::make_pair( nIDX, pMonster ) );

	pMonster->Init( nIDX, nFileTableID, vPos, fRot, pProp, CMonster::MONSTER_NORMAL );
	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pMonster;
	std::string strPath = pMonster->GetModelPath();
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	return pMonster;
}

CsC_AvObject* CCharMng::GetDebugChar( uint nIDX )
{
	std::map<DWORD, CsC_AvObject*>::iterator it = m_mapDebugChar.find( nIDX );
	if( it == m_mapDebugChar.end() )
		return NULL;
	return it->second;
}

void CCharMng::CheckDebugChar()
{
	/*if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		return;

	std::map<DWORD,CsC_AvObject*>::iterator it = m_mapDebugChar.begin();
	CsVectorLink< CsC_AvObject* >::iterator* itEnd = m_mapDebugChar.End();	
	for( ; it!=itEnd; it = it->s_pNext )
	{
		if( it->s_Element->GetClass() == nClass::Digimon )
		{

		}
	}
	if( m_mapDebugChar.IsElement( pDeleteChar->GetIDX() ) == false )
		return;

	m_mapDebugChar.DeleteElement( pDeleteChar->GetIDX() );	
	g_pCharResMng->DeleteChar( pDeleteChar );
	g_pCharResMng->ThreadDeleteChar( pDeleteChar );*/
}

#endif



//==============================================================================================================
//
//	제거 될 케릭터
//
//==============================================================================================================

void CCharMng::_UpdateDeleteChar()
{
	CsC_AvObject* pObject = NULL;
	std::list< CsC_AvObject* >::iterator it = m_listDeleteChar.begin();
	std::list< CsC_AvObject* >::iterator itEnd = m_listDeleteChar.end();
	while( it!=itEnd )
	{
		pObject = (*it);
		pObject->CsC_AvObject::Update( g_fDeltaTime );
		//pObject->Update( g_fDeltaTime );
		if( pObject->UpdateDelete( g_fDeltaTime ) == false )
		{
			g_pCharResMng->ThreadDeleteChar( pObject );
			it = m_listDeleteChar.erase( it );
		}
		else
		{
			++it;
		}
	}
}

void CCharMng::_RenderDeleteChar()
{
	std::list< CsC_AvObject* >::iterator it = m_listDeleteChar.begin();
	for( ; it != m_listDeleteChar.end(); ++it )
		(*it)->Render();
}

void CCharMng::_RenderPostChar_DeleteChar()
{
	std::list< CsC_AvObject* >::iterator it = m_listDeleteChar.begin();
	for( ; it != m_listDeleteChar.end(); ++it )
		(*it)->Render_PostChar();
}
//==============================================================================================================
//
// 몬스터 스킬 이펙트용 임시 오브젝트
//
//==============================================================================================================
void CCharMng::_UpdateTempObject()
{
	std::map< DWORD, sTempObj >::iterator it = m_mapTempObject.begin();
	for( ; it != m_mapTempObject.end();  )
	{
		CsC_AvObject* pObj = it->second.m_pObj;
		
		if( pObj )
			pObj->Update( g_fDeltaTime );

		if( !it->second.m_bAutoDel )
			++it;
		else
		{
			if( it->second.m_fLifeTime > 0.0f )
				it->second.m_fLifeTime -= g_fDeltaTime;

			if( it->second.m_fLifeTime < 0.0f )
			{
				it->second.m_fLifeTime = 0.0f;

				m_mapTempObject.erase( it++ );
				PushTempObjectID( pObj->GetIDX() );
				g_pCharResMng->DeleteChar( pObj );
				g_pCharResMng->ThreadDeleteChar( pObj );
			}
			else
				++it;
		}
	}
}

void CCharMng::_RenderTempObject()
{
	std::map< DWORD, sTempObj >::iterator it = m_mapTempObject.begin();
	for( ; it != m_mapTempObject.end(); ++it )
	{
		SAFE_POINTER_CON(it->second.m_pObj);
		it->second.m_pObj->Render();
	}
}

void CCharMng::DeleteTempObject( CsC_AvObject* pDeleteChar )
{
	SAFE_POINTER_RET( pDeleteChar );
	std::map< DWORD, sTempObj >::iterator it = m_mapTempObject.find( pDeleteChar->GetIDX() );
	if( it == m_mapTempObject.end() )
		return;

	PushTempObjectID( pDeleteChar->GetIDX() );
	g_pCharResMng->DeleteChar( pDeleteChar );
	g_pCharResMng->ThreadDeleteChar( pDeleteChar );
	m_mapTempObject.erase(it);
}

void CCharMng::DeleteTempObject( uint nMonsterUID )
{
	CsC_AvObject* pObject = GetTempObject( nMonsterUID );
	while( pObject != NULL )
	{
		DeleteTempObject( pObject );
		pObject = GetTempObject( nMonsterUID );
	}
}

CsC_AvObject* CCharMng::AddTempObject( uint nMonsterUID, NiPoint3 vPos, bool bAutoDel, float fLifeTime )
{	

	UINT nIDX = PopTempObjectID();
	sCREATEINFO pProp;
	pProp.s_dwPropPlag = PR_EFFECT_ENABLE;
	pProp.s_dwOptionPlag = _OP_ALL;

	sTempObj addTempObj;
	CsC_AvObject* pObject = CsC_AvObject::NewInstance();
	pObject->Init( nIDX , nMonsterUID, vPos, 0.0f, &pProp );
	pObject->SetScale( 0.001f );
	addTempObj.m_pObj = pObject;
	addTempObj.m_bAutoDel = bAutoDel;
	addTempObj.m_fLifeTime = fLifeTime;

	m_mapTempObject.insert( std::make_pair( nIDX , addTempObj ) );

	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::NIF_File );
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
	pUint->s_Prop = pProp;
	pUint->s_pLoadedObject = (CsC_Object*)pObject;
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, "Data\\Dummy\\dummy.nif" );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	return pObject;
}
/*
CsC_AvObject* CCharMng::AddTempObject( uint nMonsterUID, NiPoint3 vPos )
{	

	UINT nIDX = PopTempObjectID();
	sCREATEINFO pProp;
	pProp.s_dwPropPlag = PR_EFFECT_ENABLE;
	pProp.s_dwOptionPlag = _OP_ALL;

	CsC_AvObject* pObject = CsC_AvObject::NewInstance();
	pObject->Init( nIDX , nMonsterUID, vPos, 0.0f, &pProp );
	pObject->SetScale( 0.001f );
	m_mapTempObject.SetData( nIDX , pObject );

	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::NIF_File );
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
	pUint->s_Prop = pProp;
	pUint->s_pLoadedObject = (CsC_Object*)pObject;
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, "Data\\Dummy\\dummy.nif" );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	return pObject;
}
*/

CsC_AvObject* CCharMng::GetTempObject( uint nMonsterUID )
{
	std::map< DWORD, sTempObj >::iterator it = m_mapTempObject.begin();
	for( ; it != m_mapTempObject.end(); ++it )
	{
		SAFE_POINTER_CON( it->second.m_pObj );
		if( it->second.m_pObj->GetFTID() == nMonsterUID )
			return it->second.m_pObj;
	}
	return NULL;
}

uint CCharMng::PopTempObjectID()
{
	static uint nUID = 0;
	uint nReturn = 0;
	if( m_stackTempObjectID.empty() == false )
	{
		nReturn = m_stackTempObjectID.top();
		m_stackTempObjectID.pop();
	}
	else
	{
		nReturn = ++nUID;
	}
	return nReturn;
}

void CCharMng::PushTempObjectID( uint nUID )
{
	m_stackTempObjectID.push( nUID );
}

//==============================================================================================================
//
// 싱크
//
//==============================================================================================================

#ifdef SYNC_DATA_LIST

CCharMng::sSYNC_DATA* CCharMng::NewSync()
{
	sSYNC_DATA* pReturn  = NULL;
	if( m_SyncMempool.empty() == false )
	{
		pReturn = m_SyncMempool.top();
		m_SyncMempool.pop();
		pReturn->s_Type = sSYNC_DATA::NONE;
		return pReturn;
	}

	pReturn = NiNew sSYNC_DATA;
	pReturn->s_Type = sSYNC_DATA::NONE;
	return pReturn;
}

void CCharMng::DelSync( sSYNC_DATA* pData )
{	
	m_SyncMempool.push( pData );
}

void CCharMng::_DeleteSync()
{
#ifdef SYNC_DATA_LIST
	std::list< sSYNC_DATA* >::iterator it =  m_listSync_1.begin();
	std::list< sSYNC_DATA* >::iterator itEnd =  m_listSync_1.end();
	for( ; it!=itEnd; ++it )
	{
		NiDelete (*it);
	}
	m_listSync_1.clear();

	it =  m_listSync_2.begin();
	itEnd =  m_listSync_2.end();
	for( ; it!=itEnd; ++it )
	{
		NiDelete (*it);
	}
	m_listSync_2.clear();

	sSYNC_DATA* pData = NULL;
	while( m_SyncMempool.empty() == false )
	{
		pData = m_SyncMempool.top();
		NiDelete pData;
		m_SyncMempool.pop();
	}
#endif
}

void CCharMng::_InitSync()
{
}

void CCharMng::_UpdateSync()
{
	sSYNC_DATA* pData = NULL;
	if( m_listSync_2.empty() == false )
	{
		std::list< sSYNC_DATA* >::iterator it = m_listSync_2.begin();
		pData = *it;

		if( pData->s_NextFrame <= 1 )
		{
			switch( pData->s_Type )
			{
			case sSYNC_DATA::ADD_EMPLOYMENT:
				_Sync_AddEmployment( &pData->s_EmploymentData );
				break;
			case sSYNC_DATA::DEL_EMPLOYMENT:
				g_pCharMng->DeleteEmployment( pData->s_EmploymentData.s_Type.m_nIDX );
				break;
			default:
				assert_cs( false );
			}

			DelSync( pData );
			m_listSync_2.erase( it );
			return;
		}
		--pData->s_NextFrame;
	}	
}

void CCharMng::_Sync_AddEmployment( sSYNC_DATA::sEMPLOYMENT* pData )
{
	NiPoint3 vpos( (float)pData->s_Pos.m_nX, (float)pData->s_Pos.m_nY, 0.0f );
	vpos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vpos );
	CEmployment* pEmployment = g_pCharMng->AddEmployment( pData->s_Type.m_nIDX, pData->s_nItemType, vpos, pData->s_nFloat );

	int nType = cTalkBalloon::MAX_TYPE;
	ContentsStream kTmp;
	kTmp << pData->s_Type.m_nUID << pData->s_szShopName << nType;

	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_BALLOON_TITLE, &kTmp );

//#ifdef CONSIGNMENT_BALLOON_CASH
//	CsC_AvObject* pTarget = g_pMngCollector->GetObject( pData->s_Type.m_nUID );
//	TCHAR* szFileName = nsCsFileTable::g_pBaseMng->IsBallonNewType( pTarget->GetFTID() );
//	if( szFileName != NULL )
//	{
//		g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_6, pData->s_Type.m_nUID, pData->s_szShopName, true,
//			cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
//	}
//	else
//	{
//		g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_4, pData->s_Type.m_nUID, pData->s_szShopName, true,
//			cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
//	}
//#else
//	g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_4, pData->s_Type.m_nUID, pData->s_szShopName, true,
//		cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
//#endif
}

void CCharMng::InsertSync_1( sSYNC_DATA* pData )
{
	assert_cs( pData->s_Type != sSYNC_DATA::NONE );
	pData->s_NextFrame = SYNC_DATA_LIST_FRAME_1;
	m_listSync_1.push_back( pData );
}

void CCharMng::InsertSync_2( sSYNC_DATA* pData, int nNextFrame )
{
	assert_cs( pData->s_Type != sSYNC_DATA::NONE );
	pData->s_NextFrame = nNextFrame;
	m_listSync_2.push_back( pData );
}

#endif

// 테이머 레벨 가져옴
int CCharMng::GetMyTamerLv() const
{
	if( NULL == m_pTamerUser )
		return 0;

	CsC_AvObject::sBASE_STAT * pStat = m_pTamerUser->GetBaseStat();
	if( NULL == pStat )
		return 0;

	return pStat->GetLevel();
}

// 테이머 경험치 가져옴
__int64 CCharMng::GetMyTamerExp() const
{
	if( NULL == m_pTamerUser )
		return 0;

	CsC_AvObject::sBASE_STAT * pStat = m_pTamerUser->GetBaseStat();
	if( NULL == pStat )
		return 0;

	return pStat->GetExp();
}

DWORD CCharMng::GetMyDigimonFTID( int nSlotNum ) const
{
	if( MAX_DIGIMONUSER_COUNT <= nSlotNum )
		return 0;

	CDigimonUser* pDigimon = m_pDigimonUser[ nSlotNum ];
	if( NULL == pDigimon )
		return 0;

	return pDigimon->GetBaseDigimonFTID();
}


bool CCharMng::IsTamer( uint const & nIDX ) const
{ 
	std::map< DWORD, CTamer* >::const_iterator it = m_mapTamer.find( nIDX );
	if( it == m_mapTamer.end() )
		return false;
	return true;
}

bool CCharMng::IsTamerUser( uint const& nIDX ) const
{ 
	SAFE_POINTER_RETVAL( m_pTamerUser, false );
	if( m_pTamerUser->GetIDX() == nIDX )
		return true;
	return false;
}

bool CCharMng::IsTamerFromUIDX( uint const& nUIDX )
{ 
	std::map< DWORD, CTamer* >::const_iterator it = m_mapTamer.begin();
	for( ; it != m_mapTamer.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( it->second->GetUniqID() == nUIDX )
			return true;
	}

	return false;
}

bool CCharMng::IsTamerUserFromUIDX( uint const& nUIDX )
{ 
	if( NULL == m_pTamerUser )
		return false;

	return m_pTamerUser->GetUniqID() == nUIDX ? true : false;
}

bool CCharMng::IsDigimonUserFromUIDX( uint const& nUIDX )
{ 
	if( NULL == m_pDigimonUser[0] )
		return false;

	return m_pDigimonUser[0]->GetUniqID() == nUIDX ? true : false;
}

CTamer* CCharMng::GetTamerFromUIDX(uint const& nUIDX)
{
	std::map< DWORD, CTamer* >::iterator it = m_mapTamer.begin();
	for( int n =0 ; it != m_mapTamer.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );

		if( it->second->GetUniqID() == nUIDX )
			return it->second;
	}
	return NULL;
}

CDigimon* CCharMng::GetDigimonFromUIDX(uint const& nUIDX)
{
	std::map<DWORD, CDigimon* >::iterator it = m_mapDigimon.begin();
	for( int n =0 ; it != m_mapDigimon.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );

		if( it->second->GetUniqID() == nUIDX )
			return it->second;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

void CCharMng::_AddMonsterSE_HighList_TableID(DWORD const& dwTableID)
{
	if( IsSEHighLightMonster_TableID( dwTableID ) )
		return;

	m_listMonsterSE_HighLight.push_back(dwTableID);
}

void CCharMng::_RemoveMonsterSE_HighList_TableID(DWORD const& dwTableID)
{
	if( !IsSEHighLightMonster_TableID( dwTableID ) )
		return;

	std::map< DWORD, CMonster* >::iterator itMon = m_mapMonster.begin();
	for( ; itMon != m_mapMonster.end(); ++itMon )
	{
		SAFE_POINTER_CON( itMon->second );
		if( dwTableID != itMon->second->GetFTID() )
			continue;
		itMon->second->RemoveShaderEffect_Highlight();
	}

	std::list<DWORD>::iterator it = m_listMonsterSE_HighLight.begin();
	for( ; it != m_listMonsterSE_HighLight.end(); ++it )
	{
		if( dwTableID == (*it) )
		{
			m_listMonsterSE_HighLight.erase( it );
			return;
		}
	}
}

void CCharMng::_RemoveAllMonsterSE_HighList()
{
	std::list<DWORD>::const_iterator key = m_listMonsterSE_HighLight.begin();
	for( ; key != m_listMonsterSE_HighLight.end(); ++key )
	{
		std::map<DWORD, CMonster* >::iterator it = m_mapMonster.begin();	
		for( ; it != m_mapMonster.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			if( (*key) != it->second->GetFTID() )
				continue;
			it->second->RemoveShaderEffect_Highlight();
		}
	}
}

void CCharMng::SetMonsterSE_Highlight_TableID( DWORD const& dwTableID, bool bShow )
{
	if( bShow )
		_AddMonsterSE_HighList_TableID( dwTableID );
	else
		_RemoveMonsterSE_HighList_TableID( dwTableID );

	std::map<DWORD, CMonster* >::iterator it = m_mapMonster.begin();
	for( ; it != m_mapMonster.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( dwTableID != it->second->GetFTID() )
			continue;
		if( bShow )
			it->second->SetShaderEffect_Highlight();
		else
			it->second->RemoveShaderEffect_Highlight();
	}
}

bool CCharMng::IsSEHighLightMonster_TableID( DWORD const& dwTableID )
{
	std::list<DWORD>::const_iterator it = m_listMonsterSE_HighLight.begin();
	for( ; it != m_listMonsterSE_HighLight.end(); ++it )
	{
		if( dwTableID == (*it) )
			return true;
	}

	return false;
}

CMonster* CCharMng::FindNearMonster( int iMonsterID)
{
	CsC_AvObject* pFindObj = _FindObject_FromDist( GetTamerUser(), NULL, RTTI_MONSTER );
	SAFE_POINTER_RETVAL( pFindObj, NULL );

	if( pFindObj->GetFTID() == iMonsterID )
		return static_cast<CMonster*>(pFindObj);
	return NULL;
} 


CMonster* CCharMng::GetMonster( uint const& nIDX )
{ 
	std::map<DWORD,CMonster*>::iterator it = m_mapMonster.find( nIDX );
	if( it == m_mapMonster.end() )
		return NULL;
	return it->second;
}