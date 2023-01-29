/*
 *  CSBase_Private.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 9/16/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSBASE_PRIVATE_H_
#define _CSBASE_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

CFStringRef CSCopyDescriptionWithIndent(CSTypeRef, uint32_t);

//
// Get the total number of bytes being used to map files into memory.
// This value is useful for testing and debugging.
//
size_t CSTotalBytesMapped(void);

CS_EXTERN_C_END

#endif /* _CSBASE_PRIVATE_H_ */
