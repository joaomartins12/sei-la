
#include "stdafx.h"
#include "ItemMng.h"

CItemMng*	g_pItemMng = NULL;


CItemMng::CItemMng()
{
}

CItemMng::~CItemMng()
{
}

void CItemMng::Delete()
{
	std::map<unsigned int, CItem*>::iterator it = m_mapItem.begin();
	for( ; it != m_mapItem.end(); ++it )
	{
		g_pCharResMng->DeleteChar( it->second );
		g_pCharResMng->ThreadDeleteChar( it->second );
	}

	m_mapItem.clear();
}

void CItemMng::Init()
{
	Delete();
}

void CItemMng::Reset()
{
	Delete();
}

void CItemMng::ResetShader()
{
	std::map<unsigned int, CItem*>::iterator it = m_mapItem.begin();
	for( ; it != m_mapItem.end(); ++it )
	{
		CsC_AvObject* pAvObject = it->second;
		if( pAvObject->IsLoad() == false )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		if( pNode == NULL )
			continue;

		pNode->ActiveShader();
	}
}

void CItemMng::AddItem( uint const& nIDX, uint const& nFileTableID, nSync::Pos const& sPos, DWORD const& nOwnerUID, DWORD const& plag /*=0*/ )
{
	if( IsItem( nIDX ) )
		return;

	CsItem* pFT = nsCsFileTable::g_pItemMng->GetItem( nFileTableID );
	if( NULL == pFT )
	{
		assert_cs( pFT != NULL );
		return;
	}	

	CItem* pItem = CItem::NewInstance();	

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );
	pUnit->s_Prop.s_dwPropPlag = PR_EFFECT_ENABLE | PR_ANI_ENABLE;
	pUnit->s_Prop.s_dwOptionPlag = OP_FOG | OP_LIGHT | OP_ABVWIRE;
	pUnit->s_Prop.s_eInsertGeomType = CGeometry::Normal;
	pUnit->s_pLoadedObject = pItem;

	if( ( plag & PLAG_BARCODE_ITEM ) == 0 )
	{
		// 모델 로드
		if( pFT->GetInfo()->s_cModel_Nif[ 0 ] != NULL )
			sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Item\\%s.nif", pFT->GetInfo()->s_cModel_Nif );
		else
			sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Item\\Item_Box.nif" );

		// 쉐이더 여부
		if( pFT->GetInfo()->s_bModel_Shader )
			pUnit->s_Prop.s_eInsertGeomType = CGeometry::GetMeshOutLineType();

		// 루핑 여부
		if( pFT->GetInfo()->s_bModel_Loop )
			pUnit->s_Prop.s_dwPropPlag |= PR_ANI_LOOP;

		// 데이터 셋팅 다 끝내고 쓰레드에 집어 넣자 꼭~!!	
		pItem->Init( nIDX, nFileTableID, sPos, &pUnit->s_Prop, nOwnerUID );

		// 이펙트 붙이자
		if( pFT->GetInfo()->s_cModel_Effect[ 0 ] != NULL )
		{
			char cEffectPath[ MAX_PATH ] = {0, };
			sprintf_s( cEffectPath, MAX_PATH, "System\\Item\\%s.nif", pFT->GetInfo()->s_cModel_Effect );
			pItem->GetProp_Effect()->AddEffect_FT( cEffectPath );
		}
	}
	else
	{
		sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "Data\\Item\\blue_card.nif" );
		pUnit->s_Prop.s_dwPropPlag |= PR_ANI_LOOP;

		// 데이터 셋팅 다 끝내고 쓰레드에 집어 넣자 꼭~!!	
		pItem->Init( nIDX, nFileTableID, sPos, &pUnit->s_Prop, nOwnerUID );

		// 이펙트 붙이자
		pItem->GetProp_Effect()->AddEffect_FT( "System\\Item\\Blue_Card.nif" );
	}

	m_mapItem.insert( std::make_pair( nIDX, pItem ) );
	g_pCharResMng->LoadChar( pUnit );
}

bool CItemMng::IsItem( uint const& nIDX ) const
{ 
	std::map<unsigned int, CItem*>::const_iterator it = m_mapItem.find( nIDX );
	if( it == m_mapItem.end() )
		return false;
	return true;
}

void CItemMng::DeleteItem( uint const& nIDX )
{
	std::map<unsigned int, CItem*>::iterator it	= m_mapItem.find( nIDX );
	if( it == m_mapItem.end() )
		return;

	g_pCharResMng->DeleteChar( it->second );
	g_pCharResMng->ThreadDeleteChar( it->second );

	m_mapItem.erase( it );
}

CItem* CItemMng::GetItem( uint const& nIDX )
{
	std::map<unsigned int, CItem*>::iterator it	= m_mapItem.find( nIDX );
	if( it == m_mapItem.end() )
		return NULL;

	return it->second;
}

void CItemMng::Update()
{
	std::map<unsigned int, CItem*>::iterator it	= m_mapItem.begin();
	for( ; it != m_mapItem.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Update( g_fDeltaTime );
	}
}

void CItemMng::Render()
{
	std::map<unsigned int, CItem*>::iterator it	= m_mapItem.begin();
	for( ; it != m_mapItem.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render();
	}
}


CItem* CItemMng::PickupItem( DWORD const& nTamerUID, NiPoint2 const& vTamerPos, float const& fRadius )
{
	CItem* pReturn = NULL;
	float fMinDist = FLT_MAX;
	float fCurDist;
	std::map<unsigned int, CItem*>::iterator it	= m_mapItem.begin();
	for( ; it != m_mapItem.end(); ++it )
	{
		CItem* pItem = it->second;

		// ID 체크
		if( pItem->GetOwnerUID() != 0 )
		{
			// 파티일때
			if( cType::GetClass( (u2)pItem->GetOwnerUID() ) == nClass::None )
			{
				UINT uiPartyIdx = 0;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_PARTY_UIDX, &uiPartyIdx);
				if( uiPartyIdx != pItem->GetOwnerUID() )
					continue;
			}
			else if( pItem->GetOwnerUID() != nTamerUID )
				continue;
		}		

		// 거리 체크
		fCurDist = ( pItem->GetPos2D() - vTamerPos ).Length();
		if( fCurDist > fRadius )
			continue;

		// 가장 가까운거리
		if( fCurDist > fMinDist )
			continue;

		fMinDist = fCurDist;
		pReturn = pItem;
	}
	return pReturn;
}


//====================================================================================
//
//		Picking
//
//====================================================================================
CsC_AvObject* CItemMng::PickItem( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength )
{
	CsC_AvObject* pReturn = NULL;
	std::map<unsigned int, CItem*>::iterator it	= m_mapItem.begin();
	for( ; it != m_mapItem.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( it->second->PickObject( pCsGBPick, vOrigin, vDir, fLength ) == true )
		{
			type.m_nIDX = it->second->GetIDX();
			type.m_nClass = nClass::Item;
			pReturn = it->second;
		}
	}
	return pReturn;
}