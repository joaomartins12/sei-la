//############################################################################
//##                                                                        ##
//##  Miles Sound System                                                    ##
//##                                                                        ##
//##  MSSPSP.C: C API module and support routines                           ##
//##                                                                        ##
//############################################################################
//##                                                                        ##
//##  Copyright (C) RAD Game Tools, Inc.                                    ##
//##                                                                        ##
//##  Contact RAD Game Tools at 425-893-4300 for technical support.         ##
//##                                                                        ##
//############################################################################

#include "mss.h"
#include "imssapi.h"
#include "radss.h"

DXDEF S32 AILCALL AIL_platform_property (void                 *object, //)
                                         MSS_PLATFORM_PROPERTY property,
                                         void             *before_value,
                                         void const       *new_value,
                                         void             *after_value)
{
  return 0;
}

S32 AILCALL get_system_speaker_configuration(MSS_MC_SPEC *channel_spec)
{
  *channel_spec = MSS_MC_STEREO;
  return 1;
}
