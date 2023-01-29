/*
 *  SymbolicationPrivate.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 8/25/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */

/* This is the list of header files that should be considered part of
 * the "private" CoreSymbolication API.
 *
 * Because this framework can get compiled with two different names,
 * there are actually _three_ different ways that we may need to
 * specify a header so as to get the right one:
 *   1: <CoreSymbolication/header.h> when headers are being included
 *     in an external project that is a client of the
 *     CoreSymbolication framework.
 *   2: <CoreSymbolicationDT/header.h> when headers are being
 *     included in an external project that is a client of the
 *     CoreSymbolicationDT framework.
 *   3: "header.h" when headers are being included in an internal
 *     file (i.e, within the framework implemenation).
 *
 * To accomplish that, this header exists in three different verions:
 * CoreSymbolicationPrivate.h, CoreSymbolicationDTPrivate.h, and
 * CoreSymbolicationPrivateInternal.h, respectively.
 *
 * This file specifies the name of the individual headers, and it
 * is not intended to be included directly.  Instead, each of those
 * three headers specifies a CS_HEADER_PATH that translates the name of
 * each header file listed here to the correct way to include that
 * header.
 */

/* Instead include, CoreSymbolicationPrivate.h,
 * CoreSymbolicationDTPrivate.h, or CoreSymbolicationPrivateInternal.h.
 */
#ifndef CS_HEADER_PATH
#error Do not include CoreSymbolicationPrivateAPI.h directly.
#endif

#ifndef __CORESYMBOLICATION_PRIVATE_H__
#define __CORESYMBOLICATION_PRIVATE_H__ 1

#include CS_HEADER_PATH(CoreSymbolicationAPI.h)

#include CS_HEADER_PATH(CSBase_Private.h)
#include CS_HEADER_PATH(CSRegion_Private.h)
#include CS_HEADER_PATH(CSSourceInfo_Private.h)
#include CS_HEADER_PATH(CSSymbol_Private.h)
#include CS_HEADER_PATH(CSSymbolicator_Private.h)
#include CS_HEADER_PATH(CSSymbolOwner_Private.h)
#include CS_HEADER_PATH(CSSymbolOwnerCache_Private.h)
#include CS_HEADER_PATH(CSExceptionSafeThread_Private.h)
#include CS_HEADER_PATH(CSUUID_Private.h)
#include CS_HEADER_PATH(CSMMapArchiveCache_Private.h)
#include CS_HEADER_PATH(CSSampling.h)
#include CS_HEADER_PATH(CSMappedMemoryCache.h)
#include CS_HEADER_PATH(CSCoreFoundation_Private.h)

#endif
