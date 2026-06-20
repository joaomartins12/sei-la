
#include "stdafx.h"
#include "CsGBLight.h"

CsGBLight::CsGBLight()
{
	
}

void CsGBLight::Create( CsGBLight::sBASE_INFO* baseInfo )
{
	memcpy( &m_BaseInfo, baseInfo, sizeof( sBASE_INFO ) );
}