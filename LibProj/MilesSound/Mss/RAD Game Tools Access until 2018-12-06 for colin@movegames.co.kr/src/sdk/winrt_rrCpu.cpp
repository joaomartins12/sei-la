#include "rrCpu.h"
#include "rrAtomics.h"

#include <windows.h>

RADDEFFUNC S32 RADLINK rrGetTotalCPUs( void )
{
  static S32 number_cores = -1;
  
  if ( number_cores == -1 )
  {
    SYSTEM_INFO si;
    GetNativeSystemInfo( &si );
    number_cores = si.dwNumberOfProcessors;
  }

  return number_cores;
}

// CB : rrThreadGetSpinCount is a good amount to spin
//  in particular it is 1 if not multi-core
RADDEFFUNC S32 RADLINK rrGetSpinCount( void )
{
  static S32 spins = -1;
  if ( spins == -1 )
  {
    if ( rrGetTotalCPUs() >= 2 )
      spins = 500;
    else
      spins = 1;
  }
  return spins;
}

#if defined(__RADX64__) || defined(__RADX86__)

rrbool rrCPU_HasSSE2( void )
{
  return IsProcessorFeaturePresent( PF_XMMI64_INSTRUCTIONS_AVAILABLE );
}

#endif