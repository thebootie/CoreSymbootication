/*
 *  CSArchitecture.h
 *  CoreSymbolication
 *
 *  Created by James McIlree on 12/17/07.
 *  Copyright 2007 Apple Inc.. All rights reserved.
 *
 */

#ifndef _CSARCHITECTURE_H_
#define _CSARCHITECTURE_H_ 1

#include <mach/mach.h>

CS_EXTERN_C_BEGIN

struct _CSArchitecture {
    cpu_type_t		cpu_type;
    cpu_subtype_t	cpu_subtype;
};

typedef struct _CSArchitecture CSArchitecture;

static const CSArchitecture kCSArchitectureI386 = { CPU_TYPE_I386, CPU_SUBTYPE_I386_ALL };
static const CSArchitecture kCSArchitectureX86_64 = { CPU_TYPE_I386|CPU_ARCH_ABI64, CPU_SUBTYPE_I386_ALL }; // "Generic" 64-bit x86 (Note that CPU_SUBTYPE_I386_ALL == CPU_SUBTYPE_X86_64_ALL)
static const CSArchitecture kCSArchitectureX86_64h = { CPU_TYPE_I386|CPU_ARCH_ABI64, CPU_SUBTYPE_X86_64_H };
static const CSArchitecture kCSArchitectureArm = { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_ALL }; // "Generic" arm
static const CSArchitecture kCSArchitectureArmV4T = { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_V4T };
static const CSArchitecture kCSArchitectureArmV5TEJ = { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_V5TEJ };
static const CSArchitecture kCSArchitectureXScale = { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_XSCALE };
static const CSArchitecture kCSArchitectureArmV6 = { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_V6 };
static const CSArchitecture kCSArchitectureArmV7 = { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_V7 };
static const CSArchitecture kCSArchitectureArmV7F =  { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_V7F };
static const CSArchitecture kCSArchitectureArmV7S =  { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_V7S };
static const CSArchitecture kCSArchitectureArmV7K =  { CPU_TYPE_ARM, CPU_SUBTYPE_ARM_V7K };
static const CSArchitecture kCSArchitectureArm64 =   { CPU_TYPE_ARM | CPU_ARCH_ABI64, CPU_SUBTYPE_ARM64_ALL };
static const CSArchitecture kCSArchitectureArm64V8 = { CPU_TYPE_ARM | CPU_ARCH_ABI64, CPU_SUBTYPE_ARM64_V8 };
#if !defined(CPU_ARCH_ABI64_32)
#define CPU_ARCH_ABI64_32 0x2000000
#endif
#if !defined(CPU_SUBTYPE_ARM64_32_V8)
#define CPU_SUBTYPE_ARM64_32_V8 ((cpu_subtype_t) 1)
#endif
static const CSArchitecture kCSArchitectureArm64_32 =   { CPU_TYPE_ARM | CPU_ARCH_ABI64_32, CPU_SUBTYPE_ARM64_ALL };
static const CSArchitecture kCSArchitectureArm64_32V8 = { CPU_TYPE_ARM | CPU_ARCH_ABI64_32, CPU_SUBTYPE_ARM64_32_V8 };
#ifndef RC_HIDE_HARDWARE_LATE_FALL_2018_IOS
#if !defined(CPU_SUBTYPE_ARM64_E)
#define CPU_SUBTYPE_ARM64_E 2
#endif
static const CSArchitecture kCSArchitectureArm64E = { CPU_TYPE_ARM | CPU_ARCH_ABI64, CPU_SUBTYPE_ARM64_E };
#endif

static const CSArchitecture kCSArchitectureAny = { CPU_TYPE_ANY, CPU_SUBTYPE_MULTIPLE };
static const CSArchitecture kCSArchitectureUnknown = { 0, 0 };

#define CPU_TYPE_ANY_64_BIT ((cpu_type_t) -64)
static const CSArchitecture kCSArchitectureAny64Bit = { CPU_TYPE_ANY_64_BIT, CPU_SUBTYPE_MULTIPLE };

CSArchitecture CSArchitectureGetCurrent(void);

bool CSArchitectureIsI386(CSArchitecture);
bool CSArchitectureIsX86_64(CSArchitecture);
bool CSArchitectureIsArm(CSArchitecture);
bool CSArchitectureIsArm64(CSArchitecture);

// Note that these functions query hardware support.  On a 64-bit
// computer, passing CSArchitectureGetCurrent() to either of these
// functions will indicate that the architecture is 64-bit even if the
// current process is 32-bit.  (CSArchitectureGetCurrent() uses the
// "hw.cpu64bit_capable" sysctl to determine if the current
// architecture is 32-bit or 64-bit.)
bool CSArchitectureIs32Bit(CSArchitecture);
bool CSArchitectureIs64Bit(CSArchitecture);

bool CSArchitectureIsLittleEndian(CSArchitecture);
bool CSArchitectureIsBigEndian(CSArchitecture);

CS_INLINE CSArchitecture CSArchitectureMake(cpu_type_t type, cpu_subtype_t subtype) {
    CSArchitecture arch;
    arch.cpu_type = type;
    arch.cpu_subtype = subtype;
    return arch;
}

// Returns true if the two architectures match, taking wildcards into account.
// To test for exact equality, use CSArchitectureEqualsArchitecture()
bool CSArchitectureMatchesArchitecture(CSArchitecture, CSArchitecture);

// Returns true if the architectures are an exact match.
CS_INLINE bool CSArchitectureEqualsArchitecture(CSArchitecture a1, CSArchitecture a2) {
    return (a1.cpu_type == a2.cpu_type) && (a1.cpu_subtype == a2.cpu_subtype);
}

// Returns kCSArchitectureI386 for any 32 bit intel variant.
// Returns kCSArchitectureX86_64 for any 64 bit intel variant.
// etc...
CSArchitecture CSArchitectureGetFamily(CSArchitecture arch);

// Returns "i386", "x86_64", "arm", "armv*" (several arm32-specific variants), "arm64" or "unknown"
const char* CSArchitectureGetFamilyName(CSArchitecture);

// Returns an architecture matching the string.
// This is the inverse of CSArchitectureGetFamilyName.
// If the input string is unrecognized, returns kCSArchitectureUnknown
CSArchitecture CSArchitectureGetArchitectureForName(const char*);

CS_EXTERN_C_END

#endif /* _CSARCHITECTURE_H_ */
