//**********************************************************************
//
// Copyright (c) 2008
// Thomas Young, PathEngine
// Lyon, France
//
// All Rights Reserved
//
//**********************************************************************

class iPathEngine;
class iErrorHandler;
class iContentProcessing3D;

// the following methods should each normally only be called once each
// calling the initialise method after the SDK is already initialised will result in an error being emitted
// calling the shutdown method when the SDK has not yet been initialised, or when it has already been shutdown, will have no effect
// it is possible, however, to reinitialise the SDK after it has been shutdown
extern "C"
{
iPathEngine* PathEngine_InitialiseAndObtainRootInterface(iErrorHandler* handler);
void PathEngine_ShutDown();
iContentProcessing3D* PathEngine_InitialiseCP3DAndObtainInterface();
void PathEngine_ShutDownCP3D();
}
