/*
 *  CSBase.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 8/7/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSBASE_H_
#define _CSBASE_H_ 1

#include <stdint.h>
#include <stdbool.h>

#if !defined(CS_EXTERN_C_BEGIN)
    #if defined(__cplusplus)
        #define CS_EXTERN_C_BEGIN extern "C" {
        #define CS_EXTERN_C_END   }
    #else
        #define CS_EXTERN_C_BEGIN
	#define CS_EXTERN_C_END
    #endif
#endif

#if !defined(CS_INLINE)
    #if defined(__GNUC__) && (__GNUC__ == 4) && !defined(DEBUG)
        #define CS_INLINE static __inline__ __attribute__((always_inline))
    #elif defined(__GNUC__)
        #define CS_INLINE static __inline__
    #else
        #warning CS_INLINE not defined...
    #endif
#endif

#if !defined(CS_EXPORT)
#define CS_EXPORT  __attribute__((__visibility__("default")))
#endif

CS_EXTERN_C_BEGIN

struct _CSTypeRef {
    uintptr_t _opaque_1;
    uintptr_t _opaque_2;
};

typedef struct _CSTypeRef CSTypeRef;

typedef uint64_t CSMachineTime;

// Default values for library load/unload times.
static const CSMachineTime kCSBeginningOfTime = 0;
static const CSMachineTime kCSEndOfTime = INT64_MAX;
static const CSMachineTime kCSNow = INT64_MAX + 1ULL;
static const CSMachineTime kCSAllTimes = INT64_MAX + 2ULL;

/*
 * To prevent circular type references, these
 * types are defined here, rather than in their
 * respective headers.
 */
typedef CSTypeRef CSNullRef;
typedef CSTypeRef CSSymbolicatorRef;
typedef CSTypeRef CSSymbolOwnerRef;
typedef CSTypeRef CSSymbolRef;
typedef CSTypeRef CSSourceInfoRef;
typedef CSTypeRef CSRegionRef;
typedef CSTypeRef CSAddressSetRef; 
typedef CSRegionRef CSSegmentRef;
typedef CSRegionRef CSSectionRef;

typedef void (^CSSymbolicatorIterator)(CSSymbolicatorRef symbolicator);
typedef void (^CSSymbolOwnerIterator)(CSSymbolOwnerRef owner);
typedef void (^CSRegionIterator)(CSRegionRef region);
typedef void (^CSSegmentIterator)(CSSegmentRef segment);
typedef void (^CSSectionIterator)(CSSectionRef section);
typedef void (^CSSymbolIterator)(CSSymbolRef symbol);
typedef void (^CSSourceInfoIterator)(CSSourceInfoRef info);
typedef void (^CSStackFrameIterator)(CSSymbolRef symbol, CSSourceInfoRef info);

typedef uint32_t CSTypeID;

static const CSTypeID kCSNullTypeID = 0;
static const CSTypeID kCSSymbolicatorTypeID = 2;
static const CSTypeID kCSSymbolOwnerTypeID = 3;
static const CSTypeID kCSSectionTypeID = 4;
static const CSTypeID kCSSymbolTypeID = 5;
static const CSTypeID kCSSourceInfoTypeID = 6;
static const CSTypeID kCSSegmentTypeID = 7;
static const CSTypeID kCSAddressSetTypeID = 8;

static const CSNullRef kCSNull = { 0, 0 };

CSTypeRef CSRetain(CSTypeRef);
void CSRelease(CSTypeRef);
uint32_t CSGetRetainCount(CSTypeRef);

bool CSEqual(CSTypeRef, CSTypeRef);

// This does the same thing as CSEqual(ref, kCSNull);
bool CSIsNull(CSTypeRef);

CFStringRef CSCopyDescription(CSTypeRef);

void CSShow(CSTypeRef);

CS_EXTERN_C_END

#endif /* _CSBASE_H_ */
