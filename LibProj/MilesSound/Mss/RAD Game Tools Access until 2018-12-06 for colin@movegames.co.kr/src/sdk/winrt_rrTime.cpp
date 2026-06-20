// Copyright 2008-2013 RAD Game Tools

#include "rrTime.h"
#include "rrAtomics.h"

#include <windows.h>

#pragma warning(push)
#pragma warning(disable:4035) // no return value

#ifdef __RADX64__

  #define our_rdtsc() __rdtsc()

#elif defined( __RADX86__ )
  static inline U64 our_rdtsc()
  { 
      __asm rdtsc;
      // eax/edx returned
  }
#elif defined( __RADARM__ )
  static inline U64 our_rdtsc()
  { 
    U64 qpc;
    QueryPerformanceCounter( (LARGE_INTEGER*) &qpc );
    return qpc;
  }
#else
  #error What cpu?
#endif

#define doyield() YieldProcessor()

static S64 volatile adjust_qpc;
static U32 volatile init_lock = 0;  // 0 = need to init, 1 = initted, 2 = currently initting
static U32 volatile lock_count = 0; 
static U64 volatile last_largest_qpc;

static U64 first_qpc;
static U64 first_gtc;
static U64 last_qpc;
static U64 last_gtc;
static U64 freq;
static F64 inv_freq;
static S64 time_of_200_ms;
static U64 time_of_100_seconds;

static U64 first_rdtsc;

#define YIELDCOUNT 300
#define SLEEPCOUNT 302

static void GetBases( void )
{
  U32 count = 0;
  
  // get the init lock
  for(;;)
  {
    U32 val;
    
    val = rrAtomicCmpXchg32( &init_lock, 2, 0 );
    if ( val == 1 )
      return;
    if ( val == 0 )
      break;

    if ( count > SLEEPCOUNT )
    { 
      count = 0;
      Sleep( 1 );
    }
    else
    {
      ++count;
      doyield();
      if ( count > YIELDCOUNT )
        Sleep(0);
    }

  }
  
  // we have the lock
  
  QueryPerformanceFrequency( (LARGE_INTEGER*) &freq );
  inv_freq = ( 1.0 / (F64) freq );
  time_of_200_ms = ( 200 * freq + ((1000/2)-1) ) / 1000;
  time_of_100_seconds = ( 100 * 1000 * freq + ((1000/2)-1) ) / 1000;
  QueryPerformanceCounter( (LARGE_INTEGER*) &first_qpc );
  
  first_gtc = GetTickCount64();
  first_rdtsc = our_rdtsc();
  
  adjust_qpc = 0;
  last_qpc = 0;
  last_largest_qpc = 0;
  last_gtc = 0;
  
  // clear lock (which write barriers)
  rrAtomicStoreRelease32( &init_lock, 1 );
}

static U64 our_get_time( void )
{
  U32 count;
  U64 qpc, ll_qpc;

  // otherwise use QPC
  QueryPerformanceCounter( (LARGE_INTEGER*)&qpc );
    
  // adjust for start time
  qpc -= first_qpc;
  
  // get the largest current qpc (using atomic add to get atomic read)
  ll_qpc = rrAtomicLoadAcquire64( &last_largest_qpc );

  // is this read smaller than our largest current?
  if ( qpc < ll_qpc )
  {
    // return previous value plus the adjust amount and return
    return ll_qpc + rrAtomicLoadAcquire64( (U64 volatile*) &adjust_qpc );
  }
   
  // update the last_largest_qpc atomically to new largest value
  for(;;)
  {
    ll_qpc = rrAtomicLoadAcquire64( &last_largest_qpc );
    
    // is the current value already larger? if so, continue...
    if ( ll_qpc >= qpc )
      break;
    // did we successfully update? if so, continue...
    if ( rrAtomicCmpXchg64( &last_largest_qpc, qpc, ll_qpc ) == ll_qpc )
      break;
    
    // spin on hyperthread machines (this is rare)
    doyield();
  }
  
  // now, we check to see if the qpc drastically varies from the gettickcount
  //   this is for oldish buggy AMD motherboards - we only do this check on
  //   the first synchronous thread calling this function.
  //   since you usually are getting time infrequently, this code almost 
  //   always runs.

  count = rrAtomicAddExchange32( &lock_count, 1 );
  if ( RAD_LIKELY ( count == 0 ) )
  {
    U64 dqpc;
    U64 dgtc;
    U64 gtc;
    
    gtc = GetTickCount64() - first_gtc;
    dgtc = gtc - last_gtc;
  
    // don't allow backwards samples from either time
    if ( RAD_UNLIKELY ( ( qpc < last_qpc ) || ( dgtc > 0xc000000000000000ULL ) ) )
    {
      U64 ret = last_qpc + adjust_qpc;
      
      // release lock count
      rrAtomicAddExchange32( &lock_count, -1 );
      
      // return previous value plus the adjust amount and return
      return ret;
    }
    
    dqpc = qpc - last_qpc;
    
    // only do the two timer check, if the delta between calls is 
    //   less than 100 seconds 
    if ( dqpc < time_of_100_seconds )
    {
      S64 diff;
  
      diff = (U64) ( ( dgtc * freq + ((1000/2)-1) ) / 1000 ) - dqpc;

      // have the two timers moved differently by more than 200 ms?
      if ( RAD_UNLIKELY ( ( diff > time_of_200_ms ) || ( -diff > time_of_200_ms ) ) )
      {
        // add in the different to now always adjust the QPC by.  That is,
        //   GetTickCount is considered more accurate, so if they are different
        //   we use GetTickCount() to skew the QPC value from then on.
        rrAtomicStoreRelease64( (U64 volatile*) &adjust_qpc, adjust_qpc + diff );
      }
    }

    // update the old values
    last_qpc = qpc;
    last_gtc = gtc;
    
    // add in the adjustment before we release the lock
    qpc += adjust_qpc;
    
    // release lock count
    rrAtomicAddExchange32( &lock_count, -1 );

    // return new count (adjust added in from above)
    return qpc;
  }
  
  // return new count plus the adjust amount and return
  return qpc + rrAtomicLoadAcquire64( (U64 volatile*) &adjust_qpc );
}

//  it's the slower reliable timer 
RADDEFFUNC U64 rrGetTime( void )
{
  if ( RAD_UNLIKELY ( rrAtomicLoadAcquire32( &init_lock ) != 1 ) )
    GetBases();
  
  return our_get_time();
}

//  fast high resolution timer
RADDEFFUNC U64 rrGetTicks( void )
{
  if ( RAD_UNLIKELY ( rrAtomicLoadAcquire32( &init_lock ) != 1 ) )
    GetBases();

  U64 ticks = our_rdtsc();
    
  ticks -= first_rdtsc;

  return ticks;
}

// converters
RADDEFFUNC F64 rrTimeToSeconds(U64 time)
{
  if ( RAD_UNLIKELY ( rrAtomicLoadAcquire32( &init_lock ) != 1 ) )
    GetBases();

  return ( (F64) time ) * inv_freq;
}

RADDEFFUNC U64 rrTimePerSecond( void )
{
  if ( RAD_UNLIKELY ( rrAtomicLoadAcquire32( &init_lock ) != 1 ) )
    GetBases();

  return freq;
}

RADDEFFUNC U64 rrTicksToCycles(U64 ticks)
{
  return ticks;
}

// speedstep will screw this up
RADDEFFUNC F64 rrTicksToSeconds(U64 ticks)
{
  // time and ticks are both deltas from start, so we can use them to tell rate :
  U64 curTime  = rrGetTime();
  U64 curTicks = rrGetTicks();
  
  return ( ( (F64) ticks ) * ( ( (F64) curTime ) / ( (F64) curTicks ) ) ) * inv_freq;
}


RADDEFFUNC U64 rrTimeToMillis(U64 time)
{
  if ( RAD_UNLIKELY ( rrAtomicLoadAcquire32( &init_lock ) != 1 ) )
    GetBases();

  return ( time * 1000 + ((freq/2)-1) ) / freq;
}


RADDEFFUNC U64 rrTimeToMicros(U64 time)
{
  if ( RAD_UNLIKELY ( rrAtomicLoadAcquire32( &init_lock ) != 1 ) )
    GetBases();

  return ( time * 1000000 + ((freq/2)-1) ) / freq;
}


RADDEFFUNC U64 rrMillisToTime(U64 millis)
{
  if ( RAD_UNLIKELY ( rrAtomicLoadAcquire32( &init_lock ) != 1 ) )
    GetBases();

    return ( millis * freq + ((1000/2)-1) ) / 1000;
}


RADDEFFUNC U64 rrMicrosToTime(U64 micros)
{
  if ( RAD_UNLIKELY ( rrAtomicLoadAcquire32( &init_lock ) != 1 ) )
    GetBases();

  return ( micros * freq + ((1000000/2)-1) ) / 1000000;
}
