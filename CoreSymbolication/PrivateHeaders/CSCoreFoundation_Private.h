/*
 *  CSCoreFoundation_Private.h
 *  CoreSymbolication
 *
 *  Created by Daniel Delwood on 11/5/09.
 *  Copyright 2009 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSCOREFOUNDATION_PRIVATE_H_
#define _CSCOREFOUNDATION_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

extern CFDictionaryKeyCallBacks kCSTypeDictionaryKeyCallBacks;
extern CFDictionaryValueCallBacks kCSTypeDictionaryValueCallBacks;

extern CFDictionaryKeyCallBacks kCSTypeDictionaryWeakKeyCallBacks;
extern CFDictionaryValueCallBacks kCSTypeDictionaryWeakValueCallBacks;

extern CFSetCallBacks kCSTypeSetCallBacks;
extern CFSetCallBacks kCSTypeSetWeakCallBacks;

CS_EXTERN_C_END

#endif /* _CSCOREFOUNDATION_PRIVATE_H_ */
