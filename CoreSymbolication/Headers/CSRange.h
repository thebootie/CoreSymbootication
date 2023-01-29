/*
 *  CSRange.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 1/22/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSRANGE_H_
#define _CSRANGE_H_ 1

CS_EXTERN_C_BEGIN

struct _CSRange {
    mach_vm_address_t	location;
    mach_vm_size_t	length;
};

typedef struct _CSRange CSRange;

CS_INLINE CSRange CSRangeMake(mach_vm_address_t loc, mach_vm_size_t len) {
    CSRange r;
    r.location = loc;
    r.length = len;
    return r;
}

CS_INLINE mach_vm_address_t CSRangeMax(CSRange range) {
    return (range.location + range.length);
}

CS_INLINE bool CSRangeContainsLocation(CSRange range, mach_vm_address_t loc) {
    return (loc - range.location < range.length);
}

CS_INLINE bool CSRangeEqualsRange(CSRange range1, CSRange range2) {
    return (range1.location == range2.location && range1.length == range2.length);
}

bool CSRangeIntersectsRange(CSRange range1, CSRange range2);

bool CSRangeContainsRange(CSRange range1, CSRange range2);

CS_EXTERN_C_END

#endif /* _CSRANGE_H_ */
