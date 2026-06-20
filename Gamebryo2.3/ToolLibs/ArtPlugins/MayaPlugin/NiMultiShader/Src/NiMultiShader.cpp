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

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h> 
#include <maya/MArrayDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnLightDataAttribute.h> 
#include <maya/MFnEnumAttribute.h> 
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MDGMessage.h>
#include <NiSystem.h>

///////////////////////////////////////
// Plugin NiMultiShader Shader Class //
///////////////////////////////////////


class NiMultiShader : public MPxNode
{
public:
                      NiMultiShader();
    virtual          ~NiMultiShader();

    virtual MStatus   compute( const MPlug&, MDataBlock& );
    virtual void      postConstructor();

    virtual bool    setInternalValue( const MPlug& kPlug, 
        const MDataHandle& kHandle);


    static  void *    creator();
    static  MStatus   initialize();
    static  MTypeId   id;

//protected:
   static MObject  aResolution;  // Resolution 32,64,128,246
   static MObject  aBaseMapOnly;  // Boolean
   static MObject  aSelectedTexture;  // Enum


   static MObject  aColorR;   // Red component of surface color
   static MObject  aColorG;   // Green component of surface color
   static MObject  aColorB;   // Blue component of surface color
   static MObject  aColor;    // Surface color
   static MObject  aColorL;    // Surface color // OBSOLETE
   static MObject  aColorK;    // Surface color // OBSOLETE
   static MObject  aColorFilter;    // Surface Filter
   static MObject  aColorTransparencyR;   
   static MObject  aColorTransparencyG;   
   static MObject  aColorTransparencyB;   
   static MObject  aColorTransparency;
   static MObject  aColorFlipBook;
   static MObject  aColorFlipBookStartFrame;
   static MObject  aColorFlipBookEndFrame;
   static MObject  aColorFlipBookFramesPerSecond;
   static MObject  aColorFlipBookStartTime;
   static MObject  aColorFlipBookCycleType;
   static MObject  aColorTempUVSet;



   static MObject  aDarkMapR;  // Red component of DarkMap
   static MObject  aDarkMapG;  // Green component of DarkMap
   static MObject  aDarkMapB;  // Blue component of DarkMap
   static MObject  aDarkMap;   // DarkMap
   static MObject  aDarkMapL;   // DarkMap      // OBSOLETE
   static MObject  aDarkMapK;   // DarkMap      // OBSOLETE
   static MObject  aDarkMapFilter;    // Surface Filter
   static MObject  aDarkMapFlipBook;
   static MObject  aDarkMapFlipBookStartFrame;
   static MObject  aDarkMapFlipBookEndFrame;
   static MObject  aDarkMapFlipBookFramesPerSecond;
   static MObject  aDarkMapFlipBookStartTime;
   static MObject  aDarkMapFlipBookCycleType;
   static MObject  aDarkMapTempUVSet;

   static MObject  aDetailMapR;  // Red component of DetailMap
   static MObject  aDetailMapG;  // Green component of DetailMap
   static MObject  aDetailMapB;  // Blue component of DetailMap
   static MObject  aDetailMap;   // DetailMap
   static MObject  aDetailMapL;   // DetailMap  // OBSOLETE
   static MObject  aDetailMapK;   // DetailMap  // OBSOLETE
   static MObject  aDetailMapFilter;    // Surface Filter
   static MObject  aDetailMapFlipBook;
   static MObject  aDetailMapFlipBookStartFrame;
   static MObject  aDetailMapFlipBookEndFrame;
   static MObject  aDetailMapFlipBookFramesPerSecond;
   static MObject  aDetailMapFlipBookStartTime;
   static MObject  aDetailMapFlipBookCycleType;
   static MObject  aDetailMapTempUVSet;

   static MObject  aDecal1MapR;  // Red component of Decal1Map
   static MObject  aDecal1MapG;  // Green component of Decal1Map
   static MObject  aDecal1MapB;  // Blue component of Decal1Map
   static MObject  aDecal1Map;   // Decal1Map
   static MObject  aDecal1MapL;   // Decal1Map  // OBSOLETE
   static MObject  aDecal1MapK;   // Decal1Map  // OBSOLETE
   static MObject  aDecal1MapFilter;    // Surface Filter
   static MObject  aDecal1MapFlipBook;
   static MObject  aDecal1MapFlipBookStartFrame;
   static MObject  aDecal1MapFlipBookEndFrame;
   static MObject  aDecal1MapFlipBookFramesPerSecond;
   static MObject  aDecal1MapFlipBookStartTime;
   static MObject  aDecal1MapFlipBookCycleType;
   static MObject  aDecal1MapTempUVSet;

   static MObject  aDecal2MapR;  // Red component of Decal2Map
   static MObject  aDecal2MapG;  // Green component of Decal2Map
   static MObject  aDecal2MapB;  // Blue component of Decal2Map
   static MObject  aDecal2Map;   // Decal2Map
   static MObject  aDecal2MapL;   // Decal2Map  // OBSOLETE
   static MObject  aDecal2MapK;   // Decal2Map  // OBSOLETE
   static MObject  aDecal2MapFilter;    // Surface Filter
   static MObject  aDecal2MapFlipBook;
   static MObject  aDecal2MapFlipBookStartFrame;
   static MObject  aDecal2MapFlipBookEndFrame;
   static MObject  aDecal2MapFlipBookFramesPerSecond;
   static MObject  aDecal2MapFlipBookStartTime;
   static MObject  aDecal2MapFlipBookCycleType;
   static MObject  aDecal2MapTempUVSet;

   static MObject  aGlowMapR;  // Red component of GlowMap
   static MObject  aGlowMapG;  // Green component of GlowMap
   static MObject  aGlowMapB;  // Blue component of GlowMap
   static MObject  aGlowMap;   // GlowMap
   static MObject  aGlowMapL;   // GlowMap  // OBSOLETE
   static MObject  aGlowMapK;   // GlowMap  // OBSOLETE
   static MObject  aGlowMapFilter;    // Surface Filter
   static MObject  aGlowMapFlipBook;
   static MObject  aGlowMapFlipBookStartFrame;
   static MObject  aGlowMapFlipBookEndFrame;
   static MObject  aGlowMapFlipBookFramesPerSecond;
   static MObject  aGlowMapFlipBookStartTime;
   static MObject  aGlowMapFlipBookCycleType;
   static MObject  aGlowMapTempUVSet;

   static MObject  aGlossMapR;  // Red component of GlossMap
   static MObject  aGlossMapG;  // Green component of GlossMap
   static MObject  aGlossMapB;  // Blue component of GlossMap
   static MObject  aGlossMap;   // GlossMap
   static MObject  aGlossMapL;   // GlossMap    // OBSOLETE
   static MObject  aGlossMapK;   // GlossMap    // OBSOLETE
   static MObject  aGlossMapFilter;    // Surface Filter
   static MObject  aGlossMapFlipBook;
   static MObject  aGlossMapFlipBookStartFrame;
   static MObject  aGlossMapFlipBookEndFrame;
   static MObject  aGlossMapFlipBookFramesPerSecond;
   static MObject  aGlossMapFlipBookStartTime;
   static MObject  aGlossMapFlipBookCycleType;
   static MObject  aGlossMapTempUVSet;

   static MObject  aBumpMapR;  // Red component of BumpMap
   static MObject  aBumpMapG;  // Green component of BumpMap
   static MObject  aBumpMapB;  // Blue component of BumpMap
   static MObject  aBumpMap;   // BumpMap
   static MObject  aBumpMapL;   // BumpMap  // OBSOLETE
   static MObject  aBumpMapK;   // BumpMap  // OBSOLETE
   static MObject  aBumpMapFilter;    // Surface Filter
   static MObject  aBumpMapLuminaScale;
   static MObject  aBumpMapLuminaOffset;
   static MObject  aBumpMapMat00;
   static MObject  aBumpMapMat01;
   static MObject  aBumpMapMat10;
   static MObject  aBumpMapMat11;
   static MObject  aBumpMapFlipBook;
   static MObject  aBumpMapFlipBookStartFrame;
   static MObject  aBumpMapFlipBookEndFrame;
   static MObject  aBumpMapFlipBookFramesPerSecond;
   static MObject  aBumpMapFlipBookStartTime;
   static MObject  aBumpMapFlipBookCycleType;
   static MObject  aBumpMapTempUVSet;

   static MObject  aNormalMapR;  // Red component of NormalMap
   static MObject  aNormalMapG;  // Green component of NormalMap
   static MObject  aNormalMapB;  // Blue component of NormalMap
   static MObject  aNormalMap;   // NormalMap
   static MObject  aNormalMapFilter;    // Surface Filter
   static MObject  aNormalMapFlipBook;
   static MObject  aNormalMapFlipBookStartFrame;
   static MObject  aNormalMapFlipBookEndFrame;
   static MObject  aNormalMapFlipBookFramesPerSecond;
   static MObject  aNormalMapFlipBookStartTime;
   static MObject  aNormalMapFlipBookCycleType;
   static MObject  aNormalMapTempUVSet;

   static MObject  aParallaxMapR;  // Red component of ParallaxMap
   static MObject  aParallaxMapG;  // Green component of ParallaxMap
   static MObject  aParallaxMapB;  // Blue component of ParallaxMap
   static MObject  aParallaxMap;   // ParallaxMap
   static MObject  aParallaxMapFilter;    // Surface Filter
   static MObject  aParallaxMapFlipBook;
   static MObject  aParallaxMapFlipBookStartFrame;
   static MObject  aParallaxMapFlipBookEndFrame;
   static MObject  aParallaxMapFlipBookFramesPerSecond;
   static MObject  aParallaxMapFlipBookStartTime;
   static MObject  aParallaxMapFlipBookCycleType;
   static MObject  aParallaxMapTempUVSet;
   static MObject  aParallaxMapOffset;  // Parallax offset

   static MObject  aOutColorR;   // Red component of output color
   static MObject  aOutColorG;   // Green component of output color
   static MObject  aOutColorB;   // Blue component of output color
   static MObject  aOutColor;    // Output color

   static MObject  aInTransR;
   static MObject  aInTransG;
   static MObject  aInTransB;
   static MObject  aInTransparency;

   static MObject  aOutTransparencyR;   
   static MObject  aOutTransparencyG;   
   static MObject  aOutTransparencyB;   
   static MObject  aOutTransparency;

   static MObject  aMaterialDiffuseR;   
   static MObject  aMaterialDiffuseG;   
   static MObject  aMaterialDiffuseB;   
   static MObject  aMaterialDiffuse;    


   static MObject  aMaterialSpecularR;  
   static MObject  aMaterialSpecularG;  
   static MObject  aMaterialSpecularB;  
   static MObject  aMaterialSpecular;   

   static MObject  aMaterialAmbientR;   
   static MObject  aMaterialAmbientG;   
   static MObject  aMaterialAmbientB;   
   static MObject  aMaterialAmbient;    // Material Diffuse color

   static MObject  aMaterialEmittanceR;   
   static MObject  aMaterialEmittanceG;   
   static MObject  aMaterialEmittanceB;   
   static MObject  aMaterialEmittance;    // Material Emittance color

   static MObject  aMaterialShineness;  // Shineness

   static MObject  aShading;  // Flat or Gouraud

   static MObject  aVertexColorOverride;
   static MObject  aVertexColorSrcMode;  
   static MObject  aVertexColorLightMode;  

   static MObject  aTextureApplyMode;  

   static MObject  aTransparencyOverride;  
   static MObject  aTransparencyUseAlphaBlending;
   static MObject  aTransparencySource;  
   static MObject  aTransparencyDestination;  
   static MObject  aTransparencyUseAlphaTesting;
   static MObject  aTransparencyNoTestingSorter;
   static MObject  aTransparencyTestReference;
   static MObject  aTransparencyTestingTestMode;

   static MObject  aPixelShaderName;
   static MObject  aPixelShaderID;
   static MObject  aPixelShaderImplementation;
   static MObject  aPixelShaderExportImplementation;

   static MObject  aNBTMethod;
};

//---------------------------------------------------------------------------

void fnConnCallback( MPlug & srcPlug, MPlug & destPlug, bool made, 
    void* clientData )
{
    if (made)
        return;

/*
    if( destPlug == NiMultiShader::aDarkMap )
    {    
        // If the srcPlug or destPlug is our shader,
        // then do something.
        MStatus Status;
        MString MSrcStr = srcPlug.info( &Status );
        MString MDstStr = destPlug.info( &Status );
        const char* pSrcStr = MSrcStr.asChar ();
        const char* pDstStr = MDstStr.asChar ();

        unsigned int iElem  = destPlug.numElements ( &Status );
        unsigned int iChild = destPlug.numChildren ( &Status );

        for (unsigned int i=0; i<iChild;i++ )
        {
            MPlug ChildPlug = destPlug.child ( i, &Status );
            MString MChildStr = ChildPlug.info( &Status );
            const char* pChildStr = MChildStr.asChar ();
            int x;
            x=0;
        }

        const char* pAPIType = NiMultiShader::aDarkMap.apiTypeStr();
    }
 */
}

//---------------------------------------------------------------------------

MTypeId NiMultiShader::id( 0x81121 );

//---------------------------------------------------------------------------

void NiMultiShader::postConstructor( )
{
    setMPSafe(true);
}

//---------------------------------------------------------------------------

///////////////////////////////////////////////////////
MObject  NiMultiShader::aResolution;
MObject  NiMultiShader::aBaseMapOnly;
MObject  NiMultiShader::aSelectedTexture;

MObject  NiMultiShader::aColor;
MObject  NiMultiShader::aColorR;
MObject  NiMultiShader::aColorG;
MObject  NiMultiShader::aColorB;
MObject  NiMultiShader::aColorL;    // OBSOLETE
MObject  NiMultiShader::aColorK;    // OBSOLETE 
MObject  NiMultiShader::aColorFilter;
MObject  NiMultiShader::aColorTransparencyR;   
MObject  NiMultiShader::aColorTransparencyG;   
MObject  NiMultiShader::aColorTransparencyB;   
MObject  NiMultiShader::aColorTransparency;
MObject  NiMultiShader::aColorFlipBook;
MObject  NiMultiShader::aColorFlipBookStartFrame;
MObject  NiMultiShader::aColorFlipBookEndFrame;
MObject  NiMultiShader::aColorFlipBookFramesPerSecond;
MObject  NiMultiShader::aColorFlipBookStartTime;
MObject  NiMultiShader::aColorFlipBookCycleType;
MObject  NiMultiShader::aColorTempUVSet;

MObject  NiMultiShader::aDarkMap;
MObject  NiMultiShader::aDarkMapR;
MObject  NiMultiShader::aDarkMapG;
MObject  NiMultiShader::aDarkMapB;
MObject  NiMultiShader::aDarkMapL;  // OBSOLETE
MObject  NiMultiShader::aDarkMapK;  // OBSOLETE
MObject  NiMultiShader::aDarkMapFilter;
MObject  NiMultiShader::aDarkMapFlipBook;
MObject  NiMultiShader::aDarkMapFlipBookStartFrame;
MObject  NiMultiShader::aDarkMapFlipBookEndFrame;
MObject  NiMultiShader::aDarkMapFlipBookFramesPerSecond;
MObject  NiMultiShader::aDarkMapFlipBookStartTime;
MObject  NiMultiShader::aDarkMapFlipBookCycleType;
MObject  NiMultiShader::aDarkMapTempUVSet;

MObject  NiMultiShader::aDetailMap;
MObject  NiMultiShader::aDetailMapR;
MObject  NiMultiShader::aDetailMapG;
MObject  NiMultiShader::aDetailMapB;
MObject  NiMultiShader::aDetailMapL;    // OBSOLETE
MObject  NiMultiShader::aDetailMapK;    // OBSOLETE
MObject  NiMultiShader::aDetailMapFilter;
MObject  NiMultiShader::aDetailMapFlipBook;
MObject  NiMultiShader::aDetailMapFlipBookStartFrame;
MObject  NiMultiShader::aDetailMapFlipBookEndFrame;
MObject  NiMultiShader::aDetailMapFlipBookFramesPerSecond;
MObject  NiMultiShader::aDetailMapFlipBookStartTime;
MObject  NiMultiShader::aDetailMapFlipBookCycleType;
MObject  NiMultiShader::aDetailMapTempUVSet;

MObject  NiMultiShader::aDecal1Map;
MObject  NiMultiShader::aDecal1MapR;
MObject  NiMultiShader::aDecal1MapG;
MObject  NiMultiShader::aDecal1MapB;
MObject  NiMultiShader::aDecal1MapL;    // OBSOLETE
MObject  NiMultiShader::aDecal1MapK;    // OBSOLETE
MObject  NiMultiShader::aDecal1MapFilter;
MObject  NiMultiShader::aDecal1MapFlipBook;
MObject  NiMultiShader::aDecal1MapFlipBookStartFrame;
MObject  NiMultiShader::aDecal1MapFlipBookEndFrame;
MObject  NiMultiShader::aDecal1MapFlipBookFramesPerSecond;
MObject  NiMultiShader::aDecal1MapFlipBookStartTime;
MObject  NiMultiShader::aDecal1MapFlipBookCycleType;
MObject  NiMultiShader::aDecal1MapTempUVSet;

MObject  NiMultiShader::aDecal2Map;
MObject  NiMultiShader::aDecal2MapR;
MObject  NiMultiShader::aDecal2MapG;
MObject  NiMultiShader::aDecal2MapB;
MObject  NiMultiShader::aDecal2MapL;    // OBSOLETE
MObject  NiMultiShader::aDecal2MapK;    // OBSOLETE
MObject  NiMultiShader::aDecal2MapFilter;
MObject  NiMultiShader::aDecal2MapFlipBook;
MObject  NiMultiShader::aDecal2MapFlipBookStartFrame;
MObject  NiMultiShader::aDecal2MapFlipBookEndFrame;
MObject  NiMultiShader::aDecal2MapFlipBookFramesPerSecond;
MObject  NiMultiShader::aDecal2MapFlipBookStartTime;
MObject  NiMultiShader::aDecal2MapFlipBookCycleType;
MObject  NiMultiShader::aDecal2MapTempUVSet;

MObject  NiMultiShader::aGlowMap;
MObject  NiMultiShader::aGlowMapR;
MObject  NiMultiShader::aGlowMapG;
MObject  NiMultiShader::aGlowMapB;
MObject  NiMultiShader::aGlowMapL;  // OBSOLETE
MObject  NiMultiShader::aGlowMapK;  // OBSOLETE
MObject  NiMultiShader::aGlowMapFilter;
MObject  NiMultiShader::aGlowMapFlipBook;
MObject  NiMultiShader::aGlowMapFlipBookStartFrame;
MObject  NiMultiShader::aGlowMapFlipBookEndFrame;
MObject  NiMultiShader::aGlowMapFlipBookFramesPerSecond;
MObject  NiMultiShader::aGlowMapFlipBookStartTime;
MObject  NiMultiShader::aGlowMapFlipBookCycleType;
MObject  NiMultiShader::aGlowMapTempUVSet;

MObject  NiMultiShader::aGlossMap;
MObject  NiMultiShader::aGlossMapR;
MObject  NiMultiShader::aGlossMapG;
MObject  NiMultiShader::aGlossMapB;
MObject  NiMultiShader::aGlossMapL; // OBSOLETE
MObject  NiMultiShader::aGlossMapK; // OBSOLETE
MObject  NiMultiShader::aGlossMapFilter;
MObject  NiMultiShader::aGlossMapFlipBook;
MObject  NiMultiShader::aGlossMapFlipBookStartFrame;
MObject  NiMultiShader::aGlossMapFlipBookEndFrame;
MObject  NiMultiShader::aGlossMapFlipBookFramesPerSecond;
MObject  NiMultiShader::aGlossMapFlipBookStartTime;
MObject  NiMultiShader::aGlossMapFlipBookCycleType;
MObject  NiMultiShader::aGlossMapTempUVSet;

MObject  NiMultiShader::aBumpMapR;
MObject  NiMultiShader::aBumpMapG;
MObject  NiMultiShader::aBumpMapB;
MObject  NiMultiShader::aBumpMap;
MObject  NiMultiShader::aBumpMapL;  // OBSOLETE
MObject  NiMultiShader::aBumpMapK;  // OBSOLETE
MObject  NiMultiShader::aBumpMapFilter;
MObject  NiMultiShader::aBumpMapLuminaScale;
MObject  NiMultiShader::aBumpMapLuminaOffset;
MObject  NiMultiShader::aBumpMapMat00;
MObject  NiMultiShader::aBumpMapMat01;
MObject  NiMultiShader::aBumpMapMat10;
MObject  NiMultiShader::aBumpMapMat11;
MObject  NiMultiShader::aBumpMapFlipBook;
MObject  NiMultiShader::aBumpMapFlipBookStartFrame;
MObject  NiMultiShader::aBumpMapFlipBookEndFrame;
MObject  NiMultiShader::aBumpMapFlipBookFramesPerSecond;
MObject  NiMultiShader::aBumpMapFlipBookStartTime;
MObject  NiMultiShader::aBumpMapFlipBookCycleType;
MObject  NiMultiShader::aBumpMapTempUVSet;

MObject  NiMultiShader::aNormalMap;
MObject  NiMultiShader::aNormalMapR;
MObject  NiMultiShader::aNormalMapG;
MObject  NiMultiShader::aNormalMapB;
MObject  NiMultiShader::aNormalMapFilter;
MObject  NiMultiShader::aNormalMapFlipBook;
MObject  NiMultiShader::aNormalMapFlipBookStartFrame;
MObject  NiMultiShader::aNormalMapFlipBookEndFrame;
MObject  NiMultiShader::aNormalMapFlipBookFramesPerSecond;
MObject  NiMultiShader::aNormalMapFlipBookStartTime;
MObject  NiMultiShader::aNormalMapFlipBookCycleType;
MObject  NiMultiShader::aNormalMapTempUVSet;

MObject  NiMultiShader::aParallaxMap;
MObject  NiMultiShader::aParallaxMapR;
MObject  NiMultiShader::aParallaxMapG;
MObject  NiMultiShader::aParallaxMapB;
MObject  NiMultiShader::aParallaxMapFilter;
MObject  NiMultiShader::aParallaxMapFlipBook;
MObject  NiMultiShader::aParallaxMapFlipBookStartFrame;
MObject  NiMultiShader::aParallaxMapFlipBookEndFrame;
MObject  NiMultiShader::aParallaxMapFlipBookFramesPerSecond;
MObject  NiMultiShader::aParallaxMapFlipBookStartTime;
MObject  NiMultiShader::aParallaxMapFlipBookCycleType;
MObject  NiMultiShader::aParallaxMapTempUVSet;
MObject  NiMultiShader::aParallaxMapOffset;

MObject  NiMultiShader::aOutColor;
MObject  NiMultiShader::aOutColorR;
MObject  NiMultiShader::aOutColorG;
MObject  NiMultiShader::aOutColorB;


MObject  NiMultiShader::aOutTransparencyR;   
MObject  NiMultiShader::aOutTransparencyG;   
MObject  NiMultiShader::aOutTransparencyB;   
MObject  NiMultiShader::aOutTransparency;

MObject  NiMultiShader::aInTransparency;
MObject  NiMultiShader::aInTransR;
MObject  NiMultiShader::aInTransG;
MObject  NiMultiShader::aInTransB;

MObject  NiMultiShader::aMaterialDiffuseR;   
MObject  NiMultiShader::aMaterialDiffuseG;   
MObject  NiMultiShader::aMaterialDiffuseB;   
MObject  NiMultiShader::aMaterialDiffuse;    // Material Diffuse color


MObject  NiMultiShader::aMaterialSpecularR;  
MObject  NiMultiShader::aMaterialSpecularG;  
MObject  NiMultiShader::aMaterialSpecularB;  
MObject  NiMultiShader::aMaterialSpecular;    // Material Specular color

MObject  NiMultiShader::aMaterialAmbientR;   
MObject  NiMultiShader::aMaterialAmbientG;   
MObject  NiMultiShader::aMaterialAmbientB;   
MObject  NiMultiShader::aMaterialAmbient;    // Material Diffuse color

MObject  NiMultiShader::aMaterialEmittanceR; 
MObject  NiMultiShader::aMaterialEmittanceG; 
MObject  NiMultiShader::aMaterialEmittanceB; 
MObject  NiMultiShader::aMaterialEmittance;    // Material Emittance color

MObject  NiMultiShader::aMaterialShineness;  // Shineness

MObject  NiMultiShader::aShading;  // Flat or Gouraud

MObject  NiMultiShader::aVertexColorOverride;
MObject  NiMultiShader::aVertexColorSrcMode;  
MObject  NiMultiShader::aVertexColorLightMode;  

MObject  NiMultiShader::aTextureApplyMode;  

MObject  NiMultiShader::aTransparencyOverride;  
MObject  NiMultiShader::aTransparencyUseAlphaBlending;
MObject  NiMultiShader::aTransparencySource;  
MObject  NiMultiShader::aTransparencyDestination;  
MObject  NiMultiShader::aTransparencyUseAlphaTesting;
MObject  NiMultiShader::aTransparencyNoTestingSorter;
MObject  NiMultiShader::aTransparencyTestReference;
MObject  NiMultiShader::aTransparencyTestingTestMode;

MObject  NiMultiShader::aPixelShaderName;
MObject  NiMultiShader::aPixelShaderID;
MObject  NiMultiShader::aPixelShaderImplementation;
MObject  NiMultiShader::aPixelShaderExportImplementation;

MObject  NiMultiShader::aNBTMethod;
//---------------------------------------------------------------------------

NiMultiShader::NiMultiShader()
{
}

//---------------------------------------------------------------------------

NiMultiShader::~NiMultiShader()
{
}

//---------------------------------------------------------------------------

void* NiMultiShader::creator()
{
    return new NiMultiShader();
}

//---------------------------------------------------------------------------

MStatus NiMultiShader::initialize()
{
    MFnNumericAttribute nAttr; 
    MFnLightDataAttribute lAttr;
    MFnEnumAttribute eAttr;
    MFnTypedAttribute tAttr;



// TEXTURE ATTRIBUTES

    aResolution = nAttr.create("resolution","res",MFnNumericData::kLong);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(32);
    
    aBaseMapOnly = nAttr.create("BaseMapOnly","bmo",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aSelectedTexture = eAttr.create( "SelectedTexture", "slt", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Base Map", 0);
    eAttr.addField("Dark Map", 1);
    eAttr.addField("Detail Map", 2);
    eAttr.addField("Decal 1 Map", 3);
    eAttr.addField("Decal 2 Map", 4);
    eAttr.addField("Glow Map", 5);
    eAttr.addField("Gloss Map", 6);
    eAttr.addField("Bump Map", 7);
    eAttr.addField("Normal Map", 8);
    eAttr.addField("Parallax Map", 9);

//---------------------------------------------------------------------------
//      Color Attributes
//---------------------------------------------------------------------------

    aColorR = nAttr.create( "colorR", "cr",MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aColorG = nAttr.create( "colorG", "cg",MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.58824f);
     

    aColorB = nAttr.create( "colorB", "cb",MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.644f);
     

    aColor = nAttr.create( "color", "c", aColorR, aColorG, aColorB);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.58824f, 0.644f);
     
    nAttr.setUsedAsColor(true);

    // OBSOLETE
    aColorL = nAttr.create( "colorL", "cl",MFnNumericData::kByte);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    // OBSOLETE
    aColorK = nAttr.create( "colorK", "ck",MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aColorFilter = eAttr.create( "colorFilter", "cfilter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aColorTransparencyR = nAttr.create( "colorTransparencyR", "ctrr",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aColorTransparencyG = nAttr.create( "colorTransparencyG", "ctrg",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aColorTransparencyB = nAttr.create( "colorTransparencyB", "ctrb",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aColorTransparency = nAttr.create( "colorTransparency", "ctran", 
        aColorTransparencyR, aColorTransparencyG, aColorTransparencyB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f, 1.0f, 1.0f);
     
    nAttr.setUsedAsColor(true);


    aColorFlipBook = nAttr.create("colorFlipBook","cflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aColorFlipBookStartFrame = nAttr.create( "colorFlipBookStartFrame", 
        "cfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aColorFlipBookEndFrame = nAttr.create( "colorFlipBookEndFrame", "cfbef",
        MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aColorFlipBookFramesPerSecond = nAttr.create( 
        "colorFlipBookFramesPerSecond", "cfbfps", MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aColorFlipBookStartTime = nAttr.create( "colorFlipBookStartTime", 
        "cfbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aColorFlipBookCycleType = eAttr.create( "colorFlipBookCycleType", 
        "cfbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aColorTempUVSet = eAttr.create("colorTempUVSet","ctuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    char szEnumBuffer[64];
    int iLoop;
    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }

//---------------------------------------------------------------------------
//      Dark Attributes
//---------------------------------------------------------------------------

    aDarkMapR = nAttr.create( "DarkMapR", "dkr", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aDarkMapG = nAttr.create( "DarkMapG", "dkg", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aDarkMapB = nAttr.create( "DarkMapB", "dkb", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aDarkMap = nAttr.create("DarkMap","dkm",aDarkMapR, aDarkMapG, aDarkMapB);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f, 1.0f, 1.0f);
    nAttr.setUsedAsColor(true);
     

    // OBSOLETE
    aDarkMapL = nAttr.create( "DarkMapL", "dkl",MFnNumericData::kByte);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    // OBSOLETE
    aDarkMapK = nAttr.create( "DarkMapK", "dkk",MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aDarkMapFilter = eAttr.create( "DarkMapFilter", "dkfilter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aDarkMapFlipBook = nAttr.create("DarkMapFlipBook","dmflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aDarkMapFlipBookStartFrame = nAttr.create( "DarkMapFlipBookStartFrame", 
        "dmfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aDarkMapFlipBookEndFrame = nAttr.create( "DarkMapFlipBookEndFrame", 
        "dmcfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aDarkMapFlipBookFramesPerSecond = nAttr.create( 
        "DarkMapFlipBookFramesPerSecond", "dmfbfps", MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aDarkMapFlipBookStartTime = nAttr.create( "DarkMapFlipBookStartTime", 
        "dmfbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aDarkMapFlipBookCycleType = eAttr.create( "DarkMapFlipBookCycleType", 
        "dmfbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aDarkMapTempUVSet = eAttr.create("DarkMapTempUVSet","dmtuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }

//---------------------------------------------------------------------------
//      Detail Attributes
//---------------------------------------------------------------------------

    aDetailMapR = nAttr.create( "DetailMapR", "dtr", MFnNumericData::kFloat);
    nAttr.setKeyable(false);
    nAttr.setStorable(false);
     
    nAttr.setDefault(0.5f);

    aDetailMapG = nAttr.create( "DetailMapG", "dtg", MFnNumericData::kFloat);
    nAttr.setKeyable(false);
    nAttr.setStorable(false);
     
    nAttr.setDefault(0.5f);

    aDetailMapB = nAttr.create( "DetailMapB", "dtb", MFnNumericData::kFloat);
    nAttr.setKeyable(false);
    nAttr.setStorable(false);
     
    nAttr.setDefault(0.5f);

    aDetailMap = nAttr.create( "DetailMap","dtm",aDetailMapR, aDetailMapG, 
        aDetailMapB);

    nAttr.setKeyable(false);
    nAttr.setStorable(false);
    nAttr.setDefault(0.5f, 0.5f, 0.5f);
     
    nAttr.setUsedAsColor(true);

    // OBSOLETE
    aDetailMapL = nAttr.create( "DetailMapL", "dtl",MFnNumericData::kByte);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    // OBSOLETE
    aDetailMapK = nAttr.create( "DetailMapK", "dtk",MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aDetailMapFilter = eAttr.create( "DetailMapFilter", "dtfilter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aDetailMapFlipBook = nAttr.create("DetailMapFlipBook","dtmflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aDetailMapFlipBookStartFrame = nAttr.create(
        "DetailMapFlipBookStartFrame", "dtmfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aDetailMapFlipBookEndFrame = nAttr.create("DetailMapFlipBookEndFrame", 
        "dtmcfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aDetailMapFlipBookFramesPerSecond = nAttr.create(
        "DetailMapFlipBookFramesPerSecond", "dtmfbfps", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aDetailMapFlipBookStartTime = nAttr.create("DetailMapFlipBookStartTime", 
        "dtfbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aDetailMapFlipBookCycleType = eAttr.create( "DetailMapFlipBookCycleType",
        "dtfbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aDetailMapTempUVSet = eAttr.create("DetailMapTempUVSet","dttuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }
//---------------------------------------------------------------------------
//      Decal 1 Attributes
//---------------------------------------------------------------------------

    aDecal1MapR = nAttr.create( "Decal1MapR", "d1r", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aDecal1MapG = nAttr.create( "Decal1MapG", "d1g", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aDecal1MapB = nAttr.create( "Decal1MapB", "d1b", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aDecal1Map = nAttr.create( "Decal1Map","d1m",aDecal1MapR, aDecal1MapG, 
        aDecal1MapB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    // OBSOLETE
    aDecal1MapL = nAttr.create( "Decal1MapL", "d1l",MFnNumericData::kByte);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    // OBSOLETE
    aDecal1MapK = nAttr.create( "Decal1MapK", "d1k",MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aDecal1MapFilter = eAttr.create( "Decal1MapFilter", "d1filter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aDecal1MapFlipBook = nAttr.create("Decal1MapFlipBook","d1mflipbook", 
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aDecal1MapFlipBookStartFrame = nAttr.create(
        "Decal1MapFlipBookStartFrame", "d1mfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aDecal1MapFlipBookEndFrame = nAttr.create( "Decal1MapFlipBookEndFrame", 
        "d1mcfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aDecal1MapFlipBookFramesPerSecond = nAttr.create( 
        "Decal1MapFlipBookFramesPerSecond", "d1mfbfps", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aDecal1MapFlipBookStartTime = nAttr.create( 
        "Decal1MapFlipBookStartTime", "d1fbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aDecal1MapFlipBookCycleType = eAttr.create("Decal1MapFlipBookCycleType", 
        "d1fbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aDecal1MapTempUVSet = eAttr.create("Decal1MapTempUVSet","d1tuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }
//---------------------------------------------------------------------------
//      Decal 2 Attributes
//---------------------------------------------------------------------------

    aDecal2MapR = nAttr.create( "Decal2MapR", "d2r", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.5f);

    aDecal2MapG = nAttr.create( "Decal2MapG", "d2g", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.5f);

    aDecal2MapB = nAttr.create( "Decal2MapB", "d2b", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.5f);

    aDecal2Map = nAttr.create( "Decal2Map","d2m",aDecal2MapR, aDecal2MapG, 
        aDecal2MapB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    // OBSOLETE
    aDecal2MapL = nAttr.create( "Decal2MapL", "d2l",MFnNumericData::kByte);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    // OBSOLETE
    aDecal2MapK = nAttr.create( "Decal2MapK", "d2k",MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aDecal2MapFilter = eAttr.create( "Decal2MapFilter", "d2filter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aDecal2MapFlipBook = nAttr.create("Decal2MapFlipBook","d2mflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aDecal2MapFlipBookStartFrame = nAttr.create(
        "Decal2MapFlipBookStartFrame", "d2mfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aDecal2MapFlipBookEndFrame = nAttr.create( 
        "Decal2MapFlipBookEndFrame", "d2mcfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aDecal2MapFlipBookFramesPerSecond = nAttr.create( 
        "Decal2MapFlipBookFramesPerSecond", "d2mfbfps", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aDecal2MapFlipBookStartTime = nAttr.create( 
        "Decal2MapFlipBookStartTime", "d2fbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aDecal2MapFlipBookCycleType = eAttr.create( 
        "Decal2MapFlipBookCycleType", "d2fbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aDecal2MapTempUVSet = eAttr.create("Decal2MapTempUVSet","d2tuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }
//---------------------------------------------------------------------------
//      Glow Attributes
//---------------------------------------------------------------------------

    aGlowMapR = nAttr.create( "GlowMapR", "glr", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aGlowMapG = nAttr.create( "GlowMapG", "glg", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aGlowMapB = nAttr.create( "GlowMapB", "glb", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aGlowMap = nAttr.create("GlowMap","glm",aGlowMapR, aGlowMapG, aGlowMapB);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    // OBSOLETE
    aGlowMapL = nAttr.create( "GlowMapL", "gll",MFnNumericData::kByte);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    // OBSOLETE
    aGlowMapK = nAttr.create( "GlowMapK", "glk",MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aGlowMapFilter = eAttr.create( "GlowMapFilter", "glfilter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aGlowMapFlipBook = nAttr.create("GlowMapFlipBook","glflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aGlowMapFlipBookStartFrame = nAttr.create( 
        "GlowMapFlipBookStartFrame", "glfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aGlowMapFlipBookEndFrame = nAttr.create( "GlowMapFlipBookEndFrame", 
        "glcfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aGlowMapFlipBookFramesPerSecond = nAttr.create( 
        "GlowMapFlipBookFramesPerSecond", "glfbfps", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aGlowMapFlipBookStartTime = nAttr.create( "GlowMapFlipBookStartTime", 
        "glfbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aGlowMapFlipBookCycleType = eAttr.create( "GlowMapFlipBookCycleType", 
        "glfbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aGlowMapTempUVSet = eAttr.create("GlowMapTempUVSet","gltuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }
//---------------------------------------------------------------------------
//      Gloss Attributes
//---------------------------------------------------------------------------

    aGlossMapR = nAttr.create( "GlossMapR", "glsr", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aGlossMapG = nAttr.create( "GlossMapG", "glsg", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aGlossMapB = nAttr.create( "GlossMapB", "glsb", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aGlossMap = nAttr.create( "GlossMap","glsm",aGlossMapR, aGlossMapG, 
        aGlossMapB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    // OBSOLETE
    aGlossMapL = nAttr.create( "GlossMapL", "glsl",MFnNumericData::kByte);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    // OBSOLETE
    aGlossMapK = nAttr.create( "GlossMapK", "glsk",MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aGlossMapFilter = eAttr.create( "GlossMapFilter", "glsfilter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aGlossMapFlipBook = nAttr.create("GlossMapFlipBook","glsflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aGlossMapFlipBookStartFrame = nAttr.create( 
        "GlossMapFlipBookStartFrame", "glsfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aGlossMapFlipBookEndFrame = nAttr.create( "GlossMapFlipBookEndFrame", 
        "glscfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aGlossMapFlipBookFramesPerSecond = nAttr.create( 
        "GlossMapFlipBookFramesPerSecond", "glsfbfps", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aGlossMapFlipBookStartTime = nAttr.create( 
        "GlossMapFlipBookStartTime", "glsfbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aGlossMapFlipBookCycleType = eAttr.create( 
        "GlossMapFlipBookCycleType", "glsfbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aGlossMapTempUVSet = eAttr.create("GlossMapTempUVSet","glstuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }
//---------------------------------------------------------------------------
//      Bump Map Attributes
//---------------------------------------------------------------------------

    aBumpMapR = nAttr.create( "BumpMapR", "bmr", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aBumpMapG = nAttr.create( "BumpMapG", "bmg", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aBumpMapB = nAttr.create( "BumpMapB", "bmb", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aBumpMap = nAttr.create("BumpMap","bmp",aBumpMapR, aBumpMapG, aBumpMapB);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);


    // OBSOLETE
    aBumpMapL = nAttr.create( "BumpMapL", "bml",MFnNumericData::kByte);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    // OBSOLETE
    aBumpMapK = nAttr.create( "BumpMapK", "bmk",MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aBumpMapFilter = eAttr.create( "BumpMapFilter", "bmfilter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);


    aBumpMapLuminaScale = nAttr.create( "BumpMapLuminaScale", "bmls", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);

    aBumpMapLuminaOffset = nAttr.create( "BumpMapLuminaOffset", "bmlo", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);

    aBumpMapMat00 = nAttr.create( "BumpMapMat00", "bmm00", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);

    aBumpMapMat01 = nAttr.create( "BumpMapMat01", "bmm01", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);

    aBumpMapMat10 = nAttr.create( "BumpMapMat10", "bmm10", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);

    aBumpMapMat11 = nAttr.create( "BumpMapMat11", "bmm11", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);

    aBumpMapFlipBook = nAttr.create("BumpMapFlipBook","bmflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aBumpMapFlipBookStartFrame = nAttr.create( "BumpMapFlipBookStartFrame", 
        "bmfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aBumpMapFlipBookEndFrame = nAttr.create( "BumpMapFlipBookEndFrame", 
        "bmcfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aBumpMapFlipBookFramesPerSecond = nAttr.create( 
        "BumpMapFlipBookFramesPerSecond", "bmfbfps", MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aBumpMapFlipBookStartTime = nAttr.create( "BumpMapFlipBookStartTime", 
        "bmfbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aBumpMapFlipBookCycleType = eAttr.create( "BumpMapFlipBookCycleType", 
        "bmfbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);


    aInTransR = nAttr.create( "transparencyR", "itr",MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);

    aInTransG = nAttr.create( "transparencyG", "itg",MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);

    aInTransB = nAttr.create( "transparencyB", "itb",MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);

    aInTransparency = nAttr.create( "transparency", "it", aInTransR, 
        aInTransG, aInTransB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
    nAttr.setUsedAsColor(true);

    aBumpMapTempUVSet = eAttr.create("BumpMapTempUVSet","bmtuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }

//---------------------------------------------------------------------------
//      Normal Attributes
//---------------------------------------------------------------------------

    aNormalMapR = nAttr.create( "NormalMapR", "nrmr", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aNormalMapG = nAttr.create( "NormalMapG", "nrmg", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aNormalMapB = nAttr.create( "NormalMapB", "nrmb", MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aNormalMap = nAttr.create( "NormalMap","nrmm",aNormalMapR, aNormalMapG, 
        aNormalMapB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    aNormalMapFilter = eAttr.create( "NormalMapFilter", "nrmfilter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aNormalMapFlipBook = nAttr.create("NormalMapFlipBook","nrmflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aNormalMapFlipBookStartFrame = nAttr.create( 
        "NormalMapFlipBookStartFrame", "nrmfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aNormalMapFlipBookEndFrame = nAttr.create( "NormalMapFlipBookEndFrame", 
        "nrmcfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aNormalMapFlipBookFramesPerSecond = nAttr.create( 
        "NormalMapFlipBookFramesPerSecond", "nrmfbfps", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aNormalMapFlipBookStartTime = nAttr.create( 
        "NormalMapFlipBookStartTime", "nrmfbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aNormalMapFlipBookCycleType = eAttr.create( 
        "NormalMapFlipBookCycleType", "nrmfbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aNormalMapTempUVSet = eAttr.create("NormalMapTempUVSet","nrmtuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }

//---------------------------------------------------------------------------
//      Parallax Attributes
//---------------------------------------------------------------------------

    aParallaxMapR = nAttr.create( "ParallaxMapR", "plxr",
        MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aParallaxMapG = nAttr.create( "ParallaxMapG", "plxg",
        MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aParallaxMapB = nAttr.create( "ParallaxMapB", "plxb",
        MFnNumericData::kFloat);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
     
    nAttr.setDefault(0.0f);

    aParallaxMap = nAttr.create( "ParallaxMap","plxm",aParallaxMapR,
        aParallaxMapG, 
        aParallaxMapB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    aParallaxMapFilter = eAttr.create( "ParallaxMapFilter", "plxfilter", 5);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Nearest", 0);
    eAttr.addField("BILerp", 1);
    eAttr.addField("Nearest MIP Nearest", 2);
    eAttr.addField("BILerp MIP Nearest", 3);
    eAttr.addField("Nearest MIPLerp", 4);
    eAttr.addField("TriLerp", 5);

    aParallaxMapFlipBook = nAttr.create("ParallaxMapFlipBook","plxflipbook",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);

    aParallaxMapFlipBookStartFrame = nAttr.create( 
        "ParallaxMapFlipBookStartFrame", "plxfbsf",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aParallaxMapFlipBookEndFrame = nAttr.create( "ParallaxMapFlipBookEndFrame",
        "plxcfbef",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aParallaxMapFlipBookFramesPerSecond = nAttr.create( 
        "ParallaxMapFlipBookFramesPerSecond", "plxfbfps", 
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     
    nAttr.setMin(0);

    aParallaxMapFlipBookStartTime = nAttr.create( 
        "ParallaxMapFlipBookStartTime", "plxfbst",MFnNumericData::kFloat);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0);
    nAttr.setMin(0.0);

    aParallaxMapFlipBookCycleType = eAttr.create( 
        "ParallaxMapFlipBookCycleType", "plxfbct", 0);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Looping", 0);
    eAttr.addField("Reverse", 1);
    eAttr.addField("Clamped", 2);

    aParallaxMapTempUVSet = eAttr.create("ParallaxMapTempUVSet","plxtuv", 0);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);


    aParallaxMapOffset = nAttr.create("ParallaxMapOffset","plxoffset",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.05f);
    nAttr.setMin(0.0f);

    for(iLoop = 0; iLoop < 256; iLoop++)
    {
        NiSprintf(szEnumBuffer, 64, "E%d", iLoop);
        eAttr.addField(szEnumBuffer, iLoop);
    }

//---------------------------------------------------------------------------
//      Material Attributes
//---------------------------------------------------------------------------

    aMaterialDiffuseR = nAttr.create( "MaterialDiffuseR", "MatDifr",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
   

    aMaterialDiffuseG = nAttr.create( "MaterialDiffuseG", "MatDifg",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
    

    aMaterialDiffuseB = nAttr.create( "MaterialDiffuseB", "MatDifb",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aMaterialDiffuse = nAttr.create( "MaterialDiffuse", "MatDif", 
        aMaterialDiffuseR, aMaterialDiffuseG, aMaterialDiffuseB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f, 1.0f, 1.0f);

    nAttr.setUsedAsColor(true);


    aMaterialSpecularR = nAttr.create( "MaterialSpecularR", "MapSpecr",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialSpecularG = nAttr.create( "MaterialSpecularG", "MapSpecg",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialSpecularB = nAttr.create( "MaterialSpecularB", "MapSpecb",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialSpecular = nAttr.create( "MaterialSpecular", "MapSpec", 
        aMaterialSpecularR, aMaterialSpecularG, aMaterialSpecularB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    aMaterialAmbientR = nAttr.create( "MaterialAmbientR", "MatAmbr",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialAmbientG = nAttr.create( "MaterialAmbientG", "MatAmbg",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialAmbientB = nAttr.create( "MaterialAmbientB", "MatAmbb",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialAmbient = nAttr.create( "MaterialAmbient", "MatAmb", 
        aMaterialAmbientR, aMaterialAmbientG, aMaterialAmbientB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    aMaterialEmittanceR = nAttr.create( "MaterialEmittanceR", "MatEmir",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialEmittanceG = nAttr.create( "MaterialEmittanceG", "MatEmig",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialEmittanceB = nAttr.create( "MaterialEmittanceB", "MatEmib",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f);
     

    aMaterialEmittance = nAttr.create( "MaterialEmittance", "MatEmi", 
        aMaterialEmittanceR, aMaterialEmittanceG, aMaterialEmittanceB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.0f, 0.0f, 0.0f);
     
    nAttr.setUsedAsColor(true);

    aMaterialShineness = nAttr.create("MaterialShineness","mshine",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(4.0f);
    nAttr.setMin(0.0f);


    aShading = eAttr.create("Shading","shading", -1);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Default", -1);
    eAttr.addField("Flat Shading", 0);
    eAttr.addField("Gouraud", 1);


//---------------------------------------------------------------------------
//      Vertex Color Attributes
//---------------------------------------------------------------------------


    aVertexColorOverride = nAttr.create("VertexColorOverride","vto",
        MFnNumericData::kBoolean);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);
    nAttr.setMax(1);


    aVertexColorSrcMode = eAttr.create("VertexColorSrcMode","vtsm", 2);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Ignore", 0);
    eAttr.addField("Emissive", 1);
    eAttr.addField("Ambient and Diffuse", 2);

    aVertexColorLightMode = eAttr.create("VertexColorLightMode","vtlm", 1);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Emissive Only", 0);
    eAttr.addField("Emissive, Ambient, and Diffuse", 1);


    aTextureApplyMode = eAttr.create("TextureApplyMode","tam",2);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Replace",0);
    eAttr.addField("Decal",1);
    eAttr.addField("Modulate",2);
    eAttr.addField("Obsolete:: Highlight 1 (PS2 Only)", 3);
    eAttr.addField("Obsolete:: Highlight 2 (PS2 Only)", 4);

//---------------------------------------------------------------------------
//      Transparency Attributes
//---------------------------------------------------------------------------

    aTransparencyOverride = nAttr.create("TransparencyOverride","trao",
        MFnNumericData::kBoolean);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aTransparencyUseAlphaBlending = nAttr.create(
        "TransparencyUseAlphaBlending","truab", MFnNumericData::kBoolean);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aTransparencySource = eAttr.create("TransparencySource","tras",6);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("One", 0);
    eAttr.addField("Zero", 1);
    eAttr.addField("Source Color", 2);
    eAttr.addField("Inverse Source Color", 3);
    eAttr.addField("Destination Color", 4);
    eAttr.addField("Inverse Destination Color", 5);
    eAttr.addField("Source Alpha", 6);
    eAttr.addField("Inverse Source Alpha", 7);
    eAttr.addField("Destination Alpha", 8);
    eAttr.addField("Inverse Destination Alpha", 9);
    eAttr.addField("Source Alpha AT", 10);

    aTransparencyDestination = eAttr.create("TransparencyDestination",
        "trad",7);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("One", 0);
    eAttr.addField("Zero", 1);
    eAttr.addField("Source Color", 2);
    eAttr.addField("Inverse Source Color", 3);
    eAttr.addField("Destination Color", 4);
    eAttr.addField("Inverse Destination Color", 5);
    eAttr.addField("Source Alpha", 6);
    eAttr.addField("Inverse Source Alpha", 7);
    eAttr.addField("Destination Alpha", 8);
    eAttr.addField("Inverse Destination Alpha", 9);
    eAttr.addField("Source Alpha AT", 10);

    aTransparencyUseAlphaTesting = nAttr.create(
        "TransparencyUseAlphaTesting","truat", MFnNumericData::kBoolean);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aTransparencyNoTestingSorter = nAttr.create(
        "TransparencyNoTestingSorter","trnts", MFnNumericData::kBoolean);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aTransparencyTestReference = nAttr.create( 
        "TransparencyTestReference", "trtr",MFnNumericData::kByte);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
     

    aTransparencyTestingTestMode = eAttr.create(
        "TransparencyTestingTestMode","trttm", 3);

    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("Always", 0);
    eAttr.addField("Less", 1);
    eAttr.addField("Equal", 2);
    eAttr.addField("Less than or Equal", 3);
    eAttr.addField("Greater", 4);
    eAttr.addField("Not Equal", 5);
    eAttr.addField("Greater than or Equal", 6);
    eAttr.addField("Never", 7);

    aPixelShaderName = tAttr.create("PixelShaderName", "pxlsn", 
        MFnData::kString);

    tAttr.setKeyable(false);
    tAttr.setStorable(true);

    aPixelShaderID = nAttr.create( "PixelShaderID", "pxlid",
        MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);
    nAttr.setMin(0);

    aPixelShaderImplementation = nAttr.create( "PixelShaderImplementation", 
        "pxlim",MFnNumericData::kShort);

    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aPixelShaderExportImplementation = nAttr.create( 
        "PixelShaderExportImplementation", "pxleim", MFnNumericData::kShort);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setDefault(0);

    aNBTMethod = eAttr.create("NBTMethod","nbtm",7);
    eAttr.setKeyable(false);
    eAttr.setStorable(true);
    eAttr.addField("None", 0);
    eAttr.addField("NDL", 1);
    eAttr.addField("ATI", 2);
    eAttr.setDefault(0);


//---------------------------------------------------------------------------
//      Output Attributes
//---------------------------------------------------------------------------


    aOutColorR = nAttr.create( "outColorR", "ocr", MFnNumericData::kFloat);
    aOutColorG = nAttr.create( "outColorG", "ocg", MFnNumericData::kFloat);
    aOutColorB = nAttr.create( "outColorB", "ocb", MFnNumericData::kFloat);
    aOutColor = nAttr.create( "outColor","oc", aOutColorR, aOutColorG, 
        aOutColorB);

    nAttr.setHidden(false);
    nAttr.setReadable(true);
    nAttr.setWritable(false);

//    OutTransparency

    aOutTransparencyR = nAttr.create( "outTransparencyR", "otr",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aOutTransparencyG = nAttr.create( "outTransparencyG", "otg",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aOutTransparencyB = nAttr.create( "outTransparencyB", "otb",
        MFnNumericData::kFloat);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0f);
     

    aOutTransparency = nAttr.create( "outTransparency", "ot", 
        aOutTransparencyR, aOutTransparencyG, aOutTransparencyB);

    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(0.5f, 0.5f, 0.5f);
     
    nAttr.setUsedAsColor(true);

    addAttribute(aResolution);
    addAttribute(aBaseMapOnly);
    addAttribute(aSelectedTexture);

    addAttribute(aColor);
    // OBSOLETE
    addAttribute(aColorL);
    // OBSOLETE
    addAttribute(aColorK);
    addAttribute(aColorFilter);
    addAttribute(aColorTransparency);
    addAttribute(aColorFlipBook);
    addAttribute(aColorFlipBookStartFrame);
    addAttribute(aColorFlipBookEndFrame);
    addAttribute(aColorFlipBookFramesPerSecond);
    addAttribute(aColorFlipBookStartTime);
    addAttribute(aColorFlipBookCycleType);
    addAttribute(aColorTempUVSet);

    addAttribute(aDarkMap);
    // OBSOLETE
    addAttribute(aDarkMapL);
    // OBSOLETE
    addAttribute(aDarkMapK);
    addAttribute(aDarkMapFilter);
    addAttribute(aDarkMapFlipBook);
    addAttribute(aDarkMapFlipBookStartFrame);
    addAttribute(aDarkMapFlipBookEndFrame);
    addAttribute(aDarkMapFlipBookFramesPerSecond);
    addAttribute(aDarkMapFlipBookStartTime);
    addAttribute(aDarkMapFlipBookCycleType);
    addAttribute(aDarkMapTempUVSet);

    addAttribute(aDetailMap);
    // OBSOLETE
    addAttribute(aDetailMapL);
    // OBSOLETE
    addAttribute(aDetailMapK);
    addAttribute(aDetailMapFilter);
    addAttribute(aDetailMapFlipBook);
    addAttribute(aDetailMapFlipBookStartFrame);
    addAttribute(aDetailMapFlipBookEndFrame);
    addAttribute(aDetailMapFlipBookFramesPerSecond);
    addAttribute(aDetailMapFlipBookStartTime);
    addAttribute(aDetailMapFlipBookCycleType);
    addAttribute(aDetailMapTempUVSet);

    addAttribute(aDecal1Map);
    // OBSOLETE
    addAttribute(aDecal1MapL);
    // OBSOLETE
    addAttribute(aDecal1MapK);
    addAttribute(aDecal1MapFilter);
    addAttribute(aDecal1MapFlipBook);
    addAttribute(aDecal1MapFlipBookStartFrame);
    addAttribute(aDecal1MapFlipBookEndFrame);
    addAttribute(aDecal1MapFlipBookFramesPerSecond);
    addAttribute(aDecal1MapFlipBookStartTime);
    addAttribute(aDecal1MapFlipBookCycleType);
    addAttribute(aDecal1MapTempUVSet);

    addAttribute(aDecal2Map);
    // OBSOLETE
    addAttribute(aDecal2MapL);
    // OBSOLETE
    addAttribute(aDecal2MapK);
    addAttribute(aDecal2MapFilter);
    addAttribute(aDecal2MapFlipBook);
    addAttribute(aDecal2MapFlipBookStartFrame);
    addAttribute(aDecal2MapFlipBookEndFrame);
    addAttribute(aDecal2MapFlipBookFramesPerSecond);
    addAttribute(aDecal2MapFlipBookStartTime);
    addAttribute(aDecal2MapFlipBookCycleType);
    addAttribute(aDecal2MapTempUVSet);

    addAttribute(aGlowMap);
    // OBSOLETE
    addAttribute(aGlowMapL);
    // OBSOLETE
    addAttribute(aGlowMapK);
    addAttribute(aGlowMapFilter);
    addAttribute(aGlowMapFlipBook);
    addAttribute(aGlowMapFlipBookStartFrame);
    addAttribute(aGlowMapFlipBookEndFrame);
    addAttribute(aGlowMapFlipBookFramesPerSecond);
    addAttribute(aGlowMapFlipBookStartTime);
    addAttribute(aGlowMapFlipBookCycleType);
    addAttribute(aGlowMapTempUVSet);

    addAttribute(aGlossMap);
    // OBSOLETE
    addAttribute(aGlossMapL);
    // OBSOLETE
    addAttribute(aGlossMapK);
    addAttribute(aGlossMapFilter);
    addAttribute(aGlossMapFlipBook);
    addAttribute(aGlossMapFlipBookStartFrame);
    addAttribute(aGlossMapFlipBookEndFrame);
    addAttribute(aGlossMapFlipBookFramesPerSecond);
    addAttribute(aGlossMapFlipBookStartTime);
    addAttribute(aGlossMapFlipBookCycleType);
    addAttribute(aGlossMapTempUVSet);

    addAttribute(aBumpMap);
    // OBSOLETE
    addAttribute(aBumpMapL);
    // OBSOLETE
    addAttribute(aBumpMapK);
    addAttribute(aBumpMapFilter);
    addAttribute(aBumpMapLuminaScale);
    addAttribute(aBumpMapLuminaOffset);
    addAttribute(aBumpMapMat00);
    addAttribute(aBumpMapMat01);
    addAttribute(aBumpMapMat10);
    addAttribute(aBumpMapMat11);
    addAttribute(aBumpMapFlipBook);
    addAttribute(aBumpMapFlipBookStartFrame);
    addAttribute(aBumpMapFlipBookEndFrame);
    addAttribute(aBumpMapFlipBookFramesPerSecond);
    addAttribute(aBumpMapFlipBookStartTime);
    addAttribute(aBumpMapFlipBookCycleType);
    addAttribute(aBumpMapTempUVSet);

    addAttribute(aNormalMap);
    addAttribute(aNormalMapFilter);
    addAttribute(aNormalMapFlipBook);
    addAttribute(aNormalMapFlipBookStartFrame);
    addAttribute(aNormalMapFlipBookEndFrame);
    addAttribute(aNormalMapFlipBookFramesPerSecond);
    addAttribute(aNormalMapFlipBookStartTime);
    addAttribute(aNormalMapFlipBookCycleType);
    addAttribute(aNormalMapTempUVSet);

    addAttribute(aParallaxMap);
    addAttribute(aParallaxMapFilter);
    addAttribute(aParallaxMapFlipBook);
    addAttribute(aParallaxMapFlipBookStartFrame);
    addAttribute(aParallaxMapFlipBookEndFrame);
    addAttribute(aParallaxMapFlipBookFramesPerSecond);
    addAttribute(aParallaxMapFlipBookStartTime);
    addAttribute(aParallaxMapFlipBookCycleType);
    addAttribute(aParallaxMapTempUVSet);
    addAttribute(aParallaxMapOffset);

    addAttribute(aInTransparency);
    addAttribute(aOutColor);
    addAttribute(aOutTransparency);

    addAttribute(aMaterialDiffuse);
    addAttribute(aMaterialSpecular);
    addAttribute(aMaterialAmbient);
    addAttribute(aMaterialEmittance);
    addAttribute(aMaterialShineness);


    addAttribute(aShading);

    addAttribute(aVertexColorOverride);
    addAttribute(aVertexColorSrcMode);
    addAttribute(aVertexColorLightMode);

    addAttribute(aTextureApplyMode);

    addAttribute(aTransparencyOverride);
    addAttribute(aTransparencyUseAlphaBlending);
    addAttribute(aTransparencySource);
    addAttribute(aTransparencyDestination);
    addAttribute(aTransparencyUseAlphaTesting);
    addAttribute(aTransparencyNoTestingSorter);
    addAttribute(aTransparencyTestReference);
    addAttribute(aTransparencyTestingTestMode);
    
    addAttribute(aPixelShaderName);
    addAttribute(aPixelShaderID);
    addAttribute(aPixelShaderImplementation);
    addAttribute(aPixelShaderExportImplementation);

    addAttribute(aNBTMethod);

    attributeAffects (aResolution, aOutColor);

    attributeAffects(aMaterialDiffuse, aOutColor);
    attributeAffects(aMaterialSpecular, aOutColor);
    attributeAffects(aMaterialAmbient, aOutColor);
    attributeAffects(aMaterialEmittance, aOutColor);
    attributeAffects(aMaterialShineness, aOutColor);

    attributeAffects(aVertexColorOverride, aOutColor);
    attributeAffects(aVertexColorSrcMode, aOutColor);
    attributeAffects(aVertexColorLightMode, aOutColor);

    attributeAffects(aTextureApplyMode, aOutColor);

    attributeAffects(aTransparencyOverride, aOutColor);
    attributeAffects(aTransparencyUseAlphaBlending, aOutColor);
    attributeAffects(aTransparencySource, aOutColor);
    attributeAffects(aTransparencyDestination, aOutColor);
    attributeAffects(aTransparencyUseAlphaTesting, aOutColor);
    attributeAffects(aTransparencyNoTestingSorter, aOutColor);
    attributeAffects(aTransparencyTestReference, aOutColor);
    attributeAffects(aTransparencyTestingTestMode, aOutColor);

    attributeAffects (aColorR, aOutColor);
    attributeAffects (aColorG, aOutColor);
    attributeAffects (aColorB, aOutColor);
    attributeAffects (aColor, aOutColor);
    // OBSOLETE
    attributeAffects (aColorL, aOutColor);
    // OBSOLETE
    attributeAffects (aColorK, aOutColor);
    attributeAffects (aColorFilter, aOutColor);
    attributeAffects (aColorFlipBook, aOutColor);
    attributeAffects (aColorFlipBookStartFrame, aOutColor);
    attributeAffects (aColorFlipBookEndFrame, aOutColor);
    attributeAffects (aColorFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aColorFlipBookStartTime, aOutColor);
    attributeAffects (aColorFlipBookCycleType, aOutColor);

    attributeAffects (aDarkMapR, aOutColor);
    attributeAffects (aDarkMapG, aOutColor);
    attributeAffects (aDarkMapB, aOutColor);
    attributeAffects (aDarkMap, aOutColor);
    // OBSOLETE
    attributeAffects (aDarkMapL, aOutColor);
    // OBSOLETE
    attributeAffects (aDarkMapK, aOutColor);
    attributeAffects (aDarkMapFilter, aOutColor);
    attributeAffects (aDarkMapFlipBook, aOutColor);
    attributeAffects (aDarkMapFlipBookStartFrame, aOutColor);
    attributeAffects (aDarkMapFlipBookEndFrame, aOutColor);
    attributeAffects (aDarkMapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aDarkMapFlipBookStartTime, aOutColor);
    attributeAffects (aDarkMapFlipBookCycleType, aOutColor);

    attributeAffects (aDetailMapR, aOutColor);
    attributeAffects (aDetailMapG, aOutColor);
    attributeAffects (aDetailMapB, aOutColor);
    attributeAffects (aDetailMap, aOutColor);
    // OBSOLETE
    attributeAffects (aDetailMapL, aOutColor);
    // OBSOLETE
    attributeAffects (aDetailMapK, aOutColor);
    attributeAffects (aDetailMapFilter, aOutColor);
    attributeAffects (aDetailMapFlipBook, aOutColor);
    attributeAffects (aDetailMapFlipBookStartFrame, aOutColor);
    attributeAffects (aDetailMapFlipBookEndFrame, aOutColor);
    attributeAffects (aDetailMapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aDetailMapFlipBookStartTime, aOutColor);
    attributeAffects (aDetailMapFlipBookCycleType, aOutColor);

    attributeAffects (aDecal1MapR, aOutColor);
    attributeAffects (aDecal1MapG, aOutColor);
    attributeAffects (aDecal1MapB, aOutColor);
    attributeAffects (aDecal1Map, aOutColor);
    // OBSOLETE
    attributeAffects (aDecal1MapL, aOutColor);
    // OBSOLETE
    attributeAffects (aDecal1MapK, aOutColor);
    attributeAffects (aDecal1MapFilter, aOutColor);
    attributeAffects (aDecal1MapFlipBook, aOutColor);
    attributeAffects (aDecal1MapFlipBookStartFrame, aOutColor);
    attributeAffects (aDecal1MapFlipBookEndFrame, aOutColor);
    attributeAffects (aDecal1MapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aDecal1MapFlipBookStartTime, aOutColor);
    attributeAffects (aDecal1MapFlipBookCycleType, aOutColor);

    attributeAffects (aDecal2MapR, aOutColor);
    attributeAffects (aDecal2MapG, aOutColor);
    attributeAffects (aDecal2MapB, aOutColor);
    attributeAffects (aDecal2Map, aOutColor);
    // OBSOLETE
    attributeAffects (aDecal2MapL, aOutColor);
    // OBSOLETE
    attributeAffects (aDecal2MapK, aOutColor);
    attributeAffects (aDecal2MapFilter, aOutColor);
    attributeAffects (aDecal2MapFlipBook, aOutColor);
    attributeAffects (aDecal2MapFlipBookStartFrame, aOutColor);
    attributeAffects (aDecal2MapFlipBookEndFrame, aOutColor);
    attributeAffects (aDecal2MapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aDecal2MapFlipBookStartTime, aOutColor);
    attributeAffects (aDecal2MapFlipBookCycleType, aOutColor);

    attributeAffects (aGlowMapR, aOutColor);
    attributeAffects (aGlowMapG, aOutColor);
    attributeAffects (aGlowMapB, aOutColor);
    attributeAffects (aGlowMap, aOutColor);
    // OBSOLETE
    attributeAffects (aGlowMapL, aOutColor);
    // OBSOLETE
    attributeAffects (aGlowMapK, aOutColor);
    attributeAffects (aGlowMapFilter, aOutColor);
    attributeAffects (aGlowMapFlipBook, aOutColor);
    attributeAffects (aGlowMapFlipBookStartFrame, aOutColor);
    attributeAffects (aGlowMapFlipBookEndFrame, aOutColor);
    attributeAffects (aGlowMapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aGlowMapFlipBookStartTime, aOutColor);
    attributeAffects (aGlowMapFlipBookCycleType, aOutColor);

    attributeAffects (aGlossMapR, aOutColor);
    attributeAffects (aGlossMapG, aOutColor);
    attributeAffects (aGlossMapB, aOutColor);
    attributeAffects (aGlossMap, aOutColor);
    // OBSOLETE
    attributeAffects (aGlossMapL, aOutColor);
    // OBSOLETE
    attributeAffects (aGlossMapK, aOutColor);
    attributeAffects (aGlossMapFilter, aOutColor);
    attributeAffects (aGlossMapFlipBook, aOutColor);
    attributeAffects (aGlossMapFlipBookStartFrame, aOutColor);
    attributeAffects (aGlossMapFlipBookEndFrame, aOutColor);
    attributeAffects (aGlossMapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aGlossMapFlipBookStartTime, aOutColor);
    attributeAffects (aGlossMapFlipBookCycleType, aOutColor);

    attributeAffects (aBumpMapR, aOutColor);
    attributeAffects (aBumpMapG, aOutColor);
    attributeAffects (aBumpMapB, aOutColor);
    attributeAffects (aBumpMap, aOutColor);
    // OBSOLETE
    attributeAffects (aBumpMapL, aOutColor);
    // OBSOLETE
    attributeAffects (aBumpMapK, aOutColor);
    attributeAffects (aBumpMapFilter, aOutColor);
    attributeAffects (aBumpMapLuminaScale, aOutColor);
    attributeAffects (aBumpMapLuminaOffset, aOutColor);
    attributeAffects (aBumpMapMat00, aOutColor);
    attributeAffects (aBumpMapMat01, aOutColor);
    attributeAffects (aBumpMapMat10, aOutColor);
    attributeAffects (aBumpMapMat11, aOutColor);
    attributeAffects (aBumpMapFlipBook, aOutColor);
    attributeAffects (aBumpMapFlipBookStartFrame, aOutColor);
    attributeAffects (aBumpMapFlipBookEndFrame, aOutColor);
    attributeAffects (aBumpMapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aBumpMapFlipBookStartTime, aOutColor);
    attributeAffects (aBumpMapFlipBookCycleType, aOutColor);

    attributeAffects (aNormalMapR, aOutColor);
    attributeAffects (aNormalMapG, aOutColor);
    attributeAffects (aNormalMapB, aOutColor);
    attributeAffects (aNormalMap, aOutColor);
    attributeAffects (aNormalMapFilter, aOutColor);
    attributeAffects (aNormalMapFlipBook, aOutColor);
    attributeAffects (aNormalMapFlipBookStartFrame, aOutColor);
    attributeAffects (aNormalMapFlipBookEndFrame, aOutColor);
    attributeAffects (aNormalMapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aNormalMapFlipBookStartTime, aOutColor);
    attributeAffects (aNormalMapFlipBookCycleType, aOutColor);

    attributeAffects (aParallaxMapR, aOutColor);
    attributeAffects (aParallaxMapG, aOutColor);
    attributeAffects (aParallaxMapB, aOutColor);
    attributeAffects (aParallaxMap, aOutColor);
    attributeAffects (aParallaxMapFilter, aOutColor);
    attributeAffects (aParallaxMapFlipBook, aOutColor);
    attributeAffects (aParallaxMapFlipBookStartFrame, aOutColor);
    attributeAffects (aParallaxMapFlipBookEndFrame, aOutColor);
    attributeAffects (aParallaxMapFlipBookFramesPerSecond, aOutColor);
    attributeAffects (aParallaxMapFlipBookStartTime, aOutColor);
    attributeAffects (aParallaxMapFlipBookCycleType, aOutColor);
    
    printf("Initializeing NiMultiShader.\n");

#ifdef MAYA30
    MStatus ReturnStatus;
    MCallbackId iId = MDGMessage::addConnectionCallback( &fnConnCallback, 
        NULL, &ReturnStatus );
#endif

    return MS::kSuccess;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus NiMultiShader::compute(
const MPlug&      plug,
      MDataBlock& block ) 
{ 


  if( plug == aOutColor || 
      plug == aOutColorR ||
      plug == aOutColorG ||
      plug == aOutColorB )
  {

    MFloatVector resultColor(0.0,0.0,0.0);

    // get sample surface shading parameters
    MFloatVector& surfaceColor  = 
        block.inputValue( aColor ).asFloatVector();

    MFloatVector& DarkMap       = 
        block.inputValue( aDarkMap ).asFloatVector();

    MFloatVector& DetailMap     = 
        block.inputValue( aDetailMap ).asFloatVector();

    MFloatVector& Decal1Map     = 
        block.inputValue( aDecal1Map ).asFloatVector();

    MFloatVector& Decal2Map     = 
        block.inputValue( aDecal2Map ).asFloatVector();

    MFloatVector& GlowMap       = 
        block.inputValue( aGlowMap ).asFloatVector();

    MFloatVector& GlossMap       = 
        block.inputValue( aGlossMap ).asFloatVector();

    MFloatVector& NormalMap       = 
        block.inputValue( aNormalMap ).asFloatVector();

    MFloatVector& ParallaxMap       = 
        block.inputValue( aParallaxMap ).asFloatVector();

    MFloatVector& ColorTransparency       = 
        block.inputValue( aColorTransparency ).asFloatVector();


    
    // [1] BASE COLOR X DARK MAP
    resultColor.x =  surfaceColor.x * DarkMap.x;
    resultColor.y =  surfaceColor.y * DarkMap.y;
    resultColor.z =  surfaceColor.z * DarkMap.z;

    // [2] + Light MAPs [1..L]
    
    // [3] X (2 X DETAIL MAP)
    resultColor.x *= 2.0f*DetailMap.x;
    resultColor.y *= 2.0f*DetailMap.y;
    resultColor.z *= 2.0f*DetailMap.z;
    
    // [4] ALPHABLEND Decal MAPS [1..D]
    // [5] X Shadow MAPS [1..S]
    // [6] X Vertex Colors
    // [7] + { Environment X GLOSS MAP }

    // [8] + GLOW MAP
    resultColor.x += GlowMap.x;
    resultColor.y += GlowMap.y;
    resultColor.z += GlowMap.z;

    // [9] ALPHABLEND FOG MAP


    // set ouput color attribute
    MDataHandle outColorHandle = block.outputValue( aOutColor );
    MFloatVector& outColor = outColorHandle.asFloatVector();
    outColor = resultColor;
    outColorHandle.setClean();

    return MS::kSuccess;

  } 
    
  
  return MS::kUnknownParameter;
}

//---------------------------------------------------------------------------

MStatus initializePlugin( MObject obj )
{ 
    const MString UserClassify( "shader/surface" );
    MString command( "if( `window -exists createRenderNodeWindow` )  "
        "{refreshCreateRenderNodeWindow(\"" );
    
    char szVersion[256];

    NiSprintf(szVersion, 256, "NiMultiShader Version:7.0\n");
        
    MFnPlugin plugin( obj, "Multi texture for objects in Maya", szVersion, 
        "Any");

    plugin.registerNode( "NiMultiShader", NiMultiShader::id, 
        NiMultiShader::creator, NiMultiShader::initialize,
        MPxNode::kDependNode, &UserClassify );
    command += UserClassify;
    command += "\");}\n";
    
    MGlobal::executeCommand(command);
    
    
    return MS::kSuccess;
}

//---------------------------------------------------------------------------

MStatus uninitializePlugin( MObject obj )
{
   const MString UserClassify( "shader/surface" );
MString command( "if( `window -exists createRenderNodeWindow` )  "
    "{refreshCreateRenderNodeWindow(\"" );

   MFnPlugin plugin( obj );
   plugin.deregisterNode( NiMultiShader::id );

   command += UserClassify;
   command += "\");}\n";

   MGlobal::executeCommand(command);

   return MS::kSuccess;
}

//---------------------------------------------------------------------------

bool NiMultiShader::setInternalValue( const MPlug& kPlug, 
    const MDataHandle& kHandle)
{
    return false;
}
