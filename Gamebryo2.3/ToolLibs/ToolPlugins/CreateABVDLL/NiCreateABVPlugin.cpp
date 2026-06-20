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

#include <StdAfx.h>
#include "NiCreateABVPlugin.h"
#include "NiCreateABVDialog.h"
#include "CreateABVResultsDialog.h"

NiImplementRTTI(NiCreateABVPlugin, NiPlugin);

//---------------------------------------------------------------------------
const char* FindSubString(const char* pcStr, const char* pcKey,
    bool bFindTermination = 0 )
{
    if (pcStr == NULL)
        return NULL;

    // looks for the first ocurrance of the given string key in the main 
    // string, returning a pointer to the start of that substring if found
    unsigned int uiLength = strlen(pcKey);

    while(pcStr && pcStr[0])
    {
        if(!strncmp(pcStr, pcKey, uiLength))
        {
            if (bFindTermination)
            {
                pcStr += uiLength;

                while (*pcStr == ' ')
                    pcStr++;
            }

            return pcStr;
        }
        pcStr++;
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool ParseWireframeABVStr(char* pcStr,
    char* pcType,
    size_t pcTypeMaxLen,
    float* pafColor,
    float& fMult,
    char* pcName,
    size_t uiNameLength)
{
    char acTag[256] ="";
    char acUpperTag[256] = "";
    pcName[0] = '\0';

    const char* pcEndStr = FindSubString(pcStr,"\n");
    unsigned int uiLen = strlen(pcStr);
    if (pcEndStr)
       uiLen -= strlen(pcEndStr);

    if (uiLen == 0 || uiLen >= 256)   
        return false;

    NiStrncpy(acTag, 256, pcStr, uiLen);
    NiStrncpy(acUpperTag, 256, pcStr, uiLen);
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _strupr_s(acUpperTag, 256);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    strupr( acUpperTag );
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    const char* pcMatch;
    pcMatch = FindSubString(acUpperTag,"NDL_WIREFRAME");
    pcMatch = FindSubString(pcMatch,"=",true);
    
    if (!pcMatch)
        return false;  

#if defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf_s(pcMatch, "%s %f %f %f %f", pcType, pcTypeMaxLen,
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf(pcMatch, "%s %f %f %f %f", pcType,
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
        &pafColor[0], &pafColor[1], &pafColor[2], &fMult);
    pcMatch = FindSubString(acTag,"=",true);
    pcMatch = pcMatch + strlen(pcType);

    const char* pcNamePtr = FindSubString(pcMatch,"\"",true);
    
    if (pcNamePtr == NULL)
        return false; // opening not found.

    const char* pcEndPtr = FindSubString(pcNamePtr,"\""); // ending
    
    if (pcEndPtr == NULL)
        return false; // closing not found

    size_t stLen = 0;
    
    if (pcNamePtr)
        stLen = strlen(pcNamePtr);
    if (pcEndPtr)
        stLen -= strlen(pcEndPtr);
    NiStrncpy(pcName, uiNameLength, pcNamePtr, stLen);

    return true;
}
//---------------------------------------------------------------------------
void CreateSelectionBox(NiNode* pkNode, NiColor kColor, float fMult,
    char *pcSelectName)
{
    NiNode* pkProxy = NiOptimizeABV::GetNodeByFirstToken(pkNode, "NDLCD");

    NIASSERT(pkProxy);

    if (pkProxy == NULL)
        return;

    NiOptimizeABV::CreateWireABVProxy(pkProxy, kColor, fMult, pcSelectName);

    NiNode* pkNodeA = pkProxy->GetParent();
    NiNode* pkNode3 = (NiNode*)pkNodeA->GetObjectByName(pcSelectName);
    pkNode3->SetAppCulled(true);
    pkNode3->SetSelectiveUpdate(false);
}
//---------------------------------------------------------------------------
void ExecuteWireframeABVInstruction(NiNodePtr spNode, char* pcType,
    float*  pafColor, float fMult, char* pcName, bool bABV, bool bABVProxy, 
    bool bSelection)
{
    if (bABV && strcmp(pcType,"ABV")==0)
    {
        NiOptimizeABV::CreateWireABV(spNode,
            NiColor(pafColor[0], pafColor[1], pafColor[2] ),
            fMult, pcName, false);
    }
    else if (bABVProxy && strcmp(pcType,"ABVPROXY")==0)
    {
       NiOptimizeABV::CreateWireABVProxy(spNode,
           NiColor(pafColor[0], pafColor[1], pafColor[2]),
           fMult, pcName);
    }
    else if (bSelection && strcmp(pcType,"ABVSELECTION")==0)
    {
        unsigned int uiChildCnt = spNode->GetChildCount();

        unsigned int uiChild;
        for(uiChild=0;uiChild < uiChildCnt; uiChild++)
        {
            NiNode* pkSel = (NiNode*)spNode->GetAt(uiChild);
            CreateSelectionBox(pkSel,
                NiColor(pafColor[0],pafColor[1],pafColor[2]), fMult,
                (char*)pcName);
        }
    }
    else
    {
        NIASSERT(0);
    }
}
//---------------------------------------------------------------------------
void CreateUserDefinedWireFrames(NiNodePtr spNode, bool bABV, bool bABVProxy,
    bool bSelection)
{
    unsigned int uiCnt = 0;
    const char* pcStr = NULL;
            
    // user define?
    NiObjectNET* pkObj = NiSmartPointerCast(NiObjectNET,spNode);

    for (uiCnt=0; uiCnt < pkObj->GetExtraDataSize(); uiCnt++)
    {
        NiExtraData* pkExtra = pkObj->GetExtraDataAt(uiCnt);

        if(NiIsKindOf(NiStringExtraData,pkExtra))
        {
            const char* pcExtra = ((NiStringExtraData*)pkExtra)->GetValue();
            char* pcNewExtra = NULL;
            if (!pcExtra)
                continue;

            unsigned int uiNewExtraLength = strlen(pcExtra) + 1;
            pcNewExtra = NiAlloc(char, uiNewExtraLength);
            NiStrcpy(pcNewExtra, uiNewExtraLength, pcExtra);

            char* pcCurr = pcNewExtra;

            const char* pcNext = NULL;
            
            while(pcCurr)
            {
                char* pcNext = (char*)FindSubString(pcCurr,"\n");
                char acType[80] = "";
                char acName[256] = "";
                float afColor[3];
                float fMult = 0.0f;

                if (ParseWireframeABVStr(pcCurr, 
                    acType,
                    sizeof(acType),
                    afColor,
                    fMult,
                    acName,
                    sizeof(acName)))
                {
                    ExecuteWireframeABVInstruction(spNode, acType, afColor,
                        fMult, acName, bABV, bABVProxy, bSelection);

                    if (pcNext)
                    {
                        // delete the portion of the string data.
                        unsigned int uiLenNext = strlen(pcNext);
                        NiStrcpy(pcCurr, uiLenNext + 1, pcNext + 1);
                        pcNext = pcCurr + uiLenNext;
                        pcNext = pcCurr;
                    }
                    else
                    {
                        pcCurr[0] = '\0';
                    }
                                   
                    pcCurr = pcNext;
                }
                else
                {
                    pcCurr = pcNext;
                    if (pcCurr)
                        pcCurr++;
                }
            }

            if (strcmp(pcExtra, pcNewExtra) != 0)
            {
                // some string data can be removed.
                pkObj->RemoveExtraDataAt(uiCnt);

                if (pcNewExtra && (strcmp(pcNewExtra,"") != 0) &&
                    strcmp(pcNewExtra,"\r\n") != 0)
                    pkObj->AddExtraData(NiNew NiStringExtraData(pcNewExtra));

                // We effect the count, so adjust.
                if (uiCnt)
                    uiCnt--;
            }
            NiFree(pcNewExtra);
        }
    }

    unsigned int uiNumChildren = spNode->GetChildCount();
    for (uiCnt = 0; uiCnt < uiNumChildren; uiCnt++)
    {
        NiAVObject* pkObj = spNode->GetAt(uiCnt);

        if (NiIsKindOf(NiNode, pkObj))
            CreateUserDefinedWireFrames((NiNode*)pkObj, bABV, bABVProxy,
            bSelection);
    }
}
//---------------------------------------------------------------------------
NiCreateABVPlugin::NiCreateABVPlugin() :
    NiPlugin("Create ABV", "1.0", 
    "Turn geometry into an alternate bounding volume.",
    "Turn geometry into an alternate bounding volume.")
{
}
//---------------------------------------------------------------------------
bool NiCreateABVPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiCreateABVPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiCreateABVPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);


    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        spScene->Update(0.0f);

        NIASSERT(spScene);
        if (spScene)
        {
            NiTPointerList<char*> pkCharErrors;

            if (pkInfo->GetValue("CreateABV") == "TRUE")
                NiOptimizeABV::CreateABVFromProxy(spScene, &pkCharErrors);

            bool bABV = (pkInfo->GetValue("CreateWireframeABV") == "TRUE");
            bool bABVProxy = (pkInfo->GetValue("CreateWireframeABVProxy")
                == "TRUE");
            bool bSelected = (pkInfo->GetValue("CreateWireframeSelection")
                == "TRUE");

            if (bABV || bABVProxy || bSelected)
                CreateUserDefinedWireFrames(spScene,bABV,bABVProxy,bSelected);

            if (pkInfo->GetValue("RemoveProxyGeometry") == "TRUE")
			{
				NiOptimizeABV::RemoveCollisionDataProxyGeometry(spScene, &pkCharErrors);
			}

			// Convert Char Errors to NiString Errors.
			while (!pkCharErrors.IsEmpty())
			{
				NiString* pkString = NiNew NiString(pkCharErrors.GetHead());
				m_pkErrors.AddTail(pkString);

				NiFree(pkCharErrors.RemoveHead());
			}

            // Show any errors which have occurred.
            ShowAllErrors();

        }
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiCreateABVPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiCreateABVPlugin");
    pkPluginInfo->SetType("PROCESS");
        
    pkPluginInfo->SetBool("CreateABV", true);
    pkPluginInfo->SetBool("CreateWireframeABVProxy", false);
    pkPluginInfo->SetBool("CreateWireframeABV", false);
    pkPluginInfo->SetBool("CreateWireframeSelection", false);
    pkPluginInfo->SetBool("RemoveProxyGeometry", true);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiCreateABVPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiCreateABVPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent)
{
    NiCreateABVDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        break;
    case IDCANCEL:
        return false;
        break;
    default:
        NIASSERT(false);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiCreateABVPlugin::ClearAllErrors()
{
    // remove all errors deleting the strings
    while (!m_pkErrors.IsEmpty())
    {
        NiDelete m_pkErrors.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void NiCreateABVPlugin::ClearAllSectionErrors()
{
    // remove all errors deleting the strings
    while (!m_pkSectionErrors.IsEmpty())
    {
        NiDelete m_pkSectionErrors.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void NiCreateABVPlugin::AppendAllSectionErrors()
{
    // remove all errors deleting the strings
    while (!m_pkSectionErrors.IsEmpty())
    {
        m_pkErrors.AddTail( m_pkSectionErrors.RemoveHead() );;
    }
}
//---------------------------------------------------------------------------
void NiCreateABVPlugin::ShowAllErrors()
{
    if (m_pkErrors.IsEmpty())
        return;

    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog;
    NiXMLLogger* pkLogger;

    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    if ((bWriteResultsToLog) && (pkLogger))
    {
        // Convert to a single string
        NiString kErrorText;
        NiTListIterator kIter = m_pkErrors.GetHeadPos();

        while (kIter != NULL)
        {
            // Append into the Text String
            kErrorText += *m_pkErrors.GetNext(kIter);
            kErrorText += "\r\n";
        }

        pkLogger->LogElement("CreateAVBPlugin", kErrorText);
    }
    else
    {
        CreateABVResultsDialog kDialog(m_hModule, NULL, &m_pkErrors);

        kDialog.DoModal();
    }

    ClearAllErrors();
    ClearAllSectionErrors();
}
//---------------------------------------------------------------------------
