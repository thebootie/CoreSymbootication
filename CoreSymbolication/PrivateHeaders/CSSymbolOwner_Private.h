/*
 *  CSSymbolOwner_Private.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 8/27/08.
 *  Copyright 2008-2018 Apple Inc. All rights reserved.
 *
 */

#ifndef _CSSYMBOLOWNER_PRIVATE_H_
#define _CSSYMBOLOWNER_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

enum CSSymbolOwnerPrivateFlags {
    kCSSymbolOwnerIsStaticLibraryArchiveEntry		= 0x00004000,
    kCSSymbolOwnerIsRestricted              = 0x00008000,	// Indicates that the symbol owner has a __RESTRICT __restrict segment/section
    /* ... */
    kCSSymbolOwnerUsesCPlusPlus             = 0x00200000,
    kCSSymbolOwnerNeedsResymbolication      = 0x00400000,  // True if _path_for_symbolication has changed since we faulted in the SymbolOwnerData
    kCSSymbolOwnerIsObsolete                = 0x00800000,
    kCSSymbolOwnerIsMutable                 = 0x01000000,
    kCSSymbolOwnerIsSegmentRegions          = 0x02000000,	// [Deprecated] Default is section based regions
    kCSSymbolOwnerIsTextSegmentOnly         = 0x04000000,	// This symbol owner won't have anything but the __TEXT segment, so certain assumptions about number of segments don't hold
    kCSSymbolOwnerDsymVersionBit1           = 0x08000000,
    kCSSymbolOwnerDsymVersionBit2           = 0x10000000,
    kCSSymbolOwnerDsymVersionBit3           = 0x20000000,
    kCSSymbolOwnerDsymVersionBits           = kCSSymbolOwnerDsymVersionBit1 | kCSSymbolOwnerDsymVersionBit2 | kCSSymbolOwnerDsymVersionBit3, // Three bits for dsym version
    kCSSymbolOwnerSuppressDyldLoadNotification	= 0x40000000,	// Do not send a load/unload notification for this symbol owner
    kCSSymbolOwnerIsPrivateKey              = 0x80000000	// Marked in SymbolOwnerCache as only matching a symbol owner with a specific address.
                                                            // Resymbolication uses this with a NULL match address to mark symbol owners as NEVER MATCH
};

// We get three bits. We're just going to raw encode here (avoiding the shifts).
// Can't make these static const's because C errors on use in switch statements then.
enum CSDsymVersions {
    kCSDsymVersion1 = 0,
    kCSDsymVersion2 = kCSSymbolOwnerDsymVersionBit1,
    kCSDsymVersion3 = kCSSymbolOwnerDsymVersionBit2,
    kCSDsymVersion4 = kCSSymbolOwnerDsymVersionBit2 | kCSSymbolOwnerDsymVersionBit1
};

enum CSSymbolOwnerDataFlags {
    kCSSymbolOwnerDataFaultedFromDisk			= 0x001,
    kCSSymbolOwnerDataFaultedFromTask			= 0x002,
    kCSSymbolOwnerDataFaultedFromSelfDyldSharedCache	= 0x004,
    kCSSymbolOwnerDataFaultedFromDiskDyldSharedCache	= 0x008,
    kCSSymbolOwnerDataFoundDsym			= 0x010,
    kCSSymbolOwnerDataFoundBinaryNextToDsym		= 0x020,
    kCSSymbolOwnerDataFoundBinaryViaDsymPlist		= 0x040,
    kCSSymbolOwnerDataEmpty				= 0x080,
    KCSSymbolOwnerDataIsTextSegmentOnly = 0x100,
    kCSSymbolOwnerDataIsObjCRetainReleaseSupported	= 0x400,
    kCSSymbolOwnerDataIsObjCGCSupported		= 0x800,
};

enum CSSymbolOwnerDataTypeID {
    kCSSymbolOwnerDataTypeRaw32			= 0x1,
    kCSSymbolOwnerDataTypeRaw64			= 0x2,
    kCSSymbolOwnerDataTypeMMap32			= 0x3,
    kCSSymbolOwnerDataTypeMMap64			= 0x4
};

uint32_t CSSymbolOwnerGetDsymVersion(CSSymbolOwnerRef);

bool CSSymbolOwnerIsStaticLibraryArchiveEntry(CSSymbolOwnerRef);
bool CSSymbolOwnerIsRestricted(CSSymbolOwnerRef); // Answers true if the symbol owner has a __RESTRICT __restrict segment/section
bool CSSymbolOwnerUsesCPlusPlus(CSSymbolOwnerRef);
bool CSSymbolOwnerIsObsolete(CSSymbolOwnerRef);
bool CSSymbolOwnerIsMutable(CSSymbolOwnerRef);

// Returns the flags for the *data* object of this symbol owner. SPI!
uint32_t CSSymbolOwnerGetDataFlags(CSSymbolOwnerRef);

// Returns the type of data that was faulted for this owner. SPI!
// This is really only useful for implementors to see what data
// type was chosen by the symbol owner cache.
uint32_t CSSymbolOwnerGetDataTypeID(CSSymbolOwnerRef);

// Transient user data
//
// Each symbol owner has a ptr sized "transient" data value.
// You may set or get this value at any time. Setting is NOT thread safe.
// This value is not preserved in signatures. You may set this
// value outside a mutable context.
uintptr_t CSSymbolOwnerGetTransientUserData(CSSymbolOwnerRef);
void CSSymbolOwnerSetTransientUserData(CSSymbolOwnerRef, uintptr_t);

CSAddressSetRef CSAddressSetCreate(void);
void CSAddressSetAdd(CSAddressSetRef set, uint64_t address);
CFDataRef CSSymbolOwnerCreateSparseSignature(CSSymbolOwnerRef symbolOwnerRef, CSAddressSetRef addresses);


// Create a signature containing only this symbol owner.
// The return value is a complete signature, you may invoke CSSymbolicatorCreateWithSignature(...) on it.
// The created symbolicator will have the correct pid and architecture values.
// It will contain only this symbol owner, though.
// The flags argument will have the same behavior as documented in CSSymbolicatorCreateSignature()
CFDataRef CSSymbolOwnerCreateSignature(CSSymbolOwnerRef, uint32_t flags);

CFStringRef CSSymbolOwnerCopyDescriptionWithIndent(CSSymbolOwnerRef, uint32_t);

// This symbolication path is used to point the symbol owner towards a
// symbol-rich binary when there is no binary present at _path.  It is not saved
// in the signature.  It is not necessary to create a mutable context to set the
// symbolication path.
// This function is not thread safe; it should not be called concurrently with
// any other function on the same symbol owner.
void CSSymbolOwnerSetPathForSymbolication(CSSymbolOwnerRef symbolOwnerRef, const char* path);

// If a path is set using CSSymbolOwnerSetPathForSymbolication, this function
// returns that path.  Otherwise, it returns the same path as
// CSSymbolOwnerGetPath.
const char* CSSymbolOwnerGetPathForSymbolication(CSSymbolOwnerRef symbolOwnerRef);

bool CSSymbolOwnerNeedsResymbolication(CSSymbolOwnerRef symbolOwnerRef);

//
// MUTATION OPS... USE AT YOUR OWN RISK!
//
// CoreSymbolication offers strong thread safety. All functions may be used MT unless otherwise documented.
// To maintain this, symbolicators use "transactional atomicity". You will always see a consitent world state,
// though that state may not be current. World state is changed by adding new objects to the symbolicator, not
// by mutation.
//
// To maintain this invariant, you may only modify symbol owners inside a "mutation context", which is provided
// by the symbolicator. Inside a mutation context, you may add to, remove from, or modify members of the symbolicator's
// set of symbol owners.
//
// Mutation contexts are transactional. They either commit and everyone can see the results, or fail, and all
// changes are unwound. The only way to create a mutation context is via the symbolicator method:
//
// CSSymbolicatorApplyMutableContextBlock(CSSymbolicatorRef symbolicator, void (^block)(CSMutableContext context));
//
// To modify a symbol owner inside a mutation context, you must first make the owner mutable in that context.
// Every call to CSSymbolOwnerAddInContext makes a new mutable copy of the symbol owner. You may add symbol owners
// from other symbolicators, or duplicate existing symbol owners. You may remove symbol owners via CSSymbolOwnerRemoveInContext
//
// There is a convenience method for editing existing symbol owners, CSSymbolOwnerMakeMutableInContext().
// This is the same as calling Add to make a copy of the original, and then remove on the original owner.
//
// Some symbol owner attributes have even stricter controls. They require both a mutation context and that the
// symbol owner has not faulted in a symbol owner data. Any access of Region, Symbol, or SourceInfo data will fault
// in a symbol owner data. Some symbol owner level attributes such may also cause a fault.
//
// A concrete example:
//
//	CSSymbolicatorRef symbolicator = ...;
//	CSSymbolOwnerRef appKitOwner = ...;
//	bool success = CSSymbolicatorApplyMutableContextBlock(symbolicator, ^(CSMutableContext context) {
//	    CSSymbolOwnerRef mutableAppKitOwner = CSSymbolOwnerMakeMutableInContext(appKitOwner, context);
//	    CSSymbolOwnerSetPath(mutableAppKitOwner, "/my/new/and/improved/path");
//	    CSSymbolOwnerSetUnloadTimestamp(mutableAppKitOwner, kCSEndOfTime);
//	});
//	if (!success)
//	    reportAbjectFailure();
//
// NOTE! Although it is not disallowed by the API, you should not use retain/release inside the mutable context block.
// It is possible to do very bad things by retaining a context symbol owner which the system expects to delete if
// the commit fails.
//
// NOTE! The dyld load / unload notification mechanism uses a mutation context as part of its implementation.
// This means that while you hold a mutation context, no dyld load or unload notifications can be delivered. Do not block
// inside a mutation context.
//
// NOTE! You cannot deallocate a CSSymbolicatorRef until all mutation contexts have completed. Do not block inside a mutation
// context.
//
// NOTE! CSMutableContext's are NOT thread safe. You would have to really work at it to hand out a mutable context and try to
// access it from multiple threads, and as a reward for all that hard work, FAIL!
//
// FIX ME! If you "make mutable" an existing symbol owner, it "removes" the immutable owner, makes a copy, and adds the mutable
// copy. If you then remove the mutable copy, the immutable owner is still deleted. This needs some thought on recovery. Should
// removing the mutable copy restore the immutable owner?
//
// FIX ME! How do you abort a transaction if you get part way through and decide you want to quit? Should we have the block return
// a boolean? What about a CSMutableContextInvalidate() method? Seems like this might be rare. Workaround is to force invalid state,
// but that is pretty hideous...

typedef void* CSMutationContext;

#pragma mark Accessors

/*
 * Sets the load timestamp.
 *
 * Returns true on success, false on failure.
 *
 * Fails if the owner is not mutable.
 */
bool CSSymbolOwnerSetLoadTimestamp(CSSymbolOwnerRef, CSMachineTime);

/*
 * Sets the unload timestamp.
 *
 * Returns true on success, false on failure.
 *
 * Fails if the owner is not mutable.
 */
bool CSSymbolOwnerSetUnloadTimestamp(CSSymbolOwnerRef, CSMachineTime);

/*
 * Sets the path. This also causes the name to be updated.
 *
 * Returns true on success, false on failure.
 *
 * Fails if the owner is not mutable.
 * Fails if the owner has faulted in a symbol owner data.
 */
bool CSSymbolOwnerSetPath(CSSymbolOwnerRef, const char* path);

/*
 * Sets the number of relocation entries.
 *
 * Returns true on success, false on failure.
 *
 * Fails if the owner is not mutable.
 * Fails if the owner has faulted in a symbol owner data.
 * Fails if the count is less than 1.
 *
 * You may increase or decrease the number of entries. If you increase the entry count, the new entries will have undefined values.
 */
bool CSSymbolOwnerSetRelocationCount(CSSymbolOwnerRef owner, size_t count);

/*
 * Allows editing of the symbol owner relocation entries.
 *
 * Returns true on successful iteration, false on failure. The values are not validated until the mutation context ends.
 *
 * Fails if the owner is not mutable.
 * Fails if the owner has faulted in a symbol owner data.
 *
 * This invokes the block once per relocation entry. The location, length, and name fields point to the current values
 * for that entry. This allows you to do things like offset every relocation by a fixed amount without editing the
 * length or name.
 *
 * NOTE! The name field is NOT a C string! It is not guaranteed to be null terminated. The field is 16 bytes in size.
 */
bool CSSymbolOwnerEditRelocations(CSSymbolOwnerRef owner, void (^block)(mach_vm_address_t* location, mach_vm_size_t* length, char name[]));

#pragma mark Context modification

/*
 * Adds a copy of the symbol owner to the context. The original is not modified.
 *
 * Returns a mutable symbol owner, or kCSNull on failure.
 *
 * Fails if the owner is mutable.
 */

CSSymbolOwnerRef CSSymbolOwnerAddInContext(CSSymbolOwnerRef owner, CSMutationContext context);

/*
 * Removes the symbol owner from the context.
 * Owners that exist only in the current context are IMMEDIATELY deallocated.
 *
 * Returns true on successful removal, false on failure.
 *
 * Fails if the owner is not a member of the context.
 * Fails if you attempt to remove the last symbol owner in the context.
 */
bool CSSymbolOwnerRemoveInContext(CSSymbolOwnerRef owner, CSMutationContext context);

/*
 * Makes an existing symbol owner mutable.
 *
 * Returns a mutable copy of the original on success, kCSNull on failure.
 *
 * Fails if the owner is not a member of the context.
 * Fails if the owner is mutable.
 */
CSSymbolOwnerRef CSSymbolOwnerMakeMutableInContext(CSSymbolOwnerRef owner, CSMutationContext context);

// FOR TESTING ONLY. DO NOT USE.
bool CSSymbolOwnerSegmentsMatch(CSSymbolOwnerRef task_owner, CSSymbolOwnerRef dsc_owner);

CS_EXTERN_C_END

#endif /* _CSSYMBOLOWNER_PRIVATE_H_ */

