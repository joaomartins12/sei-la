
#include "stdafx.h"
#include "CsGBCell.h"

NiMatrix3 CsGBCell::m_ConstantMatrix;

CsGBCell::CsGBCell():m_ptCenter(NiPoint3::ZERO),m_fRadius(0.0f)
{

}

CsGBCell::~CsGBCell()
{

}

void CsGBCell::Delete()
{
	int nSizeArray = m_vpObject.Size();
	for( int i=0; i<nSizeArray; ++i )
	{
		int nSizeObject = m_vpObject[ i ]->s_vpElement.Size();
		for( int so=0; so<nSizeObject; ++so )
		{
			NiDelete m_vpObject[ i ]->s_vpElement[ so ];
		}
		m_vpObject[ i ]->s_vpElement.Destroy();
		NiDelete m_vpObject[ i ];
	}
	m_vpObject.Destroy();
}

void CsGBCell::Init( NiPoint3 ptCenter, float fRadius )
{
	m_ptCenter = ptCenter;
	m_fRadius = fRadius;

	m_vpObject.Init( MAX_SCO_KIND, AP_ADD );
}

void CsGBCell::Update( float fElapsedTime )
{
	CsGBSpeedCell* pSpeedCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell();
	CsGBSpeedCell::sOBJECT* pBaseObject;
	int nKind = m_vpObject.Size();
	for( int k=0; k<nKind; ++k )
	{
		pBaseObject = pSpeedCell->GetBaseObject( k );
		if( pBaseObject->s_pTransformController != NULL )
		{
			sOBJECT_ARRAY* pArray = m_vpObject[ k ];
			sOBJECT* pObject;
			int nSize = pArray->s_vpElement.Size();
			for( int o=0; o<nSize; ++o )
			{
				pObject = pArray->s_vpElement[ o ];
				pObject->s_fAniTime += fElapsedTime*pObject->s_fAniSpeed;

				if( ( pObject->s_fRemainCheckTime -= fElapsedTime ) < 0.0f )
				{
					pObject->s_fRemainCheckTime = pBaseObject->s_ObjectInfo.CalRemainCheckTime();
					pObject->s_fAniSpeed = pBaseObject->s_ObjectInfo.CalAniSpeed();
				}
			}			
		}
	}
}

bool CsGBCell::Culling( float fViewerHeight )
{
	if( m_vpObject.Size() == 0 )
		return false;

	m_ptCenter.z = fViewerHeight;

	NiFrustumPlanes* pFrustumPlane = CAMERA_ST.GetFrustumPlane();
	// 탑과 바탐은 검사하지 않겠다
	for( int i=0; i<NiFrustumPlanes::MAX_PLANES; ++ i )
	{
		if( pFrustumPlane->GetPlane( i ).Distance( m_ptCenter ) <= -m_fRadius )
			return false;
	}
	return true;
}

void CsGBCell::Render( int nBaseIndex, bool& bFirst )
{
	if( m_vpObject.IsExistElement( nBaseIndex ) == false )
		return;

	sOBJECT_ARRAY* pArray = m_vpObject[ nBaseIndex ];
	int nSize = pArray->s_vpElement.Size();

	if( nSize == 0 )
		return;

	CsGBSpeedCell::sOBJECT* pObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( nBaseIndex );
	NiGeometry* pkGeometry = pObject->s_pGeometry;	
	NiQuatTransform trQuat;
	NiTransform trResult;

	NiInterpolator* pInterPolator = NULL;
	if( pObject->s_pTransformController != NULL )
	{
		pInterPolator = pObject->s_pTransformController->GetInterpolator();
	}

	int nStartIndex = 0;	
	if( bFirst == true )
	{
		nStartIndex = 1;
		bFirst = false;
				
		if( pInterPolator != NULL )
		{			
			float fScaledTime = pObject->s_pTransformController->ComputeScaledTime( pArray->s_vpElement[ 0 ]->s_fAniTime );			
			if( pInterPolator->Update( fScaledTime, NULL, trQuat ) == true )
			{
				assert_cs( NiIsKindOf( NiNode, pObject->s_pTransformController->GetTarget() ) == true );
				assert_cs( pkGeometry->GetParent() == pObject->s_pTransformController->GetTarget() );
				
				NiTransform trNode = ( (NiNode*)pObject->s_pTransformController->GetTarget() )->GetLocalTransform();
				trQuat.GetRotate().ToRotation( trNode.m_Rotate );
				trResult = pArray->s_vpElement[ 0 ]->s_Info.s_trParent*trNode;
				trResult = trResult*pkGeometry->GetLocalTransform();
			}
		}
		else
		{
			trResult = pArray->s_vpElement[ 0 ]->s_Info.s_trObject;
		}
		pObject->Render( trResult );
	}

	NiShader* pkShader = NiDynamicCast( NiD3DShaderInterface, pkGeometry->GetShaderFromMaterial() );
	NiGeometryData* pkData = pObject->s_pModelData;
	NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pObject->s_pModelData->GetRendererData();
	NiSkinInstance* pkSkinInstance = pObject->s_pSkinInstance;
	NiPropertyState* pkCurrProp = nsCsGBTerrain::g_pRenderer->GetPropertyState();
	NiDynamicEffectState* pkCurrEffects = nsCsGBTerrain::g_pRenderer->GetEffectState();
	NiBound* pkWorldBound = pObject->s_pBound;

	unsigned int uiRemainingPasses = pkShader->FirstPass();

	for( int i=nStartIndex; i<nSize; ++i )
	{		
		if( pInterPolator != NULL )
		{			
			float fScaledTime = pObject->s_pTransformController->ComputeScaledTime( pArray->s_vpElement[ i ]->s_fAniTime );			
			if( pInterPolator->Update( fScaledTime, NULL, trQuat ) == true )
			{
				assert_cs( NiIsKindOf( NiNode, pObject->s_pTransformController->GetTarget() ) == true );
				assert_cs( pkGeometry->GetParent() == pObject->s_pTransformController->GetTarget() );

				NiTransform trNode = ( (NiNode*)pObject->s_pTransformController->GetTarget() )->GetLocalTransform();
				trQuat.GetRotate().ToRotation( trNode.m_Rotate );
				trResult = pArray->s_vpElement[ i ]->s_Info.s_trParent*trNode;
				trResult = trResult*pkGeometry->GetLocalTransform();
			}
		}
		else
		{
			trResult = pArray->s_vpElement[ i ]->s_Info.s_trObject;
		}
		
		assert_cs( uiRemainingPasses == 1 );
		pkShader->SetupTransformations(pkGeometry, pkSkinInstance, NULL, pkBuffData, pkCurrProp, pkCurrEffects, trResult, *pkWorldBound);
		pkShader->SetupShaderPrograms(pkGeometry, pkSkinInstance, NULL,	pkBuffData, pkCurrProp, pkCurrEffects, trResult, *pkWorldBound);

		nsCsGBTerrain::g_pRenderer->GetRenderState()->CommitShaderConstants();

		if (pkBuffData->GetIB())
		{
			unsigned int uiStartIndex = 0;
			for (unsigned int i = 0; i < pkBuffData->GetNumArrays(); i++)
			{
				unsigned int uiPrimitiveCount;
				const unsigned short* pusArrayLengths = 
					pkBuffData->GetArrayLengths();
				if (pusArrayLengths)
				{
					// Assumes multiple arrays mean strips
					uiPrimitiveCount = pusArrayLengths[i] - 2;
				}
				else
				{
					NIASSERT(pkBuffData->GetNumArrays() == 1);
					uiPrimitiveCount = pkBuffData->GetTriCount();
				}

				nsCsGBTerrain::g_pRenderer->GetD3DDevice()->DrawIndexedPrimitive(pkBuffData->GetType(), 
					pkBuffData->GetBaseVertexIndex(), 0, 
					pkBuffData->GetVertCount(), uiStartIndex, 
					uiPrimitiveCount);

				uiStartIndex += uiPrimitiveCount + 2;
			}
		}
		else
		{
			nsCsGBTerrain::g_pRenderer->GetD3DDevice()->DrawPrimitive(pkBuffData->GetType(), 
				pkBuffData->GetBaseVertexIndex(), pkBuffData->GetTriCount());
		}
	}
}

CsGBCell::sINFO* CsGBCell::Resist( int nBaseIndex, NiTransform* trParent, NiTransform* trObject, bool bAnimation )
{
	assert_cs( nBaseIndex < MAX_SCO_KIND );

	while( m_vpObject.Size() <= nBaseIndex )
	{
		sOBJECT_ARRAY* oa = NiNew sOBJECT_ARRAY;
		oa->s_vpElement.Init( 2, AP_DOUBLE );
		m_vpObject.PushBack( oa );
	}

	sOBJECT* pNewObject = NiNew sOBJECT;
	memcpy( &pNewObject->s_Info.s_trObject, trObject, sizeof( NiTransform ) );
	memcpy( &pNewObject->s_Info.s_trParent, trParent, sizeof( NiTransform ) );

	if( bAnimation == true )
	{
		CsGBSpeedCell::sOBJECT* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( nBaseIndex );
		pNewObject->s_fAniTime = rand()%100 * 0.02f;
		pNewObject->s_fAniSpeed = pBaseObject->s_ObjectInfo.CalAniSpeed();
		pNewObject->s_fRemainCheckTime = pBaseObject->s_ObjectInfo.CalRemainCheckTime();
	}
	
	m_vpObject[ nBaseIndex ]->s_vpElement.PushBack( pNewObject );

	return &pNewObject->s_Info;
}

void CsGBCell::DeleteBaseObject( int nBaseIndex )
{
	if( m_vpObject.IsExistElement( nBaseIndex ) == false )
		return;

	int nSizeObject = m_vpObject[ nBaseIndex ]->s_vpElement.Size();
	for( int so=0; so<nSizeObject; ++so )
	{
		NiDelete m_vpObject[ nBaseIndex ]->s_vpElement[ so ];
	}
	m_vpObject[ nBaseIndex ]->s_vpElement.Destroy();
	NiDelete m_vpObject[ nBaseIndex ];

	m_vpObject.DeleteElement( nBaseIndex );
}

void CsGBCell::DeleteObject( int nBaseIndex, NiTransform* pTrParent )
{
	int nSize = m_vpObject[ nBaseIndex ]->s_vpElement.Size();

	for( int i=0; i<nSize; ++i )
	{
		NiPoint3 pt = m_vpObject[ nBaseIndex ]->s_vpElement[ i ]->s_Info.s_trParent.m_Translate - pTrParent->m_Translate;
		if( pt.Length() < 0.001f )
		{
			DeleteObject( nBaseIndex, i );
			return;
		}
	}
	assert_cs( false );
}

void CsGBCell::DeleteObject( int nBaseIndex, int nObjectIndex )
{
	int nSize = m_vpObject[ nBaseIndex ]->s_vpElement.Size();

	assert_cs( m_vpObject[ nBaseIndex ]->s_vpElement.IsExistElement( nObjectIndex ) == true );
	NiDelete m_vpObject[ nBaseIndex ]->s_vpElement[ nObjectIndex ];

	for( int i=nObjectIndex+1; i<nSize; ++i )
	{
		m_vpObject[ nBaseIndex ]->s_vpElement[ i-1 ] = m_vpObject[ nBaseIndex ]->s_vpElement[ i ];
	}
	m_vpObject[ nBaseIndex ]->s_vpElement.SetElementCount( nSize - 1 );
}

UINT CsGBCell::GetExtraDataSize()
{
	int nObjectKind = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObjectKind();
	UINT nReturn = nObjectKind*sizeof( int );
	for( int i=0; i<nObjectKind; ++i )
	{
		if( m_vpObject.IsExistElement( i ) == true )
		{
			nReturn += m_vpObject[ i ]->s_vpElement.Size()*sizeof( sINFO );
		}		
	}
	return nReturn;
}

void CsGBCell::SaveExtraData( char* pData, UINT& nOffset )
{
	int nObjectKind = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObjectKind();
	int nObjectCount;
	for( int i=0; i<nObjectKind; ++i )
	{
		if( m_vpObject.IsExistElement( i ) == true )
		{
			nObjectCount = m_vpObject[ i ]->s_vpElement.Size();
			memcpy( &pData[ nOffset ], &nObjectCount, sizeof( int ) );
			nOffset += sizeof( int );
			for( int nObjCnt=0; nObjCnt<nObjectCount; ++nObjCnt )
			{
				memcpy( &pData[ nOffset ], m_vpObject[ i ]->s_vpElement[ nObjCnt ], sizeof( sINFO ) );
				nOffset += sizeof( sINFO );
			}
		}
		else
		{
			nObjectCount = 0;
			memcpy( &pData[ nOffset ], &nObjectCount, sizeof( int ) );
			nOffset += sizeof( int );
		}		
	}
}

void CsGBCell::LoadExtraData( char* pData, UINT& nOffset )
{
	CsGBSpeedCell* pSpeedCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell();
	int nObjectKind = pSpeedCell->GetBaseObjectKind();	
	int nObjectCount;
	for( int i=0; i<nObjectKind; ++i )
	{
		bool bAnimation = ( pSpeedCell->GetBaseObject( i )->s_pTransformController != NULL );

		memcpy( &nObjectCount, &pData[ nOffset ], sizeof( int ) );
		nOffset += sizeof( int );

		for( int nObjCnt=0; nObjCnt<nObjectCount; ++nObjCnt )
		{
			sINFO info;
			memcpy( &info, &pData[ nOffset ], sizeof( sINFO ) );
			nOffset += sizeof( sINFO );

			Resist( i, &info.s_trParent, &info.s_trObject, bAnimation );
		}
	}
}