/*
 *  CSSampling.h
 *  CoreSymbolication
 *
 *  Created by Daniel Delwood on 11/11/08.
 *  Copyright 2008 Apple Inc. All rights reserved.
 *
 */

/*
 C-level API for sampling. Lower overhead, all the perks (using CFI data, platform independence, etc.)

 Note: 'sampling_context_t's are not thread-safe and should only be used from a single thread a time. With this
 in mind, it's safe to have multiple sampling_contexts for multiple processes — or even the same process.
 */

#ifndef _CSSAMPLING_H_
#define _CSSAMPLING_H_ 1

CS_EXTERN_C_BEGIN

typedef struct sampling_context_t sampling_context_t;

enum {
    SAMPLING_USE_FP_BACKTRACING  = 1 << 0,
    SAMPLING_USE_CFI_BACKTRACING = 1 << 1,
    SAMPLING_USE_CFI_FIRST_FRAME = 1 << 2,
    SAMPLING_DO_MINIMAL_BACKTRACING = 1 << 3,   // don't get dispatch queue info or parse CFI data
    SAMPLING_WITH_NO_SYMBOLICATOR = 1 << 4,     // bare-minimum sampling, cannot map PC --> function for CheckForSecondFrame()
    SAMPLING_TOUCH_PAGES_EARLY   = 1 << 13,
};

typedef int sampling_options_flags;

typedef enum {
    CSCallstackI386Simple   = 0x20, // N I386 callstack frames
    CSCallstackI386ESP     = 0x21, // N I386 callstack frames followed by ESP at frames[N]
    CSCallstackJava        = 0x30, // N Java callstack frames
    CSCallstackARMSimple   = 0x40,     // N ARM callstack frames
    CSCallstackARMLR       = 0x41,     // N ARM callstack frames followed by LR at frames[N]
} cs_callstack_flavor_t;

#define CORESYMBOLICATION_FIXUP_UKNOWN_FRAME_ADDRESS ~0x0ULL

// Creates a thread-local sampling context for analyzing task with the specified sampling options.
sampling_context_t* create_sampling_context_for_task(task_t task, sampling_options_flags options);
sampling_context_t* create_sampling_context_for_task_and_pid(task_t task, pid_t pid, sampling_options_flags options) __attribute__((deprecated));   // Remove this when both CoreSymbolication and Symbolication are submitted everywhere.
sampling_context_t* create_sampling_context_for_task_pid_symbolicator(task_t task, pid_t pid, CSSymbolicatorRef symbolicator, sampling_options_flags options);

// Clears the sampling cache; this must be done whenever time passes in the remote process before sampling a thread again;
// failure to do so may provide stale or incorrect samples.
void sampling_context_clear_cache(sampling_context_t *sampling_context);

// Destroys a thread-local sampling context and frees all associated resources.
void destroy_sampling_context(sampling_context_t *sampling_context);

// Sample a given thread.  Returns (up to max) frames of remote thread in frames_buffer.  Also returns sample_flavor, sample_time.
kern_return_t sample_remote_thread(sampling_context_t *sampling_context, thread_t thread, mach_vm_address_t *frames_buffer,
                                   uint32_t *out_count, uint32_t max, cs_callstack_flavor_t *out_sample_flavor, CSMachineTime *out_sample_time);

/*!
 Sample a given thread.
 
 @param sampling_context
 Specifies the target process, symbolicator, etc.
 
 @param thread
 Which thread to sample.
 
 @param out_frame_pcs
 Fills in array of PC addresses for the functions/offsets associated with each stack frame.
 
 @param out_frame_ptrs
 Fills in array of the addresses in the stack of where the various stack frame are;
 caller can pass NULL if it does not need this info.  This info can be useful for
 memory graph debugging -- if there are pointer references from the stack, this array
 can be used to figure out what stack frame each pointer is in, and then from the
 corresponding index in out_frame_pcs that can be associated to function name.
 
 @param in_out_count
 Incoming value should be maximum number of frames to return in out_frame_pcs_buffer
 and out_frame_ptrs_buffer.  Return value is actual number of frames being returned.
 
 @param out_sample_flavor
 Returns the type of callstack provided, which indicates the encoding of any extra data
 provided at the end of the callstack.
 
 @param out_sample_time
 Returns the mach_absolute_time() at the start of when the backtrace is taken.
 
 @param out_dispatch_queue_serial_num
 Returns the dispatch queue serial number, for translation into a dispatch queue name
 via dispatch_queue_name_for_serial_number().
 */
kern_return_t sample_remote_thread_with_dispatch_queue(sampling_context_t *sampling_context, thread_t thread,
                                                       mach_vm_address_t *out_frame_pcs,
                                                       mach_vm_address_t *out_frame_ptrs,
                                                       uint32_t *in_out_count,
                                                       cs_callstack_flavor_t *out_sample_flavor,
                                                       CSMachineTime *out_sample_time,
                                                       uint64_t *out_dispatch_queue_serial_num);

/*!
 Fix up a callstack that may be missing frames.  Any adjustments to the
 callstack happen in place in the provided frames array.

 @param symbolicator
 The CSSymbolicatorRef that goes along with the process from which the callstack
 was obtained.

 @param frames
 Call stack, hottest frame first.  Depending on the callstack flavor, the frames
 will have extra information at the end included in the incoming in_out_count.

 @param in_out_count
 Points to a uint32_t that, incoming, contains the number of entries in frames,
 including any extra data specified by the flavor.  On return, the in_out_count
 will contain the potentially adjusted call frame size, with any extra data removed
 and any necessary insertion frames inserted.

 @param extra_entries_count
 Specifies the number of extra entries that are provided at the end of frames.
 The extra entries are to be interpreted by the callstack flavor specification.
 It is permissible to be using a callstack flavor and not have the extra data
 that the flavor specifies.  In that case, the fixup logic will insert a
 CORESYMBOLICATION_FIXUP_UKNOWN_FRAME_ADDRESS entry where the extra data would
 have gone.  The intent is that we can provide that we know the callstack
 requires fixup at that location even though we don't know exactly what it
 should be.  Note the extra_entries_count should be included in the incoming
 in_out_count.  e.g. if there are 5 real frames in the backtrace, and there's
 one extra data entry, then *in_out_count should be set to 6 and extra_entries_count
 should be set to 1.

 @param in_out_flavor
 The type of callstack provided, which indicates the encoding of any extra data
 provided at the end of the callstack.

 @param sample_time
 The CSMachineTime provided to CSSymbolicatorRef methods.  If 0, uses kCSNow.

 @param backstep_caller_pcs
 If true, have the caller PCs get decremented by enough to cross into the
 caller's calling instruction.  In the case of arm32 and thumb code, if a sampling_context is
 given, the caller PCs will be rewound precisely to the start of the calling
 thumb instruction; otherwise, thumb code PCs are only backed up 2 bytes, possibly
 in the middle of a 4-byte PC instruction.  In the case of i386/x86_64, we just
 back up one byte no matter what to avoid having to do instruction analysis.

 @param sampling_context
 May be NULL if there is no sampling context available.  With a sampling
 context, the PC location in the stack can be verified.
*/
void CSSymbolicatorFixupFrames(CSSymbolicatorRef symbolicator, mach_vm_address_t *frames,
                  uint32_t *in_out_count, uint32_t extra_entries_count,
                  cs_callstack_flavor_t *in_out_flavor,
                  CSMachineTime sample_time, bool backstep_caller_pcs, sampling_context_t *sampling_context);

// Given a thread ID, get the dispatch queue serial number.
// This is provided as an alternative to sample_remote_thread() or sample_remote_thread_with_dispatch_queue(),
// in case the client does the sampling itself in some other way.  For example, on iOS we currently still use
// the old VMUSampler to do the sampling, but we want to be able to get thread names and dispatch queue names.
kern_return_t get_remote_thread_dispatch_queue(sampling_context_t *sampling_context, thread_t thread, uint64_t *out_dispatch_queue_serial_num);

// Given a thread ID, return the thread name.
const char *thread_name_for_thread_port(sampling_context_t *sampling_context, thread_t thread, uint64_t *out_thread_id, uint64_t *out_dispatch_queue_serial_num);

// Given a dispatch queue serial number, return the dispatch queue name and concurrency width.
const char *dispatch_queue_name_for_serial_number(sampling_context_t *sampling_context, uint64_t serial_num, bool *out_concurrent_flag, uint64_t *out_thread_id);

CS_EXTERN_C_END

#endif /* _CSSAMPLING_H_ */
