/*
 *  CSSignature.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 9/1/09.
 *  Copyright 2009 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSSIGNATURE_H_
#define _CSSIGNATURE_H_ 1

CS_EXTERN_C_BEGIN

//
// NOTE! The kCSSignatureDeepCopy may conflict with other flags you have
// specified at the symbolicator level. A "deep" signature is made from
// MMap style symbol owner data, and if you have specified flags disallowing
// the creation of MMap symbol owner datas, you will not get a deep signature.
//
// Basically, if for any reason a MMap symbol owner data cannot be created
// for a given symbol owner, you'll get no deep signature for that symbol owner.
// Each symbol owner stands on its own, so the failure of one symbol owner data
// creation does not prevent later symbol owners from archiving their data.
//

//
// NOTE!
//
// The deep copy data is *completely* backwards and forwards compatible with
// old and new CoreSymbolication versions. Older CoreSymbolication dylibs
// will not be able to use the deep copy data, but they can still decode
// the signature.
//

enum CSSignatureFlags {
    kCSSignatureDeepCopy = 0x1 // Include all owner, region, symbol, and source info data in signature. A "deep" signature.
};

CS_EXTERN_C_END

#endif /* _CSSIGNATURE_H_ */
