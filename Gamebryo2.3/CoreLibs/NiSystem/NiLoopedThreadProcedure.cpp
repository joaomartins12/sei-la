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

#include "NiSystemPCH.h"
#include "NiLoopedThreadProcedure.h"
#include "NiLoopedThread.h"

//---------------------------------------------------------------------------
bool NiLoopedThreadProcedure::LoopedProcedure(void* pvArg)
{
    // default behavior: do nothing once
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiLoopedThreadProcedure::ThreadProcedure(void* pvArg)
{
    NiLoopedThread* pkThread = (NiLoopedThread*)pvArg;

    pkThread->WaitStart();
    while(!pkThread->GetLastLoop())
    {
        if (!LoopedProcedure(pkThread))
        {
            // If return value is false, then the thread explicitly
            // terminates itself.
            pkThread->SignalComplete();
            return PROC_TERMINATED;
        }

        pkThread->SignalComplete();
        pkThread->WaitStart();
    }

    // If last loop was set, then thread is terminated by some controlling 
    // thread.
    pkThread->SignalComplete();
    return THREAD_TERMINATED;
}
//---------------------------------------------------------------------------
