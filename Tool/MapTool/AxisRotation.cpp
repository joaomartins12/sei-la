
#include "stdafx.h"
#include "AxisRotation.h"

cAxisRotation::cAxisRotation()
:m_bUsingOldRotation(true)
{
	m_pAxisNiNode = NULL;
	m_pAxisPlaneNiNode = NULL;
}


//==========================================================================================
//
//	Base Func 
//
//==========================================================================================

void cAxisRotation::Delete()
{
	m_pAxisNiNode = 0;
	m_pAxisPlaneNiNode = 0;	
}

void cAxisRotation::Init()
{
	assert_cs( m_pAxisNiNode == NULL );

	NiStream stream;
	bool bLoad = stream.Load( AXIS_ROTATION_OBJECT_PATH );
	if( bLoad == false )
	{
		MessageBox( NULL, L".\\ToolData\\axis.nif 가 존재 하지 않습니다. 비정상적으로 종료 될수 있습니다.", NULL, MB_OK );
		return;
	}
	m_pAxisNiNode = (NiNode*)stream.GetObjectAt( 0 );
	m_pAxisNiNode->UpdateProperties();
	stream.RemoveAllObjects();

	bLoad = stream.Load( AXIS_ROTATION_PLANE_OBJECT_PATH );
	if( bLoad == false )
	{
		MessageBox( NULL, L".\\ToolData\\axisPlane.nif 가 존재 하지 않습니다. 비정상적으로 종료 될수 있습니다.", NULL, MB_OK );
		return;
	}
	m_pAxisPlaneNiNode = (NiNode*)stream.GetObjectAt( 0 );
	m_pAxisPlaneNiNode->UpdateProperties();

	m_bRender = false;
}

void cAxisRotation::Update()
{
	if( m_bRender == false )
		return;

	NiPoint3 ptCameraWorld = CAMERA_ST.GetWorldLocation();
	float fDist = NiPoint3::VectorLength( ptCameraWorld - m_pAxisNiNode->GetWorldTranslate() );
	float fScale = sqrt( fDist*0.002f );
	
	m_pAxisNiNode->SetScale( fScale );
	m_pAxisNiNode->SetTranslate( m_ptPosition );
	m_pAxisNiNode->Update( 0.0f );
	m_pAxisPlaneNiNode->SetScale( fScale );
	m_pAxisPlaneNiNode->SetTranslate( m_ptPosition + CAMERA_ST.GetWorldDirection()*10.0f*fScale );	
	m_pAxisPlaneNiNode->Update( 0.0f );		
}

void cAxisRotation::Render()
{
	if( m_bRender == false )
		return;

	if(m_bUsingOldRotation == true)
	{
		NiDrawScene( CAMERA_ST.GetCameraObj(), m_pAxisPlaneNiNode, *g_pCuller );
		NiDrawScene( CAMERA_ST.GetCameraObj(), m_pAxisNiNode, *g_pCuller );
	}
}

//==========================================================================================
//
//	Tool NiNode
//
//==========================================================================================

void cAxisRotation::_SetDefaultEmittance()
{
	NiMaterialProperty* pMaterialProp;
	pMaterialProp = (NiMaterialProperty*)m_pAxisNiNode->GetAt( 3 )->GetProperty( NiMaterialProperty::GetType() );
	pMaterialProp->SetEmittance( NiColor( 0.0f, 1.0f, 0.0f ) );
	pMaterialProp = (NiMaterialProperty*)m_pAxisNiNode->GetAt( 4 )->GetProperty( NiMaterialProperty::GetType() );
	pMaterialProp->SetEmittance( NiColor( 1.0f, 0.0f, 0.0f ) );
	pMaterialProp = (NiMaterialProperty*)m_pAxisNiNode->GetAt( 5 )->GetProperty( NiMaterialProperty::GetType() );
	pMaterialProp->SetEmittance( NiColor( 0.0f, 0.0f, 1.0f ) );
}

void cAxisRotation::_SetEnumAxisRotation( NiColor emittance )
{
	if( emittance == NiColor( 0.0f, 1.0f, 0.0f ) )
	{
		m_eAR = AR_Z;
	}
	else if( emittance == NiColor( 1.0f, 0.0f, 0.0f ) )
	{
		m_eAR = AR_Y;
	}
	else if( emittance == NiColor( 0.0f, 0.0f, 1.0f ) )
	{
		m_eAR = AR_X;
	}
	else
	{
		assert_cs( false )
	}
}

NiAVObject* cAxisRotation::_PickAxisToRenderAxis( NiAVObject* pPickAxis )
{
	for( int i=0; i<=2; ++i )
	{
		if( m_pAxisNiNode->GetAt( i ) == pPickAxis )
			return m_pAxisNiNode->GetAt( i + 3 );
	}
	assert_cs( false );
	return NULL;
}

void cAxisRotation::_PickAxis( CPoint pt )
{
	_SetDefaultEmittance();

	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );
	CsGBPick pick;

	if( pick.Pick_IsBound( m_pAxisNiNode, origin, dir ) == false )
		return;		

	for( int i=0; i<=2; ++i )
	{
		pick.Pick( m_pAxisNiNode->GetAt( i ), origin, dir, true );
	}
	pick.Pick( m_pAxisPlaneNiNode, origin, dir, true );

	NiPick::Record* pRecord = pick.GetRecord();
	if( pRecord != NULL )
	{
		if( nsCSGBFUNC::GetRootNiNode( pRecord->GetAVObject() ) == m_pAxisPlaneNiNode )
			return;

		NiAVObject* pObject = _PickAxisToRenderAxis( pRecord->GetAVObject() );
		NiMaterialProperty* pMaterialProp = (NiMaterialProperty*)pObject->GetProperty( NiMaterialProperty::GetType() );
		pMaterialProp->SetEmittance( NiColor( 0.8f, 0.8f, 0.0f ) );
	}
}

void cAxisRotation::_RotAxis( CPoint pt )
{
	if( m_ptLButtonDown == pt )
		return;

	CPoint ptDelta;
	ptDelta.x = pt.x - m_ptLButtonDown.x;
	ptDelta.y = m_ptLButtonDown.y - pt.y;
	m_ptLButtonDown = pt;

	float fRotSpeed = 400.0f;	
	switch( m_eAR )
	{		
	case AR_X:
		{
			NiPoint3 ptCameraCross = CAMERA_ST.GetWorldRightVector();
			float dotx = NiPoint3( 0.0f, 1.0f, 0.0f ).Dot( ptCameraCross );			
			float doty = NiPoint3( 1.0f, 0.0f, 0.0f ).Dot( ptCameraCross );
			m_matRotation.MakeXRotation( (dotx*ptDelta.x + doty*ptDelta.y )/fRotSpeed*NI_PI );
		}
		break;		
	case AR_Y:
		{
			NiPoint3 ptCameraCross = CAMERA_ST.GetWorldRightVector();
			float dotx = NiPoint3( 1.0f, 0.0f, 0.0f ).Dot( ptCameraCross );
			float doty = NiPoint3( 0.0f, 1.0f, 0.0f ).Dot( ptCameraCross );
			m_matRotation.MakeYRotation( ( -dotx*ptDelta.x + doty*ptDelta.y )/fRotSpeed*NI_PI );
		}
		break;
	case AR_Z:
		{
			NiPoint3 ptCameraDir = CAMERA_ST.GetWorldDirection();
			float doty = NiPoint3( 0.0f, 0.0f, 1.0f ).Dot( ptCameraDir );			
			m_matRotation.MakeZRotation( ( -1.0f*ptDelta.x + doty*ptDelta.y )/fRotSpeed*NI_PI );
		}
		break;
	}

	if(m_bUsingOldRotation == true)
	{
		_TRApplyDestNiNode();	
	}
}

void cAxisRotation::ShowOnlyZAxis()
{
	m_pAxisNiNode->GetAt( 0 )->SetAppCulled( false );
	m_pAxisNiNode->GetAt( 1 )->SetAppCulled( true );
	m_pAxisNiNode->GetAt( 2 )->SetAppCulled( true );
	m_pAxisNiNode->GetAt( 3 )->SetAppCulled( false );
	m_pAxisNiNode->GetAt( 4 )->SetAppCulled( true );
	m_pAxisNiNode->GetAt( 5 )->SetAppCulled( true );
}

void cAxisRotation::ShowAllAxis()
{
	m_pAxisNiNode->GetAt( 0 )->SetAppCulled( false );
	m_pAxisNiNode->GetAt( 1 )->SetAppCulled( false );
	m_pAxisNiNode->GetAt( 2 )->SetAppCulled( false );
	m_pAxisNiNode->GetAt( 3 )->SetAppCulled( false );
	m_pAxisNiNode->GetAt( 4 )->SetAppCulled( false );
	m_pAxisNiNode->GetAt( 5 )->SetAppCulled( false );
}

//==========================================================================================
//
//	Dest Node 
//
//==========================================================================================

NiTransform cAxisRotation::InsertVisible( NiNode* pDestNiNode, eAXIS_TYPE at, cObjectModel* pObjectModel/* = NULL*/ )
{
	assert_cs( m_pAxisNiNode != NULL );	

	m_listDestNiNode.push_back( pDestNiNode );
	
	m_bRender = true;
	m_bClickAxis = false;
	m_eAT = at;

	CalTRNodeTransform();
	
	NiTransform tr;
	if( m_listDestNiNode.size() == 1 )
	{
		NiNode* pNode = *m_listDestNiNode.begin();
		tr = pNode->GetWorldTransform();
	}
	else
	{
		tr.MakeIdentity();
		tr.m_Translate = m_ptPosition;
		tr.m_Rotate = m_matRotation;
	}

	if( IS_ENABLEFRM( nsMAPTOOL::MV_OBJECT_CONTROL ) == true )
	{		
		GET_MV_OBJECTCONTROL_DLG->_InsertValue( at, tr );
		GET_MV_OBJECTCONTROL_DLG->_InsertValue_Act( pObjectModel );
	}

	Update();

	return tr;
}

NiTransform cAxisRotation::RemoveVisible( NiNode* pDestNiNode )
{
	assert_cs( m_pAxisNiNode != NULL );	

	LIST_DEST_NINODE_IT it = m_listDestNiNode.begin();
	for( ; it!=m_listDestNiNode.end(); ++it )
	{
		if( (*it) == pDestNiNode )
		{
			m_listDestNiNode.erase( it );
			break;
		}
	}

	if( m_listDestNiNode.empty() == false )
	{
		m_bRender = true;
		m_bClickAxis = false;

		CalTRNodeTransform();

		if( IS_ENABLEFRM( nsMAPTOOL::MV_OBJECT_CONTROL ) == true )
		{
			NiTransform tr;
			tr.MakeIdentity();
			tr.m_Rotate = m_matRotation;
			tr.m_Translate = m_ptPosition;
			GET_MV_OBJECTCONTROL_DLG->_ResetValue( tr );
		}

		Update();
	}
	else
	{
		ReleaseVisible();
	}

	NiTransform tr;
	tr.MakeIdentity();
	tr.m_Rotate = m_matRotation;
	tr.m_Translate = m_ptPosition;

	return tr;
}

void cAxisRotation::ReleaseVisible()
{
	m_listDestNiNode.clear();

	m_bRender = false;
	m_bClickAxis = false;

	if( IS_ENABLEFRM( nsMAPTOOL::MV_OBJECT_CONTROL ) == true )
	{
		GET_MV_OBJECTCONTROL_DLG->_ReleaseValue();
	}
}

void cAxisRotation::_TRApplyDestNiNode()
{
	NiTransform tr;
	tr.MakeIdentity();
	tr.m_Rotate = m_matRotation;
	NiTransform trDest, trResult;

	LIST_DEST_NINODE_IT it = m_listDestNiNode.begin();
	for( ; it!=m_listDestNiNode.end(); ++it )
	{
		trDest = (*it)->GetWorldTransform();		
		trDest.m_Translate -= m_ptPosition;
		trResult = tr*trDest;
		trResult.m_Translate += m_ptPosition;
		(*it)->SetLocalTransform( trResult );
	}

	tr.m_Translate = m_ptPosition;

	if( m_eAT == AT_CELL )
	{
		GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_SetSelectObject_Transform( trResult );
	}

	if( m_listDestNiNode.size() == 1 )
		GET_MV_OBJECTCONTROL_DLG->_ResetValue( trResult );

	else
		GET_MV_OBJECTCONTROL_DLG->_ResetValue( trResult );
}

//==========================================================================================
//
//	Transform Node
//
//==========================================================================================

void cAxisRotation::CalTRNodeTransform()
{
	NiPoint3 ptMin = NiPoint3( FLT_MAX, FLT_MAX, FLT_MAX );
	NiPoint3 ptMax = NiPoint3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	NiPoint3 pos;
	LIST_DEST_NINODE_IT it = m_listDestNiNode.begin();
	for( ; it!=m_listDestNiNode.end(); ++it )
	{
		pos = (*it)->GetWorldTranslate();
		if( ptMin.x > pos.x )	ptMin.x = pos.x;
		if( ptMin.y > pos.y )	ptMin.y = pos.y;
		if( ptMin.z > pos.z )	ptMin.z = pos.z;
		if( ptMax.x < pos.x )	ptMax.x = pos.x;
		if( ptMax.y < pos.y )	ptMax.y = pos.y;
		if( ptMax.z < pos.z )	ptMax.z = pos.z;		
	}

	m_ptPosition = ( ptMax + ptMin )*0.5f;
	m_ptPosition.z = ptMin.z;
}

//==========================================================================================
//
//	Mouse
//
//==========================================================================================

bool cAxisRotation::LButtonDown( CPoint pt )
{
	if( m_bRender == false )
		return false;

	_SetDefaultEmittance();

	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );

	CsGBPick pick;
	
	if( pick.Pick_IsBound( m_pAxisNiNode, origin, dir ) == true )
	{
		for( int i=0; i<=2; ++i )
		{
			pick.Pick( m_pAxisNiNode->GetAt( i ), origin, dir, true );
		}
		pick.Pick( m_pAxisPlaneNiNode, origin, dir, true );

		NiPick::Record* pRecord = pick.GetRecord();
		if( pRecord != NULL )
		{
			if( nsCSGBFUNC::GetRootNiNode( pRecord->GetAVObject() ) != m_pAxisPlaneNiNode )
			{
				NiAVObject* pObject = _PickAxisToRenderAxis( pRecord->GetAVObject() );
				NiMaterialProperty* pMaterialProp = (NiMaterialProperty*)pObject->GetProperty( NiMaterialProperty::GetType() );
				_SetEnumAxisRotation( pMaterialProp->GetEmittance() );
				pMaterialProp->SetEmittance( NiColor( 1.0f, 1.0f, 0.0f ) );
				m_bClickAxis = true;
				m_ptLButtonDown = pt;
				return true;
			}			
		}
	}

	m_bClickAxis = false;
	return false;
}

void cAxisRotation::LButtonUp( CPoint pt )
{
	m_bClickAxis = false;
}

void cAxisRotation::MouseMove( CPoint pt )
{
	if( m_bRender == false )
		return;

	if( m_bClickAxis == false )
	{
		_PickAxis( pt );
	}
	else
	{
		_RotAxis( pt );
	}
}
