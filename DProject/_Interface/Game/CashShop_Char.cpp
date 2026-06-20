

#include "stdafx.h"
#include "CashShop__Etc.h"

#ifdef FT_USE_CASHSHOP

cCashShop_Char::cCashShop_Char()
{
	m_pTamer		= NULL;
	m_pRenderTex	= NULL;

//디지바이스는 장비로 들어가지 않으므로 따로 처리함으므로 +1 해줌
	for( int i=0; i<nLimit::Equip + 1; ++i )
		m_nEquip[ i ] = 0;
}

void cCashShop_Char::Delete()
{
	// 테이머 제거
	if( m_pTamer )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = (CsC_Object*)m_pTamer;
		g_pThread->DeleteChar( pUnit );
		m_pTamer = NULL;
	}	

	// 랜더 텍스 제거
	SAFE_NIDELETE( m_pRenderTex );
	SAFE_NIDELETE( m_pRotLeft );
	SAFE_NIDELETE( m_pRotRight );
}

void cCashShop_Char::Init( CsPoint ptPos )
{
	// 미리 보기용 테이머 생성
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	m_pTamer = CTamer::NewInstance();	

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
	prop.s_dwOptionPlag = _OP_ALL;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
	m_pTamer->Init( 0, pUser->GetFTID(), NiPoint3::ZERO, 0, _T( "" ), &prop, 0 );
	m_pTamer->GetProp_Alpha()->SetUseDistAlpha( false );

	cItemInfo* pItemInfo = NULL;

	CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT + 1];
	for( int i=0; i<nsPART::MAX_TOTAL_COUNT + 1; ++i )
	{
		if( i == nsPART::MAX_TOTAL_COUNT)
			pItemInfo = g_pDataMng->GetTEquip()->GetDigiviceItem();
		else
			pItemInfo = g_pDataMng->GetTEquip()->GetData( i );


		cp[ i ].s_nFileTableID = pItemInfo->GetType();
		cp[ i ].s_nPartIndex = i;
		cp[ i ].s_nRemainTime = pItemInfo->m_nEndTime;
	}

	m_pTamer->ThreadLoad_Part( cp );

	m_pRenderTex = NiNew cRenderTex;
	
	if( m_pRenderTex->Init( CsPoint( 300, 450 ), "Cash\\Shop\\CharBG.tga" ) == false )
	{
		SAFE_NIDELETE( m_pRenderTex );
	}

	m_fRot = 0.0f;

	//2015-11-26-nova
	GET_SUBCAMERA(CAMERA_04)->SetRotation( -m_pTamer->GetDir() );
	GET_SUBCAMERA(CAMERA_04)->_UpdateCamera();

	m_pRotLeft = NiNew cButton;
	m_pRotLeft->Init( g_pGameIF->GetCashShop()->GetRoot(), CsPoint( 30, 437 ) + ptPos, CsPoint( 77, 50 ), "Cash\\Shop\\Store_Chbu_Le.tga", false );
	m_pRotLeft->SetTexToken( CsPoint( 0, 47 ) );

	m_pRotRight = NiNew cButton;
	m_pRotRight->Init( g_pGameIF->GetCashShop()->GetRoot(), CsPoint( 219, 437 ) + ptPos, CsPoint( 77, 50 ), "Cash\\Shop\\Store_Chbu_Ri.tga", false );
	m_pRotRight->SetTexToken( CsPoint( 0, 47 ) );


	CsSIZE size( 32, 32 );	
	m_IFIcon_Equip[ nTamer::Head ].Init( CsRect( CsPoint( 29, 110 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Coat ].Init( CsRect( CsPoint( 29, 166 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Pants ].Init( CsRect( CsPoint( 29, 223 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Glove ].Init( CsRect( CsPoint( 29, 279 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Shoes ].Init( CsRect( CsPoint( 29, 337 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Costume ].Init( CsRect( CsPoint( 29, 393 ) + ptPos, size ) );
#ifdef ACCESSORY
	m_IFIcon_Equip[ nTamer::Glass ].Init( CsRect( CsPoint( 258, 279 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Necklace ].Init( CsRect( CsPoint( 258, 166 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Ring ].Init( CsRect( CsPoint( 258, 110 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Earring ].Init( CsRect( CsPoint( 258, 223 ) + ptPos, size ) );	
	m_IFIcon_Equip[ nTamer::EquipAura ].Init( CsRect( CsPoint( 258, 337 ) + ptPos, size ) );
//디지바이스
	m_IFIcon_Equip[ DIGIVICE ].Init( CsRect( CsPoint( 258, 393 ) + ptPos, size ) );
#else
	//m_IFIcon_Equip[ nTamer::Ring ].Init( CsRect( CsPoint( 258, 45 ) + ptPos, size ) );
	m_IFIcon_Equip[ nTamer::Glass ].Init( CsRect( CsPoint( 258, 279 ) + ptPos, size ) );
	//m_IFIcon_Equip[ nTamer::Earring ].Init( CsRect( CsPoint( 258, 129 ) + ptPos, size ) );	
	//m_IFIcon_Equip[ nTamer::Glass ].Init( CsRect( CsPoint( 258, 172 ) + ptPos, size ) );
#endif

	SetAvatarTamer();

}

void cCashShop_Char::Update(float const& fDeltaTime)
{
	if( m_pRenderTex == NULL )
		return;

	m_pTamer->Update( fDeltaTime );
	if( m_pTamer->IsLoad() && ( m_pTamer->GetViewSight() == false ) )
	{
		float fAniTime = m_pTamer->GetAniTime();
		m_pTamer->GetProp_Animation()->GetActorMng()->Update( fAniTime );
		m_pTamer->GetCsNode()->m_pNiNode->UpdateSelected( fAniTime );
		m_pTamer->UpdateVisible( fAniTime );
	}	
}

void cCashShop_Char::Updatae_Mouse()
{
	if( CURSOR_ST.IsIcon() == true )
		return;

	switch( m_pRotLeft->Update_ForMouse() )	
	{
	case cButton::ACTION_PRESS:
		m_fRot -= 0.2f;	
		GET_SUBCAMERA(CAMERA_04)->SetTranslate(m_pTamer->GetPos());	//2015-11-26-nova
		GET_SUBCAMERA(CAMERA_04)->SetRotation(m_fRot, 0.0f);
		GET_SUBCAMERA(CAMERA_04)->_UpdateCamera();
		g_pRenderer->SetCameraData( GET_SUBCAMERA(CAMERA_04)->GetCameraObj() );	
		break;
	}

	switch( m_pRotRight->Update_ForMouse() )	
	{
	case cButton::ACTION_PRESS:
		m_fRot += 0.2f;		
		GET_SUBCAMERA(CAMERA_04)->SetTranslate(m_pTamer->GetPos());
		GET_SUBCAMERA(CAMERA_04)->SetRotation(m_fRot, 0.0f);
		GET_SUBCAMERA(CAMERA_04)->_UpdateCamera();
		g_pRenderer->SetCameraData( GET_SUBCAMERA(CAMERA_04)->GetCameraObj() );
		break;
	}	
	
	CsPoint ptMouseLocalPos = CURSOR_ST.GetPos() - g_pGameIF->GetCashShop()->GetRootClient();
	m_nMask = -1;
	for( int i=0; i<nLimit::Equip + 1; ++i )
	{
		if( m_nEquip[ i ] == 0 )
			continue;

		CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();
		switch( eCursor )
		{
		case CURSOR::RBUTTON_UP:
			{
				if( m_IFIcon_Equip[ i ].MouseRBtnClick( ptMouseLocalPos ) == true )
				{
					SetAvatarItem( 0, i );
				}
			}
			break;
		case CURSOR::BUTTON_NONE :
			{
				if( m_IFIcon_Equip[ i ].MouseOn( ptMouseLocalPos ) == true )
				{
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_IFIcon_Equip[ i ].GetPos() + g_pGameIF->GetCashShop()->GetRootClient(), CsPoint( 32, 32 ), 
						TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_nEquip[ i ], cBaseWindow::WT_STORE );			
					m_nMask = i;
				}
				else
				{
					m_IFIcon_Equip[ i ].ReleaseState();
				}
			}
			break;
		}	
	}
	
}

void cCashShop_Char::Render( CsPoint ptParentPos )
{
	if( m_pRenderTex == NULL )
		return;

	m_pRenderTex->BeginRender();
	m_pRenderTex->RenderObjectIncludeEffect( GET_SUBCAMERA(CAMERA_04), m_pTamer, 350.0f );	//2015-11-26-nova m_Camera -> 전역카메라의 서브카메라 사용
	m_pRenderTex->EndRender( GET_SUBCAMERA(CAMERA_04), CsPoint( 0, 20 ) + ptParentPos );

	m_pRotLeft->Render();
	m_pRotRight->Render();

	CsPoint pos;
	for( int i=0; i<nLimit::Equip + 1; ++i )
	{
		if( m_nEquip[ i ] == 0 )
			continue;		

		// 위치
		pos = m_IFIcon_Equip[ i ].GetPos() + g_pGameIF->GetCashShop()->GetRootClient();		
		// 아이템 랜더링
		g_pIconMng->RenderItem( m_nEquip[ i ], pos );

		if( i == m_nMask )			
			g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, CsPoint( 32, 32 ) );			
	}
}

void cCashShop_Char::ResetDevice()
{
	if( m_pRenderTex == NULL )
		return;
	
	m_pRenderTex->ResetDevice();
	m_pRotLeft->ResetDevice();
	m_pRotRight->ResetDevice();
}

void cCashShop_Char::SetAvatarItem( int nItemType, int nPartIdx )
{
	if(nPartIdx >= nTamer::Head && nPartIdx <= DIGIVICE +1)
	{
		m_nEquip[ nPartIdx ] = nItemType;
		CsC_PartObject::sCHANGE_PART_INFO cp;


		cp.s_nFileTableID = nItemType;
		cp.s_nPartIndex = nPartIdx;
		cp.s_nRemainTime = _TIME_TS + 300; 

		if(nPartIdx != DIGIVICE)
			m_pTamer->ChangePart( &cp );
	}
}

void cCashShop_Char::SetAvatarTamer()
{
	m_nEquip[ nTamer::Head ] = g_pDataMng->GetTEquip()->GetData( nTamer::Head )->GetType();
	m_nEquip[ nTamer::Coat ] = g_pDataMng->GetTEquip()->GetData( nTamer::Coat )->GetType();
	m_nEquip[ nTamer::Pants ] = g_pDataMng->GetTEquip()->GetData( nTamer::Pants )->GetType();
	m_nEquip[ nTamer::Glove ] = g_pDataMng->GetTEquip()->GetData( nTamer::Glove )->GetType();
	m_nEquip[ nTamer::Shoes ] = g_pDataMng->GetTEquip()->GetData( nTamer::Shoes )->GetType();
	m_nEquip[ nTamer::Costume ] = g_pDataMng->GetTEquip()->GetData( nTamer::Costume )->GetType();
	m_nEquip[ nTamer::Glass ] = g_pDataMng->GetTEquip()->GetData( nTamer::Glass )->GetType();

#ifdef ACCESSORY
	m_nEquip[ nTamer::Necklace] = g_pDataMng->GetTEquip()->GetData( nTamer::Necklace )->GetType();
	m_nEquip[ nTamer::Ring] = g_pDataMng->GetTEquip()->GetData( nTamer::Ring )->GetType();
	m_nEquip[ nTamer::Earring] = g_pDataMng->GetTEquip()->GetData( nTamer::Earring )->GetType();
	m_nEquip[ nTamer::EquipAura] = g_pDataMng->GetTEquip()->GetData( nTamer::EquipAura )->GetType();
	m_nEquip[ DIGIVICE ] =	g_pDataMng->GetTEquip()->GetDigiviceItem()->GetType();
#endif

	for( int i=0; i<nsPART::MAX_TOTAL_COUNT + 1; ++i )
	{	
		if(i == nsPART::MAX_TOTAL_COUNT)
			SetAvatarItem( g_pDataMng->GetTEquip()->GetDigiviceItem()->GetType(), i );
		else
			SetAvatarItem( g_pDataMng->GetTEquip()->GetData( i )->GetType(), i );
	}
}

void cCashShop_Char::SetAvatarDel()
{	
	for( int i=0; i<nsPART::MAX_TOTAL_COUNT + 1; ++i )
	{		
		SetAvatarItem( 0, i );
	}	
}


#endif