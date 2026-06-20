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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10DataStream.h"

#include "NiD3D10Error.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10ResourceManager.h"

//---------------------------------------------------------------------------
NiD3D10DataStream::NiD3D10DataStream(size_t uiBufferSize, D3D10_USAGE eUsage, 
    unsigned int uiBindFlags, unsigned int uiCPUAccessFlags, 
    unsigned int uiMiscFlags, D3D10_SUBRESOURCE_DATA* pkInitialData) :
    m_pkBuffer(NULL),
    m_uiBufferSize(0),
    m_uiAccessFlags(0),
    m_uiUsageFlags(0),
    m_uiMiscFlags(0),
    m_eUsage(D3D10_USAGE_DEFAULT),
    m_bLocked(false)
{
    memset(&m_kLockHelpers, 0, sizeof(m_kLockHelpers));

    AllocateBuffer(uiBufferSize, eUsage, uiBindFlags, uiCPUAccessFlags, 
        uiMiscFlags, pkInitialData);
}
//---------------------------------------------------------------------------
NiD3D10DataStream::NiD3D10DataStream(ID3D10Buffer* pkBuffer) :
    m_pkBuffer(pkBuffer),
    m_uiBufferSize(0),
    m_uiAccessFlags(0),
    m_uiUsageFlags(0),
    m_uiMiscFlags(0),
    m_eUsage(D3D10_USAGE_DEFAULT),
    m_bLocked(false)
{
    memset(&m_kLockHelpers, 0, sizeof(m_kLockHelpers));

    NIASSERT(pkBuffer);
    pkBuffer->AddRef();

    D3D10_BUFFER_DESC kDesc;
    pkBuffer->GetDesc(&kDesc);

    m_uiBufferSize = kDesc.ByteWidth;
    m_eUsage = kDesc.Usage;

    InterpretD3D10Parameters(kDesc.Usage, kDesc.BindFlags, 
        kDesc.CPUAccessFlags, m_uiUsageFlags, m_uiAccessFlags);
        
    m_uiMiscFlags = kDesc.MiscFlags;
}
//---------------------------------------------------------------------------
NiD3D10DataStream::~NiD3D10DataStream()
{
    // This really shouldn't every happen, but it might...
    if (m_bLocked)
    {
        // Don't bother trying to reallocate an IMMUTABLE buffer here.
        if (m_eUsage == D3D10_USAGE_STAGING)
            m_kLockHelpers.m_uiImmutableUsageFlags = 0;
        Unlock();
    }

    ReleaseBuffer();
}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::Create(size_t uiBufferSize, 
    unsigned int uiAccessFlags, unsigned int uiUsageFlags, 
    NiD3D10DataStreamPtr& spDataStream)
{
    if (uiBufferSize == 0)
    {
        return false;
    }

    D3D10_USAGE eUsage;
    unsigned int uiActualAccessFlags = 
        InterpretAccessFlags(uiAccessFlags, eUsage);
    unsigned int uiBindFlags = InterpretUsageFlags(uiUsageFlags);

    unsigned int uiCPUAccessFlags = 0;

    if (eUsage == D3D10_USAGE_STAGING)
    {
        uiBindFlags = 0;
        uiCPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    }
    else if (eUsage == D3D10_USAGE_DYNAMIC)
    {
        uiCPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    }

    spDataStream = NiNew NiD3D10DataStream(uiBufferSize, eUsage, 
        uiBindFlags, uiCPUAccessFlags, 0, NULL);
    if (spDataStream == NULL || spDataStream->GetBuffer() == NULL)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::Create(size_t uiBufferSize, D3D10_USAGE eUsage, 
    unsigned int uiBindFlags, unsigned int uiCPUAccessFlags, 
    unsigned int uiMiscFlags, D3D10_SUBRESOURCE_DATA* pkInitialData, 
    NiD3D10DataStreamPtr& spDataStream)
{
    if (uiBufferSize == 0)
    {
        return false;
    }

    spDataStream = NiNew NiD3D10DataStream(uiBufferSize, eUsage, 
        uiBindFlags, uiCPUAccessFlags, uiMiscFlags, pkInitialData);
    if (spDataStream == NULL || spDataStream->GetBuffer() == NULL)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::Create(ID3D10Buffer* pkBuffer, 
    NiD3D10DataStreamPtr& spDataStream)
{
    if (pkBuffer)
        spDataStream = NiNew NiD3D10DataStream(pkBuffer);

    if (spDataStream == NULL || spDataStream->GetBuffer() == NULL)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
size_t NiD3D10DataStream::GetSize() const
{
    return m_uiBufferSize;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10DataStream::GetAccessFlags() const
{
    return m_uiAccessFlags;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10DataStream::GetUsageFlags() const
{
    if (m_eUsage == D3D10_USAGE_STAGING)
        return m_kLockHelpers.m_uiImmutableUsageFlags;
    else
        return m_uiUsageFlags;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10DataStream::GetMiscFlags() const
{
    return m_uiMiscFlags;
}
//---------------------------------------------------------------------------
D3D10_USAGE NiD3D10DataStream::GetD3D10Usage() const
{
    return m_eUsage;
}
//---------------------------------------------------------------------------
void* NiD3D10DataStream::Lock(unsigned char ucLockFlags)
{
    if (m_pkBuffer == NULL)
        return NULL;

    if (ucLockFlags == 0)
    {
        return NULL;
    }

    bool bWrite = false;
    if ((ucLockFlags & LOCK_WRITE) != 0)
    {
        if ((m_uiAccessFlags & ACCESS_CPU_WRITE) == 0)
        {
            return NULL;
        }
        bWrite = true;
    }

    bool bRead = false;
    if ((ucLockFlags & LOCK_READ) != 0)
    {
        if ((m_uiAccessFlags & ACCESS_CPU_READ) == 0)
        {
            return NULL;
        }
        bRead = true;
    }

    // Immutable resources should never be locked - by this point, they 
    // should have hit a quick-out
    NIASSERT(m_eUsage != D3D10_USAGE_IMMUTABLE);

    if (m_bLocked)
    {
        return NULL;
    }

    m_bLocked = true;

    void* pvData = NULL;

    if (m_eUsage == D3D10_USAGE_DYNAMIC || m_eUsage == D3D10_USAGE_STAGING)
    {
        // dynamic or staging - use Map in either case
        NIASSERT((m_uiAccessFlags & (ACCESS_DYNAMIC | ACCESS_CPU_READ)) != 0);

        D3D10_MAP eMapping;
        if (m_eUsage == D3D10_USAGE_DYNAMIC)
        {
            // Shouldn't be able to read dynamic buffer
            NIASSERT(bRead == false);

            eMapping = D3D10_MAP_WRITE_DISCARD;
        }
        else
        {
            if (bRead)
            {
                if (bWrite)
                    eMapping = D3D10_MAP_READ_WRITE;
                else
                    eMapping = D3D10_MAP_READ;
            }
            else
            {
                NIASSERT(bWrite);
                eMapping = D3D10_MAP_WRITE;
            }
        }

        HRESULT hr = m_pkBuffer->Map(eMapping, 0, &pvData);
        if (FAILED(hr))
        {
            //...
        }
    }
    else
    {
        NIASSERT(m_eUsage == D3D10_USAGE_DEFAULT);
        // Mutable

        // Create a temporary CPU-side NiD3D10DataStream that will be used
        // to update the GPU-side resource.
        NIASSERT(m_kLockHelpers.m_pkTempStream == NULL);

        NiD3D10DataStreamPtr spTempStream;
        NIVERIFY(NiD3D10DataStream::Create(m_uiBufferSize, 
            D3D10_USAGE_STAGING, 0, 
            D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE, 0, NULL,
            spTempStream));
        NIASSERT(spTempStream);

        // A union can't have a smart pointer in it, so manually bump the
        // ref count here.
        spTempStream->IncRefCount();
        m_kLockHelpers.m_pkTempStream = spTempStream;

        pvData = m_kLockHelpers.m_pkTempStream->Lock(ucLockFlags);
    }

    return pvData;
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::Unlock()
{
    if (!m_bLocked)
        return;

    // Immutable resources should never be locked.
    NIASSERT(m_eUsage != D3D10_USAGE_IMMUTABLE);

    if (m_eUsage == D3D10_USAGE_STAGING)
    {
        m_pkBuffer->Unmap();
        if (m_kLockHelpers.m_uiImmutableUsageFlags != 0)
        {
            // This was meant to be an IMMUTABLE resource, so create one now.
            NIASSERT((m_uiAccessFlags & ACCESS_CPU_READ) != 0);

            ID3D10Buffer* pkStaging = m_pkBuffer;
            void* pvStagingData;
            HRESULT hr = pkStaging->Map(D3D10_MAP_READ, 0, &pvStagingData);
            NIASSERT(SUCCEEDED(hr) && pvStagingData != NULL);

            D3D10_SUBRESOURCE_DATA kInitialData;
            kInitialData.pSysMem = pvStagingData;
            // D3D10 docs recommend this to catch errors
            kInitialData.SysMemPitch = m_uiBufferSize;
            kInitialData.SysMemSlicePitch = m_uiBufferSize;

            unsigned int uiBindFlags = 
                InterpretUsageFlags(m_kLockHelpers.m_uiImmutableUsageFlags);

            AllocateBuffer(m_uiBufferSize, D3D10_USAGE_IMMUTABLE, uiBindFlags, 
                0, m_uiMiscFlags, &kInitialData);
            if (m_pkBuffer == NULL)
            {
                //...
            }

            // Release the STAGING resource
            pkStaging->Unmap();
            pkStaging->Release();
        }
    }
    else if (m_eUsage == D3D10_USAGE_DYNAMIC)
    {
        NIASSERT((m_uiAccessFlags & ACCESS_DYNAMIC) != 0);
        m_pkBuffer->Unmap();
    }
    else
    {
        NIASSERT(m_eUsage == D3D10_USAGE_DEFAULT);

        // Unlock the temp resource
        m_kLockHelpers.m_pkTempStream->Unlock();

        // Copy the temp resource back to the original.
        NIASSERT(NiD3D10Renderer::GetRenderer());
        ID3D10Device* pkDevice = 
            NiD3D10Renderer::GetRenderer()->GetD3D10Device();

        pkDevice->CopyResource(m_pkBuffer, 
            m_kLockHelpers.m_pkTempStream->m_pkBuffer);

        // Manually release temp resource
        m_kLockHelpers.m_pkTempStream->DecRefCount();
        m_kLockHelpers.m_pkTempStream = NULL;
    }

    m_bLocked = false;

}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::GetLocked() const
{
    return m_bLocked;
}
//---------------------------------------------------------------------------
ID3D10Buffer* NiD3D10DataStream::GetBuffer() const
{
    return m_pkBuffer;
}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::Reallocate(size_t uiBufferSize, 
    unsigned int uiAccessFlags, unsigned int uiUsageFlags)
{
    D3D10_USAGE eUsage;
    unsigned int uiActualAccessFlags = 
        InterpretAccessFlags(uiAccessFlags, eUsage);
    unsigned int uiBindFlags = InterpretUsageFlags(uiUsageFlags);

    unsigned int uiCPUAccessFlags = 0;

    if (eUsage == D3D10_USAGE_STAGING)
    {
        uiBindFlags = 0;
        uiCPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    }
    else if (eUsage == D3D10_USAGE_DYNAMIC)
    {
        uiCPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    }

    return Reallocate(uiBufferSize, eUsage, uiBindFlags, uiCPUAccessFlags, 
        0, NULL);
}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::Reallocate(size_t uiBufferSize, D3D10_USAGE eUsage, 
    unsigned int uiBindFlags, unsigned int uiCPUAccessFlags, 
    unsigned int uiMiscFlags, D3D10_SUBRESOURCE_DATA* pkInitialData)
{
    ReleaseBuffer();
    AllocateBuffer(uiBufferSize, eUsage, uiBindFlags, uiCPUAccessFlags, 
        uiMiscFlags, pkInitialData);

    return (m_pkBuffer != NULL);
}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::Reallocate(size_t uiBufferSize)
{    D3D10_USAGE eUsage;

    unsigned int uiActualAccessFlags = 
        InterpretAccessFlags(m_uiAccessFlags, eUsage);
    NIASSERT(eUsage == m_eUsage);
    unsigned int uiBindFlags = InterpretUsageFlags(m_uiUsageFlags);

    unsigned int uiCPUAccessFlags = 0;

    if (eUsage == D3D10_USAGE_STAGING)
    {
        uiBindFlags = 0;
        uiCPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    }
    else if (eUsage == D3D10_USAGE_DYNAMIC)
    {
        uiCPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    }

    return Reallocate(uiBufferSize, m_eUsage, uiBindFlags, uiCPUAccessFlags, 
        m_uiMiscFlags, NULL);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10DataStream::InterpretAccessFlags(
    unsigned int uiAccessFlags, D3D10_USAGE& eUsage)
{
    unsigned int uiActualAccessFlags = 0;

    if ((uiAccessFlags & NiD3D10DataStream::ACCESS_DYNAMIC) != 0)
    {
        // Dynamic flag overrides all else
        eUsage = D3D10_USAGE_DYNAMIC;
        uiActualAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ | 
            NiD3D10DataStream::ACCESS_CPU_WRITE | 
            NiD3D10DataStream::ACCESS_DYNAMIC;
    }
    else if (uiAccessFlags == NiD3D10DataStream::ACCESS_GPU_READ)
    {
        // If it's only readable by only the GPU, assume immutable
        eUsage = D3D10_USAGE_IMMUTABLE;
        uiActualAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ;
    }
    else if ((uiAccessFlags & NiD3D10DataStream::ACCESS_GPU_READ) != 0)
    {
        // If it's not dynamic or immutable, and it must be read by the GPU,
        // then it's default.
        eUsage = D3D10_USAGE_DEFAULT;
        uiActualAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ | 
            NiD3D10DataStream::ACCESS_GPU_WRITE | 
            NiD3D10DataStream::ACCESS_CPU_WRITE;
    }
    else
    {
        // If it's not readable by the GPU at all, then it's staging.
        eUsage = D3D10_USAGE_STAGING;
        uiActualAccessFlags = NiD3D10DataStream::ACCESS_CPU_READ | 
            NiD3D10DataStream::ACCESS_CPU_WRITE;
    }

    return uiActualAccessFlags;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10DataStream::InterpretUsageFlags(unsigned int uiUsageFlags)
{
    unsigned int uiBindFlags = 0;
    if ((uiUsageFlags & USAGE_INDEX) != 0)
        uiBindFlags |= D3D10_BIND_INDEX_BUFFER;
    if ((uiUsageFlags & USAGE_VERTEX) != 0)
        uiBindFlags |= D3D10_BIND_VERTEX_BUFFER;
    if ((uiUsageFlags & USAGE_SHADER_CONSTANT) != 0)
        uiBindFlags |= D3D10_BIND_CONSTANT_BUFFER;

    return uiBindFlags;
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::AllocateBuffer(size_t uiBufferSize, D3D10_USAGE eUsage,
    unsigned int uiBindFlags, unsigned int uiCPUAccessFlags, 
    unsigned int uiMiscFlags, D3D10_SUBRESOURCE_DATA* pkInitialData)
{
    assert (NiD3D10Renderer::GetRenderer());
    NiD3D10ResourceManager* pkResourceManager = 
        NiD3D10Renderer::GetRenderer()->GetResourceManager();
    assert (pkResourceManager);

    // If we're allocating an IMMUTABLE buffer and it doesn't have any
    // initial data, then actually create a STAGING buffer, and set the
    // m_uiImmutableUsageFlags flag. Then, during Unlock, check for that flag
    // and create the IMMUTABLE resource using the STAGING buffer.
    unsigned int uiImmutableUsageFlags = 0;
    if (eUsage == D3D10_USAGE_IMMUTABLE && pkInitialData == NULL)
    {
        eUsage = D3D10_USAGE_STAGING;
        uiCPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;

        // Store actual bind flags in m_kLockHelpers - this will be a 
        // signal that this buffer is actually intended to be IMMUTABLE.
        if ((uiBindFlags & D3D10_BIND_INDEX_BUFFER) != 0)
            uiImmutableUsageFlags |= USAGE_INDEX;
        if ((uiBindFlags & D3D10_BIND_VERTEX_BUFFER) != 0)
            uiImmutableUsageFlags |= USAGE_VERTEX;
        if ((uiBindFlags & D3D10_BIND_CONSTANT_BUFFER) != 0)
            uiImmutableUsageFlags |= USAGE_SHADER_CONSTANT;
        NIASSERT(uiImmutableUsageFlags != 0);

        uiBindFlags = 0;
    }

    m_pkBuffer = pkResourceManager->CreateBuffer(uiBufferSize, uiBindFlags, 
        eUsage, uiCPUAccessFlags, uiMiscFlags, pkInitialData);

    if (m_pkBuffer)
    {
        m_uiBufferSize = uiBufferSize;

        m_kLockHelpers.m_uiImmutableUsageFlags = uiImmutableUsageFlags;
        InterpretD3D10Parameters(eUsage, uiBindFlags, 
            uiCPUAccessFlags, m_uiUsageFlags, m_uiAccessFlags);

        m_uiMiscFlags = uiMiscFlags;
        m_eUsage = eUsage;

    }
    else
    {
        // Resource creation would have thrown error message
        NiD3D10Error::ReportWarning(
            "NiD3D10DataStream::AllocateBuffer failed because buffer "
            "could not be created.");

        m_uiBufferSize = 0;
        m_uiAccessFlags = 0;
        m_uiUsageFlags = 0;
        m_uiMiscFlags = 0;
        m_eUsage = D3D10_USAGE_DEFAULT;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::ReleaseBuffer()
{
    if (m_pkBuffer)
        m_pkBuffer->Release();

    m_uiBufferSize = 0;
    m_uiAccessFlags = 0;
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::InterpretD3D10Parameters(D3D10_USAGE eD3D10Usage, 
    unsigned int uiD3D10BindFlags, unsigned int uiD3D10CPUAccessFlags, 
    unsigned int& uiUsageFlags, unsigned int& uiAccessFlags)
{
    uiUsageFlags = 0;
    if ((uiD3D10BindFlags & D3D10_BIND_INDEX_BUFFER) != 0)
        uiUsageFlags |= USAGE_INDEX;
    if ((uiD3D10BindFlags & D3D10_BIND_VERTEX_BUFFER) != 0)
        uiUsageFlags |= USAGE_VERTEX;
    if ((uiD3D10BindFlags & D3D10_BIND_CONSTANT_BUFFER) != 0)
        uiUsageFlags |= USAGE_SHADER_CONSTANT;

    NIASSERT(uiD3D10BindFlags == InterpretUsageFlags(m_uiUsageFlags));

    if (eD3D10Usage == D3D10_USAGE_DYNAMIC)
    {
        uiAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ | 
            NiD3D10DataStream::ACCESS_CPU_WRITE | 
            NiD3D10DataStream::ACCESS_DYNAMIC;
        // Dynamic buffers must not be readable.
        NIASSERT((uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_READ) == 0);
        // Though it's not clear what it means for them to not be writable...
        if ((uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_WRITE) != 0)
            uiAccessFlags |= NiD3D10DataStream::ACCESS_CPU_WRITE;
    }
    else if (eD3D10Usage == D3D10_USAGE_IMMUTABLE)
    {
        uiAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ;
        // Immutable buffers can't be readable or writable.
        NIASSERT((uiD3D10CPUAccessFlags & 
            (D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ)) == 0);
    }
    else if (eD3D10Usage == D3D10_USAGE_DEFAULT)
    {
        uiAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ | 
            NiD3D10DataStream::ACCESS_GPU_WRITE | 
            NiD3D10DataStream::ACCESS_CPU_WRITE;
        // Default buffers can't be readable or writable, but the 
        // ACCESS_CPU_WRITE access flag is still set to indicate that 
        // Gamebryo will allow writes (through the subresource update).
        NIASSERT((uiD3D10CPUAccessFlags & 
            (D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ)) == 0);
    }
    else
    {
        NIASSERT(eD3D10Usage == D3D10_USAGE_STAGING);
        uiAccessFlags = 0;
        NiD3D10DataStream::ACCESS_CPU_READ | 
            NiD3D10DataStream::ACCESS_CPU_WRITE;
        // Staging buffers can be readable and/or writable, though I don't
        // know how useful it would be for one to be set but not the other.
        if ((uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_WRITE) != 0)
            uiAccessFlags |= NiD3D10DataStream::ACCESS_CPU_WRITE;
        if ((uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_READ) != 0)
            uiAccessFlags |= NiD3D10DataStream::ACCESS_CPU_READ;
    }
}
//---------------------------------------------------------------------------
