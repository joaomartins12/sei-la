
#include "stdafx.h"
#include "CsGBFunc.h"

//THREAD_MEMPOOL_CPP( nsCSGBFUNC::CsMaterialProp )

void nsCSGBFUNC::CsProperty::DeleteProperty( CsProperty* pProp )
{
	pProp->Delete();
	switch( pProp->s_eType )
	{
	case nsCSGBFUNC::CsProperty::PT_MATERIAL:
		//nsCSGBFUNC::CsMaterialProp::DeleteInstance( (nsCSGBFUNC::CsMaterialProp*)pProp );
		NiDelete (nsCSGBFUNC::CsMaterialProp*)pProp;
		break;
	default:
		assert_cs( false );
	}
}


void nsCSGBFUNC::CsBackupProperty::Delete()
{
	std::list< CsProperty* >::iterator it = s_listProperty.begin();
	std::list< CsProperty* >::iterator itEnd = s_listProperty.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		CsProperty::DeleteProperty( *it );
	}
	s_listProperty.clear();
}

nsCSGBFUNC::CsProperty* nsCSGBFUNC::CsBackupProperty::PopProperty( NiAVObject* pApplyNode )
{
	std::list< CsProperty* >::iterator it = s_listProperty.begin();
	std::list< CsProperty* >::iterator itEnd = s_listProperty.end();
	for( ; it!=itEnd; ++it )
	{
		if( pApplyNode == ( *it )->s_pApplyNode )
		{
			CsProperty* pProp = *it;
			s_listProperty.erase( it );
			return pProp;
		}
	}
	return NULL;
}

void nsCSGBFUNC::InsertList_Geom( CsVectorPB< CGeometry* >* vpGeom, NiAVObject* pAvObject, int nGeomType )
{
	if( ( pAvObject == NULL )||( pAvObject->GetAppCulled() == true ) )
		return;

	if( NiIsKindOf( NiNode, pAvObject ) == true )
	{
		NiNode* pNode = (NiNode*)pAvObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			InsertList_Geom( vpGeom, pNode->GetAt( i ), nGeomType );
		}		
	}
	else if( NiIsKindOf( NiGeometry, pAvObject ) == true )
	{
		NiGeometry* pGeometry =	(NiGeometry*)pAvObject;
		int nGeomTypeResult = nGeomType;
		if( pGeometry->GetBit( NiAVObject::IS_DEFAULT_SHADER ) == false )
		{
			switch( nGeomType )
			{
			case CGeometry::CharOutLine:
				{
					NiSkinInstance* pSkin = pGeometry->GetSkinInstance();
					if( ( pSkin == NULL ) || ( pSkin->GetSkinPartition() == NULL ) )
					{
						nGeomTypeResult = CGeometry::MeshOutLine;
					}
				}
				break;
			case CGeometry::CharOutLine_Gray:
				{
					NiSkinInstance* pSkin = pGeometry->GetSkinInstance();
					if( ( pSkin == NULL ) || ( pSkin->GetSkinPartition() == NULL ) )
					{
						nGeomTypeResult = CGeometry::MeshOutLine_Gray;
					}
				}
				break;
			case CGeometry::MeshOutLine:
			case CGeometry::MeshOutLine_Gray:
			case CGeometry::Normal:
				break;
			}
		}
		else
		{
			nGeomTypeResult = CGeometry::Normal;
		}


		CGeometry* pGeom = CGeometry::GetInstance( (CsNiGeometry*)pAvObject, nGeomTypeResult );
		pGeom->SetGeometry( (CsNiGeometry*)pAvObject );
		vpGeom->PushBack( pGeom );
	}
}


void nsCSGBFUNC::InsertList_Geom_IncludeHide( CsVectorPB< CGeometry* >* vpGeom, NiAVObject* pAvObject, int nGeomType )
{
	if( pAvObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pAvObject ) == true )
	{
		NiNode* pNode = (NiNode*)pAvObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			InsertList_Geom_IncludeHide( vpGeom, pNode->GetAt( i ), nGeomType );
		}		
	}
	else if( NiIsKindOf( NiGeometry, pAvObject ) == true )
	{
		NiGeometry* pGeometry =	(NiGeometry*)pAvObject;
		int nGeomTypeResult = nGeomType;
		if( pGeometry->GetBit( NiAVObject::IS_DEFAULT_SHADER ) == false )
		{
			switch( nGeomType )
			{
			case CGeometry::CharOutLine:
				{
					NiSkinInstance* pSkin = pGeometry->GetSkinInstance();
					if( ( pSkin == NULL ) || ( pSkin->GetSkinPartition() == NULL ) )
					{
						nGeomTypeResult = CGeometry::MeshOutLine;
					}
				}
				break;
			case CGeometry::CharOutLine_Gray:
				{
					NiSkinInstance* pSkin = pGeometry->GetSkinInstance();
					if( ( pSkin == NULL ) || ( pSkin->GetSkinPartition() == NULL ) )
					{
						nGeomTypeResult = CGeometry::MeshOutLine_Gray;
					}
				}
				break;
			case CGeometry::MeshOutLine:
			case CGeometry::MeshOutLine_Gray:
			case CGeometry::Normal:
				break;
			}
		}
		else
		{
			nGeomTypeResult = CGeometry::Normal;
		}


		CGeometry* pGeom = CGeometry::GetInstance( (CsNiGeometry*)pAvObject, nGeomTypeResult );
		pGeom->SetGeometry( (CsNiGeometry*)pAvObject );
		vpGeom->PushBack( pGeom );
	}
}

void nsCSGBFUNC::InsertList_Geom_Sorting( CsVectorPB< CGeometry* >* vpGeom, NiAVObject* pAvObject, int nGeomType )
{
	CsVectorPB< CGeometry* > vpSortGeom;
	_InsertList_Geom_Sorting( vpGeom, &vpSortGeom, pAvObject, nGeomType );

	int nCount = vpSortGeom.Size();
	if( nCount != 0 )
	{
		for( int i=0; i<nCount; ++i )
		{
			vpGeom->PushBack( vpSortGeom[ i ] );
		}
		vpSortGeom.Destroy();
	}
}

void nsCSGBFUNC::_InsertList_Geom_Sorting( CsVectorPB< CGeometry* >* vpGeom, CsVectorPB< CGeometry* >* vpSortGeom, NiAVObject* pAvObject, int nGeomType )
{
	if( ( pAvObject == NULL )||( pAvObject->GetAppCulled() == true ) )
		return;

	if( NiIsKindOf( NiNode, pAvObject ) == true )
	{
		NiNode* pNode = (NiNode*)pAvObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			_InsertList_Geom_Sorting( vpGeom, vpSortGeom, pNode->GetAt( i ), nGeomType );
		}		
	}
	else if( NiIsKindOf( NiGeometry, pAvObject ) == true )
	{
		NiGeometry* pGeometry =	(NiGeometry*)pAvObject;

		int nGeomTypeResult = nGeomType;
		if( pGeometry->GetBit( NiAVObject::IS_DEFAULT_SHADER ) == false )
		{
			switch( nGeomType )
			{
			case CGeometry::CharOutLine:
				{
					NiSkinInstance* pSkin = pGeometry->GetSkinInstance();
					if( ( pSkin == NULL ) || ( pSkin->GetSkinPartition() == NULL ) )
					{
						nGeomTypeResult = CGeometry::MeshOutLine;
					}
				}
				break;
			case CGeometry::CharOutLine_Gray:
				{
					NiSkinInstance* pSkin = pGeometry->GetSkinInstance();
					if( ( pSkin == NULL ) || ( pSkin->GetSkinPartition() == NULL ) )
					{
						nGeomTypeResult = CGeometry::MeshOutLine_Gray;
					}
				}
				break;
			case CGeometry::MeshOutLine:
			case CGeometry::MeshOutLine_Gray:
			case CGeometry::Normal:
				break;
			}
		}
		else
		{
			nGeomTypeResult = CGeometry::Normal;
		}
		

		CsNiGeometry* pInsertGeom = (CsNiGeometry*)pAvObject;
		CGeometry* pGeom = CGeometry::GetInstance( pInsertGeom, nGeomTypeResult );
		pGeom->SetGeometry( pInsertGeom );

		// 스펙큘라 셋팅
		if( pGeometry->GetProperty( NiProperty::SPECULAR ) )
		{
			if( ( (NiSpecularProperty*)pGeometry->GetProperty( NiProperty::SPECULAR ) )->GetSpecular() )
			{
				pGeom->AddPlag( CGeometry::GP_SPECULAR );
			}
		}

		if( ( pInsertGeom->GetSkinInstance() )&&( pInsertGeom->GetSkinInstance()->GetSkinPartition() ) )
		{
			vpGeom->PushBack( pGeom );
		}
		else
		{
			vpSortGeom->PushBack( pGeom );
		}
	}
}

NiTransformController* nsCSGBFUNC::GetTransformController( NiAVObject* pObject )
{
	if( pObject == NULL )
		return NULL;

	NiTimeController* pController = pObject->GetControllers();
	if( pController != NULL )
	{
		assert_cs( NiIsKindOf( NiTransformController, pController ) == true );
		return (NiTransformController*)pController;
	}
	
	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		int nChild = ( (NiNode*)pObject )->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			NiTransformController* pTransformController = GetTransformController( ( (NiNode*)pObject )->GetAt( i ) );
			if( pTransformController != NULL )
				return pTransformController;
		}
	}
	return NULL;
}

void nsCSGBFUNC::InitAnimation( NiObjectNET* pObject, NiTimeController::AnimType eAnimType, NiTimeController::CycleType eCycleType )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{			
			pController->SetAnimType( eAnimType );
			pController->SetCycleType( eCycleType );
		}
		int nChild = ( (NiNode*)pObject )->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			InitAnimation( ( (NiNode*)pObject )->GetAt( i ), eAnimType, eCycleType );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->SetAnimType( eAnimType );
			pController->SetCycleType( eCycleType );
		}
		
		NiProperty* pProp = ( (NiTriBasedGeom*)pObject )->GetProperty( NiTexturingProperty::GetType() );
		if( pProp != NULL )
		{
			InitAnimation( pProp, eAnimType, eCycleType );
		}
		pProp = ( (NiTriBasedGeom*)pObject )->GetProperty( NiMaterialProperty::GetType() );
		if( pProp != NULL )
		{
			InitAnimation( pProp, eAnimType, eCycleType );
		}
	}
	else if( NiIsKindOf( NiTexturingProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->SetAnimType( eAnimType );
			pController->SetCycleType( eCycleType );
		}
	}
	else if( NiIsKindOf( NiMaterialProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->SetAnimType( eAnimType );
			pController->SetCycleType( eCycleType );
		}
	}
}

void nsCSGBFUNC::StartAnimation( NiObjectNET* pObject, float fStartTime )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{			
			pController->Start( fStartTime );
		}
		int nChild = ( (NiNode*)pObject )->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			StartAnimation( ( (NiNode*)pObject )->GetAt( i ), fStartTime );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->Start( fStartTime );
		}

		NiProperty* pProp = ( (NiTriBasedGeom*)pObject )->GetProperty( NiTexturingProperty::GetType() );
		if( pProp != NULL )
		{
			StartAnimation( pProp, fStartTime );
		}
		pProp = ( (NiTriBasedGeom*)pObject )->GetProperty( NiMaterialProperty::GetType() );
		if( pProp != NULL )
		{
			StartAnimation( pProp, fStartTime );
		}
	}
	else if( NiIsKindOf( NiTexturingProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->Start( fStartTime );
		}
	}
	else if( NiIsKindOf( NiMaterialProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->Start( fStartTime );
		}
	}
}

void nsCSGBFUNC::StopAnimation( NiObjectNET* pObject )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{			
			pController->Stop();
		}
		int nChild = ( (NiNode*)pObject )->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			StopAnimation( ( (NiNode*)pObject )->GetAt( i ) );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->Stop();
		}

		NiProperty* pProp = ( (NiTriBasedGeom*)pObject )->GetProperty( NiTexturingProperty::GetType() );
		if( pProp != NULL )
		{
			StopAnimation( pProp );
		}
		pProp = ( (NiTriBasedGeom*)pObject )->GetProperty( NiMaterialProperty::GetType() );
		if( pProp != NULL )
		{
			StopAnimation( pProp );
		}
	}
	else if( NiIsKindOf( NiTexturingProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->Stop();
		}
	}
	else if( NiIsKindOf( NiMaterialProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			pController->Stop();
		}
	}
}

void nsCSGBFUNC::__EndAnimationTime( float& fReturn, NiObjectNET* pObject )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			fReturn = CsMax( pController->GetEndKeyTime(), fReturn );
		}
		int nChild = ( (NiNode*)pObject )->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			__EndAnimationTime( fReturn, ( (NiNode*)pObject )->GetAt( i ) );
		}
	}
	else if( NiIsKindOf( NiGeometry, pObject ) == true )
	{
		NiGeometry* pGeom = (NiGeometry*)pObject;
		NiTimeController* pController = pGeom->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			fReturn = CsMax( pController->GetEndKeyTime(), fReturn );
		}

		NiProperty* pProp = pGeom->GetProperty( NiTexturingProperty::GetType() );
		if( pProp != NULL )
		{
			__EndAnimationTime( fReturn, pProp );
		}
		pProp = pGeom->GetProperty( NiMaterialProperty::GetType() );
		if( pProp != NULL )
		{
			__EndAnimationTime( fReturn, pProp );
		}
		if( NiIsKindOf( NiMeshPSysData, pGeom->GetModelData() ) == true )
		{
			NiMeshPSysData* pPaticle = (NiMeshPSysData*)pGeom->GetModelData();
			__EndAnimationTime( fReturn, pPaticle->GetContainerNode() );
		}
	}
	else if( NiIsKindOf( NiTexturingProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			fReturn = CsMax( pController->GetEndKeyTime(), fReturn );
		}
	}
	else if( NiIsKindOf( NiMaterialProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			fReturn = CsMax( pController->GetEndKeyTime(), fReturn );
		}
	}
}

void nsCSGBFUNC::EndAnimationTime( float& fReturn, NiObjectNET* pObject )
{
	fReturn = 0.0f;
	__EndAnimationTime( fReturn, pObject );
}

bool nsCSGBFUNC::EndAnimation( NiObjectNET* pObject )
{
	if( pObject == NULL )
		return true;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			float fEndTime = pController->GetEndKeyTime();
			if( ( fEndTime != 0.0f )&&( fEndTime != pController->GetLastScaledTime() ) )
				return false;
		}
		int nChild = ( (NiNode*)pObject )->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			if( EndAnimation( ( (NiNode*)pObject )->GetAt( i ) ) == false )
				return false;
		}
	}
	else if( NiIsKindOf( NiGeometry, pObject ) == true )
	{
		NiGeometry* pGeom = (NiGeometry*)pObject;
		/*NiTimeController* pController = pGeom->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			float fEndTime = pController->GetEndKeyTime();
			if( ( fEndTime != 0.0f )&&( fEndTime != pController->GetLastScaledTime() ) )
				return false;
		}*/

		NiProperty* pProp = pGeom->GetProperty( NiTexturingProperty::GetType() );
		if( pProp != NULL )
		{
			if( EndAnimation( pProp ) == false )
				return false;
		}
		pProp = pGeom->GetProperty( NiMaterialProperty::GetType() );
		if( pProp != NULL )
		{
			if( EndAnimation( pProp ) == false )
				return false;
		}

		if( NiIsKindOf( NiMeshPSysData, pGeom->GetModelData() ) == true )
		{
			NiMeshPSysData* pPaticle = (NiMeshPSysData*)pGeom->GetModelData();			
			if( EndAnimation( pPaticle->GetContainerNode() ) == false )
				return false;
		}
	}
	else if( NiIsKindOf( NiTexturingProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			float fEndTime = pController->GetEndKeyTime();
			if( ( fEndTime != 0.0f )&&( fEndTime != pController->GetLastScaledTime() ) )
				return false;
		}
	}
	else if( NiIsKindOf( NiMaterialProperty, pObject ) == true )
	{
		NiTimeController* pController = pObject->GetControllers();
		for ( ; pController!=NULL; pController = pController->GetNext() )
		{
			float fEndTime = pController->GetEndKeyTime();
			if( ( fEndTime != 0.0f )&&( fEndTime != pController->GetLastScaledTime() ) )
				return false;
		}
	}
	return true;
}

void nsCSGBFUNC::SetMaterial_Alpha_Emittance( NiObjectNET* pObject, float fAlpha )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			SetMaterial_Alpha_Emittance( pNode->GetAt( i ), fAlpha );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;
		NiProperty* pAlphaProp = pTriGeom->GetProperty( NiAlphaProperty::GetType() );
		if( pAlphaProp == NULL )
		{
			NiAlphaPropertyPtr ptrAlphaProp = NiNew NiAlphaProperty;
			pTriGeom->AttachProperty( ptrAlphaProp );
		}
		
		SetMaterial_Alpha_Emittance( pTriGeom->GetProperty( NiMaterialProperty::GetType() ), fAlpha );
		SetMaterial_Alpha_Emittance( pTriGeom->GetProperty( NiAlphaProperty::GetType() ), fAlpha );
		SetMaterial_Alpha_Emittance( pTriGeom->GetProperty( NiTexturingProperty::GetType() ), fAlpha );
		pTriGeom->RemoveProperty( NiVertexColorProperty::GetType() );

		pTriGeom->UpdateProperties();
	}
	else if( NiIsKindOf( NiMaterialProperty, pObject ) == true )
	{
		NiMaterialProperty* pMatProp = (NiMaterialProperty*)pObject;
		if( pMatProp != NULL )
		{
			pMatProp->SetAlpha( fAlpha );
			pMatProp->SetEmittance( NiColor( 1.0f, 1.0f, 1.0f ) );		
		}		
	}
	else if( NiIsKindOf( NiAlphaProperty, pObject ) == true )
	{
		NiAlphaProperty* pAlphaProp = (NiAlphaProperty*)pObject;
		pAlphaProp->SetAlphaBlending( true );
		pAlphaProp->SetAlphaTesting( false );
	}
	else if( NiIsKindOf( NiTexturingProperty, pObject ) == true )
	{
		NiTexturingProperty* pTexProp = (NiTexturingProperty*)pObject;
		pTexProp->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );
	}
}

void nsCSGBFUNC::Set_Emittance( NiObjectNET* pObject, NiColor Emittance )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			Set_Emittance( pNode->GetAt( i ), Emittance );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;		

		Set_Emittance( pTriGeom->GetProperty( NiMaterialProperty::GetType() ), Emittance );
		Set_Emittance( pTriGeom->GetProperty( NiTexturingProperty::GetType() ), Emittance );
		pTriGeom->RemoveProperty( NiVertexColorProperty::GetType() );
		pTriGeom->UpdateProperties();
	}
	else if( NiIsKindOf( NiMaterialProperty, pObject ) == true )
	{
		NiMaterialProperty* pMatProp = (NiMaterialProperty*)pObject;		
		if( pMatProp != NULL )
		{
			pMatProp->SetEmittance( Emittance );
		}		
	}
	else if( NiIsKindOf( NiTexturingProperty, pObject ) == true )
	{
		NiTexturingProperty* pTexProp = (NiTexturingProperty*)pObject;
		pTexProp->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );
	}
}


void nsCSGBFUNC::SetMaterial_Color( NiObjectNET* pObject, NiColor color )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			SetMaterial_Color( pNode->GetAt( i ), color );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;		

		SetMaterial_Color( pTriGeom->GetProperty( NiMaterialProperty::GetType() ), color );
	}
	else if( NiIsKindOf( NiMaterialProperty, pObject ) == true )
	{
		NiMaterialProperty* pMatProp = (NiMaterialProperty*)pObject;		
		pMatProp->SetDiffuseColor( color );
		pMatProp->SetAmbientColor( color );
	}
}

void nsCSGBFUNC::SetTextureApplyMode( NiObjectNET* pObject, NiTexturingProperty::ApplyMode eApplyMode )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			SetTextureApplyMode( pNode->GetAt( i ), eApplyMode );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;
				
		// 텍스쳐
		NiTexturingProperty* pTexProp = (NiTexturingProperty*)pTriGeom->GetProperty( NiTexturingProperty::GetType() );
		if( pTexProp != NULL )
			pTexProp->SetApplyMode( eApplyMode );
	}
}

void nsCSGBFUNC::SetTextureFilterMode( NiObjectNET* pObject, NiTexturingProperty::FilterMode eFilterMode )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			SetTextureFilterMode( pNode->GetAt( i ), eFilterMode );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;

		// 텍스쳐
		NiTexturingProperty* pTexProp = (NiTexturingProperty*)pTriGeom->GetProperty( NiTexturingProperty::GetType() );
		if( pTexProp != NULL )
		{
			pTexProp->SetBaseFilterMode( eFilterMode );
			NiTexturingProperty::Map* pDarkMap = pTexProp->GetDarkMap();
			if( pDarkMap )
			{
				pDarkMap->SetFilterMode( eFilterMode );
			}
		}
	}
}

void nsCSGBFUNC::SetMaterial_Alpha( NiObjectNET* pObject, float fAlpha, CsBackupProperty* pBackupProperty )
{
	_SetMaterial_Alpha( pObject, fAlpha, pBackupProperty );
	_SetAlphaBlend( pObject );
}

void nsCSGBFUNC::_SetMaterial_Alpha( NiObjectNET* pObject, float fAlpha, CsBackupProperty* pBackupProperty )
{
	if( pObject == NULL )
		return;

	assert_cs( pBackupProperty != NULL );

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			_SetMaterial_Alpha( pNode->GetAt( i ), fAlpha, pBackupProperty );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;
		//CsMaterialProp* pCsMaterialProp = CsMaterialProp::NewInstance();
		CsMaterialProp* pCsMaterialProp = NiNew CsMaterialProp;
		pCsMaterialProp->Init( pTriGeom );

		// 알파 블랜드
		NiAlphaProperty* pAlphaProp = (NiAlphaProperty*)pTriGeom->GetProperty( NiAlphaProperty::GetType() );		
		pCsMaterialProp->s_bUseAlphaProp = ( pAlphaProp != NULL );
		if( pAlphaProp != NULL )
		{			
			pCsMaterialProp->s_bAlphaBlend = pAlphaProp->GetAlphaBlending();
			pCsMaterialProp->s_bAlphaTest = pAlphaProp->GetAlphaTesting();
			assert_cs( pAlphaProp->GetNoSorter() == false );
			//assert_cs( pAlphaProp->GetSrcBlendMode() == NiAlphaProperty::ALPHA_SRCALPHA );
			//assert_cs( pAlphaProp->GetDestBlendMode() == NiAlphaProperty::ALPHA_INVSRCALPHA );			
		}
		else
		{
			NiAlphaPropertyPtr ptrAlphaProp = NiNew NiAlphaProperty;			
			pTriGeom->AttachProperty( ptrAlphaProp );
		}

		// 버택스 칼라
		pCsMaterialProp->s_pVertexColorProp = NiDynamicCast( NiVertexColorProperty, pTriGeom->RemoveProperty( NiVertexColorProperty::GetType() ) );
		// 텍스쳐
		NiTexturingProperty* pTexProp = (NiTexturingProperty*)pTriGeom->GetProperty( NiTexturingProperty::GetType() );
		if( pTexProp != NULL )
		{
			pCsMaterialProp->s_eTextureApplyMode = pTexProp->GetApplyMode();
			pTexProp->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );
		}
		// 메터리얼
		NiMaterialProperty* pMatProp = (NiMaterialProperty*)pTriGeom->GetProperty( NiMaterialProperty::GetType() );
		if( pMatProp != NULL )
		{
			pMatProp->SetAlpha( fAlpha );
		}
		pBackupProperty->s_listProperty.push_back( pCsMaterialProp );
	}
}

void nsCSGBFUNC::SetMaterial_Alpha( NiObjectNET* pObject, float fAlpha )
{
	if( pObject == NULL )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			SetMaterial_Alpha( pNode->GetAt( i ), fAlpha );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;		
		// 메터리얼
		NiMaterialProperty* pMatProp = (NiMaterialProperty*)pTriGeom->GetProperty( NiMaterialProperty::GetType() );
		if( pMatProp != NULL )
		{
			pMatProp->SetAlpha( fAlpha );
		}
	}
}

void nsCSGBFUNC::_SetAlphaBlend( NiObjectNET* pObject )
{
	if( pObject == NULL )
		return;


	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			_SetAlphaBlend( pNode->GetAt( i ) );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;
		assert_cs( pTriGeom->GetProperty( NiAlphaProperty::GetType() ) != NULL );
		NiAlphaProperty* pAlphaProp = (NiAlphaProperty*)pTriGeom->GetProperty( NiAlphaProperty::GetType() );
		pAlphaProp->SetAlphaBlending( true );
		pAlphaProp->SetAlphaTesting( false );

		pTriGeom->UpdateProperties();
	}
}

void nsCSGBFUNC::SetPropertyBack( NiObjectNET* pObject, CsBackupProperty* pBackupProperty )
{
	if( pObject == NULL )
		return;

	assert_cs( pBackupProperty != NULL );

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			SetPropertyBack( pNode->GetAt( i ), pBackupProperty );
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;		

		CsProperty* pCsProp = pBackupProperty->PopProperty( pTriGeom );
		if( pCsProp != NULL )
		{
			switch( pCsProp->s_eType )
			{
			case CsProperty::PT_MATERIAL:
				{
					CsMaterialProp* pCsMaterialProp = (CsMaterialProp*)pCsProp;
					// 알파 블랜드
					if( pCsMaterialProp->s_bUseAlphaProp == false )
					{
						NiPropertyPtr pAlphaProp = pTriGeom->RemoveProperty( NiAlphaProperty::GetType() );
						pAlphaProp = 0;
					}
					else
					{
						NiAlphaProperty* pAlphaProp = (NiAlphaProperty*)pTriGeom->GetProperty( NiAlphaProperty::GetType() );
						pAlphaProp->SetAlphaBlending( pCsMaterialProp->s_bAlphaBlend );
						pAlphaProp->SetAlphaTesting( pCsMaterialProp->s_bAlphaTest );
					}
					// 버택스 칼라
					if( pCsMaterialProp->s_pVertexColorProp != NULL )
						pTriGeom->AttachProperty( pCsMaterialProp->s_pVertexColorProp );
					// 텍스쳐
					NiTexturingProperty* pTexProp = (NiTexturingProperty*)pTriGeom->GetProperty( NiTexturingProperty::GetType() );
					if( pTexProp != NULL )
						pTexProp->SetApplyMode( pCsMaterialProp->s_eTextureApplyMode );

					// 메터리얼
					NiMaterialProperty* pMatProp = (NiMaterialProperty*)pTriGeom->GetProperty( NiMaterialProperty::GetType() );
					if( pMatProp != NULL )
					{
						pMatProp->SetAlpha( 1.0f );
					}
					pTriGeom->UpdateProperties();
				}
				break;
			default:
				assert_cs( false );
			}

			CsProperty::DeleteProperty( pCsProp );
		}
	}
}

bool nsCSGBFUNC::CalMinMaxBound( NiObjectNET* pObject, NiTransform Transform, sTERRAIN_BOUND* pBound )
{
	bool bReturn = false;
	if( pObject == NULL )
		return bReturn;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		NiAVObject* pChild;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			pChild = pNode->GetAt( i );
			if( pChild == NULL )
				continue;
			if( CalMinMaxBound( pChild, Transform*pChild->GetLocalTransform(), pBound ) == true )
				bReturn = true;
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriObject = (NiTriBasedGeom*)pObject;
		unsigned short usVert = pTriObject->GetVertexCount();
		NiPoint3* pVert = pTriObject->GetVertices();
		for( unsigned short i=0; i<usVert; ++i )
		{
			NiPoint3 trVert = Transform*pVert[ i ];
			if( pBound->s_ptMin.x > trVert.x )	pBound->s_ptMin.x = trVert.x;
			if( pBound->s_ptMin.y > trVert.y )	pBound->s_ptMin.y = trVert.y;
			if( pBound->s_ptMax.x < trVert.x )	pBound->s_ptMax.x = trVert.x;
			if( pBound->s_ptMax.y < trVert.y )	pBound->s_ptMax.y = trVert.y;
		}
		bReturn = true;		
	}
	return bReturn;
}

bool nsCSGBFUNC::CalMinMax3DBound_Local( NiObjectNET* pObject, NiPoint3* ptMin, NiPoint3* ptMax )
{
	bool bReturn = false;

	if( pObject == NULL )
		return bReturn;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		NiAVObject* pChild;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			pChild = pNode->GetAt( i );
			if( CalMinMax3DBound_Local( pChild, ptMin, ptMax ) == true )
				bReturn = true;
		}
	}
	else if( NiIsKindOf( NiTriBasedGeom, pObject ) == true )
	{
		NiTriBasedGeom* pTriObject = (NiTriBasedGeom*)pObject;
		unsigned short usVert = pTriObject->GetVertexCount();
		NiPoint3* pVert = pTriObject->GetVertices();
		for( unsigned short i=0; i<usVert; ++i )
		{
			NiPoint3 trVert = pTriObject->GetLocalTransform()*pVert[ i ];
			if( ptMin->x > trVert.x )	ptMin->x = trVert.x;
			if( ptMin->y > trVert.y )	ptMin->y = trVert.y;
			if( ptMin->z > trVert.z )	ptMin->z = trVert.z;

			if( ptMax->x < trVert.x )	ptMax->x = trVert.x;
			if( ptMax->y < trVert.y )	ptMax->y = trVert.y;
			if( ptMax->z < trVert.z )	ptMax->z = trVert.z;
		}
		bReturn = true;
	}
	return bReturn;
}

NiAVObject* nsCSGBFUNC::GetEnableFirstNode( NiNode* pNode )
{
	NiAVObject* pReturn;
	int nChildCount = pNode->GetArrayCount();
	const char* cName = NULL;
	for( int i=0; i<nChildCount; ++i )
	{
		pReturn = pNode->GetAt( i );
		cName = pReturn->GetName();
		if( pReturn->GetAppCulled() == false )
		{
			if( cName && ( strncmp( cName, "Dummy", 5 ) != 0 ) ) 
			{
				return pReturn;
			}
		}
	}
	assert_csm( false, L"읽을수 있는 오브젝트가 없습니다. - Dummy 오브젝트는 읽지 않습니다." );
	return NULL;
}

NiGeometry* nsCSGBFUNC::GetFirstGeomety( NiAVObject* pObject )
{
	if( pObject == NULL )
		return NULL;

	if( pObject->GetAppCulled() == true )
		return NULL;
	
	NiGeometry* pReturn;
	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{			
			if( ( pReturn = GetFirstGeomety( pNode->GetAt( i ) ) ) != NULL )
			{
				return pReturn;
			}
		}
	}
	else if( NiIsKindOf( NiGeometry, pObject ) == true )
	{
		return (NiGeometry*)pObject;
	}
	else
	{
		assert_cs( false );
	}
	return NULL;
}

NiGeometry* nsCSGBFUNC::GetSkinGeomety( NiAVObject* pObject )
{
	if( pObject == NULL )
		return NULL;

	if( pObject->GetAppCulled() == true )
		return NULL;

	NiGeometry* pReturn;
	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{			
			if( ( pReturn = GetSkinGeomety( pNode->GetAt( i ) ) ) != NULL )
			{
				return pReturn;
			}
		}
	}
	else if( NiIsKindOf( NiGeometry, pObject ) == true )
	{
		if( ( (NiGeometry*)pObject )->GetSkinInstance() == NULL )
			return NULL;

		return (NiGeometry*)pObject;
	}
	else
	{
		assert_cs( false );
	}
	return NULL;
}

void nsCSGBFUNC::GetSkinGeometyList( std::list< NiAVObject* >* pList, NiAVObject* pObject )
{
	if( pObject == NULL )
		return;

	if( pObject->GetAppCulled() == true )
		return;

	NiGeometry* pReturn;
	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{			
			if( ( pReturn = GetSkinGeomety( pNode->GetAt( i ) ) ) != NULL )
			{
				pList->push_back( pReturn );
			}
		}
	}
	else if( NiIsKindOf( NiGeometry, pObject ) == true )
	{
		if( ( (NiGeometry*)pObject )->GetSkinInstance() )
		{
			pList->push_back( (NiGeometry*)pObject );
		}		
	}
	else
	{
		assert_cs( false );
	}
}

bool nsCSGBFUNC::IsAlphaBlend( NiAVObject* pObject )
{
	if( pObject == NULL )
		return false;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		int nChild = pNode->GetArrayCount();
		for( int i=0; i<nChild; ++i )
		{
			if( IsAlphaBlend( pNode->GetAt( i ) ) == true )
				return true;
		}
	}
	else if( NiIsKindOf( NiGeometry, pObject ) == true )
	{
		NiTriBasedGeom* pTriGeom = (NiTriBasedGeom*)pObject;
		NiAlphaProperty* pAlphaProp = (NiAlphaProperty*)pTriGeom->GetProperty( NiAlphaProperty::GetType() );
		if( pAlphaProp != NULL )
		{
			if( pAlphaProp->GetAlphaBlending() == true )
				return true;
		}
		NiMaterialProperty* pMatProp = (NiMaterialProperty*)pTriGeom->GetProperty( NiMaterialProperty::GetType() );
		if( pMatProp != NULL )
		{
			if( pMatProp->GetAlpha() < 1.0f )
				return true;
		}
	}

	return false;
}

NiPoint3 nsCSGBFUNC::CalMullRom( NiPoint3* p0, NiPoint3* p1, NiPoint3* p2, NiPoint3* p3, float t )
{
	float t2 = t*t;
	float t3 = t2*t;

	NiPoint3 out;	
	out.x = 0.5f * ( (2.0f*p1->x) + (-p0->x+p2->x)*t + (2.0f*p0->x - 5.0f*p1->x + 4.0f*p2->x - p3->x )*t2 +
					  (-p0->x + 3.0f*p1->x - 3.0f*p2->x + p3->x )*t3 );
	out.y = 0.5f * ( (2.0f*p1->y) + (-p0->y+p2->y)*t + (2.0f*p0->y - 5.0f*p1->y + 4.0f*p2->y - p3->y )*t2 +
					  (-p0->y + 3.0f*p1->y - 3.0f*p2->y + p3->y )*t3 );
	out.z = 0.5f * ( (2.0f*p1->z) + (-p0->z+p2->z)*t + (2.0f*p0->z - 5.0f*p1->z + 4.0f*p2->z - p3->z )*t2 +
					  (-p0->z + 3.0f*p1->z - 3.0f*p2->z + p3->z )*t3 );
	return out;
}

float nsCSGBFUNC::CalRotation( NiPoint3 vSrcPos, NiPoint3 vSrcDir, NiPoint3 vDestPos )
{
	NiPoint3 vDestDir = vDestPos - vSrcPos;
	vDestDir.z = 0.0f;
	vDestDir.Unitize();
	float fRot = NiACos( vSrcDir.Dot( vDestDir ) );
	if( vSrcDir.Cross( vDestDir ).z > 0 )
		fRot = -fRot;

	while( fRot < -NI_PI )
	{
		fRot += NI_PI*2.0f;
	}
	while( fRot > NI_PI )
	{
		fRot -= NI_PI*2.0f;
	}
	return fRot;
}

void nsCSGBFUNC::SetBit( NiAVObject* pNiObject, bool bVal, unsigned short nMask )
{
	if( pNiObject == NULL )
		return;

	pNiObject->SetBit( bVal, nMask );
	if( pNiObject->IsNode() )
	{
		assert_cs( NiIsKindOf( NiNode, pNiObject ) );
		int nArrayCnt = ( (NiNode*)pNiObject )->GetArrayCount();
		for( int i=0; i<nArrayCnt; ++i )
		{
			SetBit( ( (NiNode*)pNiObject )->GetAt( i ), bVal, nMask );
		}
	}
}

NiScreenElementsPtr	nsCSGBFUNC::MakeScreenElements(NiTexture* pBaseTexture)
{
	if( NULL == pBaseTexture )
		return NULL;

	NiScreenElementsPtr spElemetsImage = NiNew NiScreenElements( NiNew NiScreenElementsData(false, false, 1) );
	spElemetsImage->Insert(4);//4개의 점
	NiTexturingPropertyPtr pkTex = (NiTexturingProperty*)spElemetsImage->GetProperty( NiTexturingProperty::GetType() );

	if( !pkTex )
	{
		pkTex = NiNew NiTexturingProperty();
		pkTex->SetBaseClampMode( NiTexturingProperty::CLAMP_S_CLAMP_T );

		pkTex->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
		pkTex->SetApplyMode( NiTexturingProperty::APPLY_MODULATE);

		spElemetsImage->AttachProperty(pkTex); 
		spElemetsImage->UpdateProperties();
	}
	
	pkTex->SetBaseTexture( pBaseTexture );
	return spElemetsImage;	
}


NiNode* nsCSGBFUNC::GetRootNiNode( NiAVObject* pObject )
{
	assert_cs( pObject != NULL );
	NiAVObject* pParent = pObject->GetParent();
	while( pParent != NULL )
	{
		pObject = pParent;
		pParent = pObject->GetParent();	
	}
	assert_cs( NiIsKindOf( NiNode, pObject ) == true );
	return (NiNode*)pObject;
}

NiAVObject* nsCSGBFUNC::GetFirst_HaveTexProp( NiAVObject* pObject )
{
	assert_cs( pObject != NULL );

	NiTexturingProperty* pTexProp = (NiTexturingProperty*)pObject->GetProperty( NiTexturingProperty::GetType() );
	if( pTexProp != NULL )
		return pObject;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiAVObject* pAVReturn = NULL;
		int nChildCount = ( (NiNode*)pObject )->GetArrayCount();
		for( int sub=0; sub<nChildCount; ++sub )
		{
			pAVReturn = GetFirst_HaveTexProp( ( (NiNode*)pObject )->GetAt( sub ) );
			if( pAVReturn != NULL )
				return pAVReturn;
		}		
	}
	return NULL;
}

NiAVObject* nsCSGBFUNC::GetNext_HaveTexProp( NiAVObject* pObject, NiAVObject* pSrcChild, bool* bSearchChild /*=NULL*/ )
{
	assert_cs( pObject != NULL );
	assert_cs( pSrcChild != NULL );

	bool bSearch = false;
	if( bSearchChild == NULL )
	{
		bSearchChild = &bSearch;
	}

	NiTexturingProperty* pTexProp = (NiTexturingProperty*)pObject->GetProperty( NiTexturingProperty::GetType() );
	if( pTexProp != NULL )
	{
		if( *bSearchChild == false )
		{
			if( pObject == pSrcChild )
			{
				*bSearchChild = true;
			}
		}
		else
		{
			return pObject;
		}		
	}

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiAVObject* pAVReturn = NULL;
		int nChildCount = ( (NiNode*)pObject )->GetArrayCount();
		for( int sub=0; sub<nChildCount; ++sub )
		{
			pAVReturn = GetNext_HaveTexProp( ( (NiNode*)pObject )->GetAt( sub ), pSrcChild, bSearchChild );
			if( pAVReturn != NULL )
				return pAVReturn;
		}		
	}
	return NULL;
}

NiAVObject* nsCSGBFUNC::GetFirstAppCulledObject( NiAVObject* pNode, char* cKey, size_t siKeyLen )
{
	if( NiIsKindOf( NiGeometry, pNode ) == true )
	{
		if( pNode->GetAppCulled() == true )
		{
			if( ( strncmp( pNode->GetName(), cKey, siKeyLen ) == 0 )||
				( strncmp( pNode->GetParent()->GetName(), cKey, siKeyLen ) == 0 ) )
			{
				return pNode;
			}		
		}
	}
	else if( NiIsKindOf( NiNode, pNode ) == true )
	{		
		NiAVObject* pAVReturn = NULL;
		int nChildCount = ( (NiNode*)pNode )->GetArrayCount();		
		for( int sub=0; sub<nChildCount; ++sub )
		{
			pAVReturn = GetFirstAppCulledObject( ( (NiNode*)pNode )->GetAt( sub ), cKey, siKeyLen );
			if( pAVReturn != NULL )
				return pAVReturn;
		}			
	}
	return NULL;
}

