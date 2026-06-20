#include "stdafx.h"
#include "GeometryList.h"

THREAD_MEMPOOL_CPP( sNODE_SHADER_BLUREFFECT )
THREAD_MEMPOOL_CPP( CGeometryData )
THREAD_MEMPOOL_CPP( CGeomCharOutLine )
THREAD_MEMPOOL_CPP( CGeomMeshOutLine )
THREAD_MEMPOOL_CPP( CGeomCharOutLine_Gray )
THREAD_MEMPOOL_CPP( CGeomMeshOutLine_Gray )

void sNODE_SHADER_BLUREFFECT::Init( float fColorRate, float fStartAlpha, float fEndAlpha, int nMatCnt )
{
	s_fColorRate = fColorRate;
	s_fStartAlpha = fStartAlpha;
	s_fEndAlpha = fEndAlpha;

	s_pmatLastWorld = csnew D3DXMATRIX[ nMatCnt ];
	s_nMatrixCnt = nMatCnt;
	s_nCurInputIndex = 0;
	s_nSyncIndex = -1;	
}

void sNODE_SHADER_BLUREFFECT::PreRender( CGeometry* pGeom )
{
	SAFE_POINTER_RET( pGeom );
	SAFE_POINTER_RET( pGeom->m_pGeometry );

	if( 0 == pGeom->m_pGeometry->GetRefCount() )
		return;

	NiD3D10Utility::GetD3DFromNi( s_pmatLastWorld[ s_nCurInputIndex ], pGeom->m_pGeometry->GetWorldTransform() );

	++s_nCurInputIndex;
	if( s_nSyncIndex == -1 )
	{
		if( s_nCurInputIndex >= s_nMatrixCnt )
			s_nSyncIndex = 0;			
	}
	else
	{
		++s_nSyncIndex;
	}

	if( s_nCurInputIndex >= s_nMatrixCnt )
		s_nCurInputIndex = 0;

	if( s_nSyncIndex >= s_nMatrixCnt )
		s_nSyncIndex = 0;

	if( s_nSyncIndex != -1 )
	{
		NiD3DShaderFactory::UpdateGlobalShaderConstant( "LastWorld", sizeof( D3DXMATRIX ), &s_pmatLastWorld[ s_nSyncIndex ] );
	}
	else
	{
		NiD3DShaderFactory::UpdateGlobalShaderConstant( "LastWorld", sizeof( D3DXMATRIX ), &s_pmatLastWorld[ 0 ] );			
	}

	D3DXVECTOR4 vVal;
	vVal.x = s_fColorRate;
	vVal.y = s_fStartAlpha;
	vVal.z = s_fEndAlpha;
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "BlurEffect", sizeof( D3DXVECTOR4 ), &vVal );
}

void sNODE_SHADER_BLUREFFECT::PostRender( CGeometry* pGeom )
{
}











void CGeometry::Delete()
{
	m_nPlag = 0;
	m_pGeometry = NULL;
	m_fOutLineAlpha = 1.0f;
	m_nOrgNiBit = 0;
	m_StateType = None;

	DeleteShader();
}

void CGeometry::DeleteShader()
{
	std::list< sNODE_SHADER_BASE* >::iterator it = m_listShader.begin();
	std::list< sNODE_SHADER_BASE* >::iterator itEnd = m_listShader.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();

		switch( (*it)->GetType() )
		{
		case sNODE_SHADER_BASE::BLUR_EFFECT:
			sNODE_SHADER_BLUREFFECT::DeleteInstance( (sNODE_SHADER_BLUREFFECT*)(*it) );
			break;
		default:
			assert_cs( false );
		}
	}
	m_listShader.clear();
}

CGeometry::eGEOM_TYPE CGeometry::GetMeshOutLineType()
{
	return nsCsGBTerrain::g_Device.g_bUseMyShader ? CGeometry::MeshOutLine : CGeometry::Normal;
}

CGeometry::eGEOM_TYPE CGeometry::GetCharOutLineType()
{
	return nsCsGBTerrain::g_Device.g_bUseMyShader ? CGeometry::CharOutLine : CGeometry::Normal;
}

CGeometry::eGEOM_TYPE CGeometry::GetCharOutLineGrayType()
{
	return nsCsGBTerrain::g_Device.g_bUseMyShader ? CGeometry::CharOutLine_Gray : CGeometry::Normal;
}

CGeometry* CGeometry::GetInstance( NiGeometry* pGeomety, int nGeomType )
{
	CGeometry* pGeom = NULL;
	switch( nGeomType )
	{	
	case UseTR:
		pGeom = NiNew CGeomTR;
		break;
	case Normal:
		pGeom = CGeometryData::NewInstance();
		break;
	case CharOutLine:
		{
			if( ( pGeomety->GetSkinInstance() == NULL )||( pGeomety->GetSkinInstance()->GetSkinPartition() == NULL ) )
				return GetInstance( pGeomety, MeshOutLine );
			pGeom = CGeomCharOutLine::NewInstance();
		}		
		break;
	case MeshOutLine:
		pGeom = CGeomMeshOutLine::NewInstance();
		break;
	case CharOutLine_Gray:
		{
			if( ( pGeomety->GetSkinInstance() == NULL )||( pGeomety->GetSkinInstance()->GetSkinPartition() == NULL ) )
				return GetInstance( pGeomety, MeshOutLine_Gray );
			pGeom = CGeomCharOutLine_Gray::NewInstance();
		}
		break;
	case MeshOutLine_Gray:
		pGeom = CGeomMeshOutLine_Gray::NewInstance();
		break;
	default:
		assert_cs( false );
	}
	assert_cs( pGeom != NULL );
	assert_cs( pGeom->m_nPlag == 0 );

	return pGeom;
}

void CGeometry::DelInstance( CGeometry* pGeom )
{
	pGeom->Delete();
	switch( pGeom->GetType() )
	{
	case UseTR:				NiDelete (CGeomTR*)pGeom;												break;
	case Normal :			CGeometryData::DeleteInstance( (CGeometryData*)pGeom );					break;	
	case CharOutLine:		CGeomCharOutLine::DeleteInstance( (CGeomCharOutLine*)pGeom );			break;
	case MeshOutLine:		CGeomMeshOutLine::DeleteInstance( (CGeomMeshOutLine*)pGeom );			break;
	case CharOutLine_Gray:	CGeomCharOutLine_Gray::DeleteInstance( (CGeomCharOutLine_Gray*)pGeom );	break;
	case MeshOutLine_Gray:	CGeomMeshOutLine_Gray::DeleteInstance( (CGeomMeshOutLine_Gray*)pGeom );	break;
	default:				assert_cs( false );
	}
}

void CGeometry::SetGeometry( CsNiGeometry* pGeom )
{
	m_pGeometry = pGeom;
	ResetZBuffer();

	switch( GetType() )
	{
	case Normal:
	case UseTR:
		ResetAlpha();
		break;
	case CharOutLine:
	case CharOutLine_Gray:
	case MeshOutLine:
	case MeshOutLine_Gray:
		SetAlpha( m_fOutLineAlpha );
		ResetAlpha();
		break;
	default:
		assert_cs( false );
	}
}

float CGeomCharOutLine::g_fOldSetupCameraDist = FLT_MAX;
float g_fOldSetupAlpha = FLT_MAX;

void CGeomCharOutLine::Render()
{
	if( m_listShader.size() )
	{
		std::list< sNODE_SHADER_BASE* >::iterator it = m_listShader.begin();
		std::list< sNODE_SHADER_BASE* >::iterator itEnd = m_listShader.end();
		for( ; it!=itEnd; ++it )
		{
			(*it)->PreRender( this );
		}
	}
	

	if( ( ( g_fOldSetupCameraDist != m_fCameraDist )||( g_fOldSetupAlpha != m_fOutLineAlpha ) )
		&&( m_fCameraDist > 0.0f ) )
	{
		assert_cs( m_fOutLineAlpha >= 0.0f );
		assert_cs( m_fOutLineAlpha <= 1.0f );

		float val = m_fCameraDist*0.004f;
		float valMax = CsMax( 1.5f, val );
		float f = logf( CsMin( 4.7f, valMax ) );
		float afSize[4] = { f, m_fOutLineAlpha, 0.0f, 0.0f };
		NiD3DShaderFactory::UpdateGlobalShaderConstant( "OutLineSize", sizeof(afSize), &afSize );

		g_fOldSetupCameraDist = m_fCameraDist;
		g_fOldSetupAlpha = m_fOutLineAlpha;
	}

	CGeometryData::Render();
}



void CGeometry::ResetAlpha()
{
	DelPlag( CGeometry::GP_ALPHA );

	SAFE_POINTER_RET( m_pGeometry );
	if( 0 == m_pGeometry->GetRefCount() )
		return;

	NiAlphaProperty* pAlphaProp = (NiAlphaProperty*)m_pGeometry->GetProperty( NiProperty::ALPHA );
	if( pAlphaProp != NULL )
	{
		if( pAlphaProp->GetAlphaBlending() )
			AddPlag( CGeometry::GP_ALPHA );
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////


void CGeometryData::Render()
{
	SAFE_POINTER_RET( m_pGeometry );
	if( 0 == m_pGeometry->GetRefCount() )
		return;
	m_pGeometry->RenderImmediate( NiRenderer::GetRenderer() );
}


//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void CGeomTR::Render()
{
	SAFE_POINTER_RET( m_pGeometry );
	if( 0 == m_pGeometry->GetRefCount() )
		return;
	m_pGeometry->RenderImmediate( NiRenderer::GetRenderer() );
}


void CGeometry::SetAlpha( float fAlpha )
{
	SAFE_POINTER_RET( m_pGeometry );
	if( 0 == m_pGeometry->GetRefCount() )
		return;

	if( fAlpha < 1.0f ){		
		NiAlphaProperty* pAlphaProp = (NiAlphaProperty*)m_pGeometry->GetProperty( NiProperty::ALPHA );
		if( pAlphaProp != NULL )
		{
			if( pAlphaProp->GetAlphaBlending() )
				AddPlag( CGeometry::GP_ALPHA );				
		}

	}
	else if( m_fOutLineAlpha != fAlpha )
		ResetAlpha();

	m_fOutLineAlpha = fAlpha;
}
