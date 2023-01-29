/*
 *  CSSymbol_Private.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 9/11/08.
 *  Copyright 2008 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSSYMBOL_PRIVATE_H_
#define _CSSYMBOL_PRIVATE_H_ 1

CS_EXTERN_C_BEGIN

// We could encode the symbol name source fields using less bits,
// but at this point space is not at a premium, so we avoid the
// extra complexity.

enum CSSymbolPrivateFlags {
    kCSSymbolIsMarked				= 0x00001000, // Here and above are internal use only
    kCSSymbolLock				= 0x00002000,
    kCSSymbolNameSourceNlist			= 0x00004000,
    kCSSymbolNameSourceDwarf			= 0x00008000,
    kCSSymbolNameSourceDwarfMIPSLinkage 	= 0x00010000,
    kCSSymbolNameValid				= 0x00020000,
    kCSSymbolNameAllocated			= 0x00040000,
    kCSSymbolMangledNameSourceNlist		= 0x00080000,
    kCSSymbolMangledNameSourceDwarf		= 0x00100000,
    kCSSymbolMangledNameSourceDwarfMIPSLinkage	= 0x00200000,
    kCSSymbolMangledNameValid			= 0x00400000,
    kCSSymbolMangledNameAllocated		= 0x00800000,
    kCSSymbolIsObjcMethodValid			= 0x01000000,
    kCSSymbolNList				= 0x02000000,
    kCSSymbolDwarf				= 0x04000000,
    kCSSymbolDebugMap				= 0x08000000,
    kCSSymbolFunctionStarts			= 0x10000000,
    kCSSymbolMerged				= 0x20000000,
    kCSSymbolIsLastChild			= 0x40000000,
    kCSSymbolHasInlinedSourceInfo	= 0x80000000,
};

static const uint32_t kCSSymbolExcludeFromMergeFlagMask = kCSSymbolIsMarked |
							kCSSymbolLock |
							kCSSymbolNameSourceNlist |
							kCSSymbolNameSourceDwarf |
							kCSSymbolNameSourceDwarfMIPSLinkage |
							kCSSymbolNameValid |
						    	kCSSymbolNameAllocated |
							kCSSymbolMangledNameSourceNlist |
							kCSSymbolMangledNameSourceDwarf |
							kCSSymbolMangledNameSourceDwarfMIPSLinkage |
							kCSSymbolMangledNameValid |
							kCSSymbolMangledNameAllocated |
							kCSSymbolIsObjcMethodValid;

//
// This method returns a pointer to the raw instruction data
// for the symbol. This is the raw data, it is not endian swapped
// or otherwise processed. If the instruction data cannot be
// retrieved, NULL will be returned.
//
const uint8_t* CSSymbolGetInstructionData(CSSymbolRef);

bool CSSymbolIsNList(CSSymbolRef);
bool CSSymbolIsDwarf(CSSymbolRef);
bool CSSymbolIsDebugMap(CSSymbolRef);
bool CSSymbolIsFunctionStarts(CSSymbolRef);
bool CSSymbolIsMerged(CSSymbolRef);

bool CSSymbolIsNameSourceNList(CSSymbolRef);
bool CSSymbolIsNameSourceDwarf(CSSymbolRef);
bool CSSymbolIsNameSourceDwarfMIPSLinkage(CSSymbolRef);
bool CSSymbolIsMangledNameSourceNList(CSSymbolRef);
bool CSSymbolIsMangledNameSourceDwarf(CSSymbolRef);
bool CSSymbolIsMangledNameSourceDwarfMIPSLinkage(CSSymbolRef);

CFStringRef CSSymbolCopyDescriptionWithIndent(CSSymbolRef, uint32_t);

CS_EXTERN_C_END

#endif /* _CSSYMBOL_PRIVATE_H_ */
