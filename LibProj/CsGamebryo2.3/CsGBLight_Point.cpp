
#include "stdafx.h"
#include "CsGBLight_Point.h"

CsGBLight_Point::CsGBLight_Point()
{
	m_pLight = NULL;
}

void CsGBLight_Point::Delete()
{
	m_pLight = 0;
}

void CsGBLight_Point::Create( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Point::sINFO* Info )
{
	assert_cs( m_pLight == NULL );

	CsGBLight::Create( baseInfo );
	memcpy( &m_Info, Info, sizeof( sINFO ) );

	m_pLight = NiNew NiPointLight;
	m_pLight->SetDiffuseColor( m_BaseInfo.s_Diffuse );
	m_pLight->SetAmbientColor( m_BaseInfo.s_Ambient );
	m_pLight->SetSpecularColor( m_BaseInfo.s_Specular );
	m_pLight->SetTranslate( m_Info.s_Pos );
	m_pLight->SetDimmer( m_BaseInfo.s_fPower );
	SetConstant( m_Info.s_C, m_Info.s_L, m_Info.s_Q );
	m_pLight->Update( 0.0f, false );
}

void CsGBLight_Point::Reset( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Point::sINFO* Info )
{
	assert_cs( m_pLight != NULL );
	CsGBLight::Create( baseInfo );
	memcpy( &m_Info, Info, sizeof( sINFO ) );
	m_pLight->SetDiffuseColor( m_BaseInfo.s_Diffuse );
	m_pLight->SetAmbientColor( m_BaseInfo.s_Ambient );
	m_pLight->SetSpecularColor( m_BaseInfo.s_Specular );
	m_pLight->SetTranslate( m_Info.s_Pos );
	m_pLight->SetDimmer( m_BaseInfo.s_fPower );
	SetConstant( m_Info.s_C, m_Info.s_L, m_Info.s_Q );
	m_pLight->Update( 0.0f, false );
}

void CsGBLight_Point::SetPosition( NiPoint3 pos )
{
	m_pLight->SetTranslate( pos );
	m_Info.s_Pos = pos;
	m_pLight->Update( 0.0f, false );
}

void CsGBLight_Point::SetPower( float fPower )
{
	m_BaseInfo.s_fPower = fPower;
	m_pLight->SetDimmer( fPower );
}

void CsGBLight_Point::SetConstant( float c, float l, float q )
{
	m_Info.s_C = c;
	m_Info.s_L = l;
	m_Info.s_Q = q;

	m_pLight->SetConstantAttenuation( c );
	m_pLight->SetLinearAttenuation( l );
	m_pLight->SetQuadraticAttenuation( q );	
}

void CsGBLight_Point::SetRange( float range, float c )
{
	m_Info.s_Range = range;
	m_Info.CalConstant( range, c );
	SetConstant( m_Info.s_C, m_Info.s_L, m_Info.s_Q );
}

void CsGBLight_Point::SetDiffuseColor( NiColor color )
{
	m_pLight->SetDiffuseColor( color );
	m_BaseInfo.s_Diffuse = color;
}

void CsGBLight_Point::SetAmbientColor( NiColor color )
{
	m_pLight->SetAmbientColor( color );
	m_BaseInfo.s_Ambient = color;
}

void CsGBLight_Point::SetSpecularColor( NiColor color )
{
	m_pLight->SetSpecularColor( color );
	m_BaseInfo.s_Specular = color;
}
