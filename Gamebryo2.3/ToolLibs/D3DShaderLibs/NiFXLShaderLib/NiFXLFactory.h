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

#ifndef NIFXLFACTORY_H
#define NIFXLFACTORY_H

#include "NiFXLShaderLibType.h"

#include <NiD3DDefines.h>
#include <NiSmartPointer.h>
#include <NiTStringMap.h>

class NiFXLFile;

class NIFXLSHADER_ENTRY NiFXLFactory : public NiMemObject
{
public:
    static NiFXLFactory* GetInstance(bool bCreateNew = true);

    //*** Helper functions
    // FXL creation
    static NiFXLFile* CreateFXLFromFile(const char* pcFileName);
    static NiFXLFile* CreateFXLFromCode(const char* pcShaderName, 
        unsigned int uiCodeSize, void* pvCode);

    // Shader assembly flags
    static unsigned int GetFXLCreationFlags();
    static void SetFXLCreationFlags(unsigned int uiFlags);

    // *** begin Emergent internal use only ***
    static const char* GetFXLDirectory();
    static void SetFXLDirectory(const char* pcDirectory);

    bool LoadFXLFromFile(const char* pcFileName, NiFXLFile*& pkEffectFile);
    bool LoadFXLFromCode(const char* pcEffectName, unsigned int uiCodeSize, 
        void* pvCode, NiFXLFile*& pkEffectFile);

    bool LoadFXLFromFile_Xenon(const char* pcFileName, 
        NiFXLFile*& pkEffectFile);
    bool LoadFXLFromCode_Xenon(const char* pcEffectName, 
        unsigned int uiCodeSize, void* pvCode, NiFXLFile*& pkEffectFile);

    // Find functions
    virtual NiFXLFile* GetFXLFile(const char* pcFileName) const;

    // Shader insertion/removal
    virtual void InsertFXLFileIntoMap(NiFXLFile* pkEffectFile);
    virtual void RemoveFXLFileFromMap(const char* pcName);

    static bool NiFXLFactory::ResolveFXLFileName(const char* pcOriginalName, 
        char* pcTrueName, unsigned int uiBufferSize);
    static char* GetShaderText(const char* pcShaderName,
        unsigned int& uiLength);

    void DestroyRendererData();

    static void _SDMInit();
    static void _SDMShutdown();

    // *** end Emergent internal use only ***

protected:
    NiFXLFactory();
    virtual ~NiFXLFactory();


    static NiFXLFactory* ms_pkFactory;

    static unsigned int ms_uiCreationFlags;

    static char* ms_pcFXLDirectory;

    // Maps for effect file names to effects.
    NiTStringPointerMap<NiFXLFile*>* m_pkFXLMap;
};

typedef NiPointer<NiFXLFactory> NiFXLFactoryPtr;

#endif  //NID3DXEFFECTFACTORY_H
