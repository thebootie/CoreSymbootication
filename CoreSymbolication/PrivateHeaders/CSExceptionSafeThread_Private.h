/*
 *  CSExceptionSafeThread_Private.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 9/20/10.
 *  Copyright 2010 Apple Inc. All rights reserved.
 *
 */

#ifndef _CSEXCEPTIONSAFETHREAD_PRIVATE_H_
#define _CSEXCEPTIONSAFETHREAD_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

//
// This is a wrapper around the CPP exception safe thread code.
//
// The exception safe thread will catch accesses to memory that has no backing store.
// Any other exceptions/crashers will occur as normal. This thread is used to work
// around issues with targets that are mmap'd from devices that "go away", and then
// when we attempt to reference those pages, the OS nukes us instead of providing the
// data.
//
// The function will start a new thread each time it is called. The return value is
// one of the codes below. Any value other than OKAY means you cannot trust the data
// from your block.
//
// Use is fairly straightforward:
//
// uint32_t value = CSExceptionSafeThreadRunBlock(^() { blah = doSomethingThatMightBewm(); });
// if (value != kCSExceptionSafeThreadExitedOkay) { /* Handle error */ }
//

enum CSExceptionSafeThreadPrivateFlags {
    kCSExceptionSafeThreadCreatingSacrificialThread		= 0x0,
    kCSExceptionSafeThreadSacrificialThreadRunning		= 0x100,
    kCSExceptionSafeThreadExitedOkay				= 0x1000,
    kCSExceptionSafeThreadSacrificialThreadFailedToStart	= 0x1001,
    kCSExceptionSafeThreadSacrificialThreadBadAccess		= 0x1002,
    kCSExceptionSafeThreadSacrificialThreadThrewException	= 0x1003,
    kCSExceptionSafeThreadMasterThreadIPCError		= 0x1004
};

uint32_t CSExceptionSafeThreadRunBlock(void (^)(void));

CS_EXTERN_C_END

#endif /* _CSEXCEPTIONSAFETHREAD_PRIVATE_H_ */
