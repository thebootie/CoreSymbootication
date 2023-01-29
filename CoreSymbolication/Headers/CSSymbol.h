/*
 *  CSSymbol.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 12/6/07.
 *  Copyright 2007 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSSYMBOL_H_
#define _CSSYMBOL_H_ 1

CS_EXTERN_C_BEGIN

// To allow building against an earlier OSX SDK without newer availability macros
#ifdef __MAC_10_9
#define _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0 __OSX_AVAILABLE_STARTING(__MAC_10_9, __IPHONE_7_0)
#else
#define _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0
#endif

// Returns NULL on failure or if the name was not mangled.
// If a !NULL value is returned, it is malloc'd data and must be freed by the caller!
// NOTE! Exported as C api for use by dtrace.
const char* demangle(const char* name) API_DEPRECATED("Use CSDemangleSymbolName instead", macos(10.0,10.14), ios(1.0,12.0), watchos(1.0,4.0), tvos(1.0,12.0));
const char* CSDemangleSymbolName(const char* name) API_AVAILABLE(macos(10.13), ios(12.0), watchos(4.0), tvos(12.0));
// Set macOS availability to 10.13 rather than 10.14 since for Dev Tools trains, it is available there in the updated CoreSymbolication.

enum CSSymbolFlags {
    kCSSymbolIsFunction			= 0x1,
    kCSSymbolIsDyldStub			= 0x2,
    kCSSymbolIsObjcMethod		= 0x4,
    kCSSymbolIsJavaMethod		= 0x8, /* Obsolete, not supported */
    kCSSymbolIsExternal			= 0x10,
    kCSSymbolIsPrivateExternal		= 0x20,
    kCSSymbolIsThumb			= 0x40,
    kCSSymbolIsOmitFramePointer		= 0x80, /* This flag is a hint. If it is set, there is no frame pointer. However, the flag will not always be set for all frame pointerless symbols */
    kCSSymbolIsKnownLength		= 0x100, /* If this flag is set, the length was explicitly provided by debug info. Otherwise it is a best guess. */
    /* 0x1000+ for internal use only */
};

//
// Accessors
//
CSRange CSSymbolGetRange(CSSymbolRef);
uint32_t CSSymbolGetFlags(CSSymbolRef); // This may be expensive. Some flags are calculated lazily.

//
// NOTE! Starting in 10.7, and some future version of iOS, these may return NULL.
// Certain sources of symbol information do not contain names. Symbols constructed from
// those sources will return NULL. If you need to quickly differentiate between a kCSNull
// symbol and a valid symbol returning a NULL name, use the CSSymbolIsUnnamed() predicate.
//
const char* CSSymbolGetName(CSSymbolRef);
const char* CSSymbolGetMangledName(CSSymbolRef);

//
// Predicates
//
bool CSSymbolIsFunction(CSSymbolRef);
bool CSSymbolIsDyldStub(CSSymbolRef);
bool CSSymbolIsObjcMethod(CSSymbolRef);
bool CSSymbolIsExternal(CSSymbolRef);
bool CSSymbolIsPrivateExternal(CSSymbolRef);
bool CSSymbolIsArm(CSSymbolRef); // Note! This is not a test of arm arch, but of !thumb()
bool CSSymbolIsThumb(CSSymbolRef);
bool CSSymbolIsOmitFramePointer(CSSymbolRef);
bool CSSymbolIsKnownLength(CSSymbolRef);
// Some sources of symbol information do not contain a name/mangled-name for the symbol.
// If the CSSymbolRef was constructed from such a source, and a name could not be found
// in any other data, this will return TRUE. Unnamed symbols will return NULL when asked
// for a name/mangled-name.
bool CSSymbolIsUnnamed(CSSymbolRef);

CSSymbolicatorRef CSSymbolGetSymbolicator(CSSymbolRef);
CSSymbolOwnerRef CSSymbolGetSymbolOwner(CSSymbolRef);

CSSectionRef CSSymbolGetSection(CSSymbolRef) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;
CSSegmentRef CSSymbolGetSegment(CSSymbolRef) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;

size_t CSSymbolForeachSourceInfo(CSSymbolRef, CSSourceInfoIterator);


CSRegionRef CSSymbolGetRegion(CSSymbolRef); // Informally deprecated
	// Defaults to the behavior of CSSymbolGetSection, unless the symbolicator was created with the also-deprecated and private
	// 'segments' flag instead, in which case it behaves like CSSymbolGetSegment.

CS_EXTERN_C_END 

#endif /* _CSSYMBOL_H_ */
