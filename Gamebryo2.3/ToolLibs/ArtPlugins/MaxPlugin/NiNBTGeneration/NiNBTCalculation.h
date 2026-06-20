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

#ifndef NINBTCALCULATION_H
#define NINBTCALCULATION_H

#include <NiGeometryData.h>

class NiBinaryStream;
class NiGeometry;
class NiTriShape;
class NiTriStrips;
class NiPoint3;
class NiPoint2;

class NiNBTCalculation
{
public:
    NiNBTCalculation();
    ~NiNBTCalculation();

    static bool SetupBinormalTangentData(NiGeometry* pkGeometry,
        unsigned short usUVSet);

protected:
    static bool CreateTangentSpace_TriShape(NiTriShape* pkTriShape,
        NiGeometryData::DataFlags eNBTMethod, unsigned short usUVSet);
 
    static bool CreateBasisMatrices_NDL(unsigned int uiNumVertices, 
        NiPoint3* pkVertices, NiPoint3* pkNormals, NiPoint2* pkTextures, 
        unsigned int uiNumIndices, unsigned short* pusIndices);
 
    static bool CreateBasisMatrices_ATI(unsigned int uiNumVertices, 
        NiPoint3* pkVertices, NiPoint3* pkNormals, NiPoint2* pkTextures, 
        unsigned int uiNumIndices, unsigned short* pusIndices);

    static bool CreateBasisMatrices_MAX(unsigned int uiNumVertices, 
        NiPoint3* pkVertices, NiPoint3* pkNormals, NiPoint2* pkTextures, 
        unsigned int uiNumIndices, unsigned short* pusIndices);

    static void NiNBTCalculation::FindNBT(NiPoint3* pkNBTs, 
        unsigned int uiNBTIndex, unsigned int uiNumVertices, NiPoint3 kVert0, 
        NiPoint3 kVert1, NiPoint3 kVert2, NiPoint3 kNorm0, NiPoint3 kNorm1, 
        NiPoint3 kNorm2, NiPoint2 kTex0, NiPoint2 kTex1, NiPoint2 kTex2);
};

#endif  //NINBTCALCULATION_H
