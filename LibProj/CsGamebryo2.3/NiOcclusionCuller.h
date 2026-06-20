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

#ifndef NIOCCLUSIONCULLER_H
#define NIOCCLUSIONCULLER_H

#include <NiPlane.h>
#include <NiPoint3.h>
#include <NiTArray.h>
#include <NiTransform.h>

#include "Occluder.h"

class NiGeometry;
class NiScene;
class NiEntityInterface;

class NiOcclusionCuller : public NiRefObject
{
public:
    NiOcclusionCuller();
    virtual ~NiOcclusionCuller();

	void Update(const NiCamera* pkCamera);
	bool IsCulling(NiCamera* pkCamera, NiAVObject* pkObject);

	void AddOcclusionGeometry(NiNode* pkEntityScene);
    void FindOcclusionGeometry(NiNode* pkEntityScene);
    void FindOcclusionGeometry(NiAVObject* pkAVObject, bool bMarkedParent);
    void RemoveAllOcclusionGeometry();
	void ShowOccluder(bool bVisible);

protected:
    void RecursiveFindOcclusionGeometry(NiNode* pkEntity);


    NiTPrimitiveArray<Occluder*> m_kOccluders;
};

#endif
