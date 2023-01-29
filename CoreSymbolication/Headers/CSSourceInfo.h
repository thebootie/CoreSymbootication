/*
 *  CSSourceInfo.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 12/18/07.
 *  Copyright 2007 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSSOURCEINFO_H_
#define _CSSOURCEINFO_H_ 1

CS_EXTERN_C_BEGIN

CSRange CSSourceInfoGetRange(CSSourceInfoRef);

const char* CSSourceInfoGetPath(CSSourceInfoRef);
const char* CSSourceInfoGetFilename(CSSourceInfoRef);

uint32_t CSSourceInfoGetLineNumber(CSSourceInfoRef);
uint32_t CSSourceInfoGetColumn(CSSourceInfoRef);

CSSymbolicatorRef CSSourceInfoGetSymbolicator(CSSourceInfoRef);
CSSymbolOwnerRef CSSourceInfoGetSymbolOwner(CSSourceInfoRef);
CSRegionRef CSSourceInfoGetRegion(CSSourceInfoRef);
CSSymbolRef CSSourceInfoGetSymbol(CSSourceInfoRef);

CS_EXTERN_C_END

#endif /* _CSSOURCEINFO_H_ */
