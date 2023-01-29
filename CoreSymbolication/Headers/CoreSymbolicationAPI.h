/*
 *  CoreSymbolicationAPI.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 11/15/07.
 *  Copyright 2007 Apple Inc.. All rights reserved.
 *
 */

/* This is the list of header files that should be considered part of
 * the "public" CoreSymbolication API.
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
 * CoreSymbolication.h, CoreSymbolicationDT.h, and
 * CoreSymbolicationInternal.h, respectively.
 *
 * This file specifies the name of the individual headers, and it
 * is not intended to be included directly.  Instead, each of those
 * three headers specifies a CS_HEADER_PATH that translates the name of
 * each header file listed here to the correct way to include that
 * header.
 */

/* Instead include, CoreSymbolication.h, CoreSymbolicationDTPrivate.h,
 * or CoreSymbolicationPrivateInternal.h.
 */
#include <limits.h>
#include <stdbool.h>
#include <Block.h>

#include <CoreFoundation/CoreFoundation.h>

#include CS_HEADER_PATH(CSBase.h)
#include CS_HEADER_PATH(CSArchitecture.h)
#include CS_HEADER_PATH(CSRange.h)
#include CS_HEADER_PATH(CSSourceInfo.h)
#include CS_HEADER_PATH(CSSymbol.h)
#include CS_HEADER_PATH(CSRegion.h)
#include CS_HEADER_PATH(CSSignature.h)
#include CS_HEADER_PATH(CSSymbolOwner.h)
#include CS_HEADER_PATH(CSSymbolOwnerCache.h)
#include CS_HEADER_PATH(CSSymbolicator.h)
#include CS_HEADER_PATH(CSUUID.h)
