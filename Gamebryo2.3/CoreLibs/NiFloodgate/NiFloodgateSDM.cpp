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

//---------------------------------------------------------------------------
#include "NiFloodgatePCH.h"
#include "NiFloodgateSDM.h"
#include "NiStreamProcessor.h"
//---------------------------------------------------------------------------
NiImplementSDMConstructor(NiFloodgate);
//---------------------------------------------------------------------------
#ifdef NIFLOODGATE_EXPORT
NiImplementDllMain(NiFloodgate);
#endif
//---------------------------------------------------------------------------
void NiFloodgateSDM::Init()
{
    NiImplementSDMInitCheck();

    NiStreamProcessor::InitializePools();
    
    NiUInt32 uiMaxQueueSize = 512;
    
    // Initialize the Stream Processor
    NiStreamProcessor::Initialize(uiMaxQueueSize);

}
//---------------------------------------------------------------------------
void NiFloodgateSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiStreamProcessor::ShutdownPools();
    NiStreamProcessor::Shutdown();
}
//---------------------------------------------------------------------------
