
#include "stdafx.h"
#include "CsGBLightMng.h"

CsGBLightMng::CsGBLightMng()
{
	m_pDynamicShadowLight = NULL;
	m_fLight = 1.0f;
	memset( m_cTrShaderExtra, 0, sizeof(char)* 16 );
}

void CsGBLightMng::Delete()
{
	std::list< CsGBLight* >::iterator it = m_listLight.begin();
	for( ; it != m_listLight.end(); ++it )
		NiDelete ( *it );

	m_listLight.clear();

	m_listLightObject.clear();
	m_listLightChar.clear();

	m_fLight = 1.0f;
	m_Info.s_nLightCount = (int)m_listLight.size();
	m_pDynamicShadowLight = NULL;
}

void CsGBLightMng::Init()
{
	assert_cs( m_listLight.empty() == true );
	m_Info.Reset();

	m_fLight = 1.0f;
	m_nCharDirection = 0;
	m_nCharPoint = 0;
	m_nTrDirection = 0;
	m_nTrPoint = 0;
	memset( m_cTrShaderExtra, 0, sizeof(char)* 16 );
}

void CsGBLightMng::DeleteLight( CsGBLight* pLight )
{
	RemoveObject( pLight );
	RemoveChar( pLight );

	std::list< CsGBLight* >::iterator it = m_listLight.begin();
	for( ; it != m_listLight.end(); ++it )
	{
		if( pLight != *it )
			continue;

		NiDelete ( *it );
		m_listLight.erase( it );
		break;
	}

	m_Info.s_nLightCount = (int)m_listLight.size();
}

CsGBLight_Dir* CsGBLightMng::AddDirectionLight( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Dir::sINFO* dirInfo, bool bLoad )
{
	if( baseInfo->s_bApplyChar )
	{
		if( m_nCharDirection >= 2 )
			return NULL;

		++m_nCharDirection;
	}

	if( baseInfo->s_bApplyObject )
		++m_nTrDirection;

	CsGBLight_Dir* pLight = NiNew CsGBLight_Dir;
	pLight->Create( baseInfo, dirInfo );
	
	_InsertList( pLight, baseInfo, bLoad );	

	if( !bLoad )
		ShaderApply_Char();

	return pLight;
}

CsGBLight_Point* CsGBLightMng::AddPointLight( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Point::sINFO* pointInfo, bool bLoad )
{
	if( baseInfo->s_bApplyChar )
	{
		assert_cs( false );
		++m_nCharPoint;
	}

	if( baseInfo->s_bApplyObject )
		m_nTrPoint = 1;

	CsGBLight_Point* pLight = NiNew CsGBLight_Point;
	pLight->Create( baseInfo, pointInfo );
	
	_InsertList( pLight, baseInfo, bLoad );
	

	if( !bLoad )
		ShaderApply_Char();

	return pLight;
}

void CsGBLightMng::_InsertList( CsGBLight* pLight, CsGBLight::sBASE_INFO* baseInfo, bool bLoad )
{
	m_listLight.push_back( pLight );

	if( bLoad == false )
		m_Info.s_nLightCount = (int)m_listLight.size();	

	if( !baseInfo->s_bEnableLight )
		return;

	if( baseInfo->s_bApplyObject == true )
		m_listLightObject.push_back( pLight );
	if( baseInfo->s_bApplyChar == true )
		m_listLightChar.push_back( pLight );
	if( baseInfo->s_bApplyDynamicShadow == true )
	{
		assert_cs( m_pDynamicShadowLight == NULL );
		m_pDynamicShadowLight = pLight;
	}
}

void CsGBLightMng::ApplyObject( NiNode* pNiNode )
{
// 	CsGBLight_Dir* pDir;
// 	CsGBLight_Point* pPoint;
	std::list< CsGBLight* >::iterator it = m_listLightObject.begin();
	for( ; it != m_listLightObject.end(); ++it )
	{
		switch( (*it)->GetType() )
		{
		case CsGBLight::DIRECTION:
		case CsGBLight::POINT:
			break;			
		default:
			{
				assert_cs( false );
				continue;
			}
			break;
		}

		pNiNode->AttachEffect( ( *it )->GetNiLight() );
	}
}

void CsGBLightMng::ApplyChar( NiNode* pNiNode )
{
	SAFE_POINTER_RET( pNiNode );

	std::list< CsGBLight* >::iterator it = m_listLightChar.begin();
	for( ; it != m_listLightChar.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		CsGBLight::sBASE_INFO* pInfo = (*it)->GetBaseInfo();
		SAFE_POINTER_CON( pInfo );
		assert_cs( pInfo->s_bEnableLight );
		pNiNode->AttachEffect( ( *it )->GetNiLight() );
	}
}

void CsGBLightMng::DetachChar( NiNode* pNiNode )
{
	SAFE_POINTER_RET( pNiNode );
	std::list< CsGBLight* >::iterator it = m_listLightChar.begin();
	for( ; it != m_listLightChar.end(); ++it )
	{
		assert_cs( (*it)->GetBaseInfo()->s_bEnableLight );
		pNiNode->DetachEffect( ( *it )->GetNiLight() );
	}	
}

CsGBLight* CsGBLightMng::GetLight( int nIndex )
{
	std::list< CsGBLight* >::iterator it = m_listLight.begin();
	std::list< CsGBLight* >::iterator itEnd = m_listLight.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return *it;
		--nIndex;
	}
	assert_cs( false );
	return NULL;
}

void CsGBLightMng::AddObject( CsGBLight* pLight )
{
	RemoveObject( pLight );
	m_listLightObject.push_back( pLight );
	nsCsGBTerrain::g_pCurRoot->ApplyLight_Object( pLight );

	if( pLight->GetType() == CsGBLight::DIRECTION )
	{
		++m_nTrDirection;
	}
	else
	{
		assert_cs( pLight->GetType() == CsGBLight::POINT );
		m_nTrPoint = 1;
	}
	MakeTrShaderExtra();
}

void CsGBLightMng::RemoveObject( CsGBLight* pLight )
{
	std::list< CsGBLight* >::iterator it = m_listLightObject.begin();
	std::list< CsGBLight* >::iterator itEnd = m_listLightObject.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == pLight )
		{
			if( pLight->GetType() == CsGBLight::DIRECTION )
			{
				--m_nTrDirection;
			}
			else
			{
				assert_cs( pLight->GetType() == CsGBLight::POINT );
				--m_nTrPoint;
			}
			MakeTrShaderExtra();

			m_listLightObject.erase( it );
			nsCsGBTerrain::g_pCurRoot->ReleaseLight_Object( pLight );
			return;
		}
	}
}

void CsGBLightMng::AddChar( CsGBLight* pLight )
{
	RemoveChar( pLight );
	m_listLightChar.push_back( pLight );

	if( pLight->GetType() == CsGBLight::DIRECTION )
	{
		++m_nCharDirection;
	}
	else
	{
		assert_cs( pLight->GetType() == CsGBLight::POINT );
		++m_nCharPoint;
	}
	ShaderApply_Char();
}

void CsGBLightMng::RemoveChar( CsGBLight* pLight )
{
	std::list< CsGBLight* >::iterator it = m_listLightChar.begin();
	std::list< CsGBLight* >::iterator itEnd = m_listLightChar.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == pLight )
		{
			if( pLight->GetType() == CsGBLight::DIRECTION )
			{
				--m_nCharDirection;
			}
			else
			{
				assert_cs( pLight->GetType() == CsGBLight::POINT );
				--m_nCharPoint;
			}
			ShaderApply_Char();

			m_listLightChar.erase( it );
			return;
		}
	}
}

//===============================================================================================
//
//		Extra Data
//
//===============================================================================================

void CsGBLightMng::SaveExtraData( CsNiNodePtr pNiNode )
{
	// ==== 인포 저장	
	assert_cs( sizeof( char ) == 1 );

	// 케릭터 창만 체크된거는 빼준다.
	int nLightCount = 0;
	std::list< CsGBLight* >::iterator it = m_listLight.begin();
	std::list< CsGBLight* >::iterator itEnd = m_listLight.end();
	for( ; it!=itEnd; ++it )
	{
		if( IsOnlyCharLight( *it ) == false )
		{
			++nLightCount;
		}
	}
	
	// 저장할 사이즈 구한다
	UINT uiSize = sizeof( CsGBLightMng::sINFO ) + sizeof( UINT )*nLightCount;	
	it = m_listLight.begin();
	for( ; it!=itEnd; ++it )
	{
		if( IsOnlyCharLight( *it ) )
		{
			continue;
		}

		uiSize += sizeof( CsGBLight::sBASE_INFO );
		switch( (*it)->GetType() )
		{
		case CsGBLight::DIRECTION:
			uiSize += sizeof( CsGBLight_Dir::sINFO );
			break;
		case CsGBLight::POINT:
			uiSize += sizeof( CsGBLight_Point::sINFO );
			break;
		default:
			assert_cs( false );
		}
	}

	// 메모리 할당에서 저장할 데이터 기입
	char* pData = NiAlloc( char, uiSize );
	UINT nOffset = 0;
	int nBackupLightCount = m_Info.s_nLightCount;
	m_Info.s_nLightCount = nLightCount;
	memcpy( &pData[ nOffset ], &m_Info, sizeof( CsGBLightMng::sINFO ) );
	nOffset += sizeof( CsGBLightMng::sINFO );
	it = m_listLight.begin();
	for( ; it!=itEnd; ++it )
	{
		if( IsOnlyCharLight( *it ) )
		{
			continue;
		}

		memcpy( &pData[ nOffset ], (*it)->GetBaseInfo(), sizeof( CsGBLight::sBASE_INFO ) );
		nOffset += sizeof( CsGBLight::sBASE_INFO );
		switch( (*it)->GetType() )
		{
		case CsGBLight::DIRECTION:
			{
				CsGBLight_Dir* pLight = (CsGBLight_Dir*)(*it);
				memcpy( &pData[ nOffset ], pLight->GetInfo(), sizeof( CsGBLight_Dir::sINFO ) );
				nOffset += sizeof( CsGBLight_Dir::sINFO );
			}
			break;
		case CsGBLight::POINT:
			{
				CsGBLight_Point* pLight = (CsGBLight_Point*)(*it);
				memcpy( &pData[ nOffset ], pLight->GetInfo(), sizeof( CsGBLight_Point::sINFO ) );
				nOffset += sizeof( CsGBLight_Point::sINFO );
			}
			break;
		default:
			assert_cs( false );
		}
	}

	// Extra Data 기입
	NiBinaryExtraData* pExtData = NiNew NiBinaryExtraData( uiSize, pData );
	pNiNode->AddExtraData( CsGBLight_ExtraKey, pExtData );

	m_Info.s_nLightCount = nBackupLightCount;
}


void CsGBLightMng::LoadExtraData( CsNiNodePtr pNiNode )
{
	NiBinaryExtraData* pExtData = NULL;
	unsigned int uiSize = 0;
	char* pData = NULL;

	pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBLight_ExtraKey );
	assert_cs( pExtData != NULL );
	pExtData->GetValue( uiSize, pData );

	UINT nOffset = 0;
	// 헤더 정보
	memcpy( &m_Info, &pData[ nOffset ], sizeof( sINFO ) );
	nOffset += sizeof( sINFO );	

	CsGBLight::sBASE_INFO baseInfo;
	CsGBLight_Dir::sINFO dirInfo;
	CsGBLight_Point::sINFO pointInfo;
	for( int i=0; i<m_Info.s_nLightCount; ++i )
	{
		memcpy( &baseInfo, &pData[ nOffset ], sizeof( CsGBLight::sBASE_INFO ) );
		nOffset += sizeof( CsGBLight::sBASE_INFO );
		switch( baseInfo.s_eType )
		{
		case CsGBLight::DIRECTION:
			{
				memcpy( &dirInfo, &pData[ nOffset ], sizeof( CsGBLight_Dir::sINFO ) );
				nOffset += sizeof( CsGBLight_Dir::sINFO );
				AddDirectionLight( &baseInfo, &dirInfo, true );
			}			
			break;
		case CsGBLight::POINT:
			{
				memcpy( &pointInfo, &pData[ nOffset ], sizeof( CsGBLight_Point::sINFO ) );
				nOffset += sizeof( CsGBLight_Point::sINFO );
				AddPointLight( &baseInfo, &pointInfo, true );
			}
			break;
		default:
			assert_cs( false );
		}
	}

	// 케릭터 전용 라이트를 파일테이블에서 가져온다	
	baseInfo.s_bApplyObject = false;
	baseInfo.s_bTRShadow = false;
	baseInfo.s_bObjectShadow = false;
	CsMapCharLight::LIST* pList = nsCsMapTable::g_pMapCharLightMng->GetGroup( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )->GetList();
	CsMapCharLight::LIST_IT it = pList->begin();
	CsMapCharLight::LIST_IT itEnd = pList->end();
	CsMapCharLight::sINFO_BASE* baseFT;
	CsMapCharLight::sINFO_DIR* dirFT;
	CsMapCharLight::sINFO_POINT* pointFT;
	for( ;it!=itEnd; ++it )
	{
		baseFT = (*it)->GetInfo();
		baseInfo.s_eType = (CsGBLight::eTYPE)baseFT->s_nType;
		baseInfo.s_bApplyChar = baseFT->s_bChar;
		baseInfo.s_bApplyDynamicShadow = baseFT->s_bShadow;
		baseInfo.s_bEnableLight = baseFT->s_bEnableLight;
		baseInfo.s_Diffuse = NiColor( baseFT->s_fDiffuse[ 0 ], baseFT->s_fDiffuse[ 1 ], baseFT->s_fDiffuse[ 2 ] );
		baseInfo.s_Ambient = NiColor( baseFT->s_fAmbient[ 0 ], baseFT->s_fAmbient[ 1 ], baseFT->s_fAmbient[ 2 ] );
		baseInfo.s_Specular = NiColor( baseFT->s_fSpecular[ 0 ], baseFT->s_fSpecular[ 1 ], baseFT->s_fSpecular[ 2 ] );
		baseInfo.s_fPower = baseFT->s_fPower;	

		if( baseFT->s_nType == CsGBLight::DIRECTION )
		{
			dirFT = (CsMapCharLight::sINFO_DIR*)baseFT;
			dirInfo.s_Rotation.SetRow( 0, dirFT->s_fRot[ 0 ] );
			dirInfo.s_Rotation.SetRow( 1, dirFT->s_fRot[ 1 ] );
			dirInfo.s_Rotation.SetRow( 2, dirFT->s_fRot[ 2 ] );
			dirInfo.s_Pos = NiPoint3( dirFT->s_fPos[ 0 ], dirFT->s_fPos[ 1 ], dirFT->s_fPos[ 2 ] );
			dirInfo.s_fScale = dirFT->s_fScale;

			++m_Info.s_nLightCount;
			AddDirectionLight( &baseInfo, &dirInfo, true );
		}
		else
		{
			assert_cs( baseFT->s_nType == CsGBLight::POINT );
			pointFT = (CsMapCharLight::sINFO_POINT*)baseFT;
			pointInfo.s_Pos = NiPoint3( pointFT->s_Pos[ 0 ], pointFT->s_Pos[ 1 ], pointFT->s_Pos[ 2 ] );
			pointInfo.s_C = pointFT->s_C;
			pointInfo.s_L = pointFT->s_L;
			pointInfo.s_Q = pointFT->s_Q;
			pointInfo.s_Range = pointFT->s_Range;

			++m_Info.s_nLightCount;
			AddPointLight( &baseInfo, &pointInfo, true );
		}
	}
	

	ShaderApply_Char();
	MakeTrShaderExtra();
	
	if( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD )
	{
		g_pShadow->ResetMap();
	}
	else
	{
		g_pShadow->Tool_ResetMap( NiColor::BLACK );
	}
}

void CsGBLightMng::ShaderApply_Char()
{
#ifdef NDEBUG
	assert_cs( m_nCharDirection <= 2 );
	assert_cs( m_nCharDirection >= 1 );
#endif

	float fLightCnt = (float)m_nCharDirection;
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "LightCount", sizeof( float ), &fLightCnt );
}

void CsGBLightMng::MakeTrShaderExtra()
{
	sprintf_s( m_cTrShaderExtra, 16, "%d%d", m_nTrDirection, m_nTrPoint );

	assert_cs(	nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD || (
		( strcmp( m_cTrShaderExtra, "20" ) == 0 )||
		( strcmp( m_cTrShaderExtra, "21" ) == 0 )||
		( strcmp( m_cTrShaderExtra, "11" ) == 0 )||
		( strcmp( m_cTrShaderExtra, "10" ) == 0 )		)
		);
}

bool CsGBLightMng::IsOnlyCharLight( CsGBLight* pLight )
{
	if( ( pLight->GetBaseInfo()->s_bObjectShadow == false )&&
		( pLight->GetBaseInfo()->s_bTRShadow == false )&&
		( pLight->GetBaseInfo()->s_bApplyObject == false )
		)			
	{
		return pLight->GetBaseInfo()->s_bApplyChar || pLight->GetBaseInfo()->s_bApplyDynamicShadow;
	}
	return false;
}

void CsGBLightMng::SetLight( float fLight )
{	
	std::list< CsGBLight* >::iterator it = m_listLight.begin();
	std::list< CsGBLight* >::iterator itEnd = m_listLight.end();
	for( ; it!=itEnd; ++it )
	{				
		(*it)->GetNiLight()->SetDiffuseColor( (*it)->GetBaseInfo()->s_Diffuse * fLight );
		(*it)->GetNiLight()->SetAmbientColor( (*it)->GetBaseInfo()->s_Ambient * fLight );
	}
	ChangeShaderLightRate( fLight );
}


void CsGBLightMng::SetLight( float fLightDelta, float fLimit )
{	
	if( m_fLight == fLimit )
		return;

	if( m_fLight > fLimit ) //점점 어두워진다
	{
		m_fLight -= fLightDelta;
		if( m_fLight <= fLimit )
			m_fLight = fLimit;
	}
	else //점점 밝이진다
	{
		m_fLight += fLightDelta;
		if( m_fLight >= fLimit )
			m_fLight = fLimit;
	}		
	
	std::list< CsGBLight* >::iterator it = m_listLight.begin();
	std::list< CsGBLight* >::iterator itEnd = m_listLight.end();
	for( ; it!=itEnd; ++it )
	{				
		(*it)->GetNiLight()->SetDiffuseColor( (*it)->GetBaseInfo()->s_Diffuse * m_fLight );
		(*it)->GetNiLight()->SetAmbientColor( (*it)->GetBaseInfo()->s_Ambient * m_fLight );
	}
	ChangeShaderLightRate( m_fLight );
}

void CsGBLightMng::ChangeShaderLightRate( float fRate )
{
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_fLightRate", sizeof( float ), &fRate );
}
