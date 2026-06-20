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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiEntityStreamingAscii.h"
#include "NiScene.h"
#include "NiFactories.h"
#include "NiExternalAssetParams.h"

// Internal Aliases
#define PT_ENTITYPOINTER    NiEntityPropertyInterface::PT_ENTITYPOINTER
#define PT_NIOBJECTPOINTER  NiEntityPropertyInterface::PT_NIOBJECTPOINTER
#define PT_FLOAT            NiEntityPropertyInterface::PT_FLOAT
#define PT_BOOL             NiEntityPropertyInterface::PT_BOOL
#define PT_INT              NiEntityPropertyInterface::PT_INT
#define PT_UINT             NiEntityPropertyInterface::PT_UINT
#define PT_SHORT            NiEntityPropertyInterface::PT_SHORT
#define PT_USHORT           NiEntityPropertyInterface::PT_USHORT
#define PT_STRING           NiEntityPropertyInterface::PT_STRING
#define PT_POINT2           NiEntityPropertyInterface::PT_POINT2
#define PT_POINT3           NiEntityPropertyInterface::PT_POINT3
#define PT_QUATERNION       NiEntityPropertyInterface::PT_QUATERNION
#define PT_MATRIX3          NiEntityPropertyInterface::PT_MATRIX3
#define PT_COLOR            NiEntityPropertyInterface::PT_COLOR
#define PT_COLORA           NiEntityPropertyInterface::PT_COLORA

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

NiFixedString NiEntityStreamingAscii::STREAMING_EXTENSION;
NiFixedString NiEntityStreamingAscii::STREAMING_DESCRIPTION;

//---------------------------------------------------------------------------
NiEntityStreamingAscii::NiEntityStreamingAscii()
{
}
//---------------------------------------------------------------------------
NiEntityStreamingAscii::~NiEntityStreamingAscii() 
{
    Flush();
}
//---------------------------------------------------------------------------
NiFixedString NiEntityStreamingAscii::GetFileExtension() const
{
    return STREAMING_EXTENSION;
}
//---------------------------------------------------------------------------
NiFixedString NiEntityStreamingAscii::GetFileDescription() const
{
    return STREAMING_DESCRIPTION;
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::Flush()
{
    m_kDOM.Flush();
    m_kLinkMaps.RemoveLinks();
    NiStrcpy(m_acFullPath, NI_MAX_PATH, "");
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::StoreWorkingPath(const char* pcFileName)
{
    // Assets store file names as unique absolute paths, however, 
    // when a GSA file is saved out, relative paths are used.
    // In order to restore a  unique absolute path, we need to know what 
    // directory the relative paths are "relative to".
    // In most cases, this will be the current working directory, but that
    // can't be guaranteed. For now, we assume pcFileName has a full 
    // standarized path so that we can grab.

    NiStrcpy(m_acFullPath, NI_MAX_PATH, pcFileName);

    if (!NiPath::IsUniqueAbsolute(m_acFullPath))
    {
          if (NiPath::IsRelative(m_acFullPath))
          {
              // Use the current working directory to build unique abs path.
              // On XENON, this should be something like "D:\\Data\\filename"
              // while on PS3, it might be "/app_home/../Data/Filename"
              NIVERIFY(
                  NiPath::ConvertToAbsolute(m_acFullPath, NI_MAX_PATH) > 0);
          }
          else
          {
              // This is an absolute path, though not unique, ie. it has
              // relative component parts.
              //Make unique and store in m_acFullPath
              NiPath::RemoveDotDots(m_acFullPath);
          }
    }

    // Create filename error message.
    NiSprintf(m_acFilenameErrorMsg, 512, "GSA File Error: %s",
        m_acFullPath);

    // Strip off the filename from the path string.
    char* pcLastSlash = strrchr(m_acFullPath, NI_PATH_DELIMITER_CHAR);
    if (pcLastSlash)
    {
        *pcLastSlash = '\0';
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadMasterRefLinkID(unsigned int& uiRefLinkID)
{
    const char* pcMasterLinkID = 
        m_kDOM.GetAttributeFromCurrent("MasterLinkID");

    // Having a master is not required
    if (pcMasterLinkID == NULL)
        return false;

    if (NiStricmp(pcMasterLinkID, "NULL")==0)
    {
        // Users might do this since NULL tag is supported for RefLinkID's
        // The default, however is already NULL, so just return false.
        return false;
    }

    // Look up Master
#ifdef _DEBUG
    int iFieldsAssigned =
#endif  // #ifdef _DEBUG
#if defined(_MSC_VER) && _MSC_VER >= 1400
        sscanf_s(pcMasterLinkID, "%u", &uiRefLinkID);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        sscanf(pcMasterLinkID, "%u", &uiRefLinkID);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
#ifdef _DEBUG
    NIASSERT(iFieldsAssigned == 1);
#endif  // #ifdef _DEBUG

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadRefLinkID(unsigned int& uiRefLinkID)
{
    const char* pcRefLinkID = m_kDOM.GetAttributeFromCurrent("RefLinkID");
    if (!pcRefLinkID)
    {
        return false;
    }

    if (NiStricmp(pcRefLinkID, "NULL") == 0)
    {
        uiRefLinkID = REF_LINK_NULL;
        return true;
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf_s(pcRefLinkID, "%u", &uiRefLinkID);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf(pcRefLinkID, "%u", &uiRefLinkID);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadClassValueNameLinkID(
    const char*& pcClass, const char*& pcValue, const char*& pcName,
    unsigned int& uiLinkID)
{
    NIASSERT(m_kDOM.IsCurrentSectionValid());

    pcClass = m_kDOM.GetAttributeFromCurrent("Class");
    pcValue = m_kDOM.GetValueFromCurrent();

    // Some entities and components can have their names set.
    pcName = m_kDOM.GetAttributeFromCurrent("Name");

    const char* pcLinkID = m_kDOM.GetAttributeFromCurrent("LinkID");

    if (!pcValue || !pcName || !pcClass || !pcLinkID)
        return false;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf_s(pcLinkID, "%u", &uiLinkID);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf(pcLinkID, "%u", &uiLinkID);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
        return false;

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::CreateObjectsAndAssignUniqueLinkIDs(
    NiEntityStreaming::IDLinkMaps& kMaps)
{
    // Child objects can be "ENTITY" or "COMPONENT"
    m_kDOM.SetSectionToFirstChild();

    while (m_kDOM.IsCurrentSectionValid())
    {
        const char *pcValue, *pcClass, *pcName;
        unsigned int uiID;

        if (!ReadClassValueNameLinkID(pcClass, pcValue, pcName, uiID))
        {
            ReportError("Could not parse ENTITY/COMPONENT section.",
                m_acFilenameErrorMsg, NULL, NULL);
            return false;
        }

        NiTFactory<NiEntityPropertyInterface*>* pkCompFactory = 
            NiFactories::GetEntityCompFactory();
        if (pkCompFactory == NULL)
        {
            char acErrorMsg[1024];
            NiSprintf(acErrorMsg, 1024, "Could not create object of type "
                "\"%s\".\nThe EntityCompFactory could not be found. Ensure "
                "that NiEntity.h is included.", pcClass);
            ReportError(acErrorMsg, m_acFilenameErrorMsg, NULL, NULL);
            return false;
        }
        
        NiEntityPropertyInterface* pkPropIntf =
            pkCompFactory->Create(pcClass);
        if (pkPropIntf == NULL)
        {
            // REPORT_ERROR: Don't know how to create this component.
            // Application or plug-in needs to register it.
            char acErrorMsg[1024];
            NiSprintf(acErrorMsg, 1024, "Could not create object of type "
                "\"%s\".\nNo creation function has been registered for this "
                "type.", pcClass);
            ReportError(acErrorMsg, m_acFilenameErrorMsg, NULL, NULL);
            return false;
        }

        pkPropIntf->SetName(pcName); // Set the name if allowed.

        if (NiStricmp(pcValue, "COMPONENT") == 0)
        {
            m_kLinkMaps.AssignID(pkPropIntf, uiID);
        }
        else if (NiStricmp(pcValue, "ENTITY") == 0)
        {
            m_kLinkMaps.AssignID(pkPropIntf, uiID);
        }

        m_kDOM.SetSectionToNextSibling();
    }

    m_kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::Load(const char* pcFileName)
{
    Flush();

    if (!StoreWorkingPath(pcFileName))
        return false;

    RemoveAllScenes();

    //-- Initialize
    m_kDOM.Init(pcFileName);

    //-- Read from disk
    if (!m_kDOM.LoadFile())
    {
        //if (m_kDOM.HasError())
        //{
        //    char acString[2048];
        //    sprintf(acString,"Error ID: %d\nError Desciption: %s\n" 
        //       "Row: %d\nCol:%d", m_kDOM.GetErrorID(), m_kDOM.GetErrorDesc(),
        //        m_kDOM.GetErrorRow(), m_kDOM.GetErrorCol());
        //}       
        Flush();

        ReportError("The GSA file could not be parsed as valid XML.",
            m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    //-- Read from DOM (and create a Scene)
    if (!ReadFromDOM())
    {   
        Flush();
        RemoveAllScenes();

        return false;
    }

    Flush();
    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadAndCheckVersionFromDOM()
{
    const char* pcMajor = m_kDOM.GetAttributeFromCurrent("Major");
    const char* pcMinor = m_kDOM.GetAttributeFromCurrent("Minor");
    const char* pcPatch = m_kDOM.GetAttributeFromCurrent("Patch");

    if (!pcMajor || !pcMinor || !pcPatch)
        return false;

    unsigned int uiMajor, uiMinor, uiPatch;
    int iFieldsAssigned;
    
#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf_s(pcMajor, "%u", &uiMajor);  
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf(pcMajor, "%u", &uiMajor);  
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1 || uiMajor > VERSION_MAJOR)
        return false;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf_s(pcMinor, "%u", &uiMinor);  
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf(pcMinor, "%u", &uiMinor);  
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
        return false;
    if (uiMajor == VERSION_MAJOR && uiMinor > VERSION_MINOR)
        return false;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf_s(pcPatch, "%u", &uiPatch);  
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf(pcPatch, "%u", &uiPatch);  
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
        return false;
    if (uiMajor == VERSION_MAJOR && uiMinor == VERSION_MINOR &&
        uiPatch > VERSION_PATCH)
        return false;

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadFromDOM()
{
    if (!m_kDOM.ResetSectionTo("GSA"))
    {
        ReportError("No GSA section found in file.", m_acFilenameErrorMsg,
            NULL, NULL);
        return false;
    }

    if (!ReadAndCheckVersionFromDOM())
    {
        ReportError("GSA file has incompatible version number.",
            m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    m_kDOM.SetSectionToFirstChild();
    if (!m_kDOM.IsCurrentSectionValid())
    {
        ReportError("Empty GSA file.", m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    while (m_kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = m_kDOM.GetValueFromCurrent();
        if (NiStricmp(pcValue, "OBJECTS") == 0)
        {
            // Pass 1: Create objects in a map with unsigned int ID's as keys
            if (!CreateObjectsAndAssignUniqueLinkIDs(m_kLinkMaps))
            {
                return false;
            }

            // Pass 2: Read object information and link up reference ID's
            if (!ReadObjectsFromDOM())
            {
                return false;
            }
        }
        else if (NiStricmp(pcValue, "SCENES") == 0)
        {
            if (!ReadScenesFromDOM())
                return false;       
        }

        m_kDOM.SetSectionToNextSibling();
    }

    m_kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadObjectsFromDOM()
{
    // Child objects can be "ENTITY" or "COMPONENT"
    m_kDOM.SetSectionToFirstChild();

    while (m_kDOM.IsCurrentSectionValid())
    {
        const char *pcValue, *pcClass, *pcName;
        unsigned int uiID;

        if (!ReadClassValueNameLinkID(pcClass, pcValue, pcName, uiID))
        {
            ReportError("Could not parse ENTITY/COMPONENT section.",
                m_acFilenameErrorMsg, NULL, NULL);
            return false;
        }

        if ((NiStricmp(pcValue, "COMPONENT") != 0) &&
            (NiStricmp(pcValue, "ENTITY") != 0))
        {
            // Unknown type: Expecting component or entity.
            continue;
        }

        NiEntityPropertyInterface* pkPropIntf = m_kLinkMaps.GetAt(uiID);
        NIASSERT(pkPropIntf);
        if (!ReadEntityOrComponentFromDOM(pkPropIntf))
        {
            return false;
        }

        m_kDOM.SetSectionToNextSibling();
    }

    m_kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadEntityRefFromDOM(
    NiEntityInterface*& pkEntity)
{
    unsigned int uiRefLinkID;
    
    if (!ReadRefLinkID(uiRefLinkID))
        return false;

    if (uiRefLinkID == REF_LINK_NULL)
    {
        pkEntity = NULL;
        return true;
    }

    pkEntity = (NiEntityInterface*)m_kLinkMaps.GetAt(uiRefLinkID);
    if (!pkEntity)
    {
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadScenesFromDOM()
{
    // [2] Traverse DOM for NiScene, and create NiScene as needed...
    //      Link to Entity's... each Entity... links it's master component,
    //      (recursively), then links components, and then it's property
    //      pointers...
    m_kDOM.SetSectionToFirstChild();

    if (!m_kDOM.IsCurrentSectionValid())
    {
        // Should have at least one scene.
        ReportError("No SCENE sections defined in GSA file.",
            m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    while (m_kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = m_kDOM.GetValueFromCurrent();
        if (NiStricmp(pcValue, "SCENE") != 0)
        {
            m_kDOM.SetSectionToNextSibling();
            continue;
        }

        // Read in class (ensure it is NiScene)
        // Read in name...
        const char* pcClass = m_kDOM.GetAttributeFromCurrent("Class");
        const char* pcName = m_kDOM.GetAttributeFromCurrent("Name");

        if (!pcClass || !pcName)
        {
            ReportError("Error parsing SCENE tag.", m_acFilenameErrorMsg,
                NULL, NULL);
            return false;
        }

        // Only NiScene classes are currently supported.
        if (NiStricmp(pcClass, "NiScene") != 0)
        {
            ReportError("Non NiScene specified in SCENE tag. Only NiScene is "
                "supported by GSA files.", m_acFilenameErrorMsg, NULL,
                NULL);
            return false;
        }
        NiScene* pkScene = NiNew NiScene(pcName);
        InsertScene(pkScene);

        if (!ReadSceneFromDOM(pkScene))
        {
            return false;
        }

        m_kDOM.SetSectionToNextSibling();
    }

    m_kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadSceneFromDOM(NiScene* pkScene)
{
    m_kDOM.SetSectionToFirstChild();

    while (m_kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = m_kDOM.GetValueFromCurrent();

        if (NiStricmp(pcValue, "ENTITY") == 0)
        {
            NiEntityInterface* pkEntity; 
            if (!ReadEntityRefFromDOM(pkEntity))
            {
                ReportError("Cannot resolve entity RefLinkID.",
                    m_acFilenameErrorMsg, NULL, NULL);
                return false;
            }

            NIVERIFY(pkScene->AddEntity(pkEntity));
        }
        else if (NiStricmp(pcValue, "SELECTIONS") == 0)
        {
            if (!ReadSelectionSetsFromDOM(pkScene))
            {
                return false;
            }
        }

        m_kDOM.SetSectionToNextSibling();
    }

    m_kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadSelectionSetsFromDOM(NiScene* pkScene)
{
    // <SELECTIONS>
    //     <SET Name="The Set Name">
    //          <ENTITY RefLinkID="#">
    //     </SET>
    // </SELECTIONS>

    // Set to first "SET"
    m_kDOM.SetSectionToFirstChild();
    NIASSERT(m_kDOM.IsCurrentSectionValid());

    while (m_kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = m_kDOM.GetValueFromCurrent();

        if (NiStricmp(pcValue, "SET") != 0)
        {
            m_kDOM.SetSectionToNextSibling();
            continue;
        }

        const char* pcName = m_kDOM.GetAttributeFromCurrent("Name");
        if (!pcName)
        {
            ReportError("Unnamed selection set found in scene. Selection set "
                "will not be created.", m_acFilenameErrorMsg, NULL, NULL);
            continue;
        }

        NiEntitySelectionSet* pkSelectionSet = 
            NiNew NiEntitySelectionSet(pcName);

        // Set to first "ENTITY" entry
        m_kDOM.SetSectionToFirstChild();
        NIASSERT(m_kDOM.IsCurrentSectionValid());
        while (m_kDOM.IsCurrentSectionValid())
        {
            pcValue = m_kDOM.GetValueFromCurrent();

            if (NiStricmp(pcValue, "ENTITY") != 0)
            {
                m_kDOM.SetSectionToNextSibling();
                continue;
            }

            NiEntityInterface* pkEntity; 
            if (!ReadEntityRefFromDOM(pkEntity))
            {
                m_kDOM.SetSectionToNextSibling();
                char acErrorMsg[1024];
                NiSprintf(acErrorMsg, 1024, "Unable to resolve an entity "
                    "RefLinkID in the \"%s\" selection set. Entity will not "
                    "be added to the selection set.",
                    (const char*) pkSelectionSet->GetName());
                ReportError(acErrorMsg, m_acFilenameErrorMsg, NULL, NULL);
                continue;
            }

            pkSelectionSet->AddEntity(pkEntity);

            m_kDOM.SetSectionToNextSibling();
        }
        m_kDOM.EndSection();

        pkScene->AddSelectionSet(pkSelectionSet);

        m_kDOM.SetSectionToNextSibling();
    }

    m_kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadMasterFromDOM(NiEntityPropertyInterface*
    pkPropIntf)
{
    unsigned int uiRefLinkID;
    if (!ReadMasterRefLinkID(uiRefLinkID))
    {
        // Not having a Master is legitimate.
        return true; 
    }

    if (pkPropIntf->GetInterfaceType() == 
        NiEntityInterface::IT_ENTITYINTERFACE)
    {
        NiEntityInterface* pkMasterEntity = 
            (NiEntityInterface*)m_kLinkMaps.GetAt(uiRefLinkID);
        ((NiEntityInterface*)pkPropIntf)->SetMasterEntity(pkMasterEntity);
    }
    else if (pkPropIntf->GetInterfaceType() ==
        NiEntityComponentInterface::IT_COMPONENTINTERFACE)
    {
        NiEntityComponentInterface* pkMasterComp = 
            (NiEntityComponentInterface*)m_kLinkMaps.GetAt(uiRefLinkID);
        ((NiEntityComponentInterface*)pkPropIntf)->
            SetMasterComponent(pkMasterComp);
    }
    else
    {
        // Unknown interface type.
        ReportError("Unknown object interface type.", m_acFilenameErrorMsg,
            pkPropIntf->GetName(), NULL);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadEntityOrComponentFromDOM(
    NiEntityPropertyInterface* pkPropIntf)
{
    if (!ReadMasterFromDOM(pkPropIntf))
    {
        return false;
    }

    const char* pcTemplateID = m_kDOM.GetAttributeFromCurrent("TemplateID");
    if (pcTemplateID)
    {
        int iA, iB, iC, iD, iE, iF, iG, iH, iI, iJ, iK, iL, iM, iN, iO, iP;
#if defined(_MSC_VER) && _MSC_VER >= 1400
        int iFieldsAssigned = sscanf_s(pcTemplateID, 
            "%x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x", 
            &iA,&iB,&iC,&iD,&iE,&iF,&iG,&iH,&iI,&iJ,&iK,&iL,&iM,&iN,&iO,&iP);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        int iFieldsAssigned = sscanf(pcTemplateID, 
            "%x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x", 
            &iA,&iB,&iC,&iD,&iE,&iF,&iG,&iH,&iI,&iJ,&iK,&iL,&iM,&iN,&iO,&iP);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
        if (iFieldsAssigned != 16)
        {
            char acErrorMsg[1024];
            NiSprintf(acErrorMsg, 1024, "Unable to parse TemplateID: \"%s\".",
                pcTemplateID);
            ReportError(acErrorMsg, m_acFilenameErrorMsg,
                pkPropIntf->GetName(), NULL);
            return false;
        }

        const NiUniqueID kUniqueID(iA, iB, iC, iD, iE, iF, iG, iH, iI, iJ, iK,
            iL, iM, iN, iO, iP);
        pkPropIntf->SetTemplateID(kUniqueID);
    }

    const char* pcHidden = m_kDOM.GetAttributeFromCurrent("Hidden");
    if (pcHidden)
    {
        if (pkPropIntf->GetInterfaceType() == 
            NiEntityInterface::IT_ENTITYINTERFACE)
        {
            bool bHidden = false;
            if (NiStricmp(pcHidden, "TRUE") == 0)
                bHidden = true;

            NiEntityInterface* pkEntity = (NiEntityInterface*)pkPropIntf;
            pkEntity->SetHidden(bHidden);
        }
        else
        {
            ReportError("Hidden tag found on non-entity object.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), NULL);
        }
    }

    m_kDOM.SetSectionToFirstChild();

    // In case an entity does not have any properties or components...
    if (!m_kDOM.IsCurrentSectionValid())
    {
        m_kDOM.EndSection();
        return true;
    }

    // [1] LINK COMPONENTS
    while (m_kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = m_kDOM.GetValueFromCurrent();

        if (NiStricmp(pcValue, "COMPONENT") == 0)
        {
            // We must be dealing with an Entity that has components
            // if we enter this section.

            unsigned int uiRefLinkID;
            if (!ReadRefLinkID(uiRefLinkID))
            {
                ReportError("Error parsing component RefLinkID.",
                    m_acFilenameErrorMsg, pkPropIntf->GetName(), NULL);
                return false;
            }

            NiEntityComponentInterface* pkComp = NULL;
            if (uiRefLinkID != REF_LINK_NULL)
            {
                pkComp = (NiEntityComponentInterface*)
                    m_kLinkMaps.GetAt(uiRefLinkID);
                if (!pkComp)
                {
                    ReportError("Invalid component RefLinkID.",
                        m_acFilenameErrorMsg, pkPropIntf->GetName(), NULL);
                    return false;
                }
            }

            if (pkComp)
            {
                // Component properties assumed      
                if (!((NiEntityInterface*)pkPropIntf)->AddComponent(pkComp,
                    false))
                {
                    char acErrorMsg[1024];
                    NiSprintf(acErrorMsg, 1024, "Unable to add component "
                        "\"%s\" to entity.", (const char*) pkComp->GetName());
                    ReportError(acErrorMsg, m_acFilenameErrorMsg,
                        pkPropIntf->GetName(), NULL);
                    return false;
                }
            }
        }
        else if (NiStricmp(pcValue, "PROPERTY") == 0)
        {
            if (!ReadPropertyFromDOM(pkPropIntf))
            {
                return false;
            }
        }
        else
        {
            // Unknown tag in ENTITY section. Ignore and continue.
        }

        m_kDOM.SetSectionToNextSibling();
    }

    m_kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPropertyFromDOM(
    NiEntityPropertyInterface* pkPropIntf)
{
    // If pkPropIntf is a Entity interface, then it is assumed that
    // non-component properties are being read in.

    const char* pcClass = m_kDOM.GetAttributeFromCurrent("Class");
    const char* pcName = m_kDOM.GetAttributeFromCurrent("Name");

    if (!pcClass || !pcName)
    {
        ReportError("Error parsing PROPERTY tag.", m_acFilenameErrorMsg,
            pkPropIntf->GetName(), NULL);
        return false;
    }

    if (pkPropIntf->IsAddPropertySupported())
    {
        const char* pcSemanticType = 
            m_kDOM.GetAttributeFromCurrent("SemanticType");
        const char* pcDisplayName = 
            m_kDOM.GetAttributeFromCurrent("DisplayName");
        const char* pcDescription = 
            m_kDOM.GetAttributeFromCurrent("Description");

        if (!pcSemanticType || !pcDisplayName || !pcDescription)
        {
            ReportError("Error parsing PROPERTY tag.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), pcName);
            return false;
        }

        // Property must be constructed.
        if (!ReadPropertyFromDOM(pcClass, pcName, pcSemanticType, 
            pcDisplayName, pcDescription, pkPropIntf))
        {
            ReportError("Error reading property value.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), pcName);
            return false;
        }
    }
    else
    {
        // Handle each property appropriately
        if (!ReadPropertyFromDOM(pcClass, pcName, pkPropIntf))
        {
            ReportError("Error reading property value.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), pcName);
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPropertyFromDOM(const char* pcPrimitive,
    const char* pcName, const char* pcSemanticType, 
    const char* pcDisplayName, const char* pcDescription,
    NiEntityPropertyInterface* pkPropIntf)
{
    pkPropIntf->AddProperty(pcName, pcDisplayName, pcPrimitive,
        pcSemanticType, pcDescription);

    return ReadPropertyFromDOM(pcPrimitive, pcName, pkPropIntf);
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPropertyArrayFromDOM(
    const char* pcPrimitive, const char* pcName, 
    NiEntityPropertyInterface* pkPropIntf)
{
    unsigned int ui=0;
    // Section is set at ITEM...

    NIASSERT(m_kDOM.IsCurrentSectionValid());
    if( pkPropIntf->IsAddPropertySupported())
    {
        pkPropIntf->MakeCollection(pcName, true);
    }
    while (m_kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = m_kDOM.GetValueFromCurrent();

        if (NiStricmp(pcValue, "ITEM") != 0)
        {
            m_kDOM.SetSectionToNextSibling();
            continue;
        }

        NiBool bSuccess;

        if (m_kDOM.IsCurrentLeaf() && 
            m_kDOM.GetAttributeCountFromCurrent() == 0)
        {
            // Special case where <ITEM/> represents an empty array
            bSuccess = true;
        }
        else if (NiStricmp(pcPrimitive, PT_ENTITYPOINTER) == 0)
            bSuccess = ReadEntityPointerFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_NIOBJECTPOINTER) == 0)
            bSuccess = ReadObjectPointerFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_FLOAT) == 0)
            bSuccess = ReadFloatFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_BOOL) == 0)
            bSuccess = ReadBoolFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_INT) == 0)
            bSuccess = ReadIntFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_UINT) == 0)
            bSuccess = ReadUIntFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_SHORT) == 0)
            bSuccess = ReadShortFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_USHORT) == 0)
            bSuccess = ReadUShortFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_STRING) == 0)
            bSuccess = ReadStringFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_POINT2) == 0)
            bSuccess = ReadPoint2FromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_POINT3) == 0)
            bSuccess = ReadPoint3FromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_QUATERNION) == 0)
            bSuccess = ReadQuaternionFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_MATRIX3) == 0)
            bSuccess = ReadMatrix3FromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_COLOR) == 0)
            bSuccess = ReadColorFromDOM(pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_COLORA) == 0)
            bSuccess = ReadColorAFromDOM(pkPropIntf, pcName, ui);
        else
            bSuccess = ReadUnknownTypeFromDOM(pkPropIntf, pcName, ui);

        if (!bSuccess)
            return false;

        m_kDOM.SetSectionToNextSibling();
        ui++;
    }

    m_kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPropertyFromDOM(const char* pcPrimitive,
    const char* pcName, NiEntityPropertyInterface* pkPropIntf)
{
    if (m_kDOM.SetSectionTo("ITEM"))
        return ReadPropertyArrayFromDOM(pcPrimitive, pcName, pkPropIntf);
    
    m_kDOM.EndSection();

    NiBool bSuccess;

    if (NiStricmp(pcPrimitive, PT_ENTITYPOINTER) == 0)
        bSuccess = ReadEntityPointerFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_NIOBJECTPOINTER) == 0)
        bSuccess = ReadObjectPointerFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_FLOAT) == 0)
        bSuccess = ReadFloatFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_BOOL) == 0)
        bSuccess = ReadBoolFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_INT) == 0)
        bSuccess = ReadIntFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_UINT) == 0)
        bSuccess = ReadUIntFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_SHORT) == 0)
        bSuccess = ReadShortFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_USHORT) == 0)
        bSuccess = ReadUShortFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_STRING) == 0)
        bSuccess = ReadStringFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_POINT2) == 0)
        bSuccess = ReadPoint2FromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_POINT3) == 0)
        bSuccess = ReadPoint3FromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_QUATERNION) == 0)
        bSuccess = ReadQuaternionFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_MATRIX3) == 0)
        bSuccess = ReadMatrix3FromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_COLOR) == 0)
        bSuccess = ReadColorFromDOM(pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_COLORA) == 0)
        bSuccess = ReadColorAFromDOM(pkPropIntf, pcName);
    else
        bSuccess = ReadUnknownTypeFromDOM(pkPropIntf, pcName);

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::Save(const char* pcFileName)
{
    Flush();

    if (!StoreWorkingPath(pcFileName))
        return false;

    AssignUniqueLinkIDs(m_kLinkMaps);

    //-- Initialize
    m_kDOM.Init(pcFileName);

    //-- Write to DOM
    WriteToDOM();

    //-- Write to disk
    NiBool bSuccess = m_kDOM.SaveFile();
    if (!bSuccess)
    {
        ReportError("An error occurred when saving the GSA file.",
            m_acFilenameErrorMsg, NULL, NULL);
    }

    Flush();

    return bSuccess;
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteToDOM()
{
    m_kDOM.WriteHeader();

    m_kDOM.BeginSection("GSA");

    WriteVersionToDOM();

    WriteMapObjectsToDOM();

    m_kDOM.BeginSection("SCENES");

    // Process each scene
    unsigned int uiSceneCount = GetSceneCount();
    for (unsigned int uiScene=0; uiScene < uiSceneCount; uiScene++)
    {
        NiScene* pkScene = GetSceneAt(uiScene);
        if (pkScene == NULL)
            continue;

        WriteSceneToDOM(pkScene);
    }

    m_kDOM.EndSection(); // End SCENES

    m_kDOM.EndSection(); // End "GSA"
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteMapObjectsToDOM()
{
    m_kDOM.BeginSection("OBJECTS");

    // Write Map out...
    unsigned int uiMaxID = m_kLinkMaps.GetIDCount();
    
    for (unsigned int ui=0; ui<uiMaxID; ui++)
    {
        NiEntityPropertyInterface* pkPropIntf = m_kLinkMaps.GetAt(ui);

        unsigned int uiID;
        NIVERIFY(NIBOOL_IS_TRUE(m_kLinkMaps.GetID(pkPropIntf, uiID)));
        NIASSERT(uiID == ui);

        if (pkPropIntf->GetInterfaceType() == 
            NiEntityComponentInterface::IT_COMPONENTINTERFACE)
        {    
            WriteComponentToDOM((NiEntityComponentInterface*)pkPropIntf);
        }
        else if (pkPropIntf->GetInterfaceType() ==
            NiEntityInterface::IT_ENTITYINTERFACE)
        {
            WriteEntityToDOM((NiEntityInterface*)pkPropIntf);
        }
        else
        {
            NIASSERT(!"Unknown Interface Type!");
        }
    }

    m_kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteSceneToDOM(NiScene* pkScene)
{
    m_kDOM.BeginSection("SCENE");
    m_kDOM.AssignAttribute("Class", "NiScene");
    m_kDOM.AssignAttribute("Name", pkScene->GetName());

    WriteSelectionSetsToDOM(pkScene);

    // Process each entity
    unsigned int uiEntityCount = pkScene->GetEntityCount();
    for (unsigned int uiEntity = 0; uiEntity < uiEntityCount; uiEntity++)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(uiEntity);
        WriteEntityRefToDOM(pkEntity);
    }

    m_kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteEntityRefToDOM(NiEntityInterface* pkEntity)
{
    if (pkEntity == NULL)
    {
        m_kDOM.BeginSection("ENTITY");
        m_kDOM.AssignAttribute("RefLinkID", "NULL");
        m_kDOM.EndSection();
        return;
    }

    unsigned int uiEntityID;
    NIVERIFY(NIBOOL_IS_TRUE(m_kLinkMaps.GetID(pkEntity, uiEntityID)));

    m_kDOM.BeginSection("ENTITY");
    m_kDOM.AssignAttribute("RefLinkID", uiEntityID);
    m_kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteTemplateIDToDOM(NiEntityPropertyInterface* 
    pkPropIntf)
{
    NiUniqueID kUniqueID = pkPropIntf->GetTemplateID();

    if (!pkPropIntf->SetTemplateID(kUniqueID))
        return;

    // If the unique id can be set, then it should be written out.
    NIASSERT(kUniqueID.m_aucValue);
    char acValue[1024] = "";

    for(unsigned int ui=0; ui < NUM_BYTES_IN_UNIQUE_ID; ui++)
    {
        char acBuff[32];
        NiSprintf(acBuff, 32, "%x", kUniqueID.m_aucValue[ui]);
        NiStrcat(acValue, 1024, acBuff);
        if (ui != NUM_BYTES_IN_UNIQUE_ID-1)
            NiStrcat(acValue, 1024, ",");
    }

    m_kDOM.AssignAttribute("TemplateID", acValue);
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteEntityToDOM(NiEntityInterface* pkEntity)
{
    unsigned int uiID;
    m_kLinkMaps.GetID(pkEntity, uiID);

    m_kDOM.BeginSection("ENTITY");
    m_kDOM.AssignAttribute("LinkID", uiID);
    m_kDOM.AssignAttribute("Class", pkEntity->GetClassName());
    m_kDOM.AssignAttribute("Name", pkEntity->GetName());

    WriteTemplateIDToDOM(pkEntity);

    if (pkEntity->GetHidden())
    {
        m_kDOM.AssignAttribute("Hidden", "TRUE");
    }

    NiEntityInterface* pkMasterEntity = pkEntity->GetMasterEntity();
    if (pkMasterEntity)
    {
        // Look up... Write ID
        unsigned int uiRefLinkID;
        NIVERIFY(NIBOOL_IS_TRUE(m_kLinkMaps.GetID(pkMasterEntity, 
            uiRefLinkID)));
        m_kDOM.AssignAttribute("MasterLinkID", uiRefLinkID);
    }

    unsigned int uiComponentCount = pkEntity->GetComponentCount();

    for (unsigned int uiComp = 0; uiComp < uiComponentCount; uiComp++)
    {
        NiEntityComponentInterface* pkComp = pkEntity->GetComponentAt(uiComp);

        if (pkComp == NULL)
        {
            m_kDOM.BeginSection("COMPONENT");
            m_kDOM.AssignAttribute("RefLinkID", "NULL");
            m_kDOM.EndSection();
            continue;
        }

        unsigned int uiCompID;
        NIVERIFY(m_kLinkMaps.GetID(pkComp, uiCompID));

        m_kDOM.BeginSection("COMPONENT");
        m_kDOM.AssignAttribute("RefLinkID", uiCompID);
        m_kDOM.EndSection();
    }

    // Write properties
    NiTObjectSet<NiFixedString> kPropertyNames;
    pkEntity->GetPropertyNames(kPropertyNames);

    unsigned int uiNameCount = kPropertyNames.GetSize();
    for (unsigned int uiProp = 0; uiProp < uiNameCount; uiProp++)
    {
        const NiFixedString& kPropertyName = kPropertyNames.GetAt(uiProp);

        bool bIsCompProp = true;
        if (!pkEntity->IsComponentProperty(kPropertyName, bIsCompProp) ||
            bIsCompProp)
        {
            continue;
        }

        bool bSerializable = false;
        if (!pkEntity->IsPropertySerializable(kPropertyName, bSerializable) ||
            !bSerializable)
        {
            continue;
        }

        WritePropertyToDOM(pkEntity, kPropertyName);
    }

    m_kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteComponentToDOM(NiEntityComponentInterface* 
    pkComp)
{
    unsigned int uiID;
    NIVERIFY(NIBOOL_IS_TRUE(m_kLinkMaps.GetID(pkComp, uiID)));

    m_kDOM.BeginSection("COMPONENT");
    m_kDOM.AssignAttribute("LinkID", uiID);
    m_kDOM.AssignAttribute("Class", pkComp->GetClassName());
    m_kDOM.AssignAttribute("Name", pkComp->GetName());

    WriteTemplateIDToDOM(pkComp);

    NiEntityComponentInterface* pkMasterComp = pkComp->GetMasterComponent();
    if (pkMasterComp)
    {
        // Look up... Write ID
        unsigned int uiRefLinkID;
        NIVERIFY(NIBOOL_IS_TRUE(m_kLinkMaps.GetID(pkMasterComp, uiRefLinkID)));
        m_kDOM.AssignAttribute("MasterLinkID", uiRefLinkID);
    }

    // Write properties
    NiTObjectSet<NiFixedString> kPropertyNames;
    pkComp->GetPropertyNames(kPropertyNames);

    unsigned int uiNameCount = kPropertyNames.GetSize();
    for (unsigned int uiProp = 0; uiProp < uiNameCount; uiProp++)
    {
        const NiFixedString kPropertyName = kPropertyNames.GetAt(uiProp);

        bool bSerializable = false;
        if (!pkComp->IsPropertySerializable(kPropertyName, bSerializable) ||
            !bSerializable)
        {
            continue;
        }

        WritePropertyToDOM(pkComp, kPropertyName);
    }

    m_kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteVersionToDOM()
{
    m_kDOM.AssignAttribute("Major", VERSION_MAJOR);
    m_kDOM.AssignAttribute("Minor", VERSION_MINOR);
    m_kDOM.AssignAttribute("Patch", VERSION_PATCH);
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteSelectionSetsToDOM(NiScene* pkScene)
{
    unsigned uiSelectionSetCount = pkScene->GetSelectionSetCount();

    if (uiSelectionSetCount == 0)
        return;

    m_kDOM.BeginSection("SELECTIONS");

    for(unsigned int uiSet=0; uiSet < uiSelectionSetCount; uiSet++)
    {
        NiEntitySelectionSet* pkSet = pkScene->GetSelectionSetAt(uiSet);
        
        unsigned int uiEntityCount = pkSet->GetEntityCount();
        NIASSERT(uiEntityCount);

        m_kDOM.BeginSection("SET");

        const char* pcName = pkSet->GetName();
        m_kDOM.AssignAttribute("Name", pcName);

        for(unsigned int uiEntity=0; uiEntity < uiEntityCount; uiEntity++)
        {
            NiEntityInterface* pkEntity = pkSet->GetEntityAt(uiEntity);
            NIASSERT(pkEntity);
            WriteEntityRefToDOM(pkEntity);
        }
        m_kDOM.EndSection();
    }

    m_kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WritePropertyArrayToDOM(
    NiEntityPropertyInterface* pkPropIntf, const NiFixedString& kPropertyName,
    NiFixedString& kPrimitiveType, unsigned int uiCount)
{
    if (uiCount == 0)
    {
        m_kDOM.BeginSection("ITEM");
        m_kDOM.EndSection();
        //WritePropertyToDOM(pkPropIntf, kPropertyName, kPrimitiveType, 0);
        return;
    }
    for(unsigned int ui=0; ui<uiCount; ui++)
    {
        m_kDOM.BeginSection("ITEM");

        WritePropertyToDOM(pkPropIntf, kPropertyName, kPrimitiveType, ui);
        m_kDOM.EndSection();
    }
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WritePropertyToDOM(NiEntityPropertyInterface*
    pkPropIntf, const NiFixedString& kPropertyName, 
    NiFixedString& kPrimitiveType, unsigned int uiIndex)
{
    if (kPrimitiveType == PT_ENTITYPOINTER)
        WriteEntityPointerToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_NIOBJECTPOINTER)
        WriteObjectPointerToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_FLOAT)
        WriteFloatToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_BOOL)
        WriteBoolToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_INT)
        WriteIntToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_UINT)
        WriteUIntToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_SHORT)
        WriteShortToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_USHORT)
        WriteUShortToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_STRING)
        WriteStringToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_POINT2)
        WritePoint2ToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_POINT3)
        WritePoint3ToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_QUATERNION)
        WriteQuaternionToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_MATRIX3)
        WriteMatrix3ToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_COLOR)
        WriteColorToDOM(pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_COLORA)
        WriteColorAToDOM(pkPropIntf, kPropertyName, uiIndex);
    else
        WriteUnknownTypeToDOM(pkPropIntf, kPropertyName, uiIndex);
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WritePropertyToDOM(NiEntityPropertyInterface*
    pkPropIntf, const NiFixedString& kPropertyName)
{
    m_kDOM.BeginSection("PROPERTY");

    WritePropertyInfoToDOM(pkPropIntf, kPropertyName);

    NiFixedString kPrimitiveType;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPrimitiveType(kPropertyName, 
        kPrimitiveType)));

    bool bIsCollection;

    if (pkPropIntf->IsCollection(kPropertyName, bIsCollection) 
        && bIsCollection)
    {
        unsigned int uiCount;
        pkPropIntf->GetElementCount(kPropertyName, uiCount);

         WritePropertyArrayToDOM(pkPropIntf, kPropertyName, kPrimitiveType, 
            uiCount);
    }
    else
    {
        WritePropertyToDOM(pkPropIntf, kPropertyName, kPrimitiveType, 0);
    }

    m_kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WritePropertyInfoToDOM(
    NiEntityPropertyInterface* pkPropIntf, const NiFixedString& kPropertyName)
{
    NiFixedString kDisplayName;
    NiFixedString kPrimitiveType;
    NiFixedString kSemanticType;
    NiFixedString kDescription;

    if (pkPropIntf->GetPrimitiveType(kPropertyName, kPrimitiveType) == false)
    {
        NIASSERT(!"Warning: No Primitive Type!");
        return;
    }


    m_kDOM.AssignAttribute("Class",kPrimitiveType);
    m_kDOM.AssignAttribute("Name",kPropertyName);

    if (pkPropIntf->IsAddPropertySupported())
    {
        if (pkPropIntf->GetSemanticType(kPropertyName, kSemanticType) == false)
        {
            NIASSERT(!"Warning: No Semantic Type!");
            return;
        }

        if (pkPropIntf->GetDisplayName(kPropertyName, kDisplayName) == false)
        {
            NIASSERT(!"Warning: No Display Name!");
            return;
        }

        if (pkPropIntf->GetDescription(kPropertyName, kDescription) == false)
        {
            NIASSERT(!"Warning: No Description!");
            return;
        }
        m_kDOM.AssignAttribute("SemanticType",kSemanticType);
        m_kDOM.AssignAttribute("DisplayName",kDisplayName);
        m_kDOM.AssignAttribute("Description",kDescription);
    }
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadUnknownTypeFromDOM(
    NiEntityPropertyInterface* pkPropIntf, const char* pcName, 
    unsigned int uiIndex)
{
    size_t stDataSizeInBytes = 0;
    unsigned char* pucData = NULL;
    NiBool bSuccess = m_kDOM.ReadPrimitive(pucData, stDataSizeInBytes);

    if (!bSuccess || pucData == NULL || stDataSizeInBytes == 0)
        return false;

    bSuccess = pkPropIntf->SetPropertyData(pcName, (void*)pucData, 
        stDataSizeInBytes, uiIndex);
    NiFree(pucData);
    return bSuccess;
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::_SDMInit()
{
    STREAMING_EXTENSION = "GSA";
    STREAMING_DESCRIPTION = "Gamebryo ASCII Scene files (*.gsa)|*.gsa";
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::_SDMShutdown()
{
    STREAMING_EXTENSION = NULL;
    STREAMING_DESCRIPTION = NULL;
}
//---------------------------------------------------------------------------
