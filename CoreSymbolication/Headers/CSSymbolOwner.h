/*
 *  CSSymbolOwner.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 11/15/07.
 *  Copyright 2007 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSSYMBOLOWNER_H_
#define _CSSYMBOLOWNER_H_ 1

CS_EXTERN_C_BEGIN

// To allow building against an earlier OSX SDK without newer availability macros
#ifdef __MAC_10_9
#define _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0 __OSX_AVAILABLE_STARTING(__MAC_10_9, __IPHONE_7_0)
#else
#define _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0
#endif

enum CSSymbolOwnerFlags {
    kCSSymbolOwnerIsMachO				= 0x1,
    kCSSymbolOwnerIsProtected				= 0x2,
    /* */
    kCSSymbolOwnerIsCommpage				= 0x8, // deprecated
    kCSSymbolOwnerIsAOut				= 0x10,
    kCSSymbolOwnerIsDylib				= 0x20,
    kCSSymbolOwnerIsDylibStub				= 0x4, // Note out of order!
    kCSSymbolOwnerIsDyld				= 0x40,
    kCSSymbolOwnerIsBundle				= 0x80,
    kCSSymbolOwnerIsDsym				= 0x100,
    kCSSymbolOwnerIsDyldSharedCache			= 0x200,
    kCSSymbolOwnerIsSlid				= 0x400,
    kCSSymbolOwnerIsObject				= 0x800,
    kCSSymbolOwnerIsKextBundle				= 0x1000,
    kCSSymbolOwnerIsPIE				= 0x2000,
};

CSArchitecture CSSymbolOwnerGetArchitecture(CSSymbolOwnerRef);
const char* CSSymbolOwnerGetPath(CSSymbolOwnerRef);
const char* CSSymbolOwnerGetName(CSSymbolOwnerRef);
const char* CSSymbolOwnerGetDsymPath(CSSymbolOwnerRef); // May be NULL
// In 10.6, the GetUUID function will never return NULL.
// In 10.7, GetUUID will return NULL if the symbol owner does not have a UUID.
// This will be deprecated very soon now, you should use CSSymbolOwnerGetCFUUIDBytes instead.
const uint8_t* CSSymbolOwnerGetUUID(CSSymbolOwnerRef) __attribute__((deprecated));
const CFUUIDBytes* CSSymbolOwnerGetCFUUIDBytes(CSSymbolOwnerRef); // Returns NULL if there is no UUID.
uint32_t CSSymbolOwnerGetLastModifiedTimestamp(CSSymbolOwnerRef); // This is struct stat.st.mtimespec.tv_sec (last modified time, in seconds).
CSMachineTime CSSymbolOwnerGetLoadTimestamp(CSSymbolOwnerRef);
CSMachineTime CSSymbolOwnerGetUnloadTimestamp(CSSymbolOwnerRef);
uint32_t CSSymbolOwnerGetFlags(CSSymbolOwnerRef);
// This will return the address of the beginning of the mach header.
// Usually this is the lowest possible address, but in some cases
// (such as the dyld shared cache), it will not be.
mach_vm_address_t CSSymbolOwnerGetBaseAddress(CSSymbolOwnerRef);

// Returns the version of the symbol owner if it can be determined.
// Returns NULL if no version information is found.
//
// The version is contained in the LC_SOURCE_VERSION / LC_ID_DYLIB
// load commands.
const char* CSSymbolOwnerGetVersion(CSSymbolOwnerRef);

// NOTE! Deprecated, and soon to be removed. The values do not mean what most people thought they did. Use CSSymbolOwnerGetVersion.
//
// If the target symbol owner contains a LC_ID_DYLIB command, these functions will return its current and compat version values
uint32_t CSSymbolOwnerGetCurrentVersion(CSSymbolOwnerRef) __attribute__((deprecated));
uint32_t CSSymbolOwnerGetCompatibilityVersion(CSSymbolOwnerRef) __attribute__((deprecated));

bool CSSymbolOwnerIsMachO(CSSymbolOwnerRef);
bool CSSymbolOwnerIsProtected(CSSymbolOwnerRef);
bool CSSymbolOwnerIsCommpage(CSSymbolOwnerRef) __attribute__((deprecated));
bool CSSymbolOwnerIsAOut(CSSymbolOwnerRef);
bool CSSymbolOwnerIsDylib(CSSymbolOwnerRef);
bool CSSymbolOwnerIsDylibStub(CSSymbolOwnerRef);
bool CSSymbolOwnerIsDyld(CSSymbolOwnerRef);
bool CSSymbolOwnerIsBundle(CSSymbolOwnerRef);
bool CSSymbolOwnerIsObject(CSSymbolOwnerRef);
bool CSSymbolOwnerIsKextBundle(CSSymbolOwnerRef);
bool CSSymbolOwnerIsDsym(CSSymbolOwnerRef);
bool CSSymbolOwnerIsDyldSharedCache(CSSymbolOwnerRef );
bool CSSymbolOwnerIsSlid(CSSymbolOwnerRef); // Answers true if the symbol owner was slid by dyld.
bool CSSymbolOwnerIsPIE(CSSymbolOwnerRef); // Answers true if the symbol owner has the PIE (Position Independent Executable) flag set

// The following predicates require deeper inspection and will cause the symbol owner to fault in its data.
bool CSSymbolOwnerIsObjCRetainReleaseSupported(CSSymbolOwnerRef); // Answers true if there is an __OBCC segment, and it is marked as supporting RR
bool CSSymbolOwnerIsObjCGCSupported(CSSymbolOwnerRef); // Answers true if there is an __OBJC segment, and it is marked as either supporting or requiring GC

//
// Symbolicator
//
CSSymbolicatorRef CSSymbolOwnerGetSymbolicator(CSSymbolOwnerRef);

//
// Segments/Sections...
//
size_t CSSymbolOwnerForeachSegment(CSSymbolOwnerRef, CSSegmentIterator) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
size_t CSSymbolOwnerForeachSection(CSSymbolOwnerRef, CSSectionIterator) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
CSSegmentRef CSSymbolOwnerGetSegmentWithAddress(CSSymbolOwnerRef, mach_vm_address_t) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
CSSectionRef CSSymbolOwnerGetSectionWithAddress(CSSymbolOwnerRef, mach_vm_address_t) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
CSSectionRef CSSymbolOwnerGetSectionWithName(CSSymbolOwnerRef, const char*) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
	// Convenience; returns a CSSectionRef if there is one and only one section matching the specified name.

//
// Symbols...
//
size_t CSSymbolOwnerGetSymbolCount(CSSymbolOwnerRef);

size_t CSSymbolOwnerForeachSymbol(CSSymbolOwnerRef, CSSymbolIterator);
size_t CSSymbolOwnerForeachSymbolWithName(CSSymbolOwnerRef, const char*, CSSymbolIterator);
size_t CSSymbolOwnerForeachSymbolWithMangledName(CSSymbolOwnerRef, const char*, CSSymbolIterator);

CSSymbolRef CSSymbolOwnerGetSymbolWithAddress(CSSymbolOwnerRef, mach_vm_address_t);

// Convenience methods. They cost the same as using the "Foreach" variants.
//
// This will return a symbol if there is one-and-only-one match.
// Any other number of matches (0, 2+) returns kCSNull
CSSymbolRef CSSymbolOwnerGetSymbolWithName(CSSymbolOwnerRef symbolOwnerRef, const char* name);
CSSymbolRef CSSymbolOwnerGetSymbolWithMangledName(CSSymbolOwnerRef symbolOwnerRef, const char* mangledName);

//
// Source Infos...
//
size_t CSSymbolOwnerGetSourceInfoCount(CSSymbolOwnerRef);

size_t CSSymbolOwnerForeachSourceInfo(CSSymbolOwnerRef, CSSourceInfoIterator);

CSSourceInfoRef CSSymbolOwnerGetSourceInfoWithAddress(CSSymbolOwnerRef, mach_vm_address_t);

size_t CSSymbolOwnerForEachStackFrameAtAddress(CSSymbolOwnerRef symbolOwnerRef, mach_vm_address_t address, CSStackFrameIterator iterator);

// [Deprecated! Regions]
size_t CSSymbolOwnerGetRegionCount(CSSymbolOwnerRef); // Informally deprecated
size_t CSSymbolOwnerForeachRegion(CSSymbolOwnerRef, CSRegionIterator); // Informally deprecated
size_t CSSymbolOwnerForeachRegionWithName(CSSymbolOwnerRef, const char*, CSRegionIterator); // Informally deprecated
CSRegionRef CSSymbolOwnerGetRegionWithAddress(CSSymbolOwnerRef, mach_vm_address_t); // Informally deprecated
CSRegionRef CSSymbolOwnerGetRegionWithName(CSSymbolOwnerRef, const char*); // Informally deprecated

CS_EXTERN_C_END 

#endif /* _CSSYMBOLOWNER_H_ */
