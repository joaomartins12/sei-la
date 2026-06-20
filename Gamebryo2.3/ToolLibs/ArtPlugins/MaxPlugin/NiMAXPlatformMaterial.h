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

#ifndef NIMAXPLATFORMMATERIAL_H
#define NIMAXPLATFORMMATERIAL_H

#include "MiResource.h"
#include <stdmat.h>
#include <iparamm2.h>

extern HINSTANCE hInstance;

static Class_ID PLATFORM_MATERIAL_CLASS_ID(0x6e6b25b1,0);

#define NUM_PLATFORM_SUBMTLS 2

#define PB_REF          0
#define MTL_ONE_REF     1
#define MTL_TWO_REF     2
#define MTL_THREE_REF   3
//#define MTL_FOUR_REF    4
#define MTL_NUM_REFS    3

#define PLATFORM_PC_ID      0
#define PLATFORM_XENON_ID   1
#define PLATFORM_PS3_ID     2

class NiMAXPlatformMaterialDlgProc;

class NiMAXPlatformMaterial : public Mtl, public IReshading  
{   
    public:
        IParamBlock2*           m_pkBlock;          // ref #0
        
        Mtl*                    m_pkMtlOne;         // ref #1
        Mtl*                    m_pkMtlTwo;         // ref #2
        Mtl*                    m_pkMtlThree;       // ref #3
        //Mtl*                    m_pkMtlFour;        // ref #4

        Interval                m_kValidInt;
        ReshadeRequirements     m_kReshadeRQ;
        
        static NiMAXPlatformMaterialDlgProc*   ms_pParamDlg;

        Mtl* GetPlatformMtl(unsigned int uiWhichMtl);
        void SetPlatformMtl(Mtl* pkMtl, unsigned int uiWhichMtl);

        Mtl* GetDevImagePlatformMtl(NiDevImageConverter::Platform ePlatform);

        // methods
        NiMAXPlatformMaterial(BOOL loading);
        void NotifyChanged();
        Mtl* UseMtl();

        // From MtlBase and Mtl
        void SetAmbient(Color c, TimeValue t);
        void SetDiffuse(Color c, TimeValue t);
        void SetSpecular(Color c, TimeValue t);
        void SetShininess(float v, TimeValue t);
        
        Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
        Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
        Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
        float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
        float GetShininess(int mtlNum=0, BOOL backFace=FALSE);
        float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
        float WireSize(int mtlNum=0, BOOL backFace=FALSE);

        ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
        void EnableStuff();

        void Shade(ShadeContext& sc);
        float EvalDisplacement(ShadeContext& sc); 
        Interval DisplacementValidity(TimeValue t); 
        void Update(TimeValue t, Interval& valid);
        void Init();
        void Reset();
        Interval Validity(TimeValue t);
        
        Class_ID ClassID();
        SClass_ID SuperClassID();
        void GetClassName(TSTR& s); 

        void DeleteThis();  

        Sampler*  GetPixelSampler(int mtlNum, BOOL backFace );

        // Methods to access sub-materials of meta-materials
        int NumSubMtls();
        Mtl* GetSubMtl(int i);       
        void SetSubMtl(int i, Mtl *m);
        int VPDisplaySubMtl();
        TSTR GetSubMtlSlotName(int i);

        // Methods to access sub texture maps of material or texmap
        int NumSubTexmaps();
        Texmap* GetSubTexmap(int i);
        void SetSubTexmap(int i, Texmap *m);
        TSTR GetSubTexmapSlotName(int i);

        int NumSubs();
        Animatable* SubAnim(int i);
        TSTR SubAnimName(int i);
        int SubNumToRefNum(int subNum);

        // From ref
        int NumRefs();
        RefTargetHandle GetReference(int i);
        void SetReference(int i, RefTargetHandle rtarg);

#if MAX_RELEASE <= 8000
        RefTargetHandle Clone(RemapDir &remap = NoRemap());
#else
        RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir());
#endif
        RefResult NotifyRefChanged(Interval changeInt, 
           RefTargetHandle hTarget, 
           PartID& partID, RefMessage message);

        // IO
        IOResult Save(ISave *isave); 
        IOResult Load(ILoad *iload); 

        // direct ParamBlock access
        int NumParamBlocks();// return number of ParamBlocks in this instance
        IParamBlock2* GetParamBlock(int i);// return i'th ParamBlock
        IParamBlock2* GetParamBlockByID(BlockID id); // return id'd 
                                                     // ParamBlock
        BOOL SetDlgThing(ParamDlg* dlg);

        // begin - reshading code
        BOOL SupportsRenderElements();
        ReshadeRequirements GetReshadeRequirements() {return m_kReshadeRQ; }; 
        void PreShade(ShadeContext& sc, IReshadeFragment* pFrag);
        void PostShade(ShadeContext& sc, IReshadeFragment* pFrag, 
            int& nextTexIndex, IllumParams* ip);
        // end - reshading code

        // From Mtl
        bool IsOutputConst( ShadeContext& sc, int stdID );
        bool EvalColorStdChannel( ShadeContext& sc, int stdID, 
            Color& outClr );
        bool EvalMonoStdChannel( ShadeContext& sc, int stdID, 
            float& outVal );

        void* GetInterface(ULONG id);
};

ClassDesc* GetNiMAXPlatformMaterialDesc();

//---------------------------------------------------------------------------
//  Inline include
#include "NiMAXPlatformMaterial.inl"
//---------------------------------------------------------------------------
#endif
