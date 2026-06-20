// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Headers
#include "MayaPluginPCH.h"




//---------------------------------------------------------------------------
MyiTriShape::MyiTriShape( int iShapeNum, int iGroupID,
    MyiMaterialManager* pkMaterialManager,
    MyiTextureManager* pkTextureManager)
{
    char szName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";

    m_puiTriList = NULL;
    m_pkVertices = NULL;
    m_uiTriListLength = 0;
    m_bHasAlpha = false;
    m_bHasVertexColors = false;



    // Various Texture Groups - Ensure we get object in it's entirety.
    // Grouping breaks apart a shape by differences in texturing,
    // materials, color, etc.

    // NOTE:    NEED TO LOOK AT HOW GROUPING WORKS FROM WITHIN MAYA
    //          THERE ARE ASSUMPTIONS BEING MADE - NAMELY THAT EACH
    //          SHAPE HAS AT LEAST ONE GROUP WHICH ISN'T APPARENTLY
    //          ALWAYS TRUE.

    // UPDATE: If the shape is a pure transformation/joint node then
    // the number of groups will be Zero (for instance, no shader assignment
    // This usually only occurs when the hierarchy is set to FULL, or all of
    // the transformations are flattened out, and geometry is associated with 
    // all of them.  STILL NEED TO LOOK INTO THIS TO ENSURE GROUP != 0

    // Currently - we offer support only for 1 group
    int iGroupCnt = DtGroupGetCount( iShapeNum );

    if (iGroupID < 0)
    {
        const char *szShapeName;
        DtShapeGetShapeName( iShapeNum, &szShapeName );
        DtExt_Err("ERROR: Object [%s] has abnormal Geometry.\n",
            szShapeName);

        m_bFail = false;
        return;
    }

    // GET THE NAME OF THE SHAPE
    const char *szShapeName;
    DtShapeGetShapeName( iShapeNum, &szShapeName );

    NIASSERT(m_spTriShape == NULL);

    int iInstance = DtExt_ShapeIsInstanced( iShapeNum );

    if (iInstance >= 0)
    {
        NIASSERT(iInstance < iShapeNum);
        if (iInstance < iShapeNum)
        {
            // This Shape is a clone of another.
            // Should have already been processed, but assert anyway.
            // Let's take a look at the shape to which this is a clone of
            // and actually "clone" it.
            int iComponentNumForInstance = gMDtObjectFindComponentID(
                ETypeShape, iInstance);

            NiNode* pMasterNode = 
                gUserData.GetNode( iComponentNumForInstance );

            NIASSERT(pMasterNode);

            // Get the children 
            // Possibly more than one if trishape had multiple "groups" of
            // textures,materials, etc.
            int iNumChildren = pMasterNode->GetChildCount();

            NiAVObject* pAVObject = NULL;

// Get appropriate child
// This is proving difficult because an object with multiple material groups
// may not match one-to-one with the children... an object may have a child
// naturally in hierarchy.  My assumption here (keep an eye on this) is that
// we create the separate trishapes for each group last - so they will be
// the last children and they will correspond to each group.
// If this is the case, then we just take the number of children, subtract
// of the number of "groups", and then add the current group number.

            // Find the matching name...
            char szSearchString[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
            NiStrcpy(szSearchString, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
                szShapeName);

            if (iGroupCnt>1)
            {
// If this object has multiple groups - identify each uniquely
                char szBuffer1[16];
                char szBuffer2[16];
#if defined(_MSC_VER) && _MSC_VER >= 1400
                _itoa_s(iGroupID,szBuffer1,16,10);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
                _itoa(iGroupID,szBuffer1,10);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
                NiSprintf(szBuffer2, 16, ":%s", szBuffer1 );
                NiStrcat( szSearchString, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
                    szBuffer2 );
            }

            for(int i=iNumChildren-1; i>=0; i--)
            {
                pAVObject = pMasterNode->GetAt(i);

                if (strstr(pAVObject->GetName(),szSearchString)!=NULL)
                    break;

                pAVObject = NULL;
            }

            NIASSERT(pAVObject);
            NIASSERT(NiIsKindOf(NiTriShape, pAVObject));
            
            if(NiIsKindOf(NiTriShape, pAVObject))
            {
                NiTriShape *pTriShape = (NiTriShape*)pAVObject;
                
                NiObject* pObj = pTriShape->Clone();
                
                if (NiIsKindOf(NiTriShape,pObj))
                {
                    m_spTriShape = (NiTriShape *)pObj;

                    // Remove Shader any shader that might be on this object
                    m_spTriShape->SetShader( NULL );

                    NiTimeController* pController = 
                        m_spTriShape->GetController(
                            &NiGeomMorpherController::ms_RTTI);

                    if (pController)
                        m_spTriShape->RemoveController(pController);

                    // convert scale for the instance
                    m_fScale[0] = m_fScale[1] = m_fScale[2] = 1.0f;
                    if (!DtShapeGetFinalNonUniformNonAnimatedScale( iShapeNum, 
                        &m_fScale[0], &m_fScale[1], &m_fScale[2]))
                    {
                        // Error From DtShapeGetScale
                        m_fScale[0] = m_fScale[1] = m_fScale[2] = 1.0f;
                    }

                    float fEpsilon = m_fScale[0] * 0.001f;
        
                    if( !NiOptimize::CloseTo(m_fScale[0], m_fScale[1], 
                            fEpsilon) || 
                        !NiOptimize::CloseTo(m_fScale[0], m_fScale[2], 
                            fEpsilon) )
                    {
                        DtExt_Err("WARNING: Instanced Object [%s] " \
                            "Can not have non-uniform scale.\n",
                            szShapeName);
                    }

                    m_spTriShape->SetScale (m_fScale[0]);

                    // Check for a skin instance
                    if (m_spTriShape->GetSkinInstance())
                    {
                        NiSkinInstance* pkSkin = 
                            m_spTriShape->GetSkinInstance();

                        // Set the root parent so that when we clone the 
                        // skin instance all of the internal variables will
                        // skin instance
                        NiCloningProcess kProcess;
                        kProcess.m_pkCloneMap->SetAt(pkSkin->GetRootParent(), 
                            pkSkin->GetRootParent());

                        // Create a new cloned skin instance
                        NiSkinInstance* pkNewSkin = 
                            (NiSkinInstance*)pkSkin->CreateClone(kProcess);

                        m_spTriShape->SetSkinInstance(pkNewSkin);
                    }

                    // if we have cloned the base object, none of the rest
                    // of the conversion process is necessary
                    m_bFail = false;
                    return;
                }
            }
        }
    }

    NiOptTriShape* pkNiOptTriShape = NULL;
    NiTexturingProperty *pTexturingProperty = NULL;

    // Check that an instance wasn't made...
    if (m_spTriShape == NULL)
    {
        // Collect info from MDtAPI and store relavant data
        // in our Ni member variables.
        if (!GetShapeAttributesForNi( iShapeNum, iGroupID))
        {
            const char *szShapeName;
            DtShapeGetShapeName( iShapeNum, &szShapeName );
            DtExt_Err("ERROR: Object [%s] has no Shape Attributes and can " \
                "not be exported.\n", szShapeName);

            return;
        }

        // Create the Texturing Property
        CreateMultiTexture( iShapeNum, iGroupID, pkTextureManager, 
            pTexturingProperty, m_bHasAlpha);

        // If this shape has a skeleton,
        // then this will be a Ni3dsSkin and not a NiTriShape.
        if (DtExt_GetShapeHasSkin(iShapeNum))
        {
            // First Create the Skin Vertex Data.
            MyiSkinVertexData kSVD(iShapeNum, iGroupID, m_uiNiVertices, 
                m_pkVertices );

            if (kSVD.Fail())
            {
                const char *szShapeName;
                DtShapeGetShapeName( iShapeNum, &szShapeName );
                DtExt_Err("ERROR: Object [%s] had error in Vertex Data.\n",
                    szShapeName);

                return; // Should not fail
            }

            NiTransform xformRootToSkin = *(kSVD.GetRootToSkinXform());

            MObject kDestObj;
            pkNiOptTriShape = NiNew NiOptTriShape(
                                m_uiNiVertices, 
                            m_pkVertices, 
                            kSVD.GetNumberOfBones(), 
                            (void**)kSVD.GetBones(),
                            (void *)kSVD.GetRootBone(), 
                            (NiTransform*) kSVD.GetSkinToBoneXforms(),
                            xformRootToSkin, 
                            m_uiNiVertices,
                            m_puiTriList, 
                            true, 
                            false,
                            GetBlendShape(iShapeNum, iGroupID, kDestObj));

            // SgoTriShape is now responsible for the Vertices
            m_pkVertices = NULL; 

            // CREATE THE OPTIMIZED TRISHAPE
            NiTriShape* pkConvertedTri = NULL;
            pkNiOptTriShape->ConvertTo(pkConvertedTri );

            m_spTriShape = pkConvertedTri;

            
#ifdef SHOW_OBJECT_TYPE_IN_NAME
            NiSprintf(szName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
                "3dsSkin Shape %d: ", iShapeNum);
#endif
        }
        else
        {
            NiTransform kRootToSkin;

            kRootToSkin.MakeIdentity();

            MObject kDestObj;
            pkNiOptTriShape = NiNew NiOptTriShape(
                            m_uiNiVertices, 
                            m_pkVertices, 
                            /* uiNumBones = */ 0,
                            /* ppvBones = */ NULL,
                            /* pvRootBone = */ NULL,
                            /* pInitXForms = */ NULL,
                            /* kRootToSkin = */ kRootToSkin,
                            m_uiNiVertices,
                            m_puiTriList,
                            /* bAllowLargeTriLists = */ true,
                            false,
                            GetBlendShape(iShapeNum, iGroupID, kDestObj));

            // SgoTriShape is now responsible for the Vertices
            m_pkVertices = NULL; 

            // CREATE THE OPTIMIZED TRISHAPE
            NiTriShape* pkConvertedTri = NULL;
            pkNiOptTriShape->ConvertTo(pkConvertedTri);

            m_spTriShape = pkConvertedTri;


#ifdef SHOW_OBJECT_TYPE_IN_NAME
            NiSprintf(szName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
                "TriShape %d: ", iShapeNum);
#endif
        }


        // Add User Defined Properties for the Shape
        if(m_spTriShape != NULL)
        {
            // Get the Object and dgNode
            MObject mObj = MObject::kNullObj;
            DtExt_ShapeGetShapeNode(iShapeNum, mObj);
            
            MFnDagNode dgNode;
            dgNode.setObject( mObj );
            
            // Attach All of the Attributes to the TriShape
            AttachUserDefinedAttributes( dgNode, m_spTriShape );

            // Attach any No Stripify Options
            AddNoStripifyAttributes(dgNode);
            
            // Attach physics if necessary
            MyiPhysX::ProcessTriShape(m_spTriShape, iShapeNum, iGroupID);
        }

    }

        // ERROR CHECK
    if (m_spTriShape == NULL)
    {
        DtExt_Err("ERROR: Failed to create TriShape: %s\n" \
            "     Gamebryo has a limit of 65635 triangles per trishape " \
            "     and 65535 vertices per trishape.\n", szShapeName );

        NiDelete pkNiOptTriShape;
        return;
    }


    // Intialize the consistency to static as other controllers are added
    // they will adjust the constency flags.
    m_spTriShape->SetConsistency(NiGeometryData::STATIC);

    // ADD ON THE SHAPE NAME
    NiStrcat( szName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, szShapeName );

    if (iGroupCnt>1)
    {
        // If this object has multiple groups - identify each uniquely
        char szBuffer1[16];
        char szBuffer2[16];
#if defined(_MSC_VER) && _MSC_VER >= 1400
        _itoa_s(iGroupID,szBuffer1,16,10);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        _itoa(iGroupID,szBuffer1,10);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
        NiSprintf(szBuffer2, 16, ":%s", szBuffer1);
        NiStrcat( szName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, szBuffer2 );
    }

    // SAVE THE SHAPE NAME
    m_spTriShape->SetName( szName );


    // Create the Material Properties
    if (!GetMaterialProperties( iShapeNum, iGroupID, -1, pkMaterialManager, 
            pTexturingProperty, m_bHasAlpha, m_bHasVertexColors, 
            m_spTriShape))
    {
        const char *szShapeName;
        DtShapeGetShapeName( iShapeNum, &szShapeName );
        DtExt_Err("ERROR: Object [%s] had Material Properties error.\n",
            szShapeName);

        NiDelete pkNiOptTriShape;
        return;
    }

    // Assign the Texture Property
    if (pTexturingProperty != NULL)
    {
        m_spTriShape->AttachProperty(pTexturingProperty);
    }

    // Attach any PixelShaders and Attributes
    int iMaterialID;
    DtMtlGetID( iShapeNum, iGroupID, &iMaterialID );
    unsigned int uiNBTUVSource = NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT;
    pkMaterialManager->AddPixelShaderAndAttributes(iMaterialID, m_spTriShape,
        uiNBTUVSource);


    // Generate the Normal, BiNormal, and Tangent Data
    NiGeometryData* pkGeomData = ((NiGeometry*)m_spTriShape)->GetModelData();
    unsigned int uiWhichNBT = pkGeomData->GetNormalBinormalTangentMethod();

    if (pkGeomData && pTexturingProperty &&
        ((pTexturingProperty->GetNormalMap() != NULL) || 
        pTexturingProperty->GetParallaxMap() != NULL))
    {
        if (uiWhichNBT == NiGeometryData::NBT_METHOD_NONE)
        {
            uiWhichNBT = NiGeometryData::NBT_METHOD_ATI;
            pkGeomData->StoreNormalBinormalTangent(
                NiGeometryData::NBT_METHOD_ATI);
        }
    }

    if (pkGeomData && (uiWhichNBT != NiGeometryData::NBT_METHOD_NONE))
    {
        if (NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT == uiNBTUVSource)
        {
            NiTexturingProperty* pkTexProp = NiDynamicCast(NiTexturingProperty,
                ((NiGeometry*)m_spTriShape)->GetProperty(
                NiProperty::TEXTURING));
            
            uiNBTUVSource = 0;
            if (pkTexProp)
            {
                NiTexturingProperty::Map* pkNormalMap = 
                    pkTexProp->GetNormalMap();
                if (pkNormalMap)
                    uiNBTUVSource = pkNormalMap->GetTextureIndex();

                NiTexturingProperty::Map* pkParallaxMap = 
                    pkTexProp->GetParallaxMap();

                if (pkParallaxMap && pkNormalMap && 
                    pkParallaxMap->GetTextureIndex() != 
                    pkNormalMap->GetTextureIndex())
                {
                    char acString[1024];
                    const char *szShapeName;
                    DtShapeGetShapeName( iShapeNum, &szShapeName );
                    NiSprintf(acString, 1024,"WARNING: Object [%s]. " \
                        "Parallax Map and Normal Map use" \
                        " different UV sets, only normal map UV set will have"
                        " its NBT frame generated.\n", szShapeName);
                    DtExt_Err(acString);
                }
                else if (pkParallaxMap)
                {
                    uiNBTUVSource = pkParallaxMap->GetTextureIndex();
                }
            }
        }

        if (!NiNBTCalculation::SetupBinormalTangentData(
            (NiGeometry*)m_spTriShape, uiNBTUVSource))
        {
            const char *szShapeName;
            DtShapeGetShapeName( iShapeNum, &szShapeName );
            DtExt_Err("WARNING: Object [%s]. " \
                "The NBT frame was not created!\n",szShapeName);
        }
    }

    if(pkNiOptTriShape != NULL)
    {
        const unsigned int* puiNewToOld = 
            pkNiOptTriShape->GetNewToOldMapping();

        AttachMorpherControllers(iShapeNum, iGroupID, puiNewToOld);
        
        NiDelete pkNiOptTriShape;
    }


    // Add Extra Data based on user defined attributes
    //MObject mObj;
    //DtExt_ShapeGetShapeNode( iShapeNum, mObj );
    //MFnDagNode dgNode( mObj );
    //NiObjectNET *pObject = (NiObjectNET*)m_spTriShape;
    //GetUserAttributes( dgNode, pObject );

//    tmpNiTriShapePtr = m_spTriShape;

    m_bFail = false;
}
//---------------------------------------------------------------------------

MyiTriShape::~MyiTriShape( void )
{
    m_spTriShape = 0;

}
//---------------------------------------------------------------------------
bool MyiTriShape::GetMaterialProperties( int iShapeID, int iGroupID, 
    int iMaterialID,
    MyiMaterialManager* pkMaterialManager, 
    NiTexturingProperty* pTexturingProperty,
    bool bNeedsAlphaProperty, bool bHasVertexColors,
    NiAVObject* pkAVObject)
{
    int iMaterials;

    iMaterials = DtGroupGetCount( iShapeID );

    // For now, assign the global material (the first one in the array
    // returned by ?) to the entire model.

    if ((iMaterials > 0) || (iMaterialID >= 0))
    {
        NiMaterialProperty *pMaterial;
        NiSpecularProperty *pSpecular;
        NiVertexColorProperty *pVertexColor;
        NiAlphaProperty *pAlpha;
        
        // Get all Materials;
        // Pass them into pMaterialManger so that they will get translated
        // into ni format.

        // Create pMaterial based on iShapeNum, iMaterials=0
        if (iMaterialID == -1)
        {
            DtMtlGetID( iShapeID, iGroupID, &iMaterialID );
        }

        // Maybe pass in the m_spp instead.
        pkMaterialManager->GetMaterial(iMaterialID, pMaterial, 
                                      pSpecular, pVertexColor,pAlpha);

        NiProperty *pProp = pkAVObject->GetProperty(pMaterial->Type());
        
        if (pProp)
        {
            pkAVObject->DetachProperty(pProp);
        }

        pkAVObject->AttachProperty(pMaterial);


        // ********************************************************
        // Apply Transparency
        //*********************************************************

        // Create an Alpha property if one wasn't created already
        bool bTransparencyOverride = 
            DtTextureGetMaterialTransparencyOverride(iMaterialID);

        if((pAlpha == NULL) && 
            ((bNeedsAlphaProperty) || ( bTransparencyOverride)) )
        {
            pAlpha = NiNew NiAlphaProperty;
            pAlpha->SetSrcBlendMode( NiAlphaProperty::ALPHA_SRCALPHA);
            pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
            pAlpha->SetAlphaBlending(true);
        }

        if(bTransparencyOverride)
        {
            
            // Alpha Blending
            pAlpha->SetAlphaBlending(
                DtTextureGetMaterialTransparencyUseAlphaBlending(
                    iMaterialID));
            
            char cSrcBlendMode;
            DtTextureGetMaterialTransparencySource(iMaterialID, 
                cSrcBlendMode);

            pAlpha->SetSrcBlendMode(
                (NiAlphaProperty::AlphaFunction)cSrcBlendMode);
            
            char cDestBlendMode;
            DtTextureGetMaterialTransparencyDestination(iMaterialID, 
                cDestBlendMode);
            pAlpha->SetDestBlendMode(
                (NiAlphaProperty::AlphaFunction)cDestBlendMode);
            
            // Alpha Testing UseAlphaTesting
            pAlpha->SetAlphaTesting(
                DtTextureGetMaterialTransparencyUseAlphaTesting(iMaterialID));
            
            pAlpha->SetNoSorter(
                DtTextureGetMaterialTransparencyNoTestingSorter(iMaterialID));
            
            short sTestReference;
            DtTextureGetMaterialTransparencyTestReference(iMaterialID, 
                sTestReference);
            pAlpha->SetTestRef((unsigned char)sTestReference);
            
            char cTestingTestMode;
            DtTextureGetMaterialTransparencyTestingTestMode(iMaterialID, 
                cTestingTestMode);

            pAlpha->SetTestMode(
                (NiAlphaProperty::TestFunction)cTestingTestMode);
            
        }

        if (pAlpha != NULL)
        {
            pkAVObject->AttachProperty(pAlpha);
        }


        // ********************************************************
        // Apply Specular Highlights
        //*********************************************************
        NIASSERT(pSpecular != NULL);
        pProp = pkAVObject->GetProperty(NiProperty::SPECULAR);
        if (pProp)
        {
            pkAVObject->DetachProperty(pProp);
        }
        pkAVObject->AttachProperty(pSpecular);


        // ********************************************************
        // Apply Vertex Colors
        //*********************************************************

        NiVertexColorPropertyPtr pVertexProperty = NiNew NiVertexColorProperty;
        
        if (pVertexProperty == NULL)
            return false;
        
        // Create a VertexColor Property from what we know from the shape...
        if (!bHasVertexColors && NiIsKindOf(NiTriShape, pkAVObject))
        {
            NiTriShape* pkTriShape = (NiTriShape*)pkAVObject;

            pVertexProperty->SetSourceMode(
                NiVertexColorProperty::SOURCE_IGNORE);
            pVertexProperty->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E_A_D);
            
            // Remove Vertex Colors
            NiGeometryData* pkData = pkTriShape->GetModelData();
            NiPoint3* pkVertex = pkData->GetVertices();
            NiPoint3* pkNormal = pkData->GetNormals();
            NiPoint2* pkTexture = pkData->GetTextures();
            unsigned short usTextures = pkData->GetTextureSets();
            unsigned short usVertices = pkData->GetVertexCount();
            NiGeometryData::DataFlags eNBTMethod = 
                pkTriShape->GetNormalBinormalTangentMethod();
            
            pkData->Replace(usVertices, pkVertex, pkNormal, NULL, pkTexture, 
                usTextures, eNBTMethod);
        }
        else
        {
            // Default Values
            pVertexProperty->SetSourceMode(
                NiVertexColorProperty::SOURCE_AMB_DIFF);
            pVertexProperty->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E_A_D);
            
            if(DtTextureGetMaterialVertexColorOverride(iMaterialID))
            {
                char cSrcMode;
                DtTextureGetMaterialVertexColorSrcMode(iMaterialID, cSrcMode);
                pVertexProperty->SetSourceMode(
                    (NiVertexColorProperty::SourceVertexMode)cSrcMode);
                
                char cLightMode;
                DtTextureGetMaterialVertexColorLightMode(iMaterialID, 
                    cLightMode);

                pVertexProperty->SetLightingMode(
                    (NiVertexColorProperty::LightingMode)cLightMode);
            }
        }

        
        pProp = pkAVObject->GetProperty(pVertexProperty->Type());
        if (pProp)
        {
            pkAVObject->DetachProperty(pProp);
        }        
        pkAVObject->AttachProperty(pVertexProperty);
        


        // ********************************************************
        // Apply Shading
        //*********************************************************

        char cShading;
        if(DtTextureGetMaterialShading(iMaterialID, cShading))
        {
            NiShadePropertyPtr spShadeProperty = NiNew NiShadeProperty;
            spShadeProperty->SetSmooth(cShading == 1);
            pkAVObject->AttachProperty(spShadeProperty);
        }
        

        // ********************************************************
        // Apply Texturing Properties
        //*********************************************************

        char cTextureApplyMode;
        if((pTexturingProperty != NULL) &&
            DtTextureGetMaterialTextureApplyMode(iMaterialID, 
                cTextureApplyMode))
        {
            pTexturingProperty->SetApplyMode(
                (NiTexturingProperty::ApplyMode)cTextureApplyMode);
        }

        // ********************************************************
        // Apply NBT Method
        //*********************************************************

        char cNBTMethod;
        if( NiIsKindOf(NiGeometry, pkAVObject) &&
            DtTextureGetMaterialNBTMethod(iMaterialID, cNBTMethod))
        {
            NiGeometryData* pkGeomData = 
                ((NiGeometry*)pkAVObject)->GetModelData();

            switch(cNBTMethod)
            {
            case 0:
                pkGeomData->StoreNormalBinormalTangent(
                    NiGeometryData::NBT_METHOD_NONE);
                break;
            case 1:
                pkGeomData->StoreNormalBinormalTangent(
                    NiGeometryData::NBT_METHOD_NDL);
                break;
            case 2:
                pkGeomData->StoreNormalBinormalTangent(
                    NiGeometryData::NBT_METHOD_ATI);
                break;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriShape::GetShapeAttributesForNi( int iShapeID, int iGroupID )
{
    if (!DtShapeGetShapeName( iShapeID, &m_pName ))
    {
        // Error From DtShapeGetShapeName
        return false;
    }

    // if (!DtShapeGetScale( iShapeID, &m_fScale[0], &m_fScale[1], 
    //      &m_fScale[2]))
    m_fScale[0] = m_fScale[1] = m_fScale[2] = 1.0f;
    if (!DtShapeGetFinalNonUniformNonAnimatedScale( iShapeID, &m_fScale[0], 
        &m_fScale[1], &m_fScale[2]))
    {
        // Error From DtShapeGetScale
        return false;
    }


    if (!GetVertsForNi(iShapeID, iGroupID))
    {
        // Error From GetVertsForNi
        return false;
    }

    if (!GetTriIndexListForNi(iShapeID, iGroupID))
    {
        // Error From GetTriIndexListForNi
        return false;
    }

    if (!GetUVsForNi(iShapeID, iGroupID))
    {
        return false;
    }


// Calculation of Exploded Normals doesn't function properly
// So, we'll rely on the TriShape's CalculateNormals function.
    if (!GetNormsForNi(iShapeID, iGroupID))
    {
        // Error From GetNormsForNi
        return false;
    }

    if (!GetVertexColorsForNi(iShapeID, iGroupID))
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
bool MyiTriShape::GetVertexColorsForNi(int iShapeID, int iGroupID)
{
    int iColorCnt;
    DtFltRGBA *DtFltRGBAVertexColors;
    DtShapeGetVerticesFaceColors( iShapeID, &iColorCnt,
        &DtFltRGBAVertexColors);

    // Check if the object has vertex colors
    if (DtFltRGBAVertexColors == NULL)
        return true;

    int iVertex = 0;
    int iCount;
    long *plIndices;

    DtFaceGetColorIndexByShape( iShapeID, iGroupID, &iCount, &plIndices );

    m_bHasVertexColors = false;
    
    for( int i = 0; i < iCount; i++ )
    {
        if( plIndices[i] != DtEND_OF_FACE )
        {
            // SET THE VERTEX COLOR SKIPPING THE END OF FACE
            m_pkVertices[iVertex].m_kColor.r = 
                DtFltRGBAVertexColors[ plIndices[i] ].r;

            m_pkVertices[iVertex].m_kColor.g = 
                DtFltRGBAVertexColors[ plIndices[i] ].g;

            m_pkVertices[iVertex].m_kColor.b = 
                DtFltRGBAVertexColors[ plIndices[i] ].b;

            m_pkVertices[iVertex].m_kColor.a = 
                DtFltRGBAVertexColors[ plIndices[i] ].a;

            // CHECK FOR A VERTEX COLOR
            if( !m_bHasVertexColors && ( m_pkVertices[iVertex].m_kColor.r ||
                m_pkVertices[iVertex].m_kColor.g || 
                m_pkVertices[iVertex].m_kColor.b ||
                m_pkVertices[iVertex].m_kColor.a ) )
            {
                m_bHasVertexColors = true;
            }

            // CHECK FOR A VERTEX COLOR ALPHA
            if(m_bHasVertexColors && !m_bHasAlpha &&
                (m_pkVertices[iVertex].m_kColor.a < 1.0f))
                m_bHasAlpha = true;

            m_pkVertices[iVertex].m_kColor.Clamp();


            // Handle Negative Colors
            m_pkVertices[iVertex].m_kColor.r = max(0.0f, 
                m_pkVertices[iVertex].m_kColor.r);

            m_pkVertices[iVertex].m_kColor.g = max(0.0f, 
                m_pkVertices[iVertex].m_kColor.g);

            m_pkVertices[iVertex].m_kColor.b = max(0.0f,
                m_pkVertices[iVertex].m_kColor.b);

            m_pkVertices[iVertex].m_kColor.a = max(0.0f, 
                m_pkVertices[iVertex].m_kColor.a);

            iVertex++;

        }
    }


    NIASSERT(iVertex == (int)m_uiNiVertices);

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriShape::GetNormsForNi(int iShapeID, int iGroupID )
{
    // Translate the Normals
    int iNumNormals;
    DtVec3f *pDtNormal;
    
    if (!DtGroupGetExplodedNormals( iShapeID, iGroupID,&iNumNormals, 
        &pDtNormal))
    {
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("ERROR: Object [%s]. " \
            "DtGroupGetExplodedNormals Failed!\n",szShapeName);

        return false;
    }

    NIASSERT(iNumNormals == (int)m_uiNiVertices);

    for (int i=0; i < (int)m_uiNiVertices; i++)
    {
        m_pkVertices[i].m_kNormal.x = pDtNormal[i].vec[0];
        m_pkVertices[i].m_kNormal.y = pDtNormal[i].vec[1];
        m_pkVertices[i].m_kNormal.z = pDtNormal[i].vec[2];

        // Unitize the normal
        NiPoint3::UnitizeVector(m_pkVertices[i].m_kNormal);
    }


    // Deallocate memory used for pDtNormal
    if (pDtNormal)
        NiFree(pDtNormal);
    pDtNormal = NULL;

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriShape::GetUVsForNi(int iShapeID, int iGroupID )
{

    int iUVSetCount = 0;
    DtShapeGetTextureUVCount(iShapeID, &iUVSetCount);

    for (int iUVSet = 0; iUVSet < iUVSetCount; iUVSet++)
    {
        int iUVVertCount = 0;
        DtVec2f * pkUVs = 0;
        if (!DtGroupGetExplodedTextureVerticesByIndex(iShapeID, iGroupID, 
            iUVSet, &iUVVertCount, &pkUVs))
        {
            return false;
        }

        NIASSERT(iUVVertCount == (int)m_uiNiVertices);
        for (int i=0; i < (int)m_uiNiVertices; i++)
        {
            m_pkVertices[i].m_uiNumUVs = iUVSetCount;
            m_pkVertices[i].m_akUV[iUVSet].x = pkUVs[i].vec[0];
            m_pkVertices[i].m_akUV[iUVSet].y = pkUVs[i].vec[1];
        }

        // Deallocate memory used for pkUVs
        if (pkUVs)
            NiFree(pkUVs);
        pkUVs = NULL;
    }
    return true;
}
//---------------------------------------------------------------------------
bool MyiTriShape::GetTriIndexListForNi(int iShapeID, int iGroupID)
{
    int iNumOfGrps = DtGroupGetCount( iShapeID );
    
    if (iNumOfGrps < 1)
    {
        // Failed Assumption...
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("ERROR: Object [%s]. " \
            "Failed Assumption - DtGroupGetCount return 0.\n", szShapeName);

        return false;
    }
    
    m_uiTriListLength = 0;

//    NIASSERT(iNumOfGrps == 1);

    int *piShpIndexList;
    int *piIndexList = NULL;
    int iIndexCnt;

    m_uiNumIndices = 0;

    // Current assumption is that iNumOfGroups is actually "1" and
    // that this loop will really only be called once.
    // We are not supporting multiple groups, yet.
    //
    // Originally, the purpose of this for loop was this:  Maya
    // separates the face list into groups.  I was rebuilding a
    // single shape from Maya's multiple shapes.  In effect, I
    // could take a shape that has two textures and create a
    // a single shape out of it (although, it would only have
    // one texture.
    
    // UPDATE: Loop has been done away with because we want different
    // groups to actually be different tri-shapes - we do this so that
    // we can support maya meshes with multiple textures.

    int iGrp=iGroupID;
    //for (int iGrp=0; iGrp<iNumOfGrps; iGrp++)
    //{
        // Note that all this function is "really" doing in most cases is
        // generating a sequential list of numbers like 0,1,2,3,4,... etc.
        // because, remember, we have created 1 index for *every* vertex,
        // so our indexs naturally will just count upward.  The function
        // itself is more complicated - but generates this sequence.  If
        // *multiple* groups are ever supported, this function might need
        // to be tweaked.
        dtPolygonGetExplodedFaceList( iShapeID, iGrp, &piShpIndexList, 
            &iIndexCnt );

        int iOldTotalSize = (int)m_uiTriListLength;

        m_uiTriListLength += iIndexCnt;

        int *pi = NiAlloc(int, m_uiTriListLength);
        if (pi==NULL)
        {
            const char *szShapeName;
            DtShapeGetShapeName( iShapeID, &szShapeName );
            DtExt_Err("ERROR: Object [%s]. " \
                "MyiTriShape Constructor.  pi Memory Allocation Failed\n",
                szShapeName);

            NIASSERT(0);
            if (piIndexList)
                NiFree(piIndexList);
            piIndexList = NULL;
            return false;
        }

        int i;

        for(i=0;i<iOldTotalSize;i++)
            pi[i] = piIndexList[i];

        for(i=0;i<iIndexCnt;i++)
            pi[iOldTotalSize+i] = piShpIndexList[i];

        if (piIndexList)
            NiFree(piIndexList);
        
        piIndexList = pi;
    //}


    NIASSERT(m_puiTriList == NULL);
    m_puiTriList = NiAlloc(unsigned int, m_uiTriListLength);

    for (i=0; i<(int)m_uiTriListLength; i++)
    {
        if (piIndexList[i] >= 0)
        {
            m_puiTriList[m_uiNumIndices++] = piIndexList[i];
        }
    }


    if (piIndexList)
        NiFree(piIndexList);
    piIndexList = NULL;

    if (piShpIndexList)
        NiFree(piShpIndexList);
    piShpIndexList = NULL;

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriShape::GetVertsForNi(int iShapeID, int iGroupID)
{
    DtVec3f *dtVertexList;

    // Get the Vertices from the MDtAPI... actually,
    // DtGroupGetExplodedVertices is a custom function that
    // Explodes the shape so that each index references a 
    // unique index.
    //
    // The reason this was needed is because the NiTriShape
    // constructor requires that the number of UV coords equal
    // the number of model coords.  In our case, the UV coords
    // were greater because the model coords were shared.
    int iverts;
    if (!DtGroupGetExplodedVertices( iShapeID, iGroupID, &iverts, 
        &dtVertexList ))
    {
        NIASSERT(0);
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("ERROR: Unable to explode vertices from object [%s].\n",
            szShapeName);
        return false;
    }

    m_uiNiVertices = iverts;

    // Now we need to Translate the dtVertexList to our Ni format.
    NIASSERT(m_uiNiVertices > 0);

    // Check the number of vertices on the object.
    // As a note, Maya Quads are turned into Triangles. The vertices of those
    // Triangles become unshared (exploded). So, as an example: 
    // 10,922 Quads => 21,844 Tris => 65,532 Verts
    // To be absolutely safe, a check against 65,536 should be made.
    // The code below, however, only "warns" the user that they have a large
    // number of vertices. A second test against 196,608 is really the MAX
    // number of vertices that could possibly be handled given the current
    // flow of code.
    if (m_uiNiVertices >= 65536) 
    {
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("WARNING: The object [%s] has an excessive number of " \
            "vertices. This could cause the export to fail. " \
            "Please take steps to reduce the count.\n", szShapeName);
    }
    else if (m_uiNiVertices >= 196608) // (65536 * 3)
    {
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("ERROR: The object [%s] has an excessive number of " \
            "vertices and is failing to export. " \
            "Please take steps to reduce the count.\n", szShapeName);

        // This is just too large, so just fail.
        m_pkVertices = NULL;
        return false;
    }

    // The consturctor for NiOptVertex is very slow and making thousands
    // of function calls is also very slow. Hear we allocate in a single
    // call THEN clear Loop through "Constructing" the vertices
    try
    {
        m_pkVertices = NiNew NiOptVertex[m_uiNiVertices];
    }
    catch(...)
    {
        // There was an allocation error of some sort.     
        const char *szShapeName;
        DtShapeGetShapeName( iShapeID, &szShapeName );
        DtExt_Err("ERROR: The object [%s] has an excessive number of " \
            "vertices and is failing to export.\n", szShapeName);

        m_pkVertices = NULL;
        return false;
    }

    NiOptVertex* pkTempVertex = m_pkVertices;
    for(int iLoop = 0; iLoop < (int)m_uiNiVertices; iLoop++)
    {
        pkTempVertex->m_uiNumUVs = 0;
        pkTempVertex->m_uiNumVertexFloats = 0;
        pkTempVertex->m_uiNumBones = 0;
        pkTempVertex->m_puiBoneIndex = 0;
        pkTempVertex->m_pfWeight = 0;

        pkTempVertex->m_kPosition.x = NI_INFINITY;
        pkTempVertex->m_kPosition.y = NI_INFINITY;
        pkTempVertex->m_kPosition.z = NI_INFINITY;

        pkTempVertex->m_kNormal.x = NI_INFINITY;
        pkTempVertex->m_kNormal.y = NI_INFINITY;
        pkTempVertex->m_kNormal.z = NI_INFINITY;

        pkTempVertex->m_kColor.r = NI_INFINITY;
        pkTempVertex->m_kColor.g = NI_INFINITY;
        pkTempVertex->m_kColor.b = NI_INFINITY;
        pkTempVertex->m_kColor.a = NI_INFINITY;

        pkTempVertex++;
    }

    float fScaleX = m_fScale[0];
    float fScaleY = m_fScale[1];
    float fScaleZ = m_fScale[2];

    // Check for the current working units
    if(gExport.m_bUseCurrentWorkingUnits && (
        gExport.m_fLinearUnitMultiplier != 1.0f) )
    {
        fScaleX *= gExport.m_fLinearUnitMultiplier;
        fScaleY *= gExport.m_fLinearUnitMultiplier;
        fScaleZ *= gExport.m_fLinearUnitMultiplier;
    }

    // Modify by the scale and linear unit multiplier
    for (unsigned int i=0; i < m_uiNiVertices; i++)
    {
        m_pkVertices[i].m_kPosition.x = dtVertexList[i].vec[0] * fScaleX;
        m_pkVertices[i].m_kPosition.y = dtVertexList[i].vec[1] * fScaleY;
        m_pkVertices[i].m_kPosition.z = dtVertexList[i].vec[2] * fScaleZ;
    }

    // Deallocate memory for dtVertexList
    NiFree(dtVertexList); 
    dtVertexList = NULL;
    return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//    Code for checking Shading Model...
//
//  TAKE INTO ACCOUNT MAYA's SHADING MODEL
//    char *pszShadingModel;
//    DtMtlGetName( iShapeNum, 0, &pszShadingModel ); 
// Shape, group to get shading model name
//    
//    MObject MObj;
//    DtExt_ShapeGetShader( iShapeNum, 0, MObj);
//    const char *pszName = MObj.apiTypeStr();
//    switch (MObj.apiType())
//    {
//        case MFn::kLambert:
//        {
//            break;
//        }
//        case MFn::kBlinn:
//        {
//            break;            
//        }
//    }
//---------------------------------------------------------------------------

bool MyiTriShape::CreateMultiTexture( int iShapeID, int iGroupID, 
    MyiTextureManager* pkTextureManager,
    NiTexturingProperty *&pTexturingProperty,
    bool &bNeedsAlphaProperty)
{

    int iTextures;  // iTextures is the number of textures for the shape.
    if (!DtExt_ShapeGetTexCnt( iShapeID, &iTextures ))
    {
        NIASSERT(0);
        return false;
    }

    // If no textures exist, we set the texture property to NULL and exit.
    if (iTextures < 1)
    {
        pTexturingProperty = NULL;
        return true;
    }

    m_uiMultiIndex = 0;  // Start after base
    
    int iMaterialID;
    DtMtlGetID( iShapeID, iGroupID, &iMaterialID);
    int iNumMaps = DtExt_MtlGetNumTextureMapsByID(iMaterialID); 

    for(int iLoop = 0; iLoop < iNumMaps; iLoop++)
    {
        const char* pcMapName = 
            DtExt_MtlGetTextureMapNameByID(iMaterialID, iLoop);

        bool bNeedsAlpha = false;

        CreateMultiTextureForType(iShapeID, iGroupID, pkTextureManager, 
            pTexturingProperty, (char*)pcMapName, bNeedsAlpha);

        // Only check for alpha on the base map
        if (iLoop == 0)
        {
            bNeedsAlphaProperty |= bNeedsAlpha;
        }

    }
    BakeUVs(pTexturingProperty);

    return true;
}
//---------------------------------------------------------------------------

bool MyiTriShape::CreateMultiTexture( int iMaterialID, 
    MyiTextureManager* pkTextureManager,
    NiTexturingProperty *&pTexturingProperty,
    bool &bNeedsAlphaProperty)
{
    int iNumMaps = DtExt_MtlGetNumTextureMapsByID(iMaterialID); 

    for(int iLoop = 0; iLoop < iNumMaps; iLoop++)
    {
        const char* pcMapName = DtExt_MtlGetTextureMapNameByID(iMaterialID, 
            iLoop);

        bool bNeedsAlpha = false;

        CreateMultiTextureForType(iMaterialID, pkTextureManager, 
            pTexturingProperty, (char*)pcMapName, bNeedsAlpha);

        // Only check for alpha on the base map
        if (iLoop == 0)
        {
            bNeedsAlphaProperty |= bNeedsAlpha;
        }

    }


    return true;
}
//---------------------------------------------------------------------------
MStatus CreateDefaultTextureTransform(int mtlID, const char* pcTextureType, 
     NiTextureTransform*& pkTransform, bool bForceCreation)
{
    if (!pkTransform)
    {
        NiPoint2 kRepeatUV;
        NiPoint2 kOffsetUV;
        float fRotate;
        bool bSuccess;

        bSuccess = DtTextureGetRepeatOffsetRotateUVs(mtlID, pcTextureType,
            kRepeatUV.x, kRepeatUV.y, kOffsetUV.x, kOffsetUV.y, fRotate);

        NIASSERT(bSuccess);
       
        pkTransform = NiNew NiTextureTransform(NiPoint2(0.0f, 0.0f), 0.0f,
            NiPoint2(1.0f, 1.0f), NiPoint2(0.5f, 0.5f),
            NiTextureTransform::MAYA_TRANSFORM);

        pkTransform->SetScale(kRepeatUV);
        pkTransform->SetTranslate(kOffsetUV);
        pkTransform->SetRotate(fRotate);   
    }

    return MS::kSuccess;
}
//---------------------------------------------------------------------------
MStatus GetTextureTransform( int mtlID, const char* pcTextureType, 
    char* pcAttribute, NiTextureTransform*& pkTransform, 
    NiTextureTransformController*& pkUVController,
    NiTextureTransformController::TransformMember eMember, bool bInvert)
{

    // find shader from material ID
    MObject kMtlNode;
    int iStatus = DtExt_MtlGetShader( mtlID, kMtlNode );
    if (iStatus != 1)
        return MS::kFailure;
    
    MStatus kStatus;
    // get place2dTexture object for this texture
    MFnDependencyNode kPlace2d = DtExt_GetPlace2dTexture(kMtlNode,
        pcTextureType, &kStatus);

    // examine animatable parameters to see if they're animated
    MPlug kPlug = kPlace2d.findPlug(pcAttribute, &kStatus);

    // Go ahead and apply a texture transform per uv coordinate set.
    // We will optimize this out later...

    // Create the TextureTransform
    CreateDefaultTextureTransform(mtlID, pcTextureType, pkTransform, true);

    if (PlugIsConnected(kPlug)) 
    {
        float fScale = 1.0f;
        if (bInvert)
            fScale = -1.0f;

        unsigned int uiNumKeys;
        NiFloatKey* pkKeys;
        NiAnimationKey::KeyType eType;
        if (kPlug.isConnected() && 
            ConvertFloatAnim(kPlug, uiNumKeys, pkKeys, eType, fScale))
        {
            // Create the Animation Data
            NiFloatData* pkData = NiNew NiFloatData;
            pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);
            
            // Create the Interpolator
            NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator(pkData);
            pkInterp->Collapse();

            // create NiTextureTransformController
            pkUVController = NiNew NiTextureTransformController;
            pkUVController->SetAffectedMember(eMember);
            pkUVController->SetInterpolator(pkInterp);
            pkUVController->ResetTimeExtrema();

            SetAnimations(pkUVController);
        }
    }

    return MS::kSuccess;
}   

//---------------------------------------------------------------------------
NiTexturingProperty::ClampMode MyiTriShape::GetClampMode(char* pTextureName)
{
    
    int iHorzRepeat, iVertRepeat;
    DtTextureGetWrap( pTextureName, &iHorzRepeat, &iVertRepeat );
    
    NiTexturingProperty::ClampMode eClampMode;
    
    if (iHorzRepeat == DT_REPEAT)
    {
        if (iVertRepeat == DT_REPEAT)
            eClampMode = NiTexturingProperty::WRAP_S_WRAP_T;
        else
            eClampMode = NiTexturingProperty::WRAP_S_CLAMP_T;
    }
    else
    {
        if (iVertRepeat == DT_REPEAT)
            eClampMode = NiTexturingProperty::CLAMP_S_WRAP_T;
        else
            eClampMode = NiTexturingProperty::CLAMP_S_CLAMP_T;
    }
    
    return eClampMode;
}
//---------------------------------------------------------------------------
bool MyiTriShape::CreateMultiTextureForType(
    int iShapeID, 
    int iGroupID,
    MyiTextureManager* pkTextureManager,
    NiTexturingProperty *&pTexturingProperty,
    char *szType,
    bool &bNeedsAlphaProperty)
{
    int iTextureID;
    if (!DtTextureGetIDMulti( iShapeID, iGroupID, szType, &iTextureID ))
    {
        NIASSERT(0);
        return false;
    }

    if (iTextureID < 0)
    {
        //? pMultiTextureProperty = NULL;
        return true;
    }

    char *pTextureName;
    if (!DtTextureGetNameID(iTextureID,&pTextureName))
    {
        NIASSERT(0);
        return false;
    }


    // Assign the Texture to our NiTexturingProperty;
    NiTexturingProperty::Map *pMap;
    bool bShaderMap = false;
    
    bool bBumpMap = strcmp(szType, "BumpMap") == 0;
    bool bParallaxMap = strcmp(szType, "ParallaxMap") == 0;

    if (!bBumpMap && !bParallaxMap)
    {
        if(strlen(szType) < strlen("Ni_PixelShaderAttribute_"))
            pMap = NiNew NiTexturingProperty::Map();
        else
        {
            pMap = (NiTexturingProperty::Map*)NiNew 
                NiTexturingProperty::ShaderMap();

            bShaderMap = true;
        }
    }
    else if (bBumpMap)
    {
        pMap = (NiTexturingProperty::Map*)(
            NiNew NiTexturingProperty::BumpMap());
    }
    else if (bParallaxMap)
    {
        pMap = (NiTexturingProperty::Map*)(
            NiNew NiTexturingProperty::ParallaxMap());
    }



    // Cube Maps are only valid in the shader slot
    if (NiIsKindOf(NiSourceCubeMap, 
        pkTextureManager->GetTexture(iTextureID)) && !bShaderMap)
    {
        DtExt_Err(
            "ERROR: Texture %s: Is a Cube Map Texture in the %s. Cube "
            "Maps are only valid in as Dynamic Effects or Shader Maps.\n", 
            pTextureName, szType);
        NiDelete pMap;
        return false;
    }


    pMap->SetTexture( pkTextureManager->GetTexture( iTextureID ) );


    // If any of the textures have Alpha then we need and Alpha property
    bNeedsAlphaProperty |= pkTextureManager->NeedsAlphaProperty(iTextureID);

    if (pTexturingProperty == NULL)
        pTexturingProperty = NiNew NiTexturingProperty;

    // handle animated UVs
    int iMtlID;
    DtMtlGetID( iShapeID, iGroupID, &iMtlID);

    NiTextureTransform* pkTexTransform = NULL;
    NiTextureTransformController* pkTexControllerTU = NULL;
    NiTextureTransformController* pkTexControllerTV = NULL;
    NiTextureTransformController* pkTexControllerSU = NULL;
    NiTextureTransformController* pkTexControllerSV = NULL;
    NiTextureTransformController* pkTexControllerR = NULL;

    GetTextureTransform(iMtlID, szType, "repeatU", pkTexTransform, 
        pkTexControllerSU, NiTextureTransformController::TT_SCALE_U, false);

    GetTextureTransform(iMtlID, szType, "repeatV", pkTexTransform, 
        pkTexControllerSV, NiTextureTransformController::TT_SCALE_V,
        false);

    GetTextureTransform(iMtlID, szType, "offsetU", pkTexTransform, 
        pkTexControllerTU, NiTextureTransformController::TT_TRANSLATE_U, 
        false);

    GetTextureTransform(iMtlID, szType, "offsetV", pkTexTransform,
        pkTexControllerTV, NiTextureTransformController::TT_TRANSLATE_V,
        false);

    GetTextureTransform(iMtlID, szType, "rotateUV", pkTexTransform, 
        pkTexControllerR, NiTextureTransformController::TT_ROTATE, false);


    if (!pkTexTransform)
    {
        CreateDefaultTextureTransform(iMtlID, szType, pkTexTransform, false);
    }

    int iUVIndex = AssignUVs( iShapeID, iGroupID, pTextureName, iTextureID);


    // If we optimized the UV then use that UV set else use the one we just
    // created.
    if (iUVIndex >= 0)
    {
        pMap->SetTextureIndex((unsigned int)iUVIndex);
    }
    else
    {
        return false;
    }


    // Get the Repeat values (Does texture repeat or clamp?)
    // Check against DT_REPEAT, DT_CLAMP
    NiTexturingProperty::ClampMode eClampMode = GetClampMode(pTextureName);

    if (eClampMode != NiTexturingProperty::WRAP_S_WRAP_T)
    {
        pMap->SetClampMode(eClampMode);
    }



    // APPLY THE FILTER MODE AND L AND K VALUES FROM THE MATERIAL
    char cFilterMode;
    char cMap;


    // Assign the Filter Mode based on the global settings
    pMap->SetFilterMode( (NiTexturingProperty::FilterMode)
        gExport.m_iFilterType );

    // Check for Material Properties
    if(DtTextureGetMaterialMapProperties(iShapeID, iGroupID, szType, 
        cFilterMode, cMap))
    {
        // Only set the Filter mode if it's not the default value
        if(cFilterMode != -1)
            pMap->SetFilterMode(
                (NiTexturingProperty::FilterMode)cFilterMode);
    }


    // APPLY THE Map AND check for L and K extra attributes
    char* pcMaterialName;
    DtMtlGetName(iShapeID, iGroupID, &pcMaterialName);
    
    if (strcmp(szType,"color")==0)
    {
        pTexturingProperty->SetBaseMap(pMap);
    }
    else if (strcmp(szType,"DarkMap")==0)
    {
        pTexturingProperty->SetDarkMap(pMap);
    }
    else if (strcmp(szType,"DetailMap")==0)
    {
        pTexturingProperty->SetDetailMap(pMap);
    }
    else if (strcmp(szType,"Decal1Map")==0)
    {
        pTexturingProperty->SetDecalMap(0, pMap);
    }
    else if (strcmp(szType,"Decal2Map")==0)
    {
        pTexturingProperty->SetDecalMap(1, pMap);
    }
    else if (strcmp(szType,"GlowMap")==0)
    {
        pTexturingProperty->SetGlowMap(pMap);
    }
    else if (strcmp(szType,"GlossMap")==0)
    {
        pTexturingProperty->SetGlossMap(pMap);
    }
    else if (strcmp(szType,"BumpMap")==0)
    {
        float fBumpMapLuminaScale = 0.0f;
        float fBumpMapLuminaOffset = 0.0f;
        float fBumpMapMat00 = 0.0f;
        float fBumpMapMat01 = 0.0f;
        float fBumpMapMat10 = 0.0f;
        float fBumpMapMat11 = 0.0f;

        DtTextureGetMaterialBumpMapProperties( iShapeID, iGroupID,
            fBumpMapLuminaScale, fBumpMapLuminaOffset, fBumpMapMat00, 
            fBumpMapMat01, fBumpMapMat10, fBumpMapMat11);

        ((NiTexturingProperty::BumpMap*)pMap)->SetLumaScale(
            fBumpMapLuminaScale);

        ((NiTexturingProperty::BumpMap*)pMap)->SetLumaOffset(
            fBumpMapLuminaOffset);

        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat00(fBumpMapMat00);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat01(fBumpMapMat01);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat10(fBumpMapMat10);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat11(fBumpMapMat11);

        pTexturingProperty->SetBumpMap((NiTexturingProperty::BumpMap*)pMap);
    }
    else if (strcmp(szType,"NormalMap")==0)
    {
        pTexturingProperty->SetNormalMap(pMap);
    }
    else if (strcmp(szType,"ParallaxMap")==0)
    {
        float fOffset;
        DtTextureGetMaterialParallaxMapProperties( iShapeID, iGroupID, 
            fOffset);
        ((NiTexturingProperty::ParallaxMap*)pMap)->SetOffset(fOffset);
        pTexturingProperty->SetParallaxMap((NiTexturingProperty::ParallaxMap*)
            pMap);
    }
    else
    {
        pTexturingProperty->SetShaderMap(cMap, 
            (NiTexturingProperty::ShaderMap*)pMap);
    }


    // Setup the Texture Transforms and Animations if necessary
    if (pkTexTransform)
    {
        pMap->SetTextureTransform(pkTexTransform);
        
        if (pkTexControllerTU)
        {
            pkTexControllerTU->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerTV)
        {
            pkTexControllerTV->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerSU)
        {
            pkTexControllerSU->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerSV)
        {
            pkTexControllerSV->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerR)
        {
            pkTexControllerR->SetMap(pMap, pTexturingProperty);
        }
    }

        // Create the Flip Book Properties for this texture
    CreateFlipBookProperty(iShapeID, iGroupID, iTextureID, 
            szType, pkTextureManager, pMap, pTexturingProperty);

    return true;
}
//---------------------------------------------------------------------------
bool MyiTriShape::CreateMultiTextureForType(
    int iMaterialID,
    MyiTextureManager* pkTextureManager,
    NiTexturingProperty *&pTexturingProperty,
    char *szType,
    bool &bNeedsAlphaProperty)
{
    int iTextureID;
    if (!DtTextureGetIDMulti( iMaterialID, szType, &iTextureID ))
    {
        NIASSERT(0);
        return false;
    }

    if (iTextureID < 0)
    {
        //? pMultiTextureProperty = NULL;
        return true;
    }

    char *pTextureName;
    if (!DtTextureGetNameID(iTextureID,&pTextureName))
    {
        NIASSERT(0);
        return false;
    }


    // Assign the Texture to our NiTexturingProperty;
    NiTexturingProperty::Map *pMap;
    bool bShaderMap = false;
    
    bool bBumpMap = strcmp(szType, "BumpMap") == 0;
    bool bParallaxMap = strcmp(szType, "ParallaxMap") == 0;

    if (!bBumpMap && !bParallaxMap)
    {
        if(strlen(szType) < strlen("Ni_PixelShaderAttribute_"))
            pMap = NiNew NiTexturingProperty::Map();
        else
        {
            pMap = (NiTexturingProperty::Map*)NiNew 
                NiTexturingProperty::ShaderMap();

            bShaderMap = true;
        }
    }
    else if (bBumpMap)
    {
        pMap = (NiTexturingProperty::Map*)(
            NiNew NiTexturingProperty::BumpMap());
    }
    else if (bParallaxMap)
    {
        pMap = (NiTexturingProperty::Map*)(
            NiNew NiTexturingProperty::ParallaxMap());
    }




    // Cube Maps are only valid in the shader slot
    if (NiIsKindOf(NiSourceCubeMap, 
        pkTextureManager->GetTexture(iTextureID)) && !bShaderMap)
    {
        DtExt_Err(
            "ERROR: Texture %s: Is a Cube Map Texture in the %s. Cube "
            "Maps are only valid in as Dynamic Effects or Shader Maps.\n", 
            pTextureName, szType);
        NiDelete pMap;
        return false;
    }


    pMap->SetTexture( pkTextureManager->GetTexture( iTextureID ) );


    // If any of the textures have Alpha then we need and Alpha property
    bNeedsAlphaProperty |= pkTextureManager->NeedsAlphaProperty(iTextureID);

    if (pTexturingProperty == NULL)
        pTexturingProperty = NiNew NiTexturingProperty;

    // handle animated UVs
    NiTextureTransform* pkTexTransform = NULL;
    NiTextureTransformController* pkTexControllerTU = NULL;
    NiTextureTransformController* pkTexControllerTV = NULL;
    NiTextureTransformController* pkTexControllerSU = NULL;
    NiTextureTransformController* pkTexControllerSV = NULL;
    NiTextureTransformController* pkTexControllerR = NULL;

    GetTextureTransform(iMaterialID, szType, "repeatU", pkTexTransform, 
        pkTexControllerSU, NiTextureTransformController::TT_SCALE_U, false);

    GetTextureTransform(iMaterialID, szType, "repeatV", pkTexTransform, 
        pkTexControllerSV, NiTextureTransformController::TT_SCALE_V,
        false);

    GetTextureTransform(iMaterialID, szType, "offsetU", pkTexTransform, 
        pkTexControllerTU, NiTextureTransformController::TT_TRANSLATE_U, 
        false);

    GetTextureTransform(iMaterialID, szType, "offsetV", pkTexTransform,
        pkTexControllerTV, NiTextureTransformController::TT_TRANSLATE_V,
        false);

    GetTextureTransform(iMaterialID, szType, "rotateUV", pkTexTransform, 
        pkTexControllerR, NiTextureTransformController::TT_ROTATE, false);

    if (!pkTexTransform)
    {
        CreateDefaultTextureTransform(iMaterialID, szType, pkTexTransform, 
            false);
    }

    // Always use the base texture UVs
    pMap->SetTextureIndex( 0 );

    // Get the Repeat values (Does texture repeat or clamp?)
    // Check against DT_REPEAT, DT_CLAMP
    NiTexturingProperty::ClampMode eClampMode = GetClampMode(pTextureName);

    if (eClampMode != NiTexturingProperty::WRAP_S_WRAP_T)
    {
        pMap->SetClampMode(eClampMode);
    }



    // APPLY THE FILTER MODE FROM THE MATERIAL
    char cFilterMode;
    char cMap;


    // Assign the Filter Mode based on the global settings
    pMap->SetFilterMode( 
        (NiTexturingProperty::FilterMode)gExport.m_iFilterType );

    // Check for Material Properties
    if(DtTextureGetMaterialMapProperties(iMaterialID, szType, cFilterMode, 
            cMap))
    {
        // Only set the Filter mode if it's not the default value
        if(cFilterMode != -1)
            pMap->SetFilterMode(
            (NiTexturingProperty::FilterMode)cFilterMode);
    }


    // APPLY THE Map 
    char* pcMaterialName;
    DtMtlGetNameByID(iMaterialID, &pcMaterialName);
    
    if (strcmp(szType,"color")==0)
    {
        pTexturingProperty->SetBaseMap(pMap);
    }
    else if (strcmp(szType,"DarkMap")==0)
    {
        pTexturingProperty->SetDarkMap(pMap);
    }
    else if (strcmp(szType,"DetailMap")==0)
    {
        pTexturingProperty->SetDetailMap(pMap);
    }
    else if (strcmp(szType,"Decal1Map")==0)
    {
        pTexturingProperty->SetDecalMap(0, pMap);
    }
    else if (strcmp(szType,"Decal2Map")==0)
    {
        pTexturingProperty->SetDecalMap(1, pMap);
    }
    else if (strcmp(szType,"GlowMap")==0)
    {
        pTexturingProperty->SetGlowMap(pMap);
    }
    else if (strcmp(szType,"GlossMap")==0)
    {
        pTexturingProperty->SetGlossMap(pMap);
    }
    else if (strcmp(szType,"BumpMap")==0)
    {
        float fBumpMapLuminaScale = 0.0f;
        float fBumpMapLuminaOffset = 0.0f;
        float fBumpMapMat00 = 0.0f;
        float fBumpMapMat01 = 0.0f;
        float fBumpMapMat10 = 0.0f;
        float fBumpMapMat11 = 0.0f;

        DtTextureGetMaterialBumpMapProperties( iMaterialID,
            fBumpMapLuminaScale, fBumpMapLuminaOffset, fBumpMapMat00, 
            fBumpMapMat01, fBumpMapMat10, fBumpMapMat11);

        ((NiTexturingProperty::BumpMap*)pMap)->SetLumaScale(
            fBumpMapLuminaScale);

        ((NiTexturingProperty::BumpMap*)pMap)->SetLumaOffset(
            fBumpMapLuminaOffset);

        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat00(fBumpMapMat00);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat01(fBumpMapMat01);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat10(fBumpMapMat10);
        ((NiTexturingProperty::BumpMap*)pMap)->SetBumpMat11(fBumpMapMat11);

        pTexturingProperty->SetBumpMap((NiTexturingProperty::BumpMap*)pMap);
    }
    else if (strcmp(szType,"NormalMap")==0)
    {
        pTexturingProperty->SetNormalMap(pMap);
    }
    else if (strcmp(szType,"ParallaxMap")==0)
    {
        float fOffset;
        DtTextureGetMaterialParallaxMapProperties( iMaterialID, 
            fOffset);
        ((NiTexturingProperty::ParallaxMap*)pMap)->SetOffset(fOffset);
        pTexturingProperty->SetParallaxMap((NiTexturingProperty::ParallaxMap*)
            pMap);
    }
    else
    {
        pTexturingProperty->SetShaderMap(cMap, 
            (NiTexturingProperty::ShaderMap*)pMap);
    }


    // Setup the Texture Transforms and Animations if necessary
    if (pkTexTransform)
    {
        pMap->SetTextureTransform(pkTexTransform);
        
        if (pkTexControllerTU)
        {
            pkTexControllerTU->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerTV)
        {
            pkTexControllerTV->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerSU)
        {
            pkTexControllerSU->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerSV)
        {
            pkTexControllerSV->SetMap(pMap, pTexturingProperty);
        }
        
        if (pkTexControllerR)
        {
            pkTexControllerR->SetMap(pMap, pTexturingProperty);
        }
    }

        // Create the Flip Book Properties for this texture
    CreateFlipBookProperty(iMaterialID, iTextureID, 
            szType, pkTextureManager, pMap, pTexturingProperty);

    return true;
}
//---------------------------------------------------------------------------
int MyiTriShape::AssignUVs(int iShapeID, int iGroupID, char *pTextureName,
    int iTextureID)
{
    // Find the correct, previously exported UV set
    MObject kTextureNode;
    DtTextureGetTextureNodeID( iTextureID, kTextureNode);

    char pcUVSetName[256] = "";
    DtShapeGetUVSetNameForTexture( iShapeID, kTextureNode, pcUVSetName, 256);

    int iSetIndex = DtExt_ShapeGetUVSetByName( iShapeID, pcUVSetName );

    return iSetIndex;
}
//---------------------------------------------------------------------------
bool MyiTriShape::HasAnimatedTextureTransform(NiTexturingProperty* pkProperty,
    NiTexturingProperty::Map* pkMap)
{
    if (!pkProperty || !pkMap)
        return false;

    NiTimeController* pkController = pkProperty->GetControllers();

    while (pkController)
    {
        if (NiIsKindOf(NiTextureTransformController, pkController))
        {
            NiTextureTransformController* pkTTCont = 
                (NiTextureTransformController*) pkController;

            NIASSERT(pkTTCont->GetMap() != NULL);

            if (pkTTCont->GetMap() == pkMap)
                return true;
        }
        pkController = pkController->GetNext();
    }

    return false;
}
//---------------------------------------------------------------------------
void MyiTriShape::BakeUVs(NiTexturingProperty* pkTexProp)
{

    if (!pkTexProp || m_uiNiVertices == 0)
        return;

    // Build an array containing all maps in this property.
    NiTPrimitiveArray<NiTexturingProperty::Map*> kMapArray;
    const NiTexturingProperty::NiMapArray& kTempMaps = pkTexProp->GetMaps();
    unsigned int uiTempMapCount = kTempMaps.GetSize();
    for (unsigned int ui = 0; ui < uiTempMapCount; ui++)
    {
        if (kTempMaps.GetAt(ui))
        {
            kMapArray.Add(kTempMaps.GetAt(ui));
        }
    }

    for (unsigned int uj = 0; uj < pkTexProp->GetShaderArrayCount(); uj++)
    {
        if (pkTexProp->GetShaderMap(uj))
            kMapArray.Add(pkTexProp->GetShaderMap(uj));
    }

    // For each map, check to see if it uniquely points to a UV coordinate set
    // or if the UV transform is the same for ALL of them.
    // If so and the texture transform is NOT animated, get its texture 
    // transform matrix and apply it to all UV's in that set and then remove 
    // the texture transform. Otherwise, we will need to keep the texture
    // transforms around.


    unsigned int uiMapSize = kMapArray.GetSize();
    for (unsigned int uiIdx = 0; uiIdx < uiMapSize; uiIdx++)
    {
        NiTexturingProperty::Map* pkMap = kMapArray.GetAt(uiIdx);
        if (pkMap)
        {
            unsigned int uiMapUVSet = pkMap->GetTextureIndex();
            NiTextureTransform* pkTexTransform = pkMap->GetTextureTransform();
            bool bAnimated = HasAnimatedTextureTransform(pkTexProp, pkMap);
            if (bAnimated)
                continue;

            bool bUnique = true;

            for (unsigned int uiCheckIdx = 0; uiCheckIdx < uiMapSize; 
                uiCheckIdx++)
            {
                // Don't compare against yourself
                if (uiCheckIdx != uiIdx)
                {
                    NiTexturingProperty::Map* pkOtherMap = 
                        kMapArray.GetAt(uiCheckIdx);
                    
                    if (!pkOtherMap)
                        continue;

                    NIASSERT(pkOtherMap != pkMap);
                    NiTextureTransform* pkOtherTexTransform = 
                        pkOtherMap->GetTextureTransform();
                    unsigned int uiOtherUVSet = pkOtherMap->GetTextureIndex();

                    if (uiMapUVSet == uiOtherUVSet)
                    {
                        if (HasAnimatedTextureTransform(pkTexProp, pkOtherMap))
                        {
                            bUnique = false;
                            break;
                        }
                        else if ((pkOtherTexTransform != NULL && 
                            pkTexTransform == NULL) ||
                            (pkOtherTexTransform == NULL && 
                            pkTexTransform != NULL))
                        {
                            bUnique = false;
                            break;
                        }
                        else if (pkOtherTexTransform != NULL && 
                          pkTexTransform != NULL && 
                          *(pkOtherTexTransform) != *(pkTexTransform))
                        {
                            bUnique = false;
                            break;
                        }
                    }
                }
            }

            if (!bUnique)
                continue;

            // We know that we have a unique mapping, bake the UV's
            if (pkTexTransform != NULL)
            {
                NiMatrix3 kMatrix = *(pkTexTransform->GetMatrix());
                for (int i = 0; i < (int)m_uiNiVertices; i++)
                {
                    NiPoint3 kPt(m_pkVertices[i].m_akUV[uiMapUVSet].x,
                        m_pkVertices[i].m_akUV[uiMapUVSet].y, 1.0f);
                    kPt = kMatrix * kPt;
                    m_pkVertices[i].m_akUV[uiMapUVSet].x = kPt.x;
                    m_pkVertices[i].m_akUV[uiMapUVSet].y = kPt.y;
                }

                // Because the TextureTransform may have been the
                // same on multiple maps, we need to set ALL back to NULL
                for (unsigned int uk = 0; uk < uiMapSize; uk++)
                {
                    NiTexturingProperty::Map* pkTestMap = kMapArray.GetAt(uk);
                    if (pkTestMap && pkTestMap->GetTextureIndex() == 
                        uiMapUVSet)
                    {
                        NiTextureTransform* pkOldTT = 
                            pkTestMap->GetTextureTransform();
                        pkTestMap->SetTextureTransform(NULL);
                        NiDelete pkOldTT;
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
NiPoint3* MyiTriShape::GetUndeformedBaseVertices(
    const MFnBlendShapeDeformer& kBlendShape, 
    const unsigned int* puiNewToOld, const int* piFaceList)
{
    // get undeformed base mesh object from blendshape deformer
    MFnMesh kMesh = GetBaseMesh(kBlendShape);

    // get vertices from mesh object
    MPointArray kPointArray;
    kMesh.getPoints(kPointArray);

    // convert to exploded, collapsed, undeformed verts
    return GetCollapsedVertices(kPointArray, puiNewToOld, piFaceList);

}
//---------------------------------------------------------------------------
NiPoint3* MyiTriShape::GetCollapsedVertices(
    const MPointArray& kPointArray, 
    const unsigned int* puiNewToOld, const int* piFaceList)
{
    // get exploded, collapsed, undeformed verts 
   
    unsigned int uiVertices = m_spTriShape->GetVertexCount();
    NiPoint3* pkVertices = NiNew NiPoint3[uiVertices];

    int iPointArrayLen = (int)kPointArray.length();

    for (unsigned int ui = 0; ui < uiVertices; ui++)
    {
        unsigned int uiExploded = puiNewToOld[ui];
        // faces are separated by -1.  assume 3 verts per face.
        uiExploded = uiExploded + (uiExploded / 3);
        int iOriginalIndex = piFaceList[uiExploded];
        NIASSERT(iOriginalIndex != DtEND_OF_FACE);
        NIASSERT(iOriginalIndex < (int)kPointArray.length());

        // Check for overflowing the array
        if (iOriginalIndex >= iPointArrayLen)
        {
            // An error has been detected. Return NULL.
            NiDelete[] pkVertices;
            return NULL;
        }

        MPoint kPoint = kPointArray[iOriginalIndex];


        pkVertices[ui].x = (float)kPoint.x;
        pkVertices[ui].y = (float)kPoint.y;
        pkVertices[ui].z = (float)kPoint.z;

        if(gExport.m_bUseCurrentWorkingUnits && 
            (gExport.m_fLinearUnitMultiplier != 1.0f) )
        {
            pkVertices[ui].x *= gExport.m_fLinearUnitMultiplier;
            pkVertices[ui].y *= gExport.m_fLinearUnitMultiplier;
            pkVertices[ui].z *= gExport.m_fLinearUnitMultiplier;
        }
        
    }


    return pkVertices;
}
//---------------------------------------------------------------------------
MFnMesh MyiTriShape::GetBaseMesh(const MFnBlendShapeDeformer& kBlendShape)
{
    MPlug kInput = kBlendShape.findPlug("input");

    // currently only support 1 group 
    MPlug kInputGeometry = kInput[0].child(0);

    MPlugArray kPlugArr;
    MStatus kStatus;
    kInputGeometry.connectedTo(kPlugArr, true, false, &kStatus);

    NIASSERT(kPlugArr.length() == 1);

    MObject kDestObj = kPlugArr[0].node();

    // find mesh object
    MItDependencyGraph::Direction direction = MItDependencyGraph::kUpstream;
    MItDependencyGraph::Traversal traversalType = 
        MItDependencyGraph::kBreadthFirst;

    MItDependencyGraph::Level level = MItDependencyGraph::kNodeLevel;
    MFn::Type filter = MFn::kMesh;

    MItDependencyGraph dgIter( kDestObj, filter, direction,
                            traversalType, level, &kStatus);
                            
    // return first kMesh object found 
    if (!dgIter.isDone())
    {        
        MObject kMeshObj = dgIter.thisNode( &kStatus );
        MFnMesh kMesh(kMeshObj);
        NIASSERT(kMesh.isIntermediateObject());
        return kMesh;
    }
    else
    {
        MFnMesh kNone;
        return kNone;
    }
}
//---------------------------------------------------------------------------
bool MyiTriShape::GetBlendShape(int iShapeID, int iGroupID, 
                                MObject& BlendShape)
{
    MStatus kStatus;

    // Get Shape Node
    MObject kShapeNode;
    if (!DtExt_ShapeGetShapeNode(iShapeID, kShapeNode))
    {
        return false;
    }
    
    // Look for an MFnBlendShapeDeformer attached to the inMesh plug
    // of the shape

    MFnDependencyNode kShapeDepend(kShapeNode, &kStatus);
    MPlug kMapPlug = kShapeDepend.findPlug("inMesh");

    if (!kMapPlug.isConnected())
    {
        return false;
    }

    MPlugArray kPlugArr;
    kMapPlug.connectedTo(kPlugArr, true, false, &kStatus);

    // I think the apiType of kMesh guarantees
    // that there will only be one connection, but
    // I'm not sure.  EH.
    if (kPlugArr.length() != 1)
        return false;

    bool bMeshError = false;
    bool bFirstModifier = true;

    // Get the type of this node
    MFn::Type kType = kPlugArr[0].node().apiType();

    // Skip over any polyModifiers, Skin Clusters, or group parts
    while ((kType == MFn::kPolyTweakUV) ||
           (kType == MFn::kPolyTriangulate) ||
           (kType == MFn::kPolyCollapseEdge) ||
           (kType == MFn::kPolyCollapseF) ||
           (kType == MFn::kPolyDelVertex) ||
           (kType == MFn::kPolySplitEdge) ||
           (kType == MFn::kPolyCloseBorder) ||
           (kType == MFn::kPolyDelFacet) ||
           (kType == MFn::kPolyMapCut) ||
           (kType == MFn::kPolyMapDel) ||
           (kType == MFn::kPolyMapSew) ||
           (kType == MFn::kPolySmooth) ||
           (kType == MFn::kPolyDelEdge) ||
           (kType == MFn::kPolyFlipEdge) ||
           (kType == MFn::kPolyAppendVertex) ||
           (kType == MFn::kPolyMergeFacet) ||
           (kType == MFn::kPolyNormal) ||
           (kType == MFn::kPolySubdFacet) ||
           (kType == MFn::kPolyTransfer) ||
           (kType == MFn::kPolyTweak) ||
#ifndef MAYA50
           (kType == MFn::kPolySmoothProxy) ||
#endif
           (kType == MFn::kPolySmoothFacet) ||
           (kType == MFn::kPolyReduce) ||
           (kType == MFn::kPolyAppend) ||
           (kType == MFn::kPolyMergeEdge) ||
           (kType == MFn::kPolyColorPerVertex) ||
           (kType == MFn::kPolyMoveUV) ||
           (kType == MFn::kPolyNormalPerVertex) ||
           (kType == MFn::kPolySplit) ||
           (kType == MFn::kSkinClusterFilter) ||
           (kType == MFn::kGroupParts) ||
           (kType == MFn::kDeleteComponent) ||
           (kType == MFn::kPolySoftEdge))

    {
        char* pcPlugName = NULL;

        // determine the input plug name
        switch (kType)
        {
        case MFn::kPolyTweakUV:
        case MFn::kPolyMapCut:
        case MFn::kPolyMapDel:
        case MFn::kPolyMapSew:
        case MFn::kPolySmooth:
        case MFn::kPolyNormal:
        case MFn::kPolyTweak:
#ifndef MAYA50
        case MFn::kPolySmoothProxy:
#endif
        case MFn::kPolySmoothFacet:
        case MFn::kPolyMergeEdge:
        case MFn::kPolyColorPerVertex:
        case MFn::kPolyMoveUV:
        case MFn::kPolyNormalPerVertex:
            pcPlugName = "inputPolymesh";
            break;
        case MFn::kSkinClusterFilter:
        case MFn::kGroupParts:
            pcPlugName = "inputGeometry";
            break;
        case MFn::kPolyTriangulate:
        case MFn::kPolyCollapseEdge:
        case MFn::kPolyCollapseF:
        case MFn::kPolyDelVertex:
        case MFn::kPolySplitEdge:
        case MFn::kPolyCloseBorder:
        case MFn::kPolyDelFacet:
        case MFn::kPolyDelEdge:
        case MFn::kPolyFlipEdge:
        case MFn::kPolyAppendVertex:
        case MFn::kPolyMergeFacet:
        case MFn::kPolySubdFacet:
        case MFn::kPolyTransfer:
        case MFn::kPolyReduce:
        case MFn::kPolyAppend:
        case MFn::kPolySplit:
        case MFn::kPolySoftEdge:
            pcPlugName = "inputPolymesh";

            // Skip over the first modifier if it is a poly tranguate
            // because it was created during export.
            if (!bFirstModifier || 
                (kType != MFn::kPolyTriangulate))
                bMeshError = true;
            break;
        case MFn::kDeleteComponent:
            pcPlugName = "inputGeometry";
            bMeshError = true;
            break;
        default:
            return false;
        }


        MFnDependencyNode kModifier(kPlugArr[0].node());

        kMapPlug = kModifier.findPlug(pcPlugName);

        kMapPlug.connectedTo(kPlugArr, true, false, &kStatus);

        if (kPlugArr.length() != 1)
            return false;

        // Get the type again.
        kType = kPlugArr[0].node().apiType();
        bFirstModifier = false;
    }



    // Make sure we are looking at a blend shape.
    if (kPlugArr[0].node().apiType() == MFn::kBlendShape)
    {
        BlendShape = kPlugArr[0].node();
        MFnDependencyNode BlendShapeNode(BlendShape, &kStatus);

        if (bMeshError)
        {
            const char *szShapeName;
            DtShapeGetShapeName( iShapeID, &szShapeName);
            DtExt_Err("ERROR: Object %s: The object's history contains " \
                "changes to the number of vertices or faces after you " \
                "created the blend shape. Creating the blend shape has been "\
                "aborted.\n", szShapeName);

        }

        return (kStatus == MS::kSuccess && !bMeshError);
    }

    return false;
}
//---------------------------------------------------------------------------
void MyiTriShape::AttachMorpherControllers(int iShapeID, int iGroupID,
        const unsigned int* puiNewToOld, MStatus* pkStatus)
{

    MObject kDestObj;

    if(!GetBlendShape(iShapeID, iGroupID, kDestObj)) 
    {
        if (pkStatus)
            *pkStatus = MS::kSuccess;
        return;
    }

    const char *szShapeName;
    DtShapeGetShapeName( iShapeID, &szShapeName);


    MFnBlendShapeDeformer kBlendShape(kDestObj, pkStatus);

    if (pkStatus)
    {
        // Shape not connected to a MFnBlendShapeDeformer
        *pkStatus = MS::kSuccess;
        return;
    }

    MObjectArray kBaseArray;
    kBlendShape.getBaseObjects(kBaseArray);
    unsigned int uiNumBaseObjects = kBaseArray.length();

    if (uiNumBaseObjects == 0)
    {
        // Error From GetVertsForNi
        DtExt_Err("ERROR:: Object %s: Associated blend shape has no base "
            "objects\n.", szShapeName);

        if (pkStatus)
        {
            // No base objects on MFnBlendShapeDeformer
                *pkStatus = MS::kSuccess;
        }
        return;
    }

    // begin conversion

    // count targets
    unsigned int uiNumNiTargets = 0;
    MObjectArray kTargetArray;
    unsigned int uiNumWeights = kBlendShape.numWeights();

    MPlug kInputTargetArray = kBlendShape.findPlug("inputTarget");
    unsigned int uiNumInputTargets = kInputTargetArray.numElements();

    NIASSERT(uiNumInputTargets == uiNumBaseObjects == 1); 
    
    unsigned int uiInputTargetChildren = kInputTargetArray[0].numChildren();
    // not sure when this would be anything else
    NIASSERT(uiInputTargetChildren == 1);   

    MPlug kInputTargetGroupArray = kInputTargetArray[0].child(0);

    unsigned int uiNumInputTargetGroupElts = 
        kInputTargetGroupArray.numElements();
    NIASSERT(uiNumInputTargetGroupElts == uiNumWeights);

    unsigned int i;
    for (i = 0; i < uiNumInputTargetGroupElts; i++)
    {
        MPlug kInputTargetGroup = kInputTargetGroupArray[i];
        unsigned int uiNumTargetGroups = kInputTargetGroup.numChildren();
        // not sure when this would be anything else
        NIASSERT(uiNumTargetGroups == 1); 

        // not used 
        unsigned int uiTargetGroupIndex = kInputTargetGroup.logicalIndex(); 

        MPlug kInputTargetItemArray = kInputTargetGroup.child(0);

        // If there are any "in-between" targets, this will be > 1.
        unsigned int uiNumInputTargetItems = 
            kInputTargetItemArray.numElements();

        uiNumNiTargets += uiNumInputTargetItems;
    }
    // done counting targets

    if (uiNumNiTargets == 0)
    {
        // Error From GetVertsForNi
        DtExt_Err("ERROR:: Object %s: Has No Blend Shape Targets.\n", 
            szShapeName);

        if (pkStatus)
        {
            // No targets on MFnBlendShapeDeformer
                *pkStatus = MS::kSuccess;
        }
        return;
    }



    // allocate targets, with an extra for implied base target 
    NiMorphData::MorphTarget* pkNiTargetArray = 
        NiNew NiMorphData::MorphTarget[uiNumNiTargets+1];

    uiNumNiTargets = 1;



    // ************ BUILD THE TARGETS  *************

    unsigned int uiVertices = m_spTriShape->GetVertexCount();


    // get exploded vertex map
    int* piFaceList;
    int iCount;
    dtPolygonGetFaceList (iShapeID, iGroupID,&piFaceList, &iCount);

    NiPoint3* pkBaseVertices = GetUndeformedBaseVertices(kBlendShape,
        puiNewToOld, piFaceList);

    if (pkBaseVertices == NULL)
    {
        // For one reason or another an error occurred.
        MString kBlendShapeName = kBlendShape.name();
        const char* pcBlendShapeName = kBlendShapeName.asChar();
        const char* pcShapeName;
        DtShapeGetShapeName( iShapeID, &pcShapeName);
        DtExt_Err("ERROR: Object %s: and it's BlendShape target %s have " \
            "different numbers of vertices.\n", pcShapeName, pcBlendShapeName);
        return;
    }

    // convert implied base target 
    NiMorphData::MorphTarget& kBaseTarget = pkNiTargetArray[0];

    // geometry
    kBaseTarget.ReplaceTargetVerts(pkBaseVertices);


    const char *szBaseName;
    DtShapeGetShapeName( iShapeID, &szBaseName );
    kBaseTarget.SetName(szBaseName);

    unsigned int uiBaseNumExplodedVertices = m_uiNiVertices;

    // convert targets
    for (i = 0; i < uiNumInputTargetGroupElts; i++)
    {
        MPlug kInputTargetGroup = kInputTargetGroupArray[i];

        MPlug kInputTargetItemArray = kInputTargetGroup.child(0);

        // If there are any "in-between" targets, this will be > 1.
        unsigned int uiNumInputTargetItems =
            kInputTargetItemArray.numElements();

        for (unsigned int j = 0; j < uiNumInputTargetItems; 
            j++, uiNumNiTargets++)
        {
            MPlug kInputTargetItem = kInputTargetItemArray[j];
            NIASSERT(kInputTargetItem.numChildren() == 3);


            // convert geometry
            NiPoint3* pkTargetVertices = 0;
            if (kInputTargetItem.child(0).isConnected())
            {
                MPlugArray kPlugArr;

                kInputTargetItem.child(0).connectedTo(
                    kPlugArr, true, false, pkStatus);

                if (pkStatus && *pkStatus != MS::kSuccess)
                {
                    // Error From GetVertsForNi
                    DtExt_Err("ERROR:: Object %s: Failed to extract Target"
                        " vert data\n", szShapeName);
                    return;
                }

                // I think the apiType of kMesh guarantees
                // that there will only be one connection, but
                // I'm not sure.  EH.
                NIASSERT(kPlugArr.length() == 1);

                int iTargetID = DtExt_FindShapeWithNode(kPlugArr[0].node());

                dtPolygonCopyFaceList(iTargetID, iShapeID, iGroupID);


                if (!GetVertsForNi(iTargetID, iGroupID))
                {
                    const char *szTargetName;
                    DtShapeGetShapeName( iTargetID, &szTargetName );

                    // Error From GetVertsForNi
                    DtExt_Err("ERROR:: Object %s: Failed to extract vert data"
                        " for Target %s\n", szShapeName, szTargetName);

                    if (pkStatus)
                        *pkStatus = MS::kFailure;
                    return;
                }

                // Insure we have the same numver of vertices
                if (m_uiNiVertices != uiBaseNumExplodedVertices)
                {
                    const char *szTargetName;
                    DtShapeGetShapeName( iTargetID, &szTargetName );
                    DtExt_Err("ERROR: Object %s: and it's morph target %s " \
                        "have different numbers of vertices. (%d / %d)\n", 
                        szShapeName, szTargetName, uiVertices, 
                        m_uiNiVertices);

                    if (pkStatus)
                        *pkStatus = MS::kFailure;
                    NiDelete[] m_pkVertices;
                    return;
                }


                // Create a Unique Target Name
                const char *szTargetName;
                DtShapeGetShapeName( iTargetID, &szTargetName );
                AddUniqueName(pkNiTargetArray, szTargetName, uiNumNiTargets); 
                

                // collapse verts using mapping from base mesh
                pkTargetVertices = NiNew NiPoint3[uiVertices];
                for (unsigned int uiVertex = 0; uiVertex < uiVertices;
                    uiVertex++)
                {
                    pkTargetVertices[uiVertex] = 
                        m_pkVertices[puiNewToOld[uiVertex]].m_kPosition -
                        pkBaseVertices[uiVertex];
                }

                NiDelete[] m_pkVertices;

            }
            else
            {
                // get point data array
                MObject kTemp;

                kInputTargetItem.child(1).getValue(kTemp);
                MFnPointArrayData kPointArrayData(kTemp);
                MPointArray kPointArray = kPointArrayData.array();

                pkTargetVertices = 
                    GetCollapsedVertices(kPointArray, puiNewToOld, piFaceList);

                if (pkTargetVertices == NULL)
                {
                    // An error occurred.
                    MString kInputTargetName = kInputTargetItem.name();
                    const char* pcInputTargetName = kInputTargetName.asChar();

                    const char* pcShapeName;
                    DtShapeGetShapeName( iShapeID, &pcShapeName);
                    DtExt_Err("ERROR: Object [%s] failed to extract deleted "
                        "BlendShape at input target [%s].\n", pcShapeName, 
                        pcInputTargetName);

                    if (pkStatus)
                        *pkStatus = MS::kFailure;
                    return;
                }

                AddUniqueName(pkNiTargetArray, "NotConnected", 
                    uiNumNiTargets); 
            }

            // Replace the Target Verts
            pkNiTargetArray[uiNumNiTargets].ReplaceTargetVerts(
                pkTargetVertices);              
        }
    }


    // Create the Morph Data
    NiMorphData* pkData = NiNew NiMorphData;
    pkData->SetRelativeTargets(true);

    // Replace the Targets
    pkData->ReplaceTargets(pkNiTargetArray, uiNumNiTargets, 
        uiVertices);


    // Create the Controller.
    NiGeomMorpherController* pkMorph = NiNew NiGeomMorpherController(pkData);

    pkMorph->SetTarget(m_spTriShape);
    pkMorph->SetUpdateNormals(GetMorphUpdateNormals(iShapeID));
    pkMorph->SetAnimType(NiTimeController::APP_TIME);
    pkMorph->SetCycleType(NiTimeController::LOOP);



    // ***** BUILD THE WEIGHTS *****

    // weight keys for the implicit base
    NiLinFloatKey* pkKey = NiNew NiLinFloatKey[1];
    pkKey->SetTime(0.0f);
    pkKey->SetValue(1.0f);

    // Create the Data and Interpolator
    NiFloatData* pkFloatData = NiNew NiFloatData();
    pkFloatData->ReplaceAnim((NiFloatKey*)pkKey, 1, NiAnimationKey::LINKEY);

    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator(pkFloatData);
    pkInterp->Collapse();

    // Add into the base morph slot
    pkMorph->SetInterpolator(pkInterp, 0);

    // Reset uiNumTargets
    uiNumNiTargets = 1;


    // get weight array plug
    MPlug kWeightPlug = kBlendShape.findPlug("weight");

    // convert targets
    for (i = 0; i < uiNumInputTargetGroupElts; i++)
    {
        MPlug kInputTargetGroup = kInputTargetGroupArray[i];

        MPlug kInputTargetItemArray = kInputTargetGroup.child(0);

        // If there are any "in-between" targets, this will be > 1.
        unsigned int uiNumInputTargetItems =
            kInputTargetItemArray.numElements();

        // Handle The Inbetweens
        float fInBetweenWeightPrev;
        float fInBetweenWeight;
        float fInBetweenWeightNext;

        for (unsigned int j = 0; j < uiNumInputTargetItems; 
            j++)
        {
            MPlug kInputTargetItem = kInputTargetItemArray[j];
            NIASSERT(kInputTargetItem.numChildren() == 3);


            // Get the TargetPlug
            MPlug kTargetPlug = kWeightPlug[i];


            // Define our Key Arrays
            unsigned int uiNumKeys;
            NiFloatKey* pkKeys;
            NiAnimationKey::KeyType eType;


            // IF There is just one target then convert the animation directly
            if (uiNumInputTargetItems == 1)
            {
                // Make sure the target plug is a weight
                NIASSERT(kTargetPlug.attribute().apiType() == 
                    MFn::kNumericAttribute);

                if (ConvertFloatAnim(kTargetPlug, uiNumKeys, pkKeys, eType) != 
                    MS::kSuccess)
                {
                    double dValue = 1.0;

                    kTargetPlug.getValue(dValue);

                    // Just Create a Key
                    NiLinFloatKey* pkLinKey = NiNew NiLinFloatKey[1];
                    pkLinKey->SetTime(0.0f);
                    pkLinKey->SetValue((float)dValue);

                    uiNumKeys = 1;
                    pkKeys = (NiFloatKey*)pkLinKey;
                    eType = NiAnimationKey::LINKEY;
                }

            }
            else 
            {
                // Multiple Targets for this weight so we need to check 
                //inbetween values

                //
                // DETERMINE THE INBETWEEN WEIGHTS
                //
                // convert weight
                // index = wt * 1000 + 5000
                // wt = (index - 5000) / 1000

                unsigned char ucTargetType = 0;
                ucTargetType |= (j > 0);
                if (ucTargetType & 1)
                    fInBetweenWeightPrev = 
                    ((float)((int)kInputTargetItemArray[j-1].logicalIndex()
                    - 5000)) / 1000.0f;

                fInBetweenWeight = 
                    ((float)((int)kInputTargetItemArray[j].logicalIndex()
                    - 5000)) / 1000.0f;

                ucTargetType |= (j < uiNumInputTargetItems-1) * 2;
                if (ucTargetType & 2)
                {
                    fInBetweenWeightNext = 
                        ((float)((int)kInputTargetItemArray[j+1].logicalIndex()
                        - 5000)) / 1000.0f;
                }

                // use implied base target instead if it is closer
                if (fInBetweenWeight > 0.0f &&
                    (((ucTargetType & 1) && fInBetweenWeightPrev < 0.0f) ||
                    !(ucTargetType & 1)))
                {
                    fInBetweenWeightPrev = 0.0f;
                    ucTargetType |= 1;
                }
                else if (fInBetweenWeight < 0.0f &&
                    (((ucTargetType & 2) && fInBetweenWeightNext > 0.0f) ||
                    !(ucTargetType & 2)))
                {
                    fInBetweenWeightNext = 0.0f;
                    ucTargetType |= 2;
                }


                // Find the Min and Max times
                bool bAnimated;
                MFnAnimCurve kWeightAnimCurve;
                if (ConvertPlugToAnimCurve(kTargetPlug, bAnimated, 
                    kWeightAnimCurve) != MS::kSuccess)
                    return;

                if (kWeightAnimCurve.numKeyframes() == 0)
                    return;

                // Determine our sample info
                MTime kMinTime = kWeightAnimCurve.time(0);
                MTime kMaxTime = kWeightAnimCurve.time(
                    kWeightAnimCurve.numKeyframes() - 1);

                unsigned int uiNumSamples = 
                    (unsigned int)(kMaxTime.value() - kMinTime.value()) + 1;

                MTime kSampleDuration = (kMaxTime - kMinTime) / 
                    (double)uiNumSamples;

                // we sample this anim-curve dependent formula to
                // determine the weight keys for this target.

                NiLinFloatKey* pkLinKeys = NiNew NiLinFloatKey[uiNumSamples];


                for (unsigned int uiSample = 0;
                    uiSample < uiNumSamples; uiSample++)
                {

                    MTime kSampleTime = kMinTime + (kSampleDuration * 
                        (double)uiSample);

                    float fTargetWeight;

                    float fWeight = 
                        (float) kWeightAnimCurve.evaluate(kSampleTime);

                    switch (ucTargetType)
                    {
                    case 0:     // no next or prev target
                        // this will never happen because there's always
                        // the implied base target
                        NIASSERT(0);
                        break;
                    case 1:     // prev but no next target
                        {
                            if (fWeight <= fInBetweenWeightPrev)
                            {
                                fTargetWeight = 0.0f;
                            }
                            else if (fWeight >= fInBetweenWeight)
                            {
                                fTargetWeight = fWeight - fInBetweenWeightPrev;
                            }
                            else
                            {
                                NIASSERT(fWeight < fInBetweenWeight);

                                float fDelta = 
                                    fInBetweenWeight - fInBetweenWeightPrev;

                                fTargetWeight = 
                                    (fDelta + (fWeight - fInBetweenWeight)) / 
                                    fDelta;
                            }

                        }
                        break;
                    case 2:     // next but no prev target
                        {
                            if (fWeight >= fInBetweenWeightNext)
                            {
                                fTargetWeight = 0.0f;
                            }
                            else if (fWeight >= fInBetweenWeight)
                            {
                                float fDelta = 
                                    fInBetweenWeightNext - fInBetweenWeight;

                                fTargetWeight = 
                                    (fDelta - (fWeight - fInBetweenWeight)) / 
                                    fDelta;
                            }
                            else
                            {
                                NIASSERT(fWeight < fInBetweenWeight);
                                fTargetWeight = fInBetweenWeightNext - fWeight;
                            }
                        }
                        break;
                    default:    // next and prev targets
                        NIASSERT(ucTargetType == 3);
                        {
                            if (fWeight >= fInBetweenWeightNext ||
                                fWeight <= fInBetweenWeightPrev)
                            {
                                fTargetWeight = 0.0f;
                            }
                            else if (fWeight >= fInBetweenWeight)
                            {
                                float fDelta = 
                                    fInBetweenWeightNext - fInBetweenWeight;

                                fTargetWeight = 
                                    (fDelta - (fWeight - fInBetweenWeight)) / 
                                    fDelta;
                            }
                            else
                            {
                                NIASSERT(fWeight < fInBetweenWeight);

                                float fDelta = 
                                    fInBetweenWeight - fInBetweenWeightPrev;

                                fTargetWeight = 
                                    (fDelta + (fWeight - fInBetweenWeight)) / 
                                    fDelta;
                            }

                        }
                        break;
                    }

                    pkLinKeys[uiSample].SetTime((float)kSampleTime.as(
                        MTime::kSeconds));

                    pkLinKeys[uiSample].SetValue(fTargetWeight);
                }


                // Copy over our key values so we can build the interpolator
                uiNumKeys = uiNumSamples;
                pkKeys = (NiFloatKey*)pkLinKeys;
                eType = NiAnimationKey::LINKEY;


            }

            // Create the Float Data
            pkFloatData = NiNew NiFloatData();
            pkFloatData->ReplaceAnim((NiFloatKey*)pkKeys, uiNumKeys, eType);

            // Create the Interpolator
            pkInterp = NiNew NiFloatInterpolator(pkFloatData);
            pkInterp->Collapse();

            // Add the Interpolator to the GeoMorphController
            pkMorph->SetInterpolator(pkInterp, uiNumNiTargets++);
        }
    }

   
    pkMorph->ResetTimeExtrema();
    return;
}
//---------------------------------------------------------------------------
void MyiTriShape::AddUniqueName(NiMorphData::MorphTarget* pkNiTargetArray,
        const char *szTargetName, int iTarget)
{

    char szNameBuffer[512];
    bool bDuplicate = true;
    int iIndex = 0;

    // Keep looking until we make a unique name
    while(bDuplicate)
    {
        bDuplicate = false;

        if (iIndex > 0)
            NiSprintf(szNameBuffer, 512, "%s%d", szTargetName, iIndex);
        else
            NiStrcpy(szNameBuffer, 512, szTargetName);

        iIndex++;

        // Enusre we have a unique name
        int iLoop;
        for (iLoop = 0; iLoop < iTarget; iLoop++)
        {
            if (strcmp(szNameBuffer, pkNiTargetArray[iLoop].GetName()) == 0)
                bDuplicate = true;
        }
    }


    pkNiTargetArray[iTarget].SetName(szNameBuffer);

}
//---------------------------------------------------------------------------
bool MyiTriShape::GetMorphUpdateNormals(int iShapeID)
{
    // Create the Dag Node for Our Object
    MFnDagNode dgNode;
    MObject mObj;
    DtExt_ShapeGetTransform(iShapeID, mObj);
    dgNode.setObject( mObj );

    // By default don't update the normals
    bool bUpdateNormals = false;

    // Check for the Extra Attribute
    GetExtraAttribute(dgNode,"Ni_MorphUpdateNormals",true, bUpdateNormals);

    return bUpdateNormals;
}
//---------------------------------------------------------------------------
void MyiTriShape::AttachProperties(int iShapeID, int iGroupID, 
    NiTexturingProperty* pTexturingProperty)
{
    
    m_spTriShape->AttachProperty(pTexturingProperty);
    
    // Setup the Alpha Property
    if(m_spTriShape->GetProperty(NiProperty::ALPHA) == NULL) 
    {
        NiAlphaProperty* pAlpha = NiNew NiAlphaProperty;
        pAlpha->SetSrcBlendMode( NiAlphaProperty::ALPHA_SRCALPHA);
        pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        pAlpha->SetAlphaBlending(true);
        
        m_spTriShape->AttachProperty( pAlpha );
    }
    
    
    // Setup the Vertex Color Properties
    if(DtTextureGetMaterialVertexColorOverride(iShapeID, iGroupID))
    {
        NiVertexColorPropertyPtr spVertexColor = NiNew NiVertexColorProperty;
        
        
        char cSrcMode;
        if(DtTextureGetMaterialVertexColorSrcMode(iShapeID, iGroupID, 
            cSrcMode))
        {
            spVertexColor->SetSourceMode(
                (NiVertexColorProperty::SourceVertexMode)cSrcMode);
        }
        else
        {
            NIASSERT(false);  // SHould never get here
        }
        
        
        char cLightMode;
        if(DtTextureGetMaterialVertexColorLightMode(iShapeID, iGroupID, 
            cLightMode))
        {
            spVertexColor->SetLightingMode(
                (NiVertexColorProperty::LightingMode)cLightMode);
        }
        else
        {
            NIASSERT(false);  // SHould never get here
        }
        
        m_spTriShape->AttachProperty(spVertexColor);
    }
}
//---------------------------------------------------------------------------
void MyiTriShape::CreateFlipBookProperty(int iShapeID, int iGroupID, 
        int iTextureID, char* szType, MyiTextureManager* pkTextureManager, 
        NiTexturingProperty::Map *pMap, 
        NiTexturingProperty *pTexturingProperty)
{

    int iMaterialID;
    DtMtlGetID( iShapeID, iGroupID, &iMaterialID );

    CreateFlipBookProperty(iMaterialID, iTextureID, szType, pkTextureManager,
        pMap, pTexturingProperty);
}
//---------------------------------------------------------------------------
void MyiTriShape::CreateFlipBookProperty(int iMaterialID, 
        int iTextureID, char* szType, MyiTextureManager* pkTextureManager, 
        NiTexturingProperty::Map *pMap, 
        NiTexturingProperty *pTexturingProperty)
{
    bool bFlipBook = false;
    short sStartFrame = 0;
    short sEndFrame = 0;
    float fFramesPerSecond = 0.0f;
    float fStartTime = 0.0f;
    char cCycleType = 0;


        // Check for a Flip Book Texture
    if(DtTextureGetMaterialMapFlipBookProperties( iMaterialID, szType, 
        bFlipBook, sStartFrame, sEndFrame, fFramesPerSecond, fStartTime, 
        cCycleType))
    {
        NiFlipControllerPtr spFlip = NiNew NiFlipController;
        
        spFlip->SetTarget(pTexturingProperty);
        spFlip->SetAffectedMap(pMap);
        spFlip->SetCycleType((NiTimeController::CycleType)cCycleType);
        
        
        // Attach in each of the Source Textures
        char *szFullPath = NULL;
        DtTextureGetFileNameID(iTextureID, &szFullPath);
        
        
        for(int iFrame = sStartFrame; iFrame <= sEndFrame; iFrame++)
        {
            char szFramePath[MAX_PATH];
            
            MyiTexture::CreateSequentialTexturePath(szFullPath, szFramePath, 
                MAX_PATH, iFrame, '/');
            
            spFlip->AttachTexture( pkTextureManager->GetTexture(szFramePath), 
                (iFrame-sStartFrame) );
        }

        // Determine when the first key starts
        float fFirstKeyTime = fStartTime / 
            (float)gAnimControlGetFramesPerSecond();

        // Create the Keys
        NiLinFloatKey* pkKeys = NiNew NiLinFloatKey[ sEndFrame - sStartFrame + 
            1 ];

        for (int iLoop = 0; iLoop <=  sEndFrame - sStartFrame; iLoop++)
        {
            pkKeys[iLoop].SetValue((float)iLoop);
            pkKeys[iLoop].SetTime(fFirstKeyTime + ((float)iLoop * (1.0f / 
                fFramesPerSecond)));
        }
        
        // Create the Data
        NiFloatData* pkData = NiNew NiFloatData();
        pkData->ReplaceAnim(pkKeys, sEndFrame - sStartFrame + 1, 
            NiFloatKey::LINKEY);

        // Create the Interpolator
        NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator(pkData);
        pkInterp->Collapse();

        // Assign the Interpolator
        spFlip->SetInterpolator(pkInterp);
        spFlip->ResetTimeExtrema();
    }
}
//---------------------------------------------------------------------------
int MyiTriShape::OptimizeUVs(int iUVIndex)
{
    // Examine the UV Sets to determinie if they can be optimized.
    for(int iLoop = 0; iLoop < iUVIndex; iLoop++)
    {
        bool bFound = false;

        // Compare the UVS to make sure they are the same
        for( int iVLoop = 0; (iVLoop < (int)m_uiNiVertices) && !bFound; 
            iVLoop++)
        {
            int iUV = m_pkVertices[iVLoop].m_uiNumUVs - 1;
            
            if ((m_pkVertices[iVLoop].m_akUV[iLoop].x != 
                    m_pkVertices[iVLoop].m_akUV[iUV].x) ||
                (m_pkVertices[iVLoop].m_akUV[iLoop].y != 
                    m_pkVertices[iVLoop].m_akUV[iUV].y))
            {
                bFound = true;
            }
        }
        
        // Check if there is no difference found
        if (bFound == false)
        {
            // Get rid of the last UV set
            for( int iVLoop = 0; iVLoop < (int)m_uiNiVertices; iVLoop++)
            {
                m_pkVertices[iVLoop].m_uiNumUVs--;
            }
            
            return iLoop;
        }
    }


    // No duplicate found
    return -1;
}
//---------------------------------------------------------------------------
void MyiTriShape::AddNoStripifyAttributes(MFnDagNode& dgNode)
{
    if(CheckForExtraAttribute(dgNode, "Ni_No_Stripify", true))
    {
        bool bValue;
        if(GetExtraAttribute(dgNode, "Ni_No_Stripify", true, bValue))
        {
            if(bValue)
            {
                NiStringExtraData* pkSData = NiNew NiStringExtraData(
                    "NiNoStripify");
                m_spTriShape->AddExtraData("NiNoStripify", 
                    (NiExtraData*)pkSData);       
            }
        }
    }
}
//---------------------------------------------------------------------------
