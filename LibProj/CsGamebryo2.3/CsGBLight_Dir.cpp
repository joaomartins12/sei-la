
#include "stdafx.h"
#include "CsGBLight_Dir.h"

CsGBLight_Dir::CsGBLight_Dir()
{
	m_pLight = NULL;
}

void CsGBLight_Dir::Delete()
{
	m_pLight = 0;
}

void CsGBLight_Dir::Create( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Dir::sINFO* Info )
{
	assert_cs( m_pLight == NULL );

	CsGBLight::Create( baseInfo );
	memcpy( &m_Info, Info, sizeof( sINFO ) );

	m_pLight = NiNew NiDirectionalLight;
	m_pLight->SetDiffuseColor( m_BaseInfo.s_Diffuse );
	m_pLight->SetAmbientColor( m_BaseInfo.s_Ambient );
	m_pLight->SetSpecularColor( m_BaseInfo.s_Specular );
	m_pLight->SetRotate( m_Info.s_Rotation );
	m_pLight->SetDimmer( m_BaseInfo.s_fPower );
	m_pLight->Update( 0.0f, false );
}

void CsGBLight_Dir::Reset( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Dir::sINFO* Info )
{
	assert_cs( m_pLight != NULL );
	CsGBLight::Create( baseInfo );
	memcpy( &m_Info, Info, sizeof( sINFO ) );
	m_pLight->SetDiffuseColor( m_BaseInfo.s_Diffuse );
	m_pLight->SetAmbientColor( m_BaseInfo.s_Ambient );
	m_pLight->SetSpecularColor( m_BaseInfo.s_Specular );
	m_pLight->SetRotate( m_Info.s_Rotation );
	m_pLight->SetDimmer( m_BaseInfo.s_fPower );
	m_pLight->Update( 0.0f, false );
}

void CsGBLight_Dir::SetRotation( NiMatrix3 rot )
{
	m_pLight->SetRotate( rot );
	m_pLight->Update( 0.0f, false );
	m_Info.s_Rotation = rot;
}

void CsGBLight_Dir::SetPower( float fPower )
{
	m_BaseInfo.s_fPower = fPower;
	m_pLight->SetDimmer( fPower );
}

void CsGBLight_Dir::SetDiffuseColor( NiColor color )
{
	m_pLight->SetDiffuseColor( color );
	m_BaseInfo.s_Diffuse = color;
}

void CsGBLight_Dir::SetAmbientColor( NiColor color )
{
	m_pLight->SetAmbientColor( color );
	m_BaseInfo.s_Ambient = color;
}

void CsGBLight_Dir::SetSpecularColor( NiColor color )
{
	m_pLight->SetSpecularColor( color );
	m_BaseInfo.s_Specular = color;
}
