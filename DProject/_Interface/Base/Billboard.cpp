
#include "stdafx.h"
#include "Billboard.h"

NiAlphaProperty*		CBillboard::m_AlphaProp = NULL;
NiZBufferProperty*		CBillboard::m_ZBufferProp = NULL;
NiMaterialProperty*		CBillboard::m_MapProp = NULL;

void CBillboard::GlobalInit()
{
	assert_cs( m_AlphaProp == NULL );
	m_AlphaProp = NiNew NiAlphaProperty;
	m_AlphaProp->SetAlphaBlending(true);
	m_AlphaProp->SetAlphaTesting(false);    
	
	assert_cs( m_ZBufferProp == NULL );
	m_ZBufferProp = NiNew NiZBufferProperty;
	m_ZBufferProp->SetZBufferTest( true );
	m_ZBufferProp->SetZBufferWrite( false );

	assert_cs( m_MapProp == NULL );
	m_MapProp = NiNew NiMaterialProperty;
	m_MapProp->SetEmittance( FONT_WHITE );
	m_MapProp->SetAlpha( 1.0f );
}

void CBillboard::GlobalDelete()
{
	SAFE_NIDELETE( m_AlphaProp );
	SAFE_NIDELETE( m_ZBufferProp );
	SAFE_NIDELETE( m_MapProp );

	assert_cs( m_AlphaProp == NULL );
	assert_cs( m_ZBufferProp == NULL );
	assert_cs( m_MapProp == NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

CBillboard::CBillboard():m_pTexture(NULL),m_fAlpha(1.0f)
,m_fSizeX(0.0f),m_fSizeY(0.0f),m_Color(NiColor::WHITE)
{
}

CBillboard::~CBillboard()
{
	m_Node.Delete();
	RESOURCEMGR_ST.RemoveTexture( m_pTexture );	
}

void CBillboard::CreateFile( char const* pkFileName, float size_x, float size_y )
{
	m_pTexture = RESOURCEMGR_ST.GetTexture( pkFileName );
	CreateTexture( size_x, size_y, m_pTexture->GetTexture() );	
}

void CBillboard::CreateFile( char const* pkFileName, float size_x, float size_y, float fTexX1, float fTexX2, float fTexY1, float fTexY2 )
{
	m_pTexture = RESOURCEMGR_ST.GetTexture( pkFileName );
	CreateTexture( size_x, size_y, fTexX1, fTexX2, fTexY1, fTexY2, m_pTexture->GetTexture() );	
}

void CBillboard::CreateTexture( float size_x, float size_y, NiTexture* pTexture)
{
	assert_cs( m_Node.m_pNiGeom == NULL );

	m_fSizeX = size_x;
	m_fSizeY = size_y;

	unsigned int uiNumVerts = 4;
	NiPoint3* pkVerts = NiNew NiPoint3[uiNumVerts];
	NiPoint2* pkTex = NiNew NiPoint2[uiNumVerts];
	unsigned short usTriangles = 2;
	unsigned short* pusTriList = NiAlloc(unsigned short, 3 * usTriangles);

	pkVerts[ 0 ] = pkVerts[ 1 ] = pkVerts[ 2 ] = pkVerts[ 3 ] = NiPoint3::ZERO;

	pkTex[0] = NiPoint2(0.0f, 1.0f);
	pkTex[1] = NiPoint2(1.0f, 1.0f);
	pkTex[2] = NiPoint2(1.0f, 0.0f);
	pkTex[3] = NiPoint2(0.0f, 0.0f);

	pusTriList[0] = 0;
	pusTriList[1] = 1;
	pusTriList[2] = 2;

	pusTriList[3] = 0;
	pusTriList[4] = 2;
	pusTriList[5] = 3;

	NiTriShapePtr spTriShape = NiNew NiTriShape( uiNumVerts, pkVerts, NULL, NULL, pkTex, 1, NiGeometryData::NBT_METHOD_NONE, usTriangles, pusTriList );
	spTriShape->GetModelData()->SetConsistency(NiGeometryData::VOLATILE);

	NiTexturingProperty* pkTextProp = NiNew NiTexturingProperty;
	pkTextProp->SetBaseTexture(pTexture);
	pkTextProp->SetBaseFilterMode( NiTexturingProperty::FILTER_BILERP );
	pkTextProp->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );
	spTriShape->AttachProperty(pkTextProp);

	spTriShape->AttachProperty( (NiProperty*)m_AlphaProp->Clone() );
	spTriShape->AttachProperty( (NiProperty*)m_ZBufferProp->Clone() );
	spTriShape->AttachProperty( (NiProperty*)m_MapProp->Clone() );

	m_Node.SetNiObject( NiDynamicCast( NiGeometry, spTriShape ), CGeometry::Normal );

	NiTransform tr;
	tr.MakeIdentity();
	m_Node.m_pNiGeom->SetLocalTransform( tr );
	m_Node.m_pNiGeom->Update( 0.0f, false );

	m_Color = FONT_WHITE;
}

void CBillboard::CreateTexture( float size_x, float size_y, float fTexX1, float fTexX2, float fTexY1, float fTexY2, NiTexture* pTexture )
{
	assert_cs( m_Node.m_pNiGeom == NULL );

	m_fSizeX = size_x;
	m_fSizeY = size_y;

	unsigned int uiNumVerts = 4;
	NiPoint3* pkVerts = NiNew NiPoint3[uiNumVerts];
	NiPoint2* pkTex = NiNew NiPoint2[uiNumVerts];
	unsigned short usTriangles = 2;
	unsigned short* pusTriList = NiAlloc(unsigned short, 3 * usTriangles);

	pkVerts[ 0 ] = pkVerts[ 1 ] = pkVerts[ 2 ] = pkVerts[ 3 ] = NiPoint3::ZERO;

	pkTex[0] = NiPoint2(fTexX1, fTexY2);
	pkTex[1] = NiPoint2(fTexX2, fTexY2);
	pkTex[2] = NiPoint2(fTexX2, fTexY1);
	pkTex[3] = NiPoint2(fTexX1, fTexY1);

	pusTriList[0] = 0;
	pusTriList[1] = 1;
	pusTriList[2] = 2;

	pusTriList[3] = 0;
	pusTriList[4] = 2;
	pusTriList[5] = 3;

	NiTriShapePtr spTriShape = NiNew NiTriShape( uiNumVerts, pkVerts, NULL, NULL, pkTex, 1, NiGeometryData::NBT_METHOD_NONE, usTriangles, pusTriList );
	spTriShape->GetModelData()->SetConsistency(NiGeometryData::VOLATILE);
	
	NiTexturingProperty* pkTextProp = NiNew NiTexturingProperty;
	pkTextProp->SetBaseTexture(pTexture);
	pkTextProp->SetBaseFilterMode( NiTexturingProperty::FILTER_BILERP );
	pkTextProp->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );
	spTriShape->AttachProperty(pkTextProp);

	spTriShape->AttachProperty( (NiProperty*)m_AlphaProp->Clone() );
	spTriShape->AttachProperty( (NiProperty*)m_ZBufferProp->Clone() );
	spTriShape->AttachProperty( (NiProperty*)m_MapProp->Clone() );

	m_Node.SetNiObject( NiDynamicCast( NiGeometry, spTriShape ), CGeometry::Normal );

	m_Node.m_pNiGeom->Update( 0.0f, false );

	m_Color = FONT_WHITE;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void CBillboard::SetColor( NiColor color )
{
	if( m_Color != color )
	{
		m_Color = color;
		NiMaterialProperty* pMapProp = (NiMaterialProperty*)m_Node.m_pNiGeom->GetProperty( NiMaterialProperty::GetType() );
		assert_cs( pMapProp != NULL );
		pMapProp->SetEmittance( m_Color );		
		m_Node.m_pNiGeom->UpdateProperties();
	}	
}

void CBillboard::SetAlpha( float fAlpha )
{
	if( m_fAlpha == fAlpha )
		return;

	m_fAlpha = fAlpha;
	NiProperty* pProperty = m_Node.m_pNiGeom->GetProperty( NiMaterialProperty::GetType() );
	SAFE_POINTER_RET( pProperty );

	NiMaterialProperty* pMapProp = (NiMaterialProperty*)m_Node.m_pNiGeom->GetProperty( NiMaterialProperty::GetType() );	
	pMapProp->SetAlpha( fAlpha );
	m_Node.m_pNiGeom->UpdateProperties();
}

void CBillboard::Render( NiPoint3 vPos, float fX, float fY )
{
	assert_cs( m_Node.m_pNiGeom != NULL );

	const NiPoint3 kCamUp = CAMERA_ST.GetWorldUpVector();
	const NiPoint3 kCamRt = CAMERA_ST.GetWorldRightVector();
	NiPoint3* pkDestVerts = m_Node.m_pNiGeom->GetVertices();

	pkDestVerts[0].x = -m_fSizeX + fX;	pkDestVerts[0].z = -m_fSizeY + fY;
	pkDestVerts[1].x = m_fSizeX + fX;	pkDestVerts[1].z = -m_fSizeY + fY;
	pkDestVerts[2].x = m_fSizeX + fX;	pkDestVerts[2].z = m_fSizeY + fY;
	pkDestVerts[3].x = -m_fSizeX + fX;	pkDestVerts[3].z = m_fSizeY + fY;

	pkDestVerts[0] = pkDestVerts[0].x * kCamRt + pkDestVerts[0].z * kCamUp;
	pkDestVerts[1] = pkDestVerts[1].x * kCamRt + pkDestVerts[1].z * kCamUp;
	pkDestVerts[2] = pkDestVerts[2].x * kCamRt + pkDestVerts[2].z * kCamUp;
	pkDestVerts[3] = pkDestVerts[3].x * kCamRt + pkDestVerts[3].z * kCamUp;

	m_Node.m_pNiGeom->GetModelBound().ComputeFromData( 4, pkDestVerts );

	m_Node.m_pNiGeom->SetTranslate( vPos );
	m_Node.m_pNiGeom->Update( 0.0f, false );

	if( CAMERA_ST.UserCulling( m_Node.m_pNiGeom ) )
		m_Node.Render();
}

void CBillboard::Render( NiPoint3 vPos, float fX, float fY, float fAlpha )
{
	if( m_fAlpha != fAlpha )
	{
		m_fAlpha = fAlpha;
		NiMaterialProperty* pMapProp = (NiMaterialProperty*)m_Node.m_pNiGeom->GetProperty( NiMaterialProperty::GetType() );
		if( pMapProp != NULL )
			pMapProp->SetAlpha( fAlpha );
	}	

	Render( vPos, fX, fY );
}

void CBillboard::Render( NiPoint3 vPos, float fX, float fY, float fAlpha, float fScale )
{
	SetScale( fScale );
	Render( vPos, fX, fY, fAlpha );
}

void CBillboard::RenderAbsolute( NiPoint3 vPos, float fX, float fY )
{
	assert_cs( m_Node.m_pNiGeom != NULL );

	const NiPoint3 kCamUp = CAMERA_ST.GetWorldUpVector();
	const NiPoint3 kCamRt = CAMERA_ST.GetWorldRightVector();
	NiPoint3* pkDestVerts = m_Node.m_pNiGeom->GetVertices();

	pkDestVerts[0].x = -m_fSizeX + fX;	pkDestVerts[0].z = -m_fSizeY + fY;
	pkDestVerts[1].x = m_fSizeX + fX;	pkDestVerts[1].z = -m_fSizeY + fY;
	pkDestVerts[2].x = m_fSizeX + fX;	pkDestVerts[2].z = m_fSizeY + fY;
	pkDestVerts[3].x = -m_fSizeX + fX;	pkDestVerts[3].z = m_fSizeY + fY;

	pkDestVerts[0] = pkDestVerts[0].x * kCamRt + pkDestVerts[0].z * kCamUp;
	pkDestVerts[1] = pkDestVerts[1].x * kCamRt + pkDestVerts[1].z * kCamUp;
	pkDestVerts[2] = pkDestVerts[2].x * kCamRt + pkDestVerts[2].z * kCamUp;
	pkDestVerts[3] = pkDestVerts[3].x * kCamRt + pkDestVerts[3].z * kCamUp;

	m_Node.m_pNiGeom->GetModelBound().ComputeFromData( 4, pkDestVerts );

	m_Node.m_pNiGeom->SetTranslate( vPos );
	m_Node.m_pNiGeom->Update( 0.0f, false );

	if( CAMERA_ST.UserCulling( m_Node.m_pNiGeom ) )
		m_Node.RenderAbsolute();
}

void CBillboard::RenderAbsolute( NiPoint3 vPos, float fX, float fY, float fAlpha )
{
	SetAlpha( fAlpha );
	RenderAbsolute( vPos, fX, fY );
}

void CBillboard::RenderAbsolute( NiPoint3 vPos, float fX, float fY, float fAlpha, float fScale )
{
	SetScale( fScale );
	RenderAbsolute( vPos, fX, fY, fAlpha );
}

void CBillboard::SetSize( float x, float y )
{
	m_fSizeX = x;
	m_fSizeY = y;
}

void CBillboard::ChangeTexture( CsPoint ptSize, NiTexture* pTexture )
{	
	RESOURCEMGR_ST.RemoveTexture( m_pTexture );
	SetSize( (float)ptSize.x, (float)ptSize.y );
	NiProperty* pProperty = m_Node.m_pNiGeom->GetProperty( NiTexturingProperty::GetType() );
	SAFE_POINTER_RET( pProperty );
	
	((NiTexturingProperty*)pProperty)->SetBaseTexture( pTexture );
}

void CBillboard::ChangeTexture( const char* pFileName, CsPoint ptSize )
{
	SetSize( (float)ptSize.x, (float)ptSize.y );

	RESOURCEMGR_ST.RemoveTexture( m_pTexture );

	NiProperty* pProperty = m_Node.m_pNiGeom->GetProperty( NiTexturingProperty::GetType() );
	SAFE_POINTER_RET( pProperty );

	m_pTexture = RESOURCEMGR_ST.GetTexture( pFileName );
	((NiTexturingProperty*)pProperty)->SetBaseTexture( m_pTexture->GetTexture() );
}
