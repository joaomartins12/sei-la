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

#ifndef OCCLUDER_H
#define OCCLUDER_H

#include <NiCamera.h>

class Occluder : public NiObject
{
    NiDeclareRTTI;

    Occluder();
    virtual ~Occluder();

public:
    virtual void Update(NiCamera* pkCamera) = 0;
    virtual bool IsOccluded(NiCamera* pkCamera, const NiBound& kBound) = 0;
	virtual void ShowObject(bool bVisible) {};
};

NiSmartPointer(Occluder);

#endif // OCCLUDER_H 
