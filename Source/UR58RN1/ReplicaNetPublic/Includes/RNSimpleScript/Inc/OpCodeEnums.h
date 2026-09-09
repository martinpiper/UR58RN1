/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __OPCODEENUMS_H__
#define __OPCODEENUMS_H__
// Also update CommandEnums.h if needed. There isn't a strictly one to one mapping between commands and opcodes.

typedef unsigned char tSS_OpCode;
typedef unsigned char tSS_Register;

const int kSimpleScriptThreadRegisters = 32;

// Op codes get saved in to the binary file and emulated.
// Where Dst if >= 0 is the destination register, in the form rx
// Where Src if >= 0 is the source register, in the form rx
// Reg0 is always zero
// Registers are either floating point or integer depending on the last assignment made to them
// Val is always an immediate value in the form #
// Otherwise a label is assumed and searched for
// Offsets for branch and pushPC are always done from the current instruction address.

// Push and Pop register values using the stack

// PushPCVal pushes the current PC plus val on to the stack.
// Ret returns by popping the address from the stack
// Procedures have values passed in registers from reg1
// Procedures always return one register in reg1. That is to say the register contains whatever value it last contained.

// Branch is relative
// The branch flags are based on the last comparison, regardless of when it was done.
// "Branch EQ PP label"   will branch to a label if EQ and push the PC return value for the next instruction on to the stack before doing the branch

// AsmExternal is an external function call

// The Get commands look in the data following the instruction to get values from the program

// Threads with any signal flags enabled will be in a pause state until all signal flags are clear.

// Be careful when removing enums or adding enums in the middle of this list as this will cause previously compiled
// scripts to suddenly stop working.
enum kOpCode {
		kOpCodeNOP = 0,						// Done
		kOpCodeAdd_DstSrc1Src2,				// Done
		kOpCodeSub_DstSrc1Src2,				// Done
		kOpCodeMul_DstSrc1Src2,				// Done
		kOpCodeDiv_DstSrc1Src2,				// Done
		kOpCodeGetInt_DstVal,				// Done
		kOpCodeGetFloat_DstVal,				// Done
		kOpCodeGetNDataPointer_DstLenNData,	// Done
		kOpCodePush_Src,					// Done
		kOpCodePop_Dst,						// Done
//10
		kOpCodeCmp_Src1Src2,				// Done
		kOpCodePushPCVal,					// Done
		kOpCodeRet,							// Done
		kOpCodeBranchPP,					// Done
		kOpCodeBranchPP_EQ,					// Done
		kOpCodeBranchPP_NE,					// Done
		kOpCodeBranchPP_GT,					// Done
		kOpCodeBranchPP_LT,					// Done
		kOpCodeBranchPP_GE,					// Done
		kOpCodeBranchPP_LE,					// Done
//20
		kOpCodeBranch,						// Done
		kOpCodeBranch_EQ,					// Done
		kOpCodeBranch_NE,					// Done
		kOpCodeBranch_GT,					// Done
		kOpCodeBranch_LT,					// Done
		kOpCodeBranch_GE,					// Done
		kOpCodeBranch_LE,					// Done
		kOpCodeExternal_Val,				// Done

		kOpCodeAddr_DstOffset,				// Done
		kOpCodeLoadRegInt_DstSrc,			// Done
//30
		kOpCodeUnusedXXXXX,
		kOpCodeLoadRegFloat_DstSrc,			// Done
		kOpCodeUnusedXXXXXX,

		kOpCodeLoadReg_DstSrc,				// Done
		kOpCodeStoreReg_DstSrc,				// Done

		kOpCodeStoreReg32_DstSrc,			// Done
		kOpCodeLsl_DstSrc1Src2,				// Done
		kOpCodeLsr_DstSrc1Src2,				// Done
		kOpCodeAsr_DstSrc1Src2,				// Done
		kOpCodeRor_DstSrc1Src2,				// Done

//40
		kOpCodeIDiv_DstSrc1Src2,			// Done

		kOpCodeAnd_DstSrc1Src2,				// Done
		kOpCodeEor_DstSrc1Src2,				// Done
		kOpCodeNot_Src,						// Done

		kOpCodeFloatToInt_Src,				// Done
		kOpCodeIntToFloat_Src,				// Done
		kOpCodeSetFP_Src,					// Done
		kOpCodeCastFP_Src,					// Done
		kOpCodeCastInt_Src					// Done
	};

#endif
