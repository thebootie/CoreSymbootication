/*
 *  CSSymbolOwnerCache.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 3/4/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSSYMBOLOWNERCACHE_H_
#define _CSSYMBOLOWNERCACHE_H_ 1

CS_EXTERN_C_BEGIN

enum CSSymbolOwnerCacheFlags {
    kCSSymbolOwnerCacheAsyncCacheCleaning		= 0x10 // If this is set, the cache cleans itself with a separate thread.
};

uint32_t CSSymbolOwnerCacheGetFlags(void);
void CSSymbolOwnerCacheSetFlags(uint32_t flags); // Set custom cache behavior

// The memory limit is not a hard limit. This is the target size for the cache.
// The cache will not remove data that is in use. The cache's ultimate growth is bounded
// only by your usage.
void CSSymbolOwnerCacheSetMemoryLimit(size_t mb); // Set the target size for the cache, in MB.
size_t CSSymbolOwnerCacheGetMemoryLimit(void); // Returns the target size for the cache, in MB.

size_t CSSymbolOwnerCacheGetMemoryUsed(void); // Returns the approximate number of MB used by the cache

size_t CSSymbolOwnerCacheGetEntryCount(void); // Returns the number of unique entries in the cache

// Removes all data that it is possible to remove. Because the cache is used to unique symbol owner data, it will not remove
// entries that are currently in use by symbolicator(s).
void CSSymbolOwnerCacheFlush(void);

CS_EXTERN_C_END

#endif /* _CSSYMBOLOWNERCACHE_H_ */
