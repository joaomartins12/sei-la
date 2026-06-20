#ifndef GETDEFAULTSHADOWBIAS_H
#define GETDEFAULTSHADOWBIAS_H

#include "maya/MPxCommand.h"

class GetDefaultShadowBias : public MPxCommand 
{ 
public: 
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
}; 

#endif // GETDEFAULTSHADOWBIAS_H