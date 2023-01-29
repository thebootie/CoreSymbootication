//
//  CSMMapArchiveCache_Private.h
//  CoreSymbolication
//
//  Created by James McIlree on 3/18/12.
//  Copyright (c) 2012 Apple. All rights reserved.
//

#ifndef _CSMMAP_ARCHIVE_CACHE_PRIVATE_H_
#define _CSMMAP_ARCHIVE_CACHE_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

//
// These methods are pretty much instantly fatal if used incorrectly.
//

//
// Returns a copy of the raw MMapArchive bytes for the given UUID, if
// one exists in the cache. The returned copy has its retain count bumped,
// and will stay "live" until released.
//
void* CSMMapArchiveCacheCopyMMapArchive(const CFUUIDBytes* cfuuid_bytes, size_t* out_size, int16_t* out_major_version, int16_t* out_minor_version);

//
// Release the bytes obtained above. Passing in a bogus or NULL pointer
// will results in "undefined behavior", which is best defined as "bad".
//
void CSMMapArchiveCacheReleaseMMapArchive(void* bytes);

//
// If a write to coresymbolicationd ever fails, this flag is cleared, and
// all future writes are skipped. You can override this behavior if you know
// something about the daemon state that CS doesn't.
//
void CSMMapArchiveCacheSetShouldStoreToDaemon(bool value);

CS_EXTERN_C_END

#endif /* _CSMMAP_ARCHIVE_CACHE_PRIVATE_H_ */
