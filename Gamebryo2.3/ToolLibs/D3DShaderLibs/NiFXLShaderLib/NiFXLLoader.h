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

#ifndef NIFXLLOADER_H
#define NIFXLLOADER_H

#include <NiMain.h>

class NiShaderLibraryDesc;
class NiFXLShaderLibrary;

class NiFXLLoader : public NiMemObject
{
protected:
    NiFXLLoader();
    ~NiFXLLoader();

public:
    static NiFXLLoader* Create();
    static void Destroy();

    bool LoadAllFXLFiles(NiFXLShaderLibrary* pkLibrary, 
        const char* pcDirectory, bool bRecurseDirectories);

    static void* CreateEffectFromFile(void* pvD3DDevice,
        const char* pcShaderPath, unsigned int uiCreationFlags, 
        char*& pcError);

    static void* CreateEffectFromCode(void* pvD3DDevice, void* pvCode, 
        unsigned int uiCodeSize, unsigned int uiCreationFlags, 
        char*& pcError);

protected:
    unsigned int GetFXLFileCount();
    const char* GetFirstFXLFile();
    const char* GetNextFXLFile();

    void FindAllFXLFiles(const char* pcDirectory, 
        bool bRecurseDirectories);
    unsigned int FindAllFXLFilesInDirectory(const char* pcDirectory, 
        const char* pcExt, bool bRecurseDirectories, 
        NiTPointerList<char*>* pkFileList);

    bool ProcessFXLFile(const char* pcFilename, const char* pcExt,
        NiTPointerList<char*>* pkFileList);

    bool LoadAllFXLFiles(NiFXLShaderLibrary* pkLibrary);

    static NiFXLLoader* ms_pkLoader;

    NiTListIterator m_kFXLListIter;
    NiTPointerList<char*> m_kFXLList;
};

#endif  //NIFXLLOADER_H
