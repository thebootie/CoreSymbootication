//
//  CSUUID.h
//  CoreSymbolication
//
//  Created by James McIlree on 1/30/12.
//  Copyright (c) 2012 Apple. All rights reserved.
//

#ifndef _CSUUID_H_
#define _CSUUID_H_ 1

CS_EXTERN_C_BEGIN

// These are handy for parsing a string to and from CFUUIDBytes
// They are lighter weight than the CF equivalents, especially if
// you do not have any other need to initialize CF.

bool CSUUIDStringToCFUUIDBytes(const char* string, CFUUIDBytes* bytes);
bool CSUUIDCFUUIDBytesToString(const CFUUIDBytes* bytes, char* buffer, size_t buffer_size);

CS_EXTERN_C_END

#endif /* _CSUUID_H_ */
