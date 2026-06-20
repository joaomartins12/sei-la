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
#include "NiD3DRendererPCH.h"

#include "NiDX9IBManager.h"
#include "NiDX9Renderer.h"

#include <NiGeometryData.h>

//---------------------------------------------------------------------------
NiDX9IndexBufferManager::NiDX9IndexBufferManager(
    LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    m_pkD3DDevice9 = pkD3DDevice9;
    D3D_POINTER_REFERENCE(m_pkD3DDevice9);

    m_pkTempPointConnIB = 0;
    m_uiTempPointConnLen = 0;
    m_pkTempLineConnIB = 0;
    m_uiTempLineConnLen = 0;

    m_kDefaultResources.RemoveAll();
    m_kManagedResources.RemoveAll();
    m_kSystemMemResources.RemoveAll();
}
//---------------------------------------------------------------------------
NiDX9IndexBufferManager::~NiDX9IndexBufferManager()
{
    //  Cycle through all lists, and free the resources
    NiTMapIterator kPos;

    NiTPointerMap<unsigned int, NiDX9IBInfo*>* pkResourceMap;
    
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        switch (ui)
        {
        case 0:
            pkResourceMap = &m_kDefaultResources;
            break;
        case 1:
            pkResourceMap = &m_kManagedResources;
            break;
        case 2:
            pkResourceMap = &m_kSystemMemResources;
            break;
        }
        kPos = pkResourceMap->GetFirstPos();
        while (kPos)
        {
            unsigned int uiKey;
            NiDX9IBInfo* pkIBInfo;
            pkResourceMap->GetNext(kPos, uiKey, pkIBInfo);

            //  Kill the vertex buffers...
            if (pkIBInfo)
            {
                NiDX9IBResource* pkIBRes;
            
                for (unsigned int uj = 0; uj < DX9_MAX_IBS; uj++)
                {
                    pkIBRes = pkIBInfo->m_apkIBRes[uj];
                    if (pkIBRes)
                    {
                        if (pkIBRes->m_pkIB)
                        {
                            NiD3DRenderer::ReleaseIBResource(pkIBRes->m_pkIB);
                        }
                    }
                    NiDelete pkIBRes;
                }
                NiDelete pkIBInfo;
            }
        }
    }

    m_kDefaultResources.RemoveAll();
    m_kManagedResources.RemoveAll();
    m_kSystemMemResources.RemoveAll();
    
    m_pkD3DDevice9->SetIndices(0);

    if (m_pkTempPointConnIB != 0)
    {
        NiD3DRenderer::ReleaseIBResource(m_pkTempPointConnIB);
    }

    if (m_pkTempLineConnIB != 0)
    {
        NiD3DRenderer::ReleaseIBResource(m_pkTempLineConnIB);
    }

    if (m_pkD3DDevice9)
    {
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice9);
        m_pkD3DDevice9 = 0;
    }
}
//---------------------------------------------------------------------------
LPDIRECT3DINDEXBUFFER9 NiDX9IndexBufferManager::GetIndexBuffer(
    unsigned int uiLength, unsigned int uiUsage, D3DFORMAT eFormat, 
    D3DPOOL ePool)
{
    if (m_pkD3DDevice9 == 0)
        return NULL;

    NiDX9IBInfo* pkIBInfo = NULL;
    LPDIRECT3DINDEXBUFFER9 pkD3DIB9 = NULL;
    NiDX9IBResource* pkIBRes = NULL;

    pkIBInfo = GetIBInfo(uiUsage, eFormat, ePool);
    if (pkIBInfo)
    {
        //  This 'type' already exists!
        //  Grab the next free one and see if it's large enough
        pkIBRes = (pkIBInfo->m_apkIBRes[pkIBInfo->m_ucIndex]);
        if (pkIBRes)
        {
            //  Check the length
            if (pkIBRes->m_uiLength <= uiLength)
            {
                //  It's large enough, pass it back
                pkD3DIB9 = pkIBRes->m_pkIB;
                pkIBInfo->m_ucIndex++;
            }
            else
            {
                //  It's too small. Free it and bump it up
                pkIBRes->m_pkIB->Release();
                pkIBRes->m_pkIB = NULL;
            }
        }
        else
        {
            //  Empty resource slot, fill it in...
            pkIBRes = NiNew NiDX9IBResource;
            NIASSERT(pkIBRes);
            pkIBRes->m_eType = NiDX9Resource::TYPE_INDEXBUFFER;
            pkIBRes->m_ePool = ePool;
            pkIBRes->m_uiUsage = uiUsage;
            pkIBRes->m_uiLength = uiLength;
            pkIBRes->m_eFormat = eFormat;
            pkIBRes->m_uiRevID = 0;
            pkIBRes->m_pkIB = NULL;
        }

        if (pkIBRes->m_pkIB == NULL)
        {
            pkIBRes->m_pkIB = CreateIndexBuffer(uiLength, uiUsage, eFormat, 
                ePool, &(pkIBRes->m_kIBDesc));
            NIASSERT(pkIBRes->m_pkIB);

            pkIBRes->m_uiLength = uiLength;
            pkIBRes->m_eFormat = eFormat;
            pkIBRes->m_uiRevID = 0;
            pkD3DIB9 = pkIBRes->m_pkIB;
            pkIBInfo->m_ucIndex++;
        }

        //  Check for wrap-around
        if (pkIBInfo->m_ucIndex == DX9_MAX_IBS)
            pkIBInfo->m_ucIndex = 0;
    }
    else
    {
        //  This should NEVER happen!
        NIASSERT(false);
    }

    return pkD3DIB9;
}
//---------------------------------------------------------------------------
LPDIRECT3DINDEXBUFFER9 NiDX9IndexBufferManager::PackBuffer(
    const unsigned short* pusTriList, unsigned int uiLength, 
    unsigned int uiMaxLength, LPDIRECT3DINDEXBUFFER9 pkOldIB, 
    unsigned int& uiIBSize, D3DPOOL ePool, unsigned int uiUsage)
{
    if (m_pkD3DDevice9 == 0)
        return NULL;

    if (pusTriList == NULL)
        return NULL;

    LPDIRECT3DINDEXBUFFER9 pkD3DIB9 = NULL;
    HRESULT eD3dRet;

    //  Calculate the length
    unsigned int uiCreateSize = uiMaxLength * sizeof(unsigned short);
    unsigned int uiCopySize = uiLength * sizeof(unsigned short);
    NIASSERT(uiCreateSize >= uiCopySize);

    //  If there's an old one, see if it's compatible
    if (pkOldIB)
    {
        D3DINDEXBUFFER_DESC kIBDesc;

        memset((void*)&kIBDesc, 0, sizeof(kIBDesc));

        eD3dRet = pkOldIB->GetDesc(&kIBDesc);
        if (SUCCEEDED(eD3dRet))
        {
            if (   (kIBDesc.Format == D3DFMT_INDEX16) 
                && (kIBDesc.Type == D3DRTYPE_INDEXBUFFER) 
                && (kIBDesc.Usage == uiUsage) 
                && (kIBDesc.Pool == ePool) 
                && (kIBDesc.Size >= uiCreateSize)
               )
            {
                pkD3DIB9 = pkOldIB;
            }
        }
    }

    if (!pkD3DIB9)
    {
        pkD3DIB9 = CreateIndexBuffer(uiCreateSize, uiUsage, D3DFMT_INDEX16, 
            ePool);
        if (!pkD3DIB9)
        {
            NiDX9Renderer::Error("NiDX9IndexBufferManager::PackBuffer> "
                "Failed to create index buffer\n");
            return NULL;
        }
        uiIBSize = uiCreateSize;
    }

    if (pkD3DIB9 && uiCopySize > 0)
    {
        //  Lock it, pack it, unlock, return...
        void* pvData = NULL;

        eD3dRet = pkD3DIB9->Lock(0, uiCopySize, &pvData, 0);
        if (SUCCEEDED(eD3dRet))
        {
            NIASSERT(pvData);

            NiMemcpy(pvData, (const void*)pusTriList, uiCopySize);
            pkD3DIB9->Unlock();
        }
        else
        {
            NiDX9Renderer::Error("NiDX9IndexBufferManager::PackBuffer> "
                "Failed to lock index buffer\n");
            pkD3DIB9->Unlock();
            NiD3DRenderer::ReleaseIBResource(pkD3DIB9);
        }
    }

    return pkD3DIB9;
}
//---------------------------------------------------------------------------
LPDIRECT3DINDEXBUFFER9 NiDX9IndexBufferManager::ResizeTempPointConnectivity(
    unsigned int uiPoints, bool bRefill, unsigned int uiUsage, D3DPOOL ePool)
{
    if (m_pkD3DDevice9 == 0)
        return NULL;

    if (uiPoints == 0)
        return NULL;

    LPDIRECT3DINDEXBUFFER9 pkD3DIB9 = NULL;
    LPDIRECT3DINDEXBUFFER9 pkOldIB = NULL;
    HRESULT eD3dRet;

    //  Calculate the length
    unsigned int uiSize = uiPoints * sizeof(unsigned short) * 6;

    pkOldIB = m_pkTempPointConnIB;
    //  If there's an old one, see if it's compatible
    if (pkOldIB)
    {
        D3DINDEXBUFFER_DESC kIBDesc;

        memset((void*)&kIBDesc, 0, sizeof(kIBDesc));

        eD3dRet = pkOldIB->GetDesc(&kIBDesc);
        if (SUCCEEDED(eD3dRet))
        {
            if (   (kIBDesc.Format == D3DFMT_INDEX16) 
                && (kIBDesc.Type == D3DRTYPE_INDEXBUFFER) 
                && (kIBDesc.Usage == uiUsage) 
                && (kIBDesc.Pool == ePool) 
                && (kIBDesc.Size >= uiSize)
               )
            {
                pkD3DIB9 = pkOldIB;
                //  No need to repack...
                if (!bRefill)
                    return pkD3DIB9;
            }
            else
            {
                //  Assume it's no good
                NiD3DRenderer::ReleaseIBResource(pkOldIB);
            }
        }
    }

    if (!pkD3DIB9)
    {
        pkD3DIB9 = CreateIndexBuffer(uiSize, uiUsage, 
            D3DFMT_INDEX16, ePool);
        if (!pkD3DIB9)
        {
            NiDX9Renderer::Error("NiDX9IndexBufferManager::"
                "ResizeTempPointConnectivity> Failed to create index "
                "buffer\n");
            return NULL;
        }
        //  Force a refill
        bRefill = true;
    }

    if (pkD3DIB9 && bRefill)
    {
        //  Lock it, pack it, unlock, return...
        void* pvData = NULL;

        eD3dRet = pkD3DIB9->Lock(0, uiSize, &pvData, 0);
        if (SUCCEEDED(eD3dRet))
        {
            unsigned short* pusTemp = (unsigned short*)pvData;
            unsigned int uiBase = 0;
            unsigned int ui;
            for (ui = 0; ui < uiPoints; ui++)
            {
                *(pusTemp++) = uiBase + 0;
                *(pusTemp++) = uiBase + 1;
                *(pusTemp++) = uiBase + 2;
                *(pusTemp++) = uiBase + 0;
                *(pusTemp++) = uiBase + 2;
                *(pusTemp++) = uiBase + 3;

                uiBase += 4;
            }
            pkD3DIB9->Unlock();
        }
        else
        {
            NiDX9Renderer::Error("NiDX9IndexBufferManager::PackBuffer> "
                "Failed to lock index buffer\n");
            pkD3DIB9->Unlock();
            NiD3DRenderer::ReleaseIBResource(pkD3DIB9);
            pkD3DIB9 = 0;
        }
    }

    m_pkTempPointConnIB = pkD3DIB9;
    m_uiTempPointConnLen = uiSize;
    return pkD3DIB9;
}
//---------------------------------------------------------------------------
LPDIRECT3DINDEXBUFFER9 NiDX9IndexBufferManager::ResizeTempLineConnectivity(
    unsigned int uiPoints, NiBool* pbFlags, unsigned int& uiConn, 
    unsigned int uiUsage, D3DPOOL ePool)
{
    if (m_pkD3DDevice9 == 0)
        return NULL;

    if (uiPoints == 0)
        return NULL;
    if (pbFlags == 0)
        return NULL;

    LPDIRECT3DINDEXBUFFER9 pkD3DIB9 = NULL;
    LPDIRECT3DINDEXBUFFER9 pkOldIB = NULL;
    HRESULT eD3dRet;

    //  Calculate the length
    unsigned int uiSize = (uiPoints * 2 + 2) * sizeof(unsigned short);

    pkOldIB = m_pkTempLineConnIB;
    //  If there's an old one, see if it's compatible
    if (pkOldIB)
    {
        D3DINDEXBUFFER_DESC kIBDesc;

        memset((void*)&kIBDesc, 0, sizeof(kIBDesc));

        eD3dRet = pkOldIB->GetDesc(&kIBDesc);
        if (SUCCEEDED(eD3dRet))
        {
            if (   (kIBDesc.Format == D3DFMT_INDEX16) 
                && (kIBDesc.Type == D3DRTYPE_INDEXBUFFER) 
                && (kIBDesc.Usage == uiUsage) 
                && (kIBDesc.Pool == ePool) 
                && (kIBDesc.Size >= uiSize)
               )
            {
                pkD3DIB9 = pkOldIB;
            }
            else
            {
                //  Assume it's no good
                NiD3DRenderer::ReleaseIBResource(pkOldIB);
            }
        }
    }

    if (!pkD3DIB9)
    {
        pkD3DIB9 = CreateIndexBuffer(uiSize, uiUsage, 
            D3DFMT_INDEX16, ePool);
        if (!pkD3DIB9)
        {
            NiDX9Renderer::Error("NiDX9IndexBufferManager::"
                "ResizeTempLineConnectivity> Failed to create index "
                "buffer\n");
            return NULL;
        }
    }

    if (pkD3DIB9)
    {
        //  Lock it, pack it, unlock, return...
        void* pvData = NULL;
        uiConn = 0;

        eD3dRet = pkD3DIB9->Lock(0, uiSize, &pvData, 0);
        if (SUCCEEDED(eD3dRet))
        {
            unsigned short* pusConnTmp = (unsigned short*)pvData;
            unsigned int i;
            unsigned int uiMax = uiPoints - 1;
            for (i = 0; i < uiMax; i++)
            {
                if (*pbFlags)
                {
                    pusConnTmp[0] = (unsigned short)i;
                    pusConnTmp[1] = (unsigned short)(i+1);
                    pusConnTmp +=2;
                    uiConn +=2;
                }
                pbFlags++;
            }

            if (*pbFlags)
            {
                pusConnTmp[0] = (unsigned short)uiMax;
                pusConnTmp[1] = (unsigned short)0;
                pusConnTmp +=2;
                uiConn +=2;
            }

            pkD3DIB9->Unlock();
        }
        else
        {
            NiDX9Renderer::Error("NiDX9IndexBufferManager::PackBuffer> "
                "Failed to lock index buffer\n");
            pkD3DIB9->Unlock();
            NiD3DRenderer::ReleaseIBResource(pkD3DIB9);
            pkD3DIB9 = 0;
        }
    }

    m_pkTempLineConnIB = pkD3DIB9;
    m_uiTempLineConnLen = uiSize;
    return pkD3DIB9;
}
//---------------------------------------------------------------------------
NiDX9IndexBufferManager::NiDX9IBInfo* NiDX9IndexBufferManager::GetIBInfo(
    unsigned int uiUsage, D3DFORMAT eFormat,D3DPOOL ePool)
{
    //  Form the index value
    unsigned int uiIndex = ((uiUsage << 16) & 0xFFFF0000) | 
        (unsigned int)eFormat;
    NiDX9IBInfo* pkIBInfo = NULL;

    switch (ePool)
    {
    case D3DPOOL_DEFAULT:
        if (m_kDefaultResources.GetAt(uiIndex, pkIBInfo) == false)
        {
            //  Doesn't exist, add it
            pkIBInfo = CreateIBInfo();
            pkIBInfo->m_uiIBID = uiIndex;
            m_kDefaultResources.SetAt(uiIndex, pkIBInfo);
        }
        break;
    case D3DPOOL_MANAGED:
        if (m_kManagedResources.GetAt(uiIndex, pkIBInfo) == false)
        {
            //  Doesn't exist, add it
            pkIBInfo = CreateIBInfo();
            pkIBInfo->m_uiIBID = uiIndex;
            m_kManagedResources.SetAt(uiIndex, pkIBInfo);
        }
        break;
    case D3DPOOL_SYSTEMMEM:
        if (m_kSystemMemResources.GetAt(uiIndex, pkIBInfo) == false)
        {
            //  Doesn't exist, add it
            pkIBInfo = CreateIBInfo();
            pkIBInfo->m_uiIBID = uiIndex;
            m_kSystemMemResources.SetAt(uiIndex, pkIBInfo);
        }
        break;
    default:
        NIASSERT(false);
        break;
    }

    return pkIBInfo;
}
//---------------------------------------------------------------------------
NiDX9IndexBufferManager::NiDX9IBInfo* NiDX9IndexBufferManager::CreateIBInfo()
{
    NiDX9IBInfo* pkIBInfo = NiNew NiDX9IBInfo;
    NIASSERT(pkIBInfo);
    pkIBInfo->m_uiIBID = 0;
    pkIBInfo->m_ucIndex = 0;
    memset((void*)&(pkIBInfo->m_apkIBRes), 0, 
        sizeof(NiDX9IBResource*) * DX9_MAX_IBS);
    
    return pkIBInfo;
}
//---------------------------------------------------------------------------
LPDIRECT3DINDEXBUFFER9 NiDX9IndexBufferManager::CreateIndexBuffer(
    unsigned int uiLength, unsigned int uiUsage, D3DFORMAT eFormat, 
    D3DPOOL ePool, D3DINDEXBUFFER_DESC* pkDesc)
{
    LPDIRECT3DINDEXBUFFER9 pkIB9 = NULL;

    NIASSERT(m_pkD3DDevice9);

    HRESULT eD3dRet = m_pkD3DDevice9->CreateIndexBuffer(uiLength, uiUsage, 
        eFormat, ePool, &pkIB9, NULL);
    if (SUCCEEDED(eD3dRet))
    {
        if (pkDesc)
        {
            pkIB9->GetDesc(pkDesc);
        }

        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(INDEX_BUFFER_SIZE, (uiLength));
    }
    else
    {
        NiDX9Renderer::Error("NiDX9IndexBufferManager::CreateIndexBuffer "
            "FAILED");
        pkIB9 = NULL;
    }

    return pkIB9;
}
//---------------------------------------------------------------------------
void NiDX9IndexBufferManager::DestroyDefaultBuffers()
{
    NiTMapIterator kPos = m_kDefaultResources.GetFirstPos();
    while (kPos)
    {
        unsigned int uiKey;
        NiDX9IBInfo* pkIBInfo;
        m_kDefaultResources.GetNext(kPos, uiKey, pkIBInfo);

        //  Kill the vertex buffers...
        if (pkIBInfo)
        {
            NiDX9IBResource* pkIBRes;
        
            for (unsigned int i = 0; i < DX9_MAX_IBS; i++)
            {
                pkIBRes = pkIBInfo->m_apkIBRes[i];
                if (pkIBRes)
                {
                    if (pkIBRes->m_pkIB)
                    {
                        NiD3DRenderer::ReleaseIBResource(pkIBRes->m_pkIB);
                    }
                }
                NiDelete pkIBRes;
            }
            NiDelete pkIBInfo;
        }
    }

    m_kDefaultResources.RemoveAll();

    if (m_pkTempPointConnIB != 0)
    {
        NiD3DRenderer::ReleaseIBResource(m_pkTempPointConnIB);
        m_pkTempPointConnIB = 0;
    }

    if (m_pkTempLineConnIB != 0)
    {
        NiD3DRenderer::ReleaseIBResource(m_pkTempLineConnIB);
        m_pkTempLineConnIB = 0;
    }
}
//---------------------------------------------------------------------------
