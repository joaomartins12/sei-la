#ifndef GETSHADOWTECHNIQUESCOMMAND_H
#define GETSHADOWTECHNIQUESCOMMAND_H

#include "maya/MPxCommand.h"

class GetShadowTechniques : public MPxCommand 
{ 
public: 
    MStatus        doIt( const MArgList& args ); 
    static void*   creator(); 
}; 

#endif
