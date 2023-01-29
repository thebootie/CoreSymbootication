/*
 *  CSSymbolicator.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 12/13/07.
 *  Copyright 2007 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSSYMBOLICATOR_H_
#define _CSSYMBOLICATOR_H_ 1

#include <unistd.h>

CS_EXTERN_C_BEGIN

// To allow building against an earlier OSX SDK without newer availability macros
#ifdef __MAC_10_9
#define _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0 __OSX_AVAILABLE_STARTING(__MAC_10_9, __IPHONE_7_0)
#else
#define _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0
#endif

//
// Creating and destroying symbolicators
//

enum CSSymbolicatorFlags {
    kCSSymbolicatorTrackDyldActivity	= 0x1,  // Track dyld activity in the target process.
    kCSSymbolicatorPrivateData		= 0x2
};

// NOTE! This method increments the ref count on task. When it is finished with
// the task, it decrements it. You are responsible for managing any ref count
// issues in your code. In practice, this means you should mach_port_deallocate()
// anything you create. DO NOT use mach_port_destroy(), as this will yank the port
// out from under the symbolicator, no matter how the ref count is set.
CSSymbolicatorRef CSSymbolicatorCreateWithTask(task_t task);
CSSymbolicatorRef CSSymbolicatorCreateWithPid(pid_t pid); // You must have permission to call task_for_pid() to use this function
CSSymbolicatorRef CSSymbolicatorCreateWithPathAndArchitecture(const char* path, CSArchitecture architecture);
CSSymbolicatorRef CSSymbolicatorCreateWithURLAndArchitecture(CFURLRef url, CSArchitecture architecture);

CSSymbolicatorRef CSSymbolicatorCreateWithMachKernel(void);

// NOTE! symbolicators are valid for the period of iteration, and are destroyed immediately after.
// If you want to use them outside the scope of the block, you MUST retain them!
size_t CSSymbolicatorForeachSymbolicatorWithPath(const char* path, CSSymbolicatorIterator iterator);
size_t CSSymbolicatorForeachSymbolicatorWithURL(CFURLRef url, CSSymbolicatorIterator iterator);

size_t CSSymbolicatorForeachSharedCache(CSSymbolicatorIterator iterator);
	// Iterates shared caches on disk, calling the iterator with a CSSymbolicatorRef once per arch containing the symbol owners for that architecture.

CSSymbolicatorRef CSSymbolicatorCreateWithSignature(CFDataRef signature);

// Create a new symbolicator that contains the union of symbol owners from a signature and an
// existing symbolicator. This function is intended to deserialize signatures created with
// CSSymbolicatorCreateSignatureElidingSomeSymbolOwners().
CSSymbolicatorRef CSSymbolicatorCreateWithSignatureAndAdditionalSymbolOwners(CFDataRef signature, CSSymbolicatorRef additional_symbol_owners);

// Create a symbolicator for the given task's dyld shared cache (using
// CSSymbolicatorForeachSharedCache()) and then relocate its symbol owners so their load addresses
// match the load addresses in task_symbolicator.
// The returned symbolicator has a retain count of 1.
CSSymbolicatorRef CSSymbolicatorCreateForTaskSharedCache(CSSymbolicatorRef task_symbolicator);

//
// Symbolicator accessors
//
CSArchitecture CSSymbolicatorGetArchitecture(CSSymbolicatorRef);
pid_t CSSymbolicatorGetPid(CSSymbolicatorRef);
task_t CSSymbolicatorGetTask(CSSymbolicatorRef);
const CFUUIDBytes *CSSymbolicatorGetSharedCacheUUID(CSSymbolicatorRef);
	// For live processes (or symbolicators created via CSSymbolicatorForeachSharedCache), returns the global uuid for the dyld shared cache in use.
	// Note: on systems where this isn't available or for processes without a shared cache in use, this will return NULL.

// The signature will contain all data concerning library loads and unloads that have occurred until now.
// Future load / unload events will not be present. The flags field should either be 0, or kCSSignatureDeepCopy
CFDataRef CSSymbolicatorCreateSignature(CSSymbolicatorRef, uint32_t flags);

// Same as CSSymbolicatorCreateSignature, except that all symbol owners that exactly match a symbol
// owner in symbolOwnersToElideRef get filtered out of the resulting signature.
// Clients can use this function to create a signature for a task that doesn't include any of the
// symbol owners from the dyld shared cache. Pass the shared cache symbolicator (created with
// CSSymbolicatorCreateForTaskSharedCache()) as the symbolOwnersToElideRef parameter to filter out
// shared cache symbol owners.
// Eliding shared cache symbol owners is useful when storing signatures for many processes that use
// the same dyld shared cache. Rather than storing the same symbol owner information in every
// signature, the client can store it in one place and filter it out of the individual task
// signatures.
// To deserialize a signature created this way, use
// CSSymbolicatorCreateWithSignatureAndAdditionalSymbolOwners(), passing a symbolicator with the
// filtered-out symbol owners as the "additional_symbol_owners" parameter.
//
// When sharcedCacheRef is kCSNull, this function has identical behavior to
// CSSymbolicatorCreateSignature().
CFDataRef CSSymbolicatorCreateSignatureElidingSomeSymbolOwners(CSSymbolicatorRef symbolicatorRef, uint32_t flags, CSSymbolicatorRef symbolOwnersToElideRef);

bool CSSymbolicatorIsKernelSymbolicator(CSSymbolicatorRef); // Returns true if symbolicator was created from the kernel
bool CSSymbolicatorIsTaskTranslated(CSSymbolicatorRef) __attribute__((deprecated));
bool CSSymbolicatorIsTaskValid(CSSymbolicatorRef); // Returns true if symbolicator has a task_t for the target process, and it is still valid (live?)

//
// New API notes... (HOW TO GET COUNTS)
//
// In addition to the various GetCount methods, all "Foreach" style functions return the number of times their iterator
// was invoked. It is legal (and special case optimized) for you to pass a NULL iterator. So, to get a count of SymbolOwners:
//
// * size_t numberOfSymbolOwners = CSSymbolicatorForeachSymbolOwnerAtTime(symbolicator, NULL, kCSNow);
//
// NOTE! Methods that must do compares in order to determine a count will still have the compare overhead, even with a NULL iterator.
//
// I.E., this is still a lot of string compare overhead:
//
// * size_t numberOfSymbolsNamedFoo = CSSymbolicatorForeachSymbolWithNameAtTime(symbolicator, "Foo", kCSNow, NULL);
//

//
// New API notes... (TIME)
//
// Symbolicators track the time a library was loaded and unloaded.
// For any given moment in time, there are a set of active libraries (symbol owners).
// To make a querry against a symbolicator, you must provide a "when", so the symbolicator knows what set of active libraries to use.
// Time is based on mach_absolute_time() on the current machine.
// If the symbolicator did not observe the library being loaded, it will have a load time of kCSBeginningOfTime
// If the symbolicator did not observe the library being unloaded, it will have an unload time of kCSEndOfTime
//
// There are some special time constants:
//
// kCSBeginningOfTime - This is the youngest possible timestamp.
// kCSEndOfTime - This is the oldest possible timestamp.
// kCSNow - Symbolic constant for "now"
// kCSAllTimes -  This is a special case, meaning consider all available data, ignoring time constraints
//
// EXAMPLES
//
// Iterate the list of all symbol owners loaded at a specific time:
//
// * uint64_t aRandomTime = mach_absolute_time();
// * CSSymbolicatorForeachSymbolOwnerAtTime(symbolicator, aRandomTime, ^(CSSymbolOwnerRef owner) { /* ... */ });
//
// Get the count of currently loaded libraries
//
// * size_t libraryCount = CSSymbolicatorForeachSymbolOwnerAtTime(symbolicator, kCSNow, NULL);
//
// Iterate over every symbol owner named "libFoo.dylib", ignoring when it was loaded/unloaded.
//
// * CSSymbolicatorForeachSymbolOwnerWithNameAtTime(symbolicator, "libFoo.dylib", kCSAllTimes, ^(CSSymbolOwnerRef owner) { /* ... */ });
//
// NOTE! kCSAllTimes is special. It is not a valid input for some functions.
//       Functions that can return only a single value, but might match many
//       values if "all" times are considered will return kCSNull instead of a random answer.
//       These functions are individually called out in the documentation.

//
// SymbolOwners...
//
size_t CSSymbolicatorGetSymbolOwnerCountAtTime(CSSymbolicatorRef, CSMachineTime);

size_t CSSymbolicatorForeachSymbolOwnerAtTime(CSSymbolicatorRef, CSMachineTime, CSSymbolOwnerIterator); // The set of symbol owners loaded at the given time.
size_t CSSymbolicatorForeachSymbolOwnerWithNameAtTime(CSSymbolicatorRef, const char*, CSMachineTime, CSSymbolOwnerIterator);\
size_t CSSymbolicatorForeachSymbolOwnerWithFlagsAtTime(CSSymbolicatorRef, uint32_t flags, CSMachineTime, CSSymbolOwnerIterator);
size_t CSSymbolicatorForeachSymbolOwnerWithPathAtTime(CSSymbolicatorRef, const char*, CSMachineTime, CSSymbolOwnerIterator);
size_t CSSymbolicatorForeachSymbolOwnerWithUUIDAtTime(CSSymbolicatorRef, CFUUIDRef, CSMachineTime, CSSymbolOwnerIterator);
size_t CSSymbolicatorForeachSymbolOwnerWithCFUUIDBytesAtTime(CSSymbolicatorRef, CFUUIDBytes*, CSMachineTime, CSSymbolOwnerIterator);

// Returns kCSNull if CSMachineTime is kCSAllTimes
CSSymbolOwnerRef CSSymbolicatorGetSymbolOwnerWithAddressAtTime(CSSymbolicatorRef, mach_vm_address_t, CSMachineTime);

// Convenience methods. These are wrappers around their Foreach counterparts.
// They return a symbol owner if there is one-and-only-one match. Any other number
// of matches (0, 2+) returns kCSNull.
CSSymbolOwnerRef CSSymbolicatorGetSymbolOwnerWithNameAtTime(CSSymbolicatorRef, const char*, CSMachineTime);
CSSymbolOwnerRef CSSymbolicatorGetSymbolOwnerWithUUIDAtTime(CSSymbolicatorRef, CFUUIDRef, CSMachineTime);
CSSymbolOwnerRef CSSymbolicatorGetSymbolOwnerWithCFUUIDBytesAtTime(CSSymbolicatorRef, CFUUIDBytes*, CSMachineTime);
// NOTE: Time is implied here. An executable cannot load or unload additional aout flagged symbol owners. Legal states are one or none, throughout all possible times.
CSSymbolOwnerRef CSSymbolicatorGetAOutSymbolOwner(CSSymbolicatorRef);
// Returns the symbol owner if the symbolicator has one and only one.
CSSymbolOwnerRef CSSymbolicatorGetSymbolOwner(CSSymbolicatorRef);

//
// Segments/Sections...
// Both segments and sections are types of CSRegion references; a 'region' is a reference-counted
// object and represents an address range, has a name, and has a single parent CSSymbolOwner and
// CSSymbolicator.
//

size_t CSSymbolicatorForeachSegmentAtTime(CSSymbolicatorRef, CSMachineTime, CSSegmentIterator) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
size_t CSSymbolicatorForeachSectionAtTime(CSSymbolicatorRef, CSMachineTime, CSSectionIterator) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
CSSegmentRef CSSymbolicatorGetSegmentWithAddressAtTime(CSSymbolicatorRef, mach_vm_address_t, CSMachineTime) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
CSSectionRef CSSymbolicatorGetSectionWithAddressAtTime(CSSymbolicatorRef, mach_vm_address_t, CSMachineTime) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;

//
// Symbols...
//
size_t CSSymbolicatorGetSymbolCountAtTime(CSSymbolicatorRef, CSMachineTime);

size_t CSSymbolicatorForeachSymbolAtTime(CSSymbolicatorRef, CSMachineTime, CSSymbolIterator);
size_t CSSymbolicatorForeachSymbolWithNameAtTime(CSSymbolicatorRef, const char*, CSMachineTime, CSSymbolIterator);
size_t CSSymbolicatorForeachSymbolWithMangledNameAtTime(CSSymbolicatorRef, const char*, CSMachineTime, CSSymbolIterator);

// Returns kCSNull if CSMachineTime is kCSAllTimes
CSSymbolRef CSSymbolicatorGetSymbolWithAddressAtTime(CSSymbolicatorRef, mach_vm_address_t, CSMachineTime);

// Convenience methods. They cost the same as using the "Foreach" variants.
//
// This will return a symbol if there is one-and-only-one match.
// Any other number of matches (0, 2+) returns kCSNull
CSSymbolRef CSSymbolicatorGetSymbolWithNameAtTime(CSSymbolicatorRef, const char* symbolName, CSMachineTime);
CSSymbolRef CSSymbolicatorGetSymbolWithMangledNameAtTime(CSSymbolicatorRef, const char* symbolName, CSMachineTime);
//
// NOTE! It is MUCH CHEAPER to find a symbol in a single symbol owner instead of searching the entire symbolicator.
// If you are searching for "malloc" in "libSystem.B.dylib", find the symbol owner first, then search it for "malloc".
// If you search the entire symbolicator, *every* library must be faulted in.
CSSymbolRef CSSymbolicatorGetSymbolWithNameFromSymbolOwnerWithNameAtTime(CSSymbolicatorRef, const char* symbolName, const char* symbolOwnerName, CSMachineTime);
CSSymbolRef CSSymbolicatorGetSymbolWithMangledNameFromSymbolOwnerWithNameAtTime(CSSymbolicatorRef, const char* mangledSymbolName, const char* symbolOwnerName, CSMachineTime);

//
// Source Infos...
//
size_t CSSymbolicatorGetSourceInfoCountAtTime(CSSymbolicatorRef, CSMachineTime);

size_t CSSymbolicatorForeachSourceInfoAtTime(CSSymbolicatorRef, CSMachineTime, CSSourceInfoIterator);

// Returns kCSNull if CSMachineTime is kCSAllTimes
CSSourceInfoRef CSSymbolicatorGetSourceInfoWithAddressAtTime(CSSymbolicatorRef, mach_vm_address_t, CSMachineTime);


//
// Regions...
// [Deprecated! See "Segments/Sections" above]
// The duality of sometimes meaning 'section', sometimes meaning 'segment' was horrible and confusing.
// These deprecated API now default to the behavior of using sections, unless the symbolicator was
// created with the also-deprecated and private 'segments' flag instead, in which case the use segments.
//
size_t CSSymbolicatorGetRegionCountAtTime(CSSymbolicatorRef, CSMachineTime); // Informally deprecated
size_t CSSymbolicatorForeachRegionAtTime(CSSymbolicatorRef, CSMachineTime, CSRegionIterator); // Informally deprecated
size_t CSSymbolicatorForeachRegionWithNameAtTime(CSSymbolicatorRef, const char*, CSMachineTime, CSRegionIterator); // Informally deprecated
CSRegionRef CSSymbolicatorGetRegionWithAddressAtTime(CSSymbolicatorRef, mach_vm_address_t, CSMachineTime); // Informally deprecated
// Returns kCSNull if CSMachineTime is kCSAllTimes
CSRegionRef CSSymbolicatorGetRegionWithNameAtTime(CSSymbolicatorRef, const char*, CSMachineTime); // Informally deprecated
// Convenience method. Returns a region if there is one-and-only-one match. Any other number
// of matches (0, 2+) returns kCSNull.

CS_EXTERN_C_END

#endif /* _CSSYMBOLICATOR_H_ */
