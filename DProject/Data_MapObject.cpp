#include "stdafx.h"

#include "Data_MapObject.h"

void cData_MapObject::Init( bool bOpen/* = false*/ )
{
	SAFE_POINTER_RET( nsCsGBTerrain::g_pCurRoot );
	CsGBTerrainRoot::sINFO* pMapInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	SAFE_POINTER_RET( pMapInfo );

	if( !nsCsMapTable::g_pActObjectMng->IsGroup( pMapInfo->s_dwMapID ) )
		return;

	CsMapObjectGroup const* pFTGroup = nsCsMapTable::g_pActObjectMng->GetGroup( pMapInfo->s_dwMapID );
	SAFE_POINTER_RET( pFTGroup );

	std::list<DWORD> dwActorObjId;
	pFTGroup->GetActorObjectID( dwActorObjId );

	if( dwActorObjId.empty() )
		return;

	std::list<DWORD>::iterator it = dwActorObjId.begin();
	for( ; it != dwActorObjId.end(); ++it )
		OpenAndCloseObject( (*it), bOpen );
}

void cData_MapObject::OpenAndCloseObject( DWORD const& dwObjectId, bool bValue )
{
	SAFE_POINTER_RET( nsCsGBTerrain::g_pCurRoot );

	CsGBObject* pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( dwObjectId );
	SAFE_POINTER_RET(pCsGBObject);

	CsGBObjProp_Actor* pProp = (CsGBObjProp_Actor*)pCsGBObject->GetProp( CsGBObjProp::PROP_ACTOR );
	SAFE_POINTER_RET(pProp);

	if( _stricmp( pProp->GetKey(), "door" ) != 0 )
		return;

	DWORD dwCurAni = pProp->GetAnimation();
	switch( dwCurAni )
	{
	case CsGBObjProp_Actor::ACT_ANIMATION1:	// 애니 1 ( Door : 닫힘 Idle )
		{
			if( bValue ) // 열려라
				pProp->SetAnimation( CsGBObjProp_Actor::ACT_ANIMATION3 );	// 열림 애니
		}break;
	case CsGBObjProp_Actor::ACT_ANIMATION2:	// 애니 2 ( Door : 열림 )
	case CsGBObjProp_Actor::ACT_ANIMATION3: // 애니 3 ( Door : 열림 Idle )
		{
			if( !bValue )//닫아라
				pProp->SetAnimation( CsGBObjProp_Actor::ACT_ANIMATION1 );	// 닫기 애니
		}break;
	}
}

void cData_MapObject::SetMapObjectAni( DWORD const& dwObjectId, DWORD const& dwAniID )
{
	SAFE_POINTER_RET( nsCsGBTerrain::g_pCurRoot );

	CsGBObject* pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( dwObjectId );
	SAFE_POINTER_RET(pCsGBObject);

	CsGBObjProp_Actor* pProp = (CsGBObjProp_Actor*)pCsGBObject->GetProp( CsGBObjProp::PROP_ACTOR );
	SAFE_POINTER_RET(pProp);

	pProp->SetAnimation( dwAniID );
}
