
#include "stdafx.h"
#include "Item.h"


THREAD_MEMPOOL_CPP( CItem )
#define PARENT_CLASS	CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CItem, RTTI_ITEM )


void CItem::Init( uint nUID, uint nFileTableID, nSync::Pos sPos, sCREATEINFO* pProp, DWORD nOwnerUID )
{
	if( nsCsGBTerrain::g_pCurRoot == NULL )
	{
		g_CriticalLog.Log( _T( "현재 지형이 NULL 임에도 Item 생성 정보가 들어옴" ) );
		return;
	}	

	// 위치값
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );

	if( nsCsGBTerrain::g_pCurRoot->IsInTerrain( vPos.x, vPos.y ) == false )
	{
		g_CriticalLog.Log( _T( "지형의 크기를 벋어 났다. 지형 ID = %d\n, x = %d\n y =%d" ), nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID, sPos.m_nX, sPos.m_nY );
		return;
	}

	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	m_nOwnerUID = nOwnerUID;

	PARENT_CLASS::Init( nUID, nFileTableID, vPos, 0.0f, pProp );
}

void CItem::Delete()
{
	PARENT_CLASS::Delete();	
}

void CItem::Update( float fDeltaTime )
{
	PARENT_CLASS::Update( fDeltaTime );
}

DWORD CItem::GetOwnerUID()
{ 
	return m_nOwnerUID;
}
void* CItem::PlaySound( char* cSoundPath )
{ 
	return g_pSoundMgr->PlaySystemSound( cSoundPath, GetPos(), this );
}
UINT CItem::GetClass()
{ 
	return nClass::Item;
}
UINT CItem::GetUniqID()
{ 
	cType type;
	type.m_nClass = GetClass();
	type.m_nIDX = GetIDX(); 
	return type.m_nUID;
}
UINT CItem::GetModelID() const
{ 
	return CsC_Object::INAVLIDE_FT_ID;
}
NiColor	CItem::GetNameColor()
{ 
	return NiColor( 20.0f/255.0f, 20.0f/255.0f, 233.0f/255.0f ); 
}

#undef PARENT_CLASS
