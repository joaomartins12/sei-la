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
#include "NiParamsKF.h"
#include <NiMain.h>

//---------------------------------------------------------------------------
NiImplementRTTI(NiParamsKF, NiExternalAssetParams);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiParamsKF::NiParamsKF()
{
    m_spCtrlSeq = 0;
}
//---------------------------------------------------------------------------
NiParamsKF::~NiParamsKF()
{
    m_spCtrlSeq = 0;
}
//---------------------------------------------------------------------------
void NiParamsKF::SetAnimIndex(unsigned int uiAnimIndex)
{
    m_uiAnimIndex = uiAnimIndex;    
}
//---------------------------------------------------------------------------
unsigned int NiParamsKF::GetAnimIndex()
{
    return m_uiAnimIndex;
}
//---------------------------------------------------------------------------
NiBool NiParamsKF::GetControllerSequence(NiControllerSequence*& pkCtrlSeq)
{
    pkCtrlSeq = m_spCtrlSeq;
    return true;
}
//---------------------------------------------------------------------------
NiBool NiParamsKF::SetControllerSequence(NiControllerSequence* pkCtrlSeq)
{
    m_spCtrlSeq = pkCtrlSeq;
    return true;
}
//---------------------------------------------------------------------------
