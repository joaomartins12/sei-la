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

#ifndef NORMALUNIFIERPLUGIN_H
#define NORMALUNIFIERPLUGIN_H

#include "NiTPointerList.h"

typedef NiTObjectArray<NiGeometryPtr> NiGeometryArray;
typedef NiTPrimitiveArray<unsigned short> NiUnsignedShortArray;

//-----------------------------------------------------------------------
class NiUnifySet : public NiRefObject
{
public:
    NiGeometryArray m_kGeoms;
};
NiSmartPointer(NiUnifySet);

//-----------------------------------------------------------------------
class NiUnifyVerts : public NiRefObject
{
public:
    NiUnifyVerts()
    {
        //m_ausIndices.SetSize(10);
    };

    // all indices here are off by one so that 
    // they register as actual valid values with
    // NiTArray's
    NiUnsignedShortArray m_ausIndices;
};
NiSmartPointer(NiUnifyVerts);

//-----------------------------------------------------------------------
    /// This is our Remove Hidden Objects process plug-in. It is used to
    /// examine art during export and remove any objects which are app culled.
class NormalUnifierPlugin : public NiPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    NormalUnifierPlugin();

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiPlugin.
    //-----------------------------------------------------------------------
    /// Returns a plug-in script with default parameters.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 

    /// Does this plug-in have editable options?
    virtual bool HasManagementDialog();
    
    /// Bring up the options dialog and handle user interaction.
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    /// Determines whether or not this plug-in can handle this plug-in info
    /// object.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    
    /// Execute the NiPluginInfo script.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    
protected:
    void CollectGeometry(NiAVObject* pkObject, bool bAutoAdd = false);
    void CollectSets();
    void HandleSets();
    void Unify(NiUnifySet* pkSet);
    bool AlreadyHitThisVertex(NiUnifyVerts* pkVertexArray, 
        unsigned short uiIndex);


    bool CheckForExtraDataTags(NiAVObject* pkObject);
    bool CheckForExtraDataTags(NiAVObject* pkObject, const char* ppcTags[], 
        unsigned int uiNumTags);

    NiGeometryArray m_kGeoms;
    NiTObjectArray<NiUnifySetPtr> m_kSets;
    NiTMap<NiGeometry*, NiUnifyVertsPtr> m_kAlreadyHitVerts;

    float m_fTolerance;
};

#endif  // #ifndef NORMALUNIFIERPLUGIN_H
      