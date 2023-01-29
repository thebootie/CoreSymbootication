/*
 *  CSSymbolOwnerCache_Private.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 9/14/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSSYMBOLOWNERCACHE_PRIVATE_H_
#define _CSSYMBOLOWNERCACHE_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

// Reset the internal stat counters to zero
void CSSymbolOwnerCacheResetStats(void);

// Prints internal cache stats to stdout
void CSSymbolOwnerCachePrintStats(void);

// Dumps all entries
void CSSymbolOwnerCachePrintEntries(void);

CS_EXTERN_C_END

#endif /* _CSSYMBOLOWNERCACHE_PRIVATE_H_ */


