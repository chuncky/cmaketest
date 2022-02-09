
#include "common.h"

UINT32      FatalErrorHandler   ( void ) {return(0);}
UINT32      SWInterruptHandler  ( void ) {return(0);}
UINT32      UndefInstHandler    ( void ) {return(0);}
UINT32      ReservedIntHandler  ( void ) {return(0);}
UINT32      DataAbortHandler    ( void ) {return(0);}
UINT32      PrefetchAbortHandler( void ) {return(0);}

UINT32        BranchZeroIntHandler  ( void )
{
	return 0;
}

