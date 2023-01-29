//
//  CSUUID_Private.hpp
//  CoreSymbolication
//
//  Created by James McIlree on 3/18/12.
//  Copyright (c) 2012 Apple. All rights reserved.
//

#ifndef _CSUUID_PRIVATE_H_
#define _CSUUID_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

bool CSUUIDCFUUIDBytesToPath(const CFUUIDBytes* bytes, char* buffer, size_t buffer_size);

CS_EXTERN_C_END

#endif /* _CSUUID_PRIVATE_H_ */
