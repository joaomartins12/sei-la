
#include "stdafx.h"
#include "Region.h"


cRegion::cRegion()
{
	m_pNiRegion = NULL;
	//m_pNiBox100 = NULL;
}


void cRegion::Init()
{
	#define	REGION_PATH			".\\ToolData\\RegionCylinder.nif"

	if( _access_s( REGION_PATH, 0 ) != 0 )
	{
		MessageBox( NULL, L"ToolData\\RegionCylinder.nif 가 존재 하지않습니다.\n\n 강제 종료 될수 있습니다.", NULL, MB_OK );
	}
	else
	{
		NiStream stream;
		stream.Load( REGION_PATH );
		m_pNiRegion = (NiNode*)stream.GetObjectAt( 0 );
		m_pNiRegion->UpdateProperties();
	}

	/*#define	BOX100_PATH			".\\ToolData\\Box100.nif"

	if( _access_s( REGION_PATH, 0 ) != 0 )
	{
		MessageBox( NULL, L".\\ToolData\\Box100.nif 가 존재 하지않습니다.\n\n 강제 종료 될수 있습니다.", NULL, MB_OK );
	}
	else
	{
		NiStream stream;
		stream.Load( BOX100_PATH );
		m_pNiBox100 = (NiNode*)stream.GetObjectAt( 0 );
		m_pNiBox100->UpdateProperties();
	}*/
}

void cRegion::Delete()
{
	m_pNiRegion = 0;
//	m_pNiBox100 = 0;
}

void cRegion::Render( NiPoint2 ptPos, float fRadius, bool bSelect )
{
	if( bSelect == true )
		nsCSGBFUNC::Set_Emittance( m_pNiRegion, NiColor( 1.0f, 0.0f, 0.0f ) );
	else
		nsCSGBFUNC::Set_Emittance( m_pNiRegion, NiColor( 1.0f, 1.0f, 0.0f ) );

	NiPoint3 ptTranse = NiPoint3( ptPos.x, ptPos.y, nsCsGBTerrain::g_pCurRoot->GetHeight( ptPos.x, ptPos.y ) + 500.0f - fRadius*5.0f );

	m_pNiRegion->SetScale( fRadius*0.01f );
	m_pNiRegion->SetTranslate( ptTranse );
	m_pNiRegion->Update( 0.0f, false );
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_pNiRegion, *g_pCuller );
}

void cRegion::Render( NiPoint2 ptPos, float fRadius, NiColor color )
{
	nsCSGBFUNC::Set_Emittance( m_pNiRegion, color );

	NiPoint3 ptTranse = NiPoint3( ptPos.x, ptPos.y, nsCsGBTerrain::g_pCurRoot->GetHeight( ptPos.x, ptPos.y ) + 500.0f - fRadius*5.0f );

	m_pNiRegion->SetScale( fRadius*0.01f );
	m_pNiRegion->SetTranslate( ptTranse );
	m_pNiRegion->Update( 0.0f, false );
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_pNiRegion, *g_pCuller );
}

//void cRegion::RenderBox100( NiPoint3 ptPos, bool bRed )
//{
//	//if( bRed == true )
//	//	nsCSGBFUNC::Set_Emittance( m_pNiBox100, NiColor( 1.0f, 0.0f, 0.0f ) );
//	//else
//	//	nsCSGBFUNC::Set_Emittance( m_pNiBox100, NiColor( 1.0f, 1.0f, 1.0f ) );
//
//	m_pNiBox100->SetTranslate( ptPos );
//	m_pNiBox100->Update( 0.0f, false );
//	NiDrawScene( CAMERA_ST.GetCameraObj(), m_pNiBox100, *g_pCuller );
//}