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

#ifndef MAXIMMERSE_H
#define MAXIMMERSE_H

#define MAXIMMERSEBUILDSTRING __DATE__
#define MAXIMMERSEVERSION "6.0.0 a"
// a - Gamebryo 2.0 version


#define MAXIMMERSEVERSIONINT            6000000
#define LAST_VERSION_AFFECTING_OPTIONS  5200005
// The format for the MAXIMMERSEVERSIONINT is determined
// from the MAXIMMERSEVERSION using two digits for each
// component i.e."12.34.56.a" converts to "12345601"
// Pad with zeroes where appropriate, except in the first component 
// as this will make the number octal instead of base 10
#include <crtdbg.h>

#ifndef _DEBUG

#if defined(NI_RELEASE_MODE_LOGGING)
#   undef NI_RELEASE_MODE_LOGGING
#endif
#define NI_RELEASE_MODE_LOGGING 1

#endif

#if 0
#define CHECK_MEMORY() \
{ \
    NIASSERT( _CrtCheckMemory( ) );\
}
#else
#define CHECK_MEMORY() \
{ \
}
#endif

#define NIECHOPOS() \
{\
    NILOG("Got to: %s, %d\n", __FILE__, __LINE__);\
}\

// Uncomment the following lines to return to pre-2.0 spherical environment map
// export.
//#define USE_OLD_SPHERICAL_ENVIRONMENT_MAP

#include <NiSystem.h>
#include <NiMain.h>
#include <NiAnimation.h>

#include <NiParticle.h>

#undef NiOutputDebugString
#define NiOutputDebugString(x) NILOG(x)

#include <NiOptimize.h>
#include <NiMaterialToolkit.h>

#include "MiLibTypes.h"
#include "NiMAX.h"
#include "MiResource.h"
#include "MiUtils.h"
#include "NiMAXScriptUtils.h"
#include "MiViewerPlugin.h"
#include "MiExportPlugin.h"
#include "NiMaxShader.h"
#include "NiDevImageQuantizer.h"

#include <assert.h>
#include <NiFramework.h>
#include <NiPluginManager.h>
#include <NiScriptTemplateManager.h>
#include "NiMAXExporter.h"

#undef STRICT // Avoid a warning b/c Maxscrpt.h defines STRICT too
#include "Maxscrpt.h"
#include "Name.h"
#include "Numbers.h"    
#include "Arrays.h"
#include "Strings.h"
#include "MaxObj.h"
#include "bitmaps.h"
#include "maxmats.h"
#include "colorval.h"
#include "definsfn.h"   // generate static instances from def_x macros for 
                        // MaxScript

#include "NiMAXConverter.h"
#define NILOGERROR(x) NiMAXConverter::LogError(x)
#define NILOGWARNING(x) NiMAXConverter::LogWarning(x)

#endif