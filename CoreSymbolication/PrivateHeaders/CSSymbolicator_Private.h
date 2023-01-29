/*
 *  CSSymbolication_Private.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 8/24/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */


#ifndef _CSSYMBOLICATOR_PRIVATE_H_
#define _CSSYMBOLICATOR_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

enum CSSymbolicatorPrivateFlags {
    kCSSymbolicatorAlwaysAllowMMapLookup				= 0x00000040,  // Read an MMap SOD from the cache (but do not write one) even when kCSSymbolicatorDisallowMMapSymbolOwnerData or kCSSymbolicatorPrivateData would prevent it
    kCSSymbolicatorDoNotCaptureSourceInfo				= 0x00000080,
    kCSSymbolicatorDoNotCaptureInstructions				= 0x00000100,  // Do not inlcude the TEXT text data in symbol owner data
    kCSSymbolicatorDisallowTRawSymbolOwnerData				= 0x00000200,  // Do not allow TRaw style symbol owner data
    kCSSymbolicatorDisallowMMapSymbolOwnerData				= 0x00000400,  // Do not allow MMap style symbol owner data
    kCSSymbolicatorDisallowDaemonCommunication				= 0x00000800,  // Do not attempt reads or writes to the storage daemon for this symbolicator.
    kCSSymbolicatorSafeMachVMReads					= 0x00001000,  // EXPENSIVE! Catch errors when accessing remote memory that no longer has a backing store
    kCSSymbolicatorInitialized						= 0x00002000,  // This symbolicator is fully initialized (gates notifications)
    kCSSymbolicatorDestructorInProgress					= 0x00004000,  // This symbolicator is in the process of being deallocated.
    kCSSymbolicatorTimelineUpdateInProgress				= 0x00008000,  // This symbolicator is in the process of having a timeline update.
    kCSSymbolicatorDisallowNListData					= 0x00010000,  // Do not use nlist data when creating symbols & source infos
    kCSSymbolicatorDisallowDwarfData					= 0x00020000,  // Do not use dwarf data when creating symbols & source infos
    kCSSymbolicatorDisallowDebugMapData					= 0x00040000,  // Do not use debug map data when creating symbols & source infos
    kCSSymbolicatorDisallowDsymData					= 0x00080000,  // Do not use dSYM data when creating symbols & source infos
    kCSSymbolicatorDisallowFaultingFromDisk				= 0x00100000,  // Do not use disk based macho headers when creating symbol owner data
    kCSSymbolicatorDisallowFaultingFromTask				= 0x00200000,  // Do not use task based macho headers when creating symbol owner data
    kCSSymbolicatorDisallowFaultingFromSelfDyldSharedCache		= 0x00400000,  // Do not use self dyld-shared-cache macho headers when creating symbol owner data
    kCSSymbolicatorIsSegmentRegions						= 0x00800000,  // [Deprecated] Force CSRegion data to be generated from segments, instead of sections. Deprecated; use 'segment'-based calls instead of 'region' ones on normal symbolicators, e.g. CSSymbolOwnerForeachSegment
    kCSSymbolicatorPrintMergedSymbols					= 0x01000000,  // Print all versions of symbols about to be merged.
    kCSSymbolicatorDisallowSourceInfoPathRewriting			= 0x02000000,  // Do not use dsym Info.plist entries to update source info paths
    kCSSymbolicatorDisallowFunctionStartsData				= 0x04000000,  // Do not use the LC_FUNCTION_STARTS load command data when creating symbols
    kCSSymbolicatorDisallowDsymColocatedBinaryLocation			= 0x08000000,  // TEMP! Do not attempt to find a binary next to the dSYM
    kCSSymbolicatorDisallowDsymPlistBinaryLocation			= 0x10000000,  // TEMP! Do not attempt to find a binary using the dSYM's plist info
    kCSSymbolicatorDisallowFaultingFromDiskDyldSharedCache		= 0x20000000,  // Do not use the disk based dyld shared cache when creating symbol owner data
    kCSSymbolicatorUseSlidKernelAddresses				= 0x80000000,  // Use actual (slid) kernel addresses when creating a kernel symbolicator
};

// recomended set of flags to respect when loading a signature
static const uint32_t kCSSymbolicatorDefaultWhitelist = kCSSymbolicatorInitialized | kCSSymbolicatorUseSlidKernelAddresses | kCSSymbolicatorPrivateData;

// NOTE!
//
// The kCSSymbolicatorIsSegmentRegions is broken in several ways. If you attempt to use it against a 3.0 iPhone SDK simulator process, it will outright fail.
// This is because those apps have overlapping segments. The LINKEDIT segment from the dyld shared cache cannot be looked up by address, because all libraries
// in the dyld shared cache share a LINKEDIT segment. We must exclude it from the fast lookup cache, and so you now have regions that cannot be found by address.
// Signatures will not work, as the LINKEDIT segment is automatically stripped from signature relocation entries, to allow older archived signatures to be read
// in.

// NOTE! DEPRECATED!
//
// Clients should instead use the function call variants, which can update at the same time as CoreSymbolication.
static const uint32_t kCSSymbolicatorAllowOnlyStabsData = kCSSymbolicatorDisallowDwarfData | kCSSymbolicatorDisallowDebugMapData | kCSSymbolicatorDisallowDsymData | kCSSymbolicatorDisallowFunctionStartsData;

uint32_t CSSymbolicatorGetFlagsForNListOnlyData(void);
uint32_t CSSymbolicatorGetFlagsForDwarfOnlyData(void);
uint32_t CSSymbolicatorGetFlagsForDebugMapOnlyData(void);
uint32_t CSSymbolicatorGetFlagsForDsymOnlyData(void);
uint32_t CSSymbolicatorGetFlagsForFunctionStartsOnlyData(void);
uint32_t CSSymbolicatorGetFlagsForNoSymbolOrSourceInfoData(void);
uint32_t CSSymbolicatorGetFlagsForNoDataExceptFromTheCache(void);

// Notifications
//
// You may register with a symbolicator to receive notices of certain events. (Currently must be done at creation!)
//
// Notification types are stand alone, each notification is sent separately.
//
// Notifications are single threaded. You will receive only one notice at a time, although the thread the notice is sent from may change.
//
// Notification blocks are Block_copy'd on registration, and released when unregistered.
//
// Notification Types:
//
// Ping		Sent on a ping from the target process. This notice is synchronous. This must be generated
//		by an outside source, CoreSymbolication will never create this event on its own. Currently,
//		ping is used to simulate breakpoints in dtrace, and for handshaking during order file generation.
//
// Initialized	You are guaranteed that this will be the first notification you receive. You will receive this
//		notification before the call to CSSymbolicatorCreateXXX() returns. When you receive this notice,
//		the symbolicator will be fully initialized, with a valid list of current symbol owners in the
//		target. NOTE! At the time you receive Initialized, some library load/unloads may have already occurred.
//		You will not receive notices for those events. This is an inherent race condition.
//
// DyldLoad	Sent on any dyld action that loads a dylib, framework, or bundle. No code will have executed in
//		the target at the time this notification is sent. This notice is synchronous. The target of the
// 		symbolicator cannot complete loading until all notifications have been processed. All notification
//		callbacks must complete in less than XXX seconds. (XXX seconds for all callbacks, not XXX seconds
//		per callback). Failure to complete in that time will result in the target dropping its connection.
//		Adding a symbol ownver via private functions does not generate a DyldLoad notification.
//
// DyldUnload	Sent on any dyld action that unloads a dylib, framework, or bundle. This notice is synchronous. The
//		target of the symbolicator cannot complete loading until all notifications have been processed. All
//		notification callbacks must complete in less than XXX seconds. (XXX seconds for all callbacks, not
//		XXX seconds per callback). Failure to complete in that time will result in the target dropping its
//		connection. Removing a SymbolOwner via private functions does not generate a DyldUnload notification.
//
// TaskMain Sent synchronously just before main() is executed. This is an opt-in notification: a symbolicator will
//      only recive it if there is a call to CSSymbolicatorSubscribeToTaskMainNotification(). If main has already
//      begun execution before tracking begins (or before there is a call to
//      CSSymbolicatorSubscribeToTaskMainNotification()), then this notification will not be sent.
//
// TaskExit	Sent on detection of the target task exiting. If the communication channel between the target and the
//		symbolicator is closed (due to error, timeout, or other failure mode), you will receive a Fini, but not
//		a TaskExit.
//
// Timeout	[UNRELIABLE!] If the target decides to timeout this connection, it will make one attempt to send a
//		a timeout notice. The target will not block on this send, nor retry on error.
//
// Fini		You are guaranteed that every registered notification block will be sent a Fini notification as the
//		last notification. You are guaranteed that after receiving a Fini notification, you will receive no
//		further notifications. The Fini notification is intended for use as a final clean up / deallocation
//		checkpoint. If you unregister a notification block, it will receive a Fini before the unregister call
//		returns. If the target task exits, you will be sent TaskExit, then Fini. If you free a symbolicator,
//		all registered notification blocks are unregistered (and thus sent Fini) before the free completes.
//		At the time the Fini is sent, all symbolicator data is still valid.
//

enum CSNotificationType {
    kCSNotificationPing			= 0x1,
    kCSNotificationInitialized		= 0x10,
    kCSNotificationDyldLoad		= 0x100,
    kCSNotificationDyldUnload		= 0x101,
    kCSNotificationTaskExit		= 0x1000,
    kCSNotificationTimeout		= 0x1001,
    kCSNotificationTaskMain		= 0x1002,
    kCSNotificationFini			= 0x80000000
};

typedef struct _CSNotificationData {
    CSSymbolicatorRef symbolicator;
    union {
        struct Ping {
            uint32_t value;
        } ping;

        struct DyldLoad {
            CSSymbolOwnerRef symbolOwner;
        } dyldLoad;

        struct DyldUnload {
            CSSymbolOwnerRef symbolOwner;
        } dyldUnload;
    } u;
} CSNotificationData;

typedef void (^CSNotificationBlock)(uint32_t type, CSNotificationData data);
typedef uintptr_t CSNotificationID;

typedef struct _CSBinaryRelocationInformation {
    mach_vm_address_t base;
    mach_vm_address_t extent;
    char name[17];
} CSBinaryRelocationInformation;

typedef struct _CSBinaryImageInformation {
    mach_vm_address_t base;
    mach_vm_address_t extent;
    CFUUIDBytes uuid;
    CSArchitecture arch;
    const char* path;
    CSBinaryRelocationInformation* relocations;
    uint32_t relocationCount;
    uint32_t flags;
} CSBinaryImageInformation;

// tentative api...
//
// CSNotificationID CSSymbolicatorRegisterForNotifications(CSSymbolicatorRef symbolicator, CSNotificationBlock block);
// void CSSymbolicatorUnregisterForNotifications(CSSymbolicatorRef symbolicator, CSNotificationID which);

static const uint32_t kCSSymbolicatorDefaultCreateFlags = 0;

CSSymbolicatorRef CSSymbolicatorCreateWithBinaryImageList(CSBinaryImageInformation* binary_image_information, uint32_t count, uint32_t flags, CSNotificationBlock notification_block);
CSSymbolicatorRef CSSymbolicatorCreateWithBinaryImageListCountPidFlagsAndNotification(CSBinaryImageInformation* binary_image_information, uint32_t count, pid_t pid, uint32_t flags, CSNotificationBlock notification_block);
CSSymbolicatorRef CSSymbolicatorCreateWithTaskPidFlagsAndNotification(task_t task, pid_t pid, uint32_t flags, CSNotificationBlock block);
CSSymbolicatorRef CSSymbolicatorCreateWithTaskFlagsAndNotification(task_t task, uint32_t flags, CSNotificationBlock block);
CSSymbolicatorRef CSSymbolicatorCreateWithPidFlagsAndNotification(pid_t pid, uint32_t flags, CSNotificationBlock block); // You must have permission to call task_for_pid() to use this method
CSSymbolicatorRef CSSymbolicatorCreateWithPathArchitectureFlagsAndNotification(const char* path, CSArchitecture architecture, uint32_t flags, CSNotificationBlock block);
CSSymbolicatorRef CSSymbolicatorCreateWithURLArchitectureFlagsAndNotification(CFURLRef url, CSArchitecture architecture, uint32_t flags, CSNotificationBlock block);
size_t CSSymbolicatorForeachSymbolicatorWithPathFlagsAndNotification(const char* path, uint32_t flags, CSNotificationBlock block, CSSymbolicatorIterator iterator);
size_t CSSymbolicatorForeachSymbolicatorWithURLFlagsAndNotification(CFURLRef url, uint32_t flags, CSNotificationBlock block, CSSymbolicatorIterator iterator);
size_t CSSymbolicatorForeachSharedCacheSymbolicatorWithFlagsAndNotification(uint32_t flags, CSNotificationBlock notification_block, CSSymbolicatorIterator iterator);
CSSymbolicatorRef CSSymbolicatorCreateWithMachKernelFlagsAndNotification(uint32_t flags, CSNotificationBlock notification_block);
CSSymbolicatorRef CSSymbolicatorCreateWithSignatureAndNotification(CFDataRef signature, CSNotificationBlock notification_block);

// flags_whitelist is flags to accept from the signature.  Recomended whitelist is kCSSymbolicatorDefaultWhitelist
// extra_flags is flags to add
CSSymbolicatorRef CSSymbolicatorCreateWithSignatureFlagsAndNotification(CFDataRef signature, uint32_t flags_whitelist, uint32_t extra_flags, CSNotificationBlock notification_block);
CSSymbolicatorRef CSSymbolicatorCreateWithSignatureAdditionalSymbolOwnersFlagsAndNotification(CFDataRef signature, uint32_t flags_whitelist, uint32_t extra_flags, CSSymbolicatorRef additional_symbol_owners, CSNotificationBlock notification_block);
CSSymbolicatorRef CSSymbolicatorCreateForTaskSharedCacheWithFlagsAndNotification(CSSymbolicatorRef task_symbolicator, uint32_t flags, CSNotificationBlock notification_block);

void CSSymbolicatorSubscribeToTaskMainNotification(CSSymbolicatorRef symbolicatorRef);

// Forces some (but possibly not all) initialization.
void CSSymbolicatorForceFullSymbolExtraction(CSSymbolicatorRef symbolicator);

/*
 * This method attempts to find additional information for all symbol
 * owners in the symbolicator. Newly found dSYMs will be immediately
 * processed, this method may have significant time & memory costs.
 *
 * NOTE! This method is destructive! CSRegion, CSSymbol, and CSSourceInfo's
 * from the symbolicator may have their underlying data freed. It is the
 * callers responsibility to cease use of all data prior to calling this
 * method. NOT MT SAFE!
 *
 * NOTE! The data created by this method is persistent and global. The
 * symbol owner cache will replace older data with newly found dSYM data,
 * and any future lookups will use the newer dSYM data.
 *
 * NOTE! This method ignores the kCSSymbolicatorDisallowDsymData flag.
 * It will attempt to find dSYMs for ALL symbol owners in the symbolicator.
 *
 * DETAILS: This method processes every symbol owner in the symbolicator
 * at the time of the call. For each symbol owner, the symbol owner cache
 * is searched for existing data. If no matching data is found, OR if the
 * data that is found was created without a dSYM, DebugSymbols is called to
 * search for a dSYM. If a dSYM is found, it is immediately processed and
 * "faulted" in for the symbol owner in question. Any pre-existing data
 * in the symbol owner cache is replaced.
 *
 * See DBGCopyDSYMURLForUUIDWithOptions() in DebugSymbols.h for usage
 * details of shellCommands, spotlightScopes, flatSearchPaths, and fileMappedPaths
 *
 */

void CSSymbolicatorResymbolicate(CSSymbolicatorRef symbolicator,
                                 CFTypeRef shellCommands,
                                 CFTypeRef spotlightScopes,
                                 CFTypeRef flatSearchPaths,
                                 CFTypeRef fileMappedPaths);

/*
 * DO NOT USE. This is a temporary workaround while the resymbolication
 * api is reworked to be usable. Use of this API is a clubbing offense.
 *
 * Takes an array of CFURL's that point to dsyms. The URL must point to
 * the actual binary, not the .dSYM bundle root.
 */
void CSSymbolicatorResymbolicateFail(CSSymbolicatorRef symbolicator,
                                     CFArrayRef urls);

typedef void (^CSResymbolicateBlock)(CSSymbolOwnerRef, bool);

/*
 * See comments for CSSymbolicatorResymbolicate
 * The resymbolicateBlock parameter gets called for each resymbolicated symbol owner, with
 * a bool value that indicates if the resymbolication was successful.
 */
void CSSymbolicatorResymbolicateFromDebugSymbolsInfo(CSSymbolicatorRef symbolicatorRef, CSResymbolicateBlock resymbolicateBlock);

CFStringRef CSSymbolicatorCopyDescriptionWithIndent(CSSymbolicatorRef, uint32_t);

/*
 * This is a hack fix. Do not use. Avoid. Run Away (tm).
 *
 * ReportCrash needs to use safe mach vm reads. It is currently to expensive to
 * enable safe mach vm reads for everyone. ReportCrash calls into code that creates
 * symbolicators for ReportCrash, but that code has no flags/arguments that enable
 * ReportCrash to tell it to use safe mach vm reads.
 *
 * The workaround is to have a global override which forces *everyone* to use safe mach vm reads.
 */
void CSSymbolicatorSetForceGlobalSafeMachVMReads(bool);
void CSSetForceSafeMachVMReads(bool enabled);
bool CSGetForceSafeMachVMReads(void);

#pragma mark Mutable Context

/*
 * MUTABLE CONTEXT....
 */

typedef void* CSMutableContext;

/*
 * This function creates a mutable context which is valid for the duration of the block.
 *
 * Returns true on successful update of the symbolicator, false if the update fails. If the
 * update fails, all changes are rolled back.
 *
 * See CSSymbolOwner_Private.h for full details.
 */
bool CSSymbolicatorApplyMutableContextBlock(CSSymbolicatorRef, void (^)(CSMutableContext));

/*
 * Convenience method. Implemented entirely using the mutable context SPI.
 */
CSSymbolOwnerRef CSSymbolicatorAddSymbolOwner(CSSymbolicatorRef symbolicator, CSSymbolOwnerRef owner);

CS_EXTERN_C_END 

#endif /* _CSSYMBOLICATOR_PRIVATE_H_ */


