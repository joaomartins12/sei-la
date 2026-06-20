/**********************************************************************
 *<
	FILE: TerrainExportXML.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#ifndef __TerrainExportXML__H
#define __TerrainExportXML__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
//SIMPLE TYPE


struct GEOMINFO
{
	GEOMINFO() {
		s_pNodeName = s_pParentNodeName = NULL; s_Position = s_Rotate = s_Scale = Point3(0.0f,0.0f,0.0f);
		s_nOrientation = s_nFaceCount = s_nVertexCount = 0; for( int i=0; i<4; ++i ) s_Vertex[i] = Point3(0.0f,0.0f,0.0f);
		s_PerentTrans = Point3(0.0f,0.0f,0.0f);
	}

	struct FACE
	{
		FACE() { s_nFace1 = s_nFace2 = s_nFace3 = 0; }

		DWORD s_nFace1;
		DWORD s_nFace2;
		DWORD s_nFace3;
	};

	MCHAR*		s_pNodeName;
	MCHAR*		s_pParentNodeName;

	Point3		s_Position;
	Point3		s_Rotate;
	Point3		s_Scale;

	UINT		s_nOrientation;

	int			s_nVertexCount;
	Point3		s_Vertex[4];

	UINT		s_nFaceCount;
	FACE		s_Face[2];

	Point3		s_PerentTrans;
};


extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#endif
