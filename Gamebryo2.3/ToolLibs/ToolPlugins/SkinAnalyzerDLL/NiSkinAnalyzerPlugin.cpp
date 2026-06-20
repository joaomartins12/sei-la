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

#include "stdafx.h"
#include "resource.h"
#include "NiSkinAnalyzerPlugin.h"

NiImplementRTTI(NiSkinAnalyzerPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiSkinAnalyzerPlugin::NiSkinAnalyzerPlugin() :
    NiPlugin("Skin Analyzer", "1.0", "Displays skin partitioning statistics",
    "Searches the scene graph for skinned objects, printing skin partition"
    " information.")
{
}
//---------------------------------------------------------------------------
bool NiSkinAnalyzerPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiSkinAnalyzerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiSkinAnalyzerPlugin::FindSkinnedGeometry(NiAVObject* pkRoot, 
    NiTList<NiTriBasedGeom*>& kSkins)
{
    if(NiIsKindOf(NiNode, pkRoot))
    {
        NiNode* pkNode = (NiNode*) pkRoot;

        for(unsigned int ui=0; ui < pkNode->GetArrayCount(); ui++)
        {
            if(pkNode->GetAt(ui))
            {
                FindSkinnedGeometry(pkNode->GetAt(ui), kSkins);
            }
        }
    }
    else if (NiIsKindOf(NiTriBasedGeom, pkRoot))
    {
        NiTriBasedGeom* pkGeom = (NiTriBasedGeom*)pkRoot;

        if (pkGeom->GetSkinInstance())
            kSkins.AddTail(pkGeom);
    }
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiSkinAnalyzerPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiTList<NiTriBasedGeom*> kSkins;
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);

        FindSkinnedGeometry(spScene, kSkins);
    }

    NiTListIterator kIter = kSkins.GetHeadPos();

    m_strMessage.Empty();

    while(kIter)
    {
        NiTriBasedGeom* pkGeom = kSkins.GetNext(kIter);
        NiSkinInstance* pkSkinInst = pkGeom->GetSkinInstance();
        NiSkinData* pkSkinData = pkSkinInst->GetSkinData();
        NiSkinPartitionPtr spSkinPart = pkSkinInst->GetSkinPartition();

        const unsigned int uiGeomVerts = pkGeom->GetVertexCount();
        const unsigned int uiGeomTris = pkGeom->GetTriangleCount();

        if (spSkinPart)
        {
            // analyze the skin data
            const unsigned int uiPartitionCount 
                = spSkinPart->GetPartitionCount();

            unsigned int* puiIndexArray = NiAlloc(unsigned int, 
                uiPartitionCount);

            // prepare to sort the partitions by vertex count
            unsigned int i;
            for (i = 0; i < uiPartitionCount; i++)
                puiIndexArray[i] = i;

            NiSkinPartition::Partition* pkPartitions 
                = spSkinPart->GetPartitions();

            // sort the partitions
            unsigned int j;
            for (i = (uiPartitionCount - 1); i > 0; i--)
            {
                for (j = 0; j < i; j++)
                {
                    if (pkPartitions[puiIndexArray[j+1]].m_usVertices > 
                        pkPartitions[puiIndexArray[j]].m_usVertices)
                    {
                        unsigned int uiTemp = puiIndexArray[j+1];
                        puiIndexArray[j+1] = puiIndexArray[j];
                        puiIndexArray[j] = uiTemp;
                    }
                }
            }

            NiString strSubMsg;
            strSubMsg.Format(
                "Skin Object: %s (%u Bones,\t %u Partitions)\r\r\n", 
                pkGeom->GetName(), pkSkinData->GetBoneCount(), 
                uiPartitionCount);
            m_strMessage += strSubMsg;

            for (i = 0; i < uiPartitionCount; i++)
            {
                NiSkinPartition::Partition& kPart 
                    = pkPartitions[puiIndexArray[i]];
        
                unsigned short usVerts = kPart.m_usVertices;
                unsigned short usTris = kPart.m_usTriangles;
                unsigned short usBones = kPart.m_usBones;

                NiString strSubMsg;
                strSubMsg.Format(
                    "\tPartition[%u]:\t  %u Bones,\t %u Verts (%f%%),"
                    "\t%u Tris (%f%%)\r\r\n", 
                    puiIndexArray[i], (unsigned int)usBones, 
                    (unsigned int)usVerts, 
                    100.0f * ((float)usVerts)/uiGeomVerts, 
                    (unsigned int)usTris, 
                    100.0f * ((float)usTris)/uiGeomTris);
                m_strMessage += strSubMsg;
            }

            NiFree(puiIndexArray);
        }
        else
        {
            NiString strSubMsg;
            strSubMsg.Format(
                "Skin Object: %s (%u Bones,\t No Partitions)\r\r\n"
                "\t%u Verts, %u Tris\n", 
                pkGeom->GetName(), pkSkinData->GetBoneCount(), uiGeomVerts, 
                uiGeomTris);
            m_strMessage += strSubMsg;
        }
    }

    NiString strSubMsg;
    strSubMsg.Format("\r\r\n\r\r\n");
    m_strMessage += strSubMsg;

    kIter = kSkins.GetHeadPos();

    while(kIter)
    {
        NiTriBasedGeom* pkGeom = kSkins.GetNext(kIter);
        NiSkinInstance* pkSkinInst = pkGeom->GetSkinInstance();
        NiSkinData* pkSkinData = pkSkinInst->GetSkinData();
        NiSkinPartitionPtr spSkinPart = pkSkinInst->GetSkinPartition();
        NiAVObject*const* ppkBones = pkSkinInst->GetBones();

        const unsigned int uiGeomVerts = pkGeom->GetVertexCount();
        const unsigned int uiGeomTris = pkGeom->GetTriangleCount();

        if (spSkinPart)
        {
            // analyze the skin data
            const unsigned int uiPartitionCount 
                = spSkinPart->GetPartitionCount();

            unsigned int* puiIndexArray = NiAlloc(unsigned int, 
                uiPartitionCount);

            // prepare to sort the partitions by vertex count
            unsigned int i;
            for (i = 0; i < uiPartitionCount; i++)
                puiIndexArray[i] = i;

            NiSkinPartition::Partition* pkPartitions 
                = spSkinPart->GetPartitions();

            // sort the partitions
            unsigned int j;
            for (i = (uiPartitionCount - 1); i > 0; i--)
            {
                for (j = 0; j < i; j++)
                {
                    if (pkPartitions[puiIndexArray[j+1]].m_usVertices > 
                        pkPartitions[puiIndexArray[j]].m_usVertices)
                    {
                        unsigned int uiTemp = puiIndexArray[j+1];
                        puiIndexArray[j+1] = puiIndexArray[j];
                        puiIndexArray[j] = uiTemp;
                    }
                }
            }

            NiString strSubMsg;
            strSubMsg.Format(
                "Skin Object: %s (%u Bones,\t %u Partitions)\r\r\n", 
                pkGeom->GetName(), pkSkinData->GetBoneCount(), 
                uiPartitionCount);
            m_strMessage += strSubMsg;

            for (i = 0; i < uiPartitionCount; i++)
            {
                NiSkinPartition::Partition& kPart 
                    = pkPartitions[puiIndexArray[i]];
        
                unsigned short usVerts = kPart.m_usVertices;
                unsigned short usTris = kPart.m_usTriangles;
                unsigned short usBonesInPartition = kPart.m_usBones;
                unsigned short usBonesPerVertex = kPart.m_usBonesPerVertex;

                float* pfBoneWeightSum = NiAlloc(float, usBonesInPartition);
                memset(pfBoneWeightSum, 0, sizeof(float) * usBonesInPartition);
                float* pfBoneWeightMax = NiAlloc(float, usBonesInPartition);
                memset(pfBoneWeightMax, 0, sizeof(float) * usBonesInPartition);

                float* pfWeights = kPart.m_pfWeights;

                if (kPart.m_pucBonePalette)
                {
                    unsigned char* pucBoneIndices = kPart.m_pucBonePalette;

                    unsigned int* puiBoneUses = 
                        NiAlloc(unsigned int, usBonesInPartition);
                    memset(puiBoneUses, 0,
                        sizeof(unsigned int) * usBonesInPartition);

                    unsigned int j;
                    for (j = 0; j < usVerts; j++)
                    {
                        unsigned int k;
                        for (k = 0; k < usBonesPerVertex; k++)
                        {
                            unsigned int uiBoneIndex = *pucBoneIndices++;
                            float fWeight = *pfWeights++;
                            
                            // unused bones are set to exactly index 0 and
                            // weight 0.0f
                            if (fWeight != 0.0f)
                            {
                                puiBoneUses[uiBoneIndex]++;
                                if (pfBoneWeightMax[uiBoneIndex] < fWeight)
                                    pfBoneWeightMax[uiBoneIndex] = fWeight;
                                pfBoneWeightSum[uiBoneIndex] += fWeight;
                            }
                        }
                    }

                    for (j = 0; j < usBonesInPartition; j++)
                    {
                        if (puiBoneUses[j])
                            pfBoneWeightSum[j] /= (float)(puiBoneUses[j]);
                    }

                    NiFree(puiBoneUses);

                    NiString strSubMsg;
                    strSubMsg.Format(
                        "\tPartition[%u]:\t  %u Bones (palette)\r\r\n", 
                        puiIndexArray[i], (unsigned int)usBonesInPartition);
                    m_strMessage += strSubMsg;
                }
                else
                {
                    unsigned int j;
                    for (j = 0; j < usVerts; j++)
                    {
                        unsigned int k;
                        for (k = 0; k < usBonesPerVertex; k++)
                        {
                            float fWeight = *pfWeights++;
                            
                            if (pfBoneWeightMax[k] < fWeight)
                                pfBoneWeightMax[k] = fWeight;
                            pfBoneWeightSum[k] += fWeight;
                        }
                    }

                    for (j = 0; j < usBonesInPartition; j++)
                    {
                        pfBoneWeightSum[j] /= (float)usVerts;
                    }

                    NiString strSubMsg;
                    strSubMsg.Format(
                        "\tPartition[%u]:\t  %u Bones (no palette)\r\r\n", 
                        puiIndexArray[i], (unsigned int)usBonesInPartition);
                    m_strMessage += strSubMsg;
                }

                unsigned int k;
                for (k = 0; k < usBonesInPartition; k++)
                {
                    const char* pcName 
                        = ppkBones[kPart.m_pusBones[k]]->GetName();
                    NiString strSubMsg;

                    if (pcName)
                    {
                        strSubMsg.Format(
                            "\t\tBone[%u]: %s,\t Mean Weight "
                            "%f,\t Max Weight %f\r\r\n",
                            k, pcName, pfBoneWeightSum[k], pfBoneWeightMax[k]);
                    }
                    else
                    {
                        strSubMsg.Format(
                            "\t\tBone[%u]: NO NAME,\t "
                            "Mean Weight %f,\t Max Weight %f\r\r\n", 
                            k, pfBoneWeightSum[k], pfBoneWeightMax[k]);
                    }

                    m_strMessage += strSubMsg;
                }

                NiFree(pfBoneWeightSum);
                NiFree(pfBoneWeightMax);
            }

            NiFree(puiIndexArray);
        }
        else
        {
            NiString strSubMsg;
            strSubMsg.Format(
                "Skin Object: %s (%u Bones,\t No Partitions)\r\r\n",
                pkGeom->GetName(), pkSkinData->GetBoneCount());
            m_strMessage += strSubMsg;

            unsigned int uiBones = pkSkinData->GetBoneCount();

            unsigned int i;
            for (i = 0; i < uiBones; i++)
            {
                const char* pcName 
                    = ppkBones[i]->GetName();
                NiString strSubMsg;

                if (pcName)
                {
                    strSubMsg.Format("\tBone[%u]: %s\r\r\n", i, pcName);
                }
                else
                {
                    strSubMsg.Format("\tBone[%u]: NO NAME\r\r\n", i);
                }

                m_strMessage += strSubMsg;
            }
        }
    }

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
        pkLogger->LogElement("SkinAnalyzer", m_strMessage);
    }
    else
    {
        NiInfoDialog kDlg("Skin Analyzer Results:");
        kDlg.SetText(m_strMessage);
        kDlg.DoModal();
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiSkinAnalyzerPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiSkinAnalyzerPlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiSkinAnalyzerPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiSkinAnalyzerPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    return false;
}
//---------------------------------------------------------------------------
