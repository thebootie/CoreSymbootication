/*
 *  CoreSymbolication.h
 *  CoreSymbolication
 *
 *  Created by Justin Seyster on 10/18/13.
 *  Copyright 2013-2018 Apple Inc. All rights reserved.
 *
 */

#ifndef CS_HEADER_PATH
#ifdef BUILD_FOR_DT
#define CS_HEADER_PATH(NAME) <CoreSymbolicationDT/NAME>
#else
#define CS_HEADER_PATH(NAME) <CoreSymbolication/NAME>
#endif
#endif

/* See documentation in CoreSymbolicationAPI.h for explanation of how
 * this header works. */
#include <CoreSymbolication/CoreSymbolicationAPI.h>

#undef CS_HEADER_PATH
