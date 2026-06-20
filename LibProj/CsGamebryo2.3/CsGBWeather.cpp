#include "stdafx.h"
#include "CsGBWeather.h"

CsGBWeather*		g_pWeather = NULL;

CsGBWeather::CsGBWeather()
{
	m_eType = eType_End;

	m_nLevel = 0;	//희망레벨
	m_nCurLevel = 0;	//현재레벨
	m_nPerformance = 0;	//옵션 성능
	for( int n = 0; n < 3; ++n )
		m_nData[ n ] = 0;

	m_vPos = NiPoint3::ZERO;
	m_fVelocity = 1.0f;
	m_fAccumTime = 1.0f;

	m_bRender = false;
	m_bOption = false;
	m_fRainEndTime = 0.0f;
	m_vFarPos = NiPoint3::ZERO;
	m_vFarFarPos = NiPoint3::ZERO;
	m_vNearPos = NiPoint3::ZERO;
	m_vSnowParticlePos = NiPoint3::ZERO;
	m_vSnowParticlePos2 = NiPoint3::ZERO;

	for( int i = 0; i < WEATHER_TEXTURE_DEPTH; i++ )
	{
		m_pTexture[ i ] = 0;
		m_pTexture2[ i ] = 0;
	}	
}

CsGBWeather::~CsGBWeather()
{
	_Destroy();	
}

void CsGBWeather::GlobalShotInit()
{
	assert_cs( g_pWeather == NULL );
	g_pWeather = NiNew CsGBWeather;
	g_pWeather->_Init();
}

void CsGBWeather::GlobalShotDown()
{
	SAFE_NIDELETE( g_pWeather );	
}

void CsGBWeather::_Destroy()
{
	for( int i = 0; i < WEATHER_TEXTURE_DEPTH; i++ )
	{
		m_pTexture[ i ] = 0;
		m_pTexture2[ i ] = 0;
	}

	m_NodeFarFar.Delete();	
	m_NodeFar.Delete();	
	m_NodeNear.Delete();	
	m_NodeSnowParticle.Delete();	
	m_NodeSnowParticle2.Delete();
	m_NodeRainParticle.Delete();
	_DeleteRain();
}

void CsGBWeather::_DeleteRain()
{
	std::list < sRainINFO* >::iterator it = m_RainInfoList.begin();
	std::list < sRainINFO* >::iterator itEnd = m_RainInfoList.end();
	for( ; it != itEnd ; it++ )
	{	
		SAFE_NIDELETE( *it );			
	}
	m_RainInfoList.clear();
}

void CsGBWeather::_Init()
{
	m_eType = CsGBWeather::eType_End;
	m_fAccumTime = 0.0f;
	m_fVelocity = 0.0f;
	m_nLevel = 0;
	m_nCurLevel = 0;
	m_bRender = false;

	m_LevelTime.SetDeltaTime( 7000 );
	m_LevelTime.OffReset();
		
	m_nData[ 0 ] = NiTexturingProperty::FILTER_BILERP;				// 저사양 텍스쳐
	m_nData[ 1 ] = NiTexturingProperty::FILTER_BILERP_MIPNEAREST;	// 일반사양 텍스쳐
	m_nData[ 2 ] = NiTexturingProperty::FILTER_TRILERP;				// 고사양 텍스쳐
}

void CsGBWeather::Off()
{
	m_nCurLevel = m_nLevel = 0; 
	m_fAccumTime = 0.0f; 
	m_bRender = false; 
	_DeleteRain();
}

void CsGBWeather::SetType( eWeather eType )
{
	if( m_eType == eType )
		return;
	
	_Destroy();
	m_eType = eType;
	switch( m_eType )
	{
	case eType_Snow:
		{
			NiStream kStream;
			if( !kStream.Load( "Data\\EtcObject\\Weather\\Snow\\Farsnow0002.nif" ) )
			{
				assert_cs( false );
				return;
			}
			NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );	
			m_NodeFar.SetNiObject( pNode, CGeometry::Normal );	

			if( !kStream.Load( "Data\\EtcObject\\Weather\\Snow\\FarFarsnow0001.nif" ) )
			{
				assert_cs( false );
				return;
			}
			pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );	
			m_NodeFarFar.SetNiObject( pNode, CGeometry::Normal );		

			if( !kStream.Load( "Data\\EtcObject\\Weather\\Snow\\Nearsnow0002.nif" ) )
			{
				assert_cs( false );
				return;
			}
			pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );	
			m_NodeNear.SetNiObject( pNode, CGeometry::Normal );		

			if( !kStream.Load( "Data\\EtcObject\\Weather\\Snow\\Particlesnow0002.nif" ) )
			{
				assert_cs( false );
				return;
			}
			pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
			m_NodeSnowParticle.SetNiObject( pNode, CGeometry::Normal );	

			if( !kStream.Load( "Data\\EtcObject\\Weather\\Snow\\Particlesnow0003.nif" ) )
			{
				assert_cs( false );
				return;
			}
			pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
			m_NodeSnowParticle2.SetNiObject( pNode, CGeometry::Normal );		
		}
		break;
	case eType_Rain:
		{
			NiStream kStream;
			if( !kStream.Load( "Data\\EtcObject\\Weather\\Rain\\RainSphere0001.nif" ) )
			{
				assert_cs( false );
				return;
			}
			NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );	
			m_NodeNear.SetNiObject( pNode, CGeometry::Normal );	

			if( !kStream.Load( "Data\\EtcObject\\Weather\\Rain\\RainPlane0001.nif" ) )
			{
				assert_cs( false );
				return;
			}
			pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );	
			m_NodeFar.SetNiObject( pNode, CGeometry::Normal );	

			if( !kStream.Load( "Data\\EtcObject\\Weather\\Rain\\Fog0001.nif" ) )
			{
				assert_cs( false );
				return;
			}
			pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );	
			m_NodeFarFar.SetNiObject( pNode, CGeometry::Normal );				

			nsCSGBFUNC::EndAnimationTime( m_fRainEndTime, m_NodeRainParticle.m_pNiNode );			
			if( !kStream.Load( "Data\\EtcObject\\Weather\\Rain\\Rain_Filip0002.nif" ) )
			{
				assert_cs( false );
				return;
			}
			pNode = (NiNode*)kStream.GetObjectAt(0);
			nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );	
			m_NodeRainParticle.SetNiObject( pNode, CGeometry::Normal );	
			m_NodeRainParticle.m_pNiNode->SetScale( 1.2f );

			nsCSGBFUNC::EndAnimationTime( m_fRainEndTime, m_NodeRainParticle.m_pNiNode );						
		}
		break;
	default:
		assert_cs( false );
		break;
	}	

	_SetTexture( true );
}

void CsGBWeather::SetPerformance( int nPerformance )
{
	m_nPerformance = nPerformance; 	
	if( nPerformance == 3 ) 
	{
		m_bOption = false;
		if( m_eType == eType_Rain )
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->SetLight( 1.0f );
	}
	else
	{
		m_bOption = true;
		assert_cs( nPerformance >= 0 && nPerformance < 3 );		
		_SetTexture( true );
	}
}

void CsGBWeather::SetLevel( int nLevel )
{ 
	if( m_nLevel == nLevel ) 
	{
		return;
	}

	m_nLevel = nLevel;  

	if( m_nLevel > 0 ) 
	{
		m_bRender = true;

		if( m_nCurLevel < 0 )
			m_nCurLevel = 0;
	}	
}

void CsGBWeather::_SetTexture( bool bFileLoad )
{
	if( m_eType == CsGBWeather::eType_End || m_nCurLevel < 0 )
		return;		

	char szFileName[ MAX_FILENAME ];

	if( bFileLoad )
	{
		switch( m_nPerformance )
		{
		case 0: // 저사양
			m_pTexture[ 0 ] = m_pTexture2[ 0 ] = NiSourceTexture::Create( "Data\\EtcObject\\Weather\\Empty00.dds" );			
			break;
		case 1: case 2: //중, 고 사양
			m_pTexture[ 0 ] = m_pTexture2[ 0 ] = NiSourceTexture::Create( "Data\\EtcObject\\Weather\\Empty01.dds" );			 
			break;
		}		

		switch( m_eType )
		{
		case eType_Snow:
			{
				for( int i = 1; i < WEATHER_TEXTURE_DEPTH + 1; i++ )
				{
					switch( m_nPerformance )
					{
					case 0: // 저사양
						sprintf_s( szFileName, MAX_FILENAME, "Data\\EtcObject\\Weather\\Snow\\Snow0000_0%d.dds", i );
						break;
					case 1: case 2: //중, 고 사양
						sprintf_s( szFileName, MAX_FILENAME, "Data\\EtcObject\\Weather\\Snow\\Snow0001_0%d.dds", i );
						break;
					}

					m_pTexture[ i ] = NiSourceTexture::Create( szFileName );
				}
			}
			break;
		case eType_Rain:
			{	
				for( int i = 1; i < WEATHER_TEXTURE_DEPTH + 1; i++ )
				{
					sprintf_s( szFileName, MAX_FILENAME, "Data\\EtcObject\\Weather\\Rain\\Fog000%d.dds", i );
					m_pTexture2[ i ] = NiSourceTexture::Create( szFileName );				

					switch( m_nPerformance )
					{
					case 0: // 저사양
						sprintf_s( szFileName, MAX_FILENAME, "Data\\EtcObject\\Weather\\Rain\\rain0000_0%d.dds",  i );
						break;
					case 1: case 2: //중, 고 사양
						sprintf_s( szFileName, MAX_FILENAME, "Data\\EtcObject\\Weather\\Rain\\rain0001_0%d.dds",  i );
						break;
					}				

					m_pTexture[ i ] = NiSourceTexture::Create( szFileName );													
				}														
			}
			break;
		default:
			assert_cs( false );
			break;
		}	
	}	
	
	NiTexturingProperty* pTexProp = (NiTexturingProperty*)m_NodeFar.m_vpGeom[ 0 ]->m_pGeometry->GetProperty( NiTexturingProperty::GetType() );
	pTexProp->SetBaseTexture( m_pTexture[ m_nCurLevel ] );
	pTexProp->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );	
	pTexProp->SetBaseFilterMode( (NiTexturingProperty::FilterMode)m_nData[ m_nPerformance ] );

	pTexProp = (NiTexturingProperty*)m_NodeNear.m_vpGeom[ 0 ]->m_pGeometry->GetProperty( NiTexturingProperty::GetType() );
	pTexProp->SetBaseTexture( m_pTexture[ m_nCurLevel ] );
	pTexProp->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );	
	pTexProp->SetBaseFilterMode( (NiTexturingProperty::FilterMode)m_nData[ m_nPerformance ] );

	if( m_eType == eType_Rain )
	{
		NiTexturingProperty* pTexProp = (NiTexturingProperty*)m_NodeFarFar.m_vpGeom[ 0 ]->m_pGeometry->GetProperty( NiTexturingProperty::GetType() );
		pTexProp->SetBaseTexture( m_pTexture2[ m_nCurLevel ] );
		pTexProp->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );	
	}

}

void CsGBWeather::_UpdateLevel()
{
	if( m_nCurLevel == m_nLevel && m_nCurLevel > 0 )
		return; 

	if( m_LevelTime.IsEnable() == true )
	{
		if( (m_nCurLevel < m_nLevel) && (m_nCurLevel > -1) )
		{
			m_nCurLevel++;
			_SetTexture( false );
		}
		else 
		{
			m_nCurLevel--;
			_SetTexture( false );
		}		
	}		
}


void CsGBWeather::Render()
{
	if( !m_bRender || !m_bOption )
		return;

	switch( m_eType )
	{
	case eType_Snow:
		{
			if( m_nCurLevel > -1 )
			{
				if( CAMERA_ST.GetCurPitch() > -0.8f )
				{
					m_NodeFar.RenderAbsolute();
					m_NodeFarFar.RenderAbsolute();
				}
				m_NodeNear.RenderAbsolute();		
			}

			m_NodeSnowParticle.RenderAbsolute();
			m_NodeSnowParticle2.RenderAbsolute();	
		}
		break;
	case eType_Rain:
		{
			m_NodeFarFar.RenderAbsolute();
			if( CAMERA_ST.GetCurPitch() > -0.8f )
			{
				m_NodeFar.RenderAbsolute();				
			}
			m_NodeNear.RenderAbsolute();				
			_RendRain();			
		}
		break;
	}	
}

void CsGBWeather::Update( float fDeltaTime, D3DXMATRIX* matView )
{
	if( !m_bRender || !m_bOption )
		return;

	_UpdateLevel();

	switch( m_eType )
	{
	case eType_Snow:
		{
			// 회전값
			NiMatrix3 mat;
			mat.MakeZRotation( CAMERA_ST.GetCurRoll() + NI_PI );

			m_fVelocity = 0.25f + ( m_nCurLevel * 0.05f );
			m_fAccumTime += fDeltaTime * m_fVelocity;

			if( m_nCurLevel < 0 )
			{	
				// 파티클 더 생성 막기 위해 엄한데로 보낸다;;
				m_NodeSnowParticle.m_pNiNode->SetTranslate( NiPoint3( 0, 0, -50000.0f ) );					
				m_NodeSnowParticle2.m_pNiNode->SetTranslate( NiPoint3( 0, 0, -50000.0f ) );				

				m_NodeSnowParticle.m_pNiNode->Update( m_fAccumTime * 2.5f );
				m_NodeSnowParticle2.m_pNiNode->Update( m_fAccumTime * 2.5f );

				if( m_nCurLevel <= -3 )
				{
					m_fAccumTime = 0.0f;
					m_bRender = false;					
				}	
				return;
			}	
			else
			{
				m_vSnowParticlePos = m_vPos;						
				m_NodeSnowParticle.m_pNiNode->SetRotate( mat );
				m_NodeSnowParticle.m_pNiNode->SetTranslate( m_vSnowParticlePos );					

				m_vSnowParticlePos2 = CAMERA_ST.GetWorldTranslate();	
				m_vSnowParticlePos2.z = m_vPos.z + 370.0f;
				m_NodeSnowParticle2.m_pNiNode->SetTranslate( m_vSnowParticlePos2 );	

				m_NodeSnowParticle.m_pNiNode->Update( m_fAccumTime * 2.5f );
				m_NodeSnowParticle2.m_pNiNode->Update( m_fAccumTime * 2.5f );
			}		

			// 이동값	
			m_vFarFarPos = m_vPos;		
			m_NodeFarFar.m_pNiNode->SetTranslate( m_vFarFarPos );	
			m_NodeFarFar.m_pNiNode->SetRotate( mat );

			m_vFarPos = m_vPos;		
			m_NodeFar.m_pNiNode->SetTranslate( m_vFarPos );	
			m_NodeFar.m_pNiNode->SetRotate( mat );

			m_vNearPos = m_vPos - CAMERA_ST.GetWorldTranslate();	
			m_NodeNear.m_pNiNode->SetScale( m_vNearPos.Length() / 1500.0f );
			m_NodeNear.m_pNiNode->SetRotate( mat );
			m_NodeNear.m_pNiNode->SetTranslate( CAMERA_ST.GetWorldTranslate() );				

			// 업데이트	
			m_NodeFar.m_pNiNode->Update( m_fAccumTime );	
			m_NodeFarFar.m_pNiNode->Update( m_fAccumTime );	
			m_NodeNear.m_pNiNode->Update( m_fAccumTime );
			
		}
		break;
	case eType_Rain:
		{
			// 회전값
			// Z값 회전 결정
			NiMatrix3 mat;
			mat.MakeZRotation( CAMERA_ST.GetCurRoll() + NI_PI );

			static NiPoint3 vCameraOldPos = CAMERA_ST.GetWorldTranslate();
			NiPoint3 vCameraPos = CAMERA_ST.GetWorldTranslate();
			NiPoint3 vCameraVelocity = ( vCameraPos - vCameraOldPos ) * fDeltaTime * 0.025f;
			vCameraOldPos = vCameraPos;

			// x, y축값 회전 결정 - 카메라 속도에 따른 강수의 변화
			NiPoint3 vResult;
			D3DXVec3TransformNormal( (D3DXVECTOR3*)&vResult, (D3DXVECTOR3*)&vCameraVelocity, matView );
			vResult.y *= 2.0f;
			vResult.y = CsMax( -0.75f, CsMin( 0.75f, vResult.y ) );
			vResult.x = CsMax( -0.75f, CsMin( 0.75f, vResult.x ) );
			NiMatrix3 mat2;

			if( abs( vResult.x ) < 0.1f )
				vResult.x *= 2;
			if( abs( vResult.x ) > 0.4f )
				vResult.x *= 0.5f;

			mat2.FromEulerAnglesZXY( 0, -vResult.y, vResult.x );

			m_fVelocity = 0.32f + ( m_nCurLevel * 0.02f );
			m_fAccumTime += fDeltaTime * m_fVelocity;

			if( m_nCurLevel < 0 )
			{
				m_fAccumTime = 0.0f;
				m_bRender = false;	
				nsCsGBTerrain::g_pCurRoot->GetLightMng()->SetLight( 1.0f );
				return;
			}				

			m_vFarPos = CAMERA_ST.GetWorldTranslate();					
			m_NodeFar.m_pNiNode->SetTranslate( m_vFarPos );	
			m_NodeFar.m_pNiNode->SetRotate( mat*mat2 );	

			m_vNearPos = m_vPos - CAMERA_ST.GetWorldTranslate();	
			m_NodeNear.m_pNiNode->SetScale( m_vNearPos.Length() / 1500.0f );
			m_NodeNear.m_pNiNode->SetRotate( mat*mat2 );
			m_NodeNear.m_pNiNode->SetTranslate( m_vPos );	
			
			m_vFarFarPos = m_vPos;								
			m_NodeFarFar.m_pNiNode->SetRotate( mat );
			m_NodeFarFar.m_pNiNode->SetTranslate( m_vFarFarPos );				

			// 업데이트	
			m_NodeFar.m_pNiNode->Update( m_fAccumTime );
			m_NodeNear.m_pNiNode->Update( m_fAccumTime );
			m_NodeFarFar.m_pNiNode->Update( m_fAccumTime );
			_MakeRain( fDeltaTime );
		}
		break;
	}
}

void CsGBWeather::_MakeRain( float fDeltaTime )
{
	//비오니까 어두워지자
	nsCsGBTerrain::g_pCurRoot->GetLightMng()->SetLight( fDeltaTime * 0.01f, 1.0f - ( m_nCurLevel * 0.06f ) );						

	NiPoint3 vPos = m_vPos;
	if( (int)m_RainInfoList.size() < m_nCurLevel * 400 )
	{
		sRainINFO* Info = NiNew sRainINFO;		
		Info->s_vPos.x += _GetRandomFloat( vPos.x - 800.0f, vPos.x + 800.0f );
		Info->s_vPos.y += _GetRandomFloat( vPos.y - 800.0f, vPos.y + 800.0f );
		Info->s_vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( NiPoint2( Info->s_vPos.x, Info->s_vPos.y) ) + 5.0f;					
		m_RainInfoList.push_back( Info );
	}	
}

void CsGBWeather::_RendRain()
{
	std::list < sRainINFO* >::iterator it = m_RainInfoList.begin();
	std::list < sRainINFO* >::iterator itEnd = m_RainInfoList.end();
	for( ; it != itEnd ;  )
	{
		if( (*it)->s_AccumTime > m_fRainEndTime )
		{
			SAFE_NIDELETE( *it );
			it = m_RainInfoList.erase( it );
		}
		else
		{
			(*it)->s_AccumTime += 0.05f;													
			m_NodeRainParticle.m_pNiNode->Update( (*it)->s_AccumTime );					
			m_NodeRainParticle.m_pNiNode->SetTranslate( (*it)->s_vPos );
			m_NodeRainParticle.RenderAbsolute();
			++it;
		}
	}
}

float CsGBWeather::_GetRandomFloat( float low, float high )
{
	if( low >= high )		return low;

	float f = (rand() % 10000) * 0.0001f;

	return (f * (high - low)) +low;
}


