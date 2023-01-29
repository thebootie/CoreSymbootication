/*
 *  CSRegion.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 1/14/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSREGION_H_
#define _CSREGION_H_ 1

CS_EXTERN_C_BEGIN

// To allow building against an earlier OSX SDK without newer availability macros
#ifdef __MAC_10_9
#define _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0 __OSX_AVAILABLE_STARTING(__MAC_10_9, __IPHONE_7_0)
#else
#define _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0
#endif

extern const char* kCSRegionMachHeaderName;

//
// 'Region' is the general term used to describe both segments and sections of a symbol owner.
// A region has range and name properties, and belongs to a single symbol owner.
//

//
// Accessors
// These apply to both Segments and Sections
//
CSRange CSRegionGetRange(CSRegionRef);
const char* CSRegionGetName(CSRegionRef);

CSSymbolicatorRef CSRegionGetSymbolicator(CSRegionRef);
CSSymbolOwnerRef CSRegionGetSymbolOwner(CSRegionRef);

size_t CSRegionForeachSymbol(CSRegionRef, CSSymbolIterator);
size_t CSRegionForeachSourceInfo(CSRegionRef, CSSourceInfoIterator);

//
// Segment-Specific
//
size_t CSSegmentForeachSection(CSSegmentRef, CSSectionIterator) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;

//
// Section-Specific
//
CSSegmentRef CSSectionGetSegment(CSSectionRef) _CS_Availabile_Starting_MAC_10_9_IPHONE_7_0;

CS_EXTERN_C_END

#endif /* _CSREGION_H_ */
