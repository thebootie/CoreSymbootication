/*
 *  CSMappedMemoryCache.h
 *  CoreSymbolication
 *
 *  Created by Daniel Delwood on 11/11/08.
 *  Copyright 2008-2018 Apple Inc. All rights reserved.
 *
 */

CS_EXTERN_C_BEGIN

#define CS_LOG_MEMORY_CACHE 0

typedef struct mapped_region_node_t {
    mach_vm_address_t			remote_base_addr; // always page_aligned
    mach_vm_size_t				mapped_size; // should be an integer multiple of vm_page_size.
    vm_address_t				local_mapped_addr;
    // AVL Tree management
    struct mapped_region_node_t	*left;
    struct mapped_region_node_t *right;
    struct mapped_region_node_t *parent;
    uint32_t					height;
    task_t						source_task;
} mapped_region_node_t;

typedef struct mapped_memory_t mapped_memory_t;

mapped_memory_t *create_mapped_memory_cache_for_task(task_t task, bool touch_mapped_regions);
// Creates and returns a new mapped memory cache; when done, pass this to the destroy below.

mapped_memory_t *retain_mapped_memory_cache(mapped_memory_t *mapped_memory);
// Increments the refcount.

mapped_memory_t *release_mapped_memory_cache(mapped_memory_t *mapped_memory);
// Decrements the refcount; if it reaches 0, calls destroy_mapped_memory_cache.
// Returns mapped_memory, or NULL if it was destroyed.

void destroy_mapped_memory_cache(mapped_memory_t *mapped_memory);
// Clears all currently mapped regions and destroys the mapped_memory cache data structure.
// NOTE: should only be called directly if the client directly called create_mapped_memory_cache_for_task,
// as opposed to going through VMUTaskMemoryCache.

mapped_region_node_t *find_node(mapped_memory_t *mapped_memory, mach_vm_address_t remote_address, mach_vm_size_t size, bool fully_contained_only);
// Returns the first node containing the range, or the node that fully contains the range if fully_contained_only == true

mapped_region_node_t *map_new_node(mapped_memory_t *mapped_memory, mach_vm_address_t remote_address, mach_vm_size_t size, kern_return_t *out_error);
// Maps and inserts the region specified into the mapped_memory

mapped_region_node_t *next_node(mapped_memory_t *mapped_memory, mapped_region_node_t *starting_node);
// Returns the next mapped region in the process (ordered by remote address)

void clear_mapped_memory(mapped_memory_t *mapped_memory);
// Unmaps all currently mapped regions and removes them from the cache

kern_return_t mapped_memory_read_pointer(mapped_memory_t *memory, mach_vm_address_t target_address, mach_vm_address_t *out_value);
// Convenience method to read a pointer-sized value from the remote process

kern_return_t mapped_memory_read(mapped_memory_t *memory, mach_vm_address_t target_address, mach_vm_size_t size, void **out_buffer);
// Convenience method to read from mapped_memory and returns the local buffer; the caller does not own *out_buffer and should not free it.

bool task_is_64bit(task_t task);
// Utility function for syscall asking whether or not a task is running with a 64-bit address space (LP64)

CS_EXTERN_C_END
