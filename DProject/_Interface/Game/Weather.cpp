

#include "stdafx.h"
#include "Weather.h"

void cWeather::Delete()
{
	// 호출시점 바꾸어주자~ Mng콜레터 제거시에 제거되야 한다.
	m_NodeObject.Delete();
	SAFE_DELETE( m_pIniFile );
}

void cWeather::Init()
{
	NiStream kStream;
	if( !kStream.Load( "Data\\EtcObject\\Weather\\Weather_Rain.nif" ) )
	{
		assert_cs( false );
		return;
	}
	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
	m_NodeObject.SetNiObject( pNode, CGeometry::Normal );
	
	NiTexturingProperty* pTexProp = (NiTexturingProperty*)m_NodeObject.m_vpGeom[ 0 ]->m_pGeometry->GetProperty( NiTexturingProperty::GetType() );
	m_ptrTextureSrc = pTexProp->GetBaseTextureTransform();
	for( int i=0; i<WEATHER_TEXTURE_DEPTH; ++i )
	{
		m_trTexture[ i ] = *m_ptrTextureSrc;
	}	
	m_eCurState = STATE_NONE;

	m_pIniFile = csnew cProfile( "Weather.ini" );
	
}

void cWeather::BeginRain()
{
	m_LevelTime.SetDeltaTime( m_pIniFile->GetInt("TIME", "BEGIN_TIME") );
	m_LevelTime.OffReset();

	m_nCurLevel = 0;
	_SetRainTexture( m_nCurLevel );	

	m_eCurState = STATE_RAIN_BEGIN;
}

void cWeather::EndRain()
{
	m_eCurState = STATE_RAIN_END;

	m_LevelTime.SetDeltaTime( m_pIniFile->GetInt("TIME", "END_TIME") );
	m_LevelTime.OffReset();
}

void cWeather::Update()
{
	switch( m_eCurState )
	{
	case STATE_RAIN_BEGIN:
		if( m_LevelTime.IsEnable() == true )
		{
			_SetRainTexture( ++m_nCurLevel );
			if( m_nCurLevel == 3 )
				m_eCurState = STATE_RAINING;
		}
		break;
	case STATE_RAIN_END:
		if( m_LevelTime.IsEnable() == true )
		{
			if( m_nCurLevel == 0 )
				m_eCurState = STATE_NONE;
			else
				_SetRainTexture( --m_nCurLevel );
		}
		break;
	}

	static NiPoint3 vCameraOldPos = CAMERA_ST.GetWorldTranslate();
	NiPoint3 vCameraPos = CAMERA_ST.GetWorldTranslate();

	float SS = _GetIniValue( "S_streak" )*0.01f;
	NiPoint3 vCameraVelocity = ( vCameraPos - vCameraOldPos ) * g_fDeltaTime * 0.05f;
	vCameraOldPos = vCameraPos;
	m_fVelocity = SS*g_fDeltaTime;

	// 이동값
	m_NodeObject.m_pNiNode->SetTranslate( CAMERA_ST.GetWorldTranslate() );

	// 회전값

	// Z값 회전 결정
	NiMatrix3 mat;
	mat.MakeZRotation( CAMERA_ST.GetCurRoll() + NI_PI );

	// x, y축값 회전 결정 - 카메라 속도에 따른 강수의 변화
	NiPoint3 vResult;
	D3DXVec3TransformNormal( (D3DXVECTOR3*)&vResult, (D3DXVECTOR3*)&vCameraVelocity, g_pEngine->GetD3DView() );
	vResult.y *= 2.0f;
	vResult.y = CsMax( -0.75f, CsMin( 0.75f, vResult.y ) );
	vResult.x = CsMax( -0.75f, CsMin( 0.75f, vResult.x ) );
	NiMatrix3 mat2;

	if( abs( vResult.x ) < 0.1f )
		vResult.x *= 2;
	if( abs( vResult.x ) > 0.4f )
		vResult.x *= 0.5f;

	mat2.FromEulerAnglesZXY( 0, -vResult.y, vResult.x );

	m_NodeObject.m_pNiNode->SetRotate( mat*mat2 );

	m_NodeObject.m_pNiNode->SetScale( 0.04f );
	m_NodeObject.m_pNiNode->Update( 0.0f );
}

void cWeather::_ApplyDepth( int nDepth )
{
	float fRate;
	switch( nDepth )
	{
	case 0:		fRate = _GetIniValue( "R1_SPEED" )*0.01f;			break;
	case 1:		fRate = _GetIniValue( "R2_SPEED" )*0.01f;			break;
	case 2:		fRate = _GetIniValue( "R3_SPEED" )*0.01f;			break;
	case 3:		fRate = _GetIniValue( "R4_SPEED" )*0.01f;			break;
	default:	assert_cs( false );
	}

	switch( nDepth )
	{
	case 0:
		m_trTexture[ nDepth ].SetScale( NiPoint2( _GetIniValue( "R1_TEX_X" )*0.1f, _GetIniValue( "R1_TEX_Y" )*0.1f ) );
		break;
	case 1:
		m_trTexture[ nDepth ].SetScale( NiPoint2( _GetIniValue( "R2_TEX_X" )*0.1f, _GetIniValue( "R2_TEX_Y" )*0.1f ) );
		break;
	case 3:
		m_trTexture[ nDepth ].SetScale( NiPoint2( _GetIniValue( "R3_TEX_X" )*0.1f, _GetIniValue( "R3_TEX_Y" )*0.1f ) );
		break;
	case 2:
		m_trTexture[ nDepth ].SetScale( NiPoint2( _GetIniValue( "R4_TEX_X" )*0.1f, _GetIniValue( "R4_TEX_Y" )*0.1f ) );
		break;
	}

	NiPoint2 pos = m_trTexture[ nDepth ].GetTranslate();
	pos.y -= m_fVelocity*fRate;
	m_trTexture[ nDepth ].SetTranslate( pos );
	*m_ptrTextureSrc  = m_trTexture[ nDepth ];	
}

void cWeather::Render()
{
	if( m_eCurState == STATE_NONE )
		return;

	Update();

	for( int i=0; i<WEATHER_TEXTURE_DEPTH; ++i )
	{
		_ApplyDepth( i );
		m_NodeObject.RenderAbsolute();
	}	
}

void cWeather::_SetRainTexture( int nLevel )
{
	NiSourceTexture* pSourceTex = NULL;
	
	switch( nLevel )
	{
	case 0:		pSourceTex = NiSourceTexture::Create( "Data\\EtcObject\\Weather\\Rain_01.tga" );		break;
	case 1:		pSourceTex = NiSourceTexture::Create( "Data\\EtcObject\\Weather\\Rain_02.tga" );		break;
	case 2:		pSourceTex = NiSourceTexture::Create( "Data\\EtcObject\\Weather\\Rain_03.tga" );		break;
	case 3:		pSourceTex = NiSourceTexture::Create( "Data\\EtcObject\\Weather\\Rain_04.tga" );		break;
	default:	assert_cs( false );
	}
	
	NiTexturingProperty* pTexProp = (NiTexturingProperty*)m_NodeObject.m_vpGeom[ 0 ]->m_pGeometry->GetProperty( NiTexturingProperty::GetType() );

	pTexProp->SetBaseTexture( pSourceTex );
}

int cWeather::_GetIniValue( char* key )
{
	char cLevel[ 32 ];
	sprintf_s( cLevel, 32, "LEVEL%d", m_nCurLevel+1 );
	return m_pIniFile->GetInt( cLevel, key );
}