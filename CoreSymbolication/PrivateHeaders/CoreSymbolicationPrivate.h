/*
 *  CoreSymbolicationPrivate.h
 *  CoreSymbolication
 *
 *  Copyright 2013-2018 Apple Inc. All rights reserved.
 *
 */

// installapi requires that the Private headers be "stand-alone",
// so they need to include the Public headers explicitly.
#include <CoreSymbolication/CoreSymbolication.h>

#define CS_HEADER_PATH(NAME) <CoreSymbolication/NAME>

/* See documentation in CoreSymbolicationPrivateAPI.h for explanation
 * of how this header works. */
#include <CoreSymbolication/CoreSymbolicationPrivateAPI.h>

#undef CS_HEADER_PATH
