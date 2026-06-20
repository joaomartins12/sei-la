
#pragma once

#include "CsGBTerrainDefine.h"

#include "CsSoundMng.h"

#include "CsGBDX9Renderer.h"
#include "CsGBTerrainRenderer.h"

#include "CsGBLight.h"
#include "CsGBLight_Dir.h"
#include "CsGBLight_Point.h"
#include "CsGBLightMng.h"

#include "CsGBShadow.h"

#include "CsGBObject_UserDefine.h"

#include "CsGBObjOpt.h"
#include "CsGBObjOpt_DarkMap.h"
#include "CsGBObjOptFactor.h"

#include "CsGBObjProp.h"
#include "CsGBObjProp_Alpha.h"
#include "CsGBObjProp_Actor.h"
#include "CsGBObjProp_ABV.h"
#include "CsGBObjProp_AvCap.h"
#include "CsGBObjProp_AvTbp.h"
#include "CsGBObjProp_Sound.h"
#include "CsGBObjPropFactor.h"

#include "CsGBObject.h"
#include "CsGBObj_Portal.h"
#include "CsGBObj_Rot.h"
#include "CsGBObj_Eff.h"
#include "CsGBObjectResMng.h"

#include "CsGBSkyBox.h"

#include "CsGBCell.h"
#include "CsGBSpeedCell.h"

#include "CsGBTerrainNode.h"
#include "CsGBTerrainLeaf.h"
#include "CsGBTerrainStructVersion.h"
#include "CsGBTerrainRoot.h"
#include "CsGBTerrainMake.h"

#include "CsGBTerrainMng.h"
#include "CsGBWeather.h"



//{
//	assert_cs( NiIsKindOf( NiNode, pDestNode ) == true );
//	int nChildCount = pDestNode->GetChildCount();
//	for( int i=0; i<nChildCount; ++i )
//	{
//		NiAVObject* pChild = pDestNode->GetAt( i );
//		if( NiIsKindOf( NiNode, pChild ) == true )
//		{
//			_DeepCopy_PropertyClone( (NiNode*)pChild, (NiNode*)pSrcNode->GetAt( i ) );
//		}
//		else if( ( NiIsKindOf( NiTriStrips, pChild ) == true )||( NiIsKindOf( NiTriShape, pChild ) == true ) )
//		{
//			NiGeometry* pDestGeom = ( NiGeometry* )pChild;
//			assert_cs( NiIsKindOf( NiGeometry, pDestGeom ) == true );
//			NiGeometry* pSrcGeom = ( NiGeometry* )pSrcNode->GetAt( i );
//			assert_cs( NiIsKindOf( NiGeometry, pSrcGeom ) == true );
//
//			NiPropertyList* plistProp = &pDestGeom->GetPropertyList();
//			NiTListIterator pos = plistProp->GetHeadPos();
//			while( pos )
//			{
//				NiProperty* pProp = plistProp->GetNext( pos );
//
//				if( NiIsKindOf( NiTexturingProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiAlphaProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiDitherProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiFogProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiMaterialProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiFogProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiShadeProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiSpecularProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiShadeProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiStencilProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiVertexColorProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiWireframeProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else if( NiIsKindOf( NiZBufferProperty, pProp ) == true )
//				{
//					int a = 0;
//				}
//				else
//				{
//					assert_cs( false );
//				}
//			}
//		}
//	}
//}




