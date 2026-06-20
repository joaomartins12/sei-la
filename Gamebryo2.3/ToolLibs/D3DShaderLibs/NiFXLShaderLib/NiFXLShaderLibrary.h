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

#ifndef NIFXLSHADERLIBRARY_H
#define NIFXLSHADERLIBRARY_H

#include "NiFXLShaderLibType.h"

#include <NiD3DShaderLibrary.h>
#include <NiTPointerList.h>

NiSmartPointer(NiShaderLibraryDesc);

class NiFXLFile;

class NIFXLSHADER_ENTRY NiFXLShaderLibrary : 
    public NiD3DShaderLibrary
{
protected:
    NiFXLShaderLibrary();

public:
    virtual ~NiFXLShaderLibrary();

    static NiFXLShaderLibrary* Create(NiD3DRenderer* pkD3DRenderer, 
        int iDirectoryCount, char* apcDirectories[], bool bRecurseSubFolders);
    static void Shutdown();

    virtual void SetRenderer(NiD3DRenderer* pkRenderer);

    virtual NiShader* GetShader(NiRenderer* pkRenderer, 
        const char* pcName, unsigned int uiImplementation);
    virtual bool ReleaseShader(const char* pcName, 
        unsigned int uiImplementation);
    virtual bool ReleaseShader(NiShader* pkShader);

    virtual NiShaderLibraryDesc* GetShaderLibraryDesc();

    // *** begin Emergent internal use only ***
    virtual void SetShaderLibraryDesc(NiShaderLibraryDesc* pkLibDesc);

    bool InsertFXLFileIntoList(NiFXLFile* pkFXLFile);
    unsigned int GetFXLFileCount();
    NiFXLFile* GetFXLFile(const char* pcName);
    bool LoadFXLFiles(const char* pcDirectory, 
        bool bRecurseSubFolders);
    // *** end Emergent internal use only ***

protected:
    NiTPointerList<NiFXLFile*> m_kFXLFileList;
    NiTStringMap<NiFXLFile*> m_kFXLTechniqueMap;
};

NiSmartPointer(NiFXLShaderLibrary);

#endif  //NIFXLSHADERLIBRARY_H
