
#include "stdafx.h"
#include "Preview_Tex.h"

void cPreviewObj_Tex::Delete()
{
	m_pTexObj = 0;
}

void cPreviewObj_Tex::Init()
{
	int nVertCnt = 4;
	NiPoint3* pPtsF = NiNew NiPoint3[ nVertCnt ];
	NiPoint2* pTUVsF = NiNew NiPoint2[ nVertCnt ];

	pPtsF[ 0 ] = NiPoint3( -100.0f, 0.0f, -100.0f );
	pPtsF[ 1 ] = NiPoint3( -100.0f, 0.0f, 100.0f );
	pPtsF[ 2 ] = NiPoint3( 100.0f, 0.0f, -100.0f );
	pPtsF[ 3 ] = NiPoint3( 100.0f, 0.0f, 100.0f );

	pTUVsF[ 0 ] = NiPoint2( 0.0f, 0.0f );
	pTUVsF[ 1 ] = NiPoint2( 0.0f, 1.0f );
	pTUVsF[ 2 ] = NiPoint2( 1.0f, 0.0f );
	pTUVsF[ 3 ] = NiPoint2( 1.0f, 1.0f );

	int nXPlaneCountInNode = 1;
	int nYPlaneCountInNode = 1;

	unsigned short* pConn = NiAlloc(unsigned short, 6);
	pConn[0] = 0;
	pConn[1] = 2;
	pConn[2] = 3;
	pConn[3] = 0;
	pConn[4] = 3;
	pConn[5] = 1;

	NiTriShapePtr pTriShape = NiNew NiTriShape( 4, pPtsF, NULL, NULL, pTUVsF, 1, NiGeometryData::NBT_METHOD_NONE, 2, pConn );
	pTriShape->CalculateNormals();

	m_pTexObj = NiNew NiNode;
	m_pTexObj->AttachChild( pTriShape );
	m_pTexObj->UpdateProperties();
	m_pTexObj->Update( 0.0 );

	m_bTexRender = false;
}

void cPreviewObj_Tex::Update( CsGBSubCamera* pCamera )
{
	if( m_bTexRender == true )
	{
		NiBound Bound = m_pTexObj->GetWorldBound();
		pCamera->SetRotation( 0.0f, 0.0f );
		pCamera->SetTranslate( NiPoint3( 0.0f, 0.0f, Bound.GetCenter().z ) );
		pCamera->SetDist( 250.0f );
		pCamera->_UpdateCamera();

		m_pTexObj->Update( 0.0f );
	}
}

void cPreviewObj_Tex::Render( CsGBSubCamera* pCamera )
{
	if( m_bTexRender == true )
		NiDrawScene( pCamera->GetCameraObj(), m_pTexObj, *g_pCuller );
}

void cPreviewObj_Tex::SetModelView( TCHAR* szPath, TCHAR* szName )
{
	NiTriShape* pTriShape = (NiTriShape*)m_pTexObj->GetAt( 0 );
	assert_cs( NiIsKindOf( NiTriShape, pTriShape ) == true );

	pTriShape->RemoveProperty( NiTexturingProperty::GetType() );

	TCHAR szFullPath[ MAX_PATH ] = {0, };
	_stprintf_s( szFullPath, MAX_PATH, L"%s\\%s", szPath, szName );
	if(( szFullPath, 0 ) != 0 )
	{
		ReleaseModelView();
	}

	char cFullPath[ MAX_PATH ] = {0, };
	W2M( cFullPath, szFullPath, MAX_PATH );

	NiTexturingProperty* pNewTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( cFullPath, NiTexturingProperty::APPLY_REPLACE );
	pTriShape->AttachProperty( pNewTexProp );
	pTriShape->UpdateProperties();

	m_bTexRender = true;
}

void cPreviewObj_Tex::ReleaseModelView()
{
	m_bTexRender = false;
}
