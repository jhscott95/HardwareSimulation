/* Implementation of a pipelined cpu simulation.
 *
 * Author: Jacob Scott
 */

#include "sim5.h"

void extract_instructionFields(WORD instruction, InstructionFields *fieldsOut) {
	fieldsOut->opcode 	= instruction >> 26 & 0x3F;
	fieldsOut->rs 		= instruction >> 21 & 0x1F;
	fieldsOut->rt 		= instruction >> 16 & 0x1F;
	fieldsOut->rd 		= instruction >> 11 & 0x1F;
	fieldsOut->shamt	= instruction >> 6  & 0x1F;
	fieldsOut->funct	= instruction       & 0x3F;

	int immediate 		= instruction 		& 0xFFFF; 
	fieldsOut->imm16	= immediate;
	fieldsOut->imm32	= signExtend16to32(immediate);

	fieldsOut->address 	= instruction 		& 0x3FFFFFF;
}

int IDtoIF_get_stall(InstructionFields *fields,
                     ID_EX  *old_idex, EX_MEM *old_exmem) {
	if (old_idex->memRead == 1) {
		if (old_idex->rt == fields->rs || old_idex->rt == fields->rt) {
			return 1;
		}
	} else if (fields->opcode == 43) {
		if (old_exmem->rt == fields->rs) {
			return 1;
		}
	}
	return 0;
}

int IDtoIF_get_branchControl(InstructionFields *fields, WORD rsVal, WORD rtVal) {
	if (fields->opcode == 4 && rsVal == rtVal) {
		return 1;
	}
	if (fields->opcode == 5 && rsVal != rtVal) {
		return 1;
	}
	if (fields->opcode == 2 || fields->opcode == 3) {
		return 2;
	}
	return 0;
}

WORD calc_branchAddr(WORD pcPlus4, InstructionFields *fields) {
	return pcPlus4 + (fields->imm32 << 2);
}

WORD calc_jumpAddr  (WORD pcPlus4, InstructionFields *fields) {
	int bottomAddress = fields->address << 2 & 0x0FFFFFFF;
	int pcBits = pcPlus4 & 0xF0000000;
	return pcBits | bottomAddress;
}

int execute_ID(int IDstall,
               InstructionFields *fieldsIn,
               WORD pcPlus4,
               WORD rsVal, WORD rtVal,
               ID_EX *new_idex) {
	// nop
	if (IDstall == 1) {
		new_idex->rs 			= 0;
		new_idex->rt 			= 0;
		new_idex->rd 			= 0;
		new_idex->rsVal			= 0;
		new_idex->rtVal			= 0;
		new_idex->imm16			= 0;
		new_idex->imm32			= 0;

		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 0;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 0;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 0;
		new_idex->ALU.op 		= 5;
		new_idex->ALU.bNegate 	= 0;

		return 1;
	}

	new_idex->rs 		= fieldsIn->rs;
	new_idex->rt 		= fieldsIn->rt;
	new_idex->rd 		= fieldsIn->rd;
	new_idex->rsVal		= rsVal;
	new_idex->rtVal		= rtVal;
	new_idex->imm16		= fieldsIn->imm16;
	new_idex->imm32		= fieldsIn->imm32;

	// If instruction is an R-Format instruction
	if (fieldsIn->opcode == 0) {
		new_idex->regDst 	= 1;
		new_idex->ALUsrc 	= 0;
		new_idex->memToReg 	= 0;
		new_idex->regWrite 	= 1;
		new_idex->memRead 	= 0;
		new_idex->memWrite 	= 0;

		// add(u) instruction
		if (fieldsIn->funct == 32 || fieldsIn->funct == 33) {
			new_idex->ALU.op 		= 2;
			new_idex->ALU.bNegate 	= 0;
		// sub(u) instruction
		} else if (fieldsIn->funct == 34 || fieldsIn->funct == 35) {
			new_idex->ALU.op 		= 2;
			new_idex->ALU.bNegate 	= 1;
		// and instruction
		} else if (fieldsIn->funct == 36) {
			new_idex->ALU.op 		= 0;
			new_idex->ALU.bNegate 	= 0;
		// or instruction
		} else if (fieldsIn->funct == 37) {
			new_idex->ALU.op 		= 1;
			new_idex->ALU.bNegate 	= 0;
		// xor instruction
		} else if (fieldsIn->funct == 38) {
			new_idex->ALU.op 		= 4;
			new_idex->ALU.bNegate 	= 0;
		// nor instruction
		} else if (fieldsIn->funct == 39) {
			new_idex->ALU.op 		= 1;
			new_idex->ALU.bNegate 	= 0;
		}
		// slt instruction
		else if (fieldsIn->funct == 42) {
			new_idex->ALU.op 		= 3;
			new_idex->ALU.bNegate 	= 1;
		// If funct is invalid, return 0
		} else {
			new_idex->regDst 		= 0;
			new_idex->regWrite 		= 0;
			return 0;
		}
	return 1;
	// j instruction
	} else if (fieldsIn->opcode == 2) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 0;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 0;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 0;
		new_idex->ALU.op 		= 0;
		new_idex->ALU.bNegate 	= 0;
		return 1;
	// beq and bne instructions
	} else if (fieldsIn->opcode == 4 || fieldsIn->opcode == 5) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 0;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 0;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 0;
		new_idex->ALU.op 		= 2;
		new_idex->ALU.bNegate 	= 1;
		return 1;
	// lw instruction
	} else if (fieldsIn->opcode == 35) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 1;
		new_idex->memToReg 		= 1;
		new_idex->regWrite 		= 1;
		new_idex->memRead 		= 1;
		new_idex->memWrite 		= 0;
		new_idex->ALU.op 		= 2;
		new_idex->ALU.bNegate 	= 0;
		return 1;
	// sw instruction
	} else if (fieldsIn->opcode == 43) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 1;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 0;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 1;
		new_idex->ALU.op 		= 2;
		new_idex->ALU.bNegate 	= 0;
		return 1;
	// addi(u) instruction
	} else if (fieldsIn->opcode == 8 || fieldsIn->opcode == 9) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 1;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 1;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 0;
		new_idex->ALU.op 		= 2;
		new_idex->ALU.bNegate 	= 0;
		return 1;
	// slti(u) instruction
	} else if (fieldsIn->opcode == 10 || fieldsIn->opcode == 11) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 1;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 1;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 0;
		new_idex->ALU.op 		= 3;
		new_idex->ALU.bNegate 	= 1;
		return 1;
	// andi instruction
	} else if (fieldsIn->opcode == 12) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 2;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 1;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 0;
		new_idex->ALU.op 		= 0;
		new_idex->ALU.bNegate 	= 0;
		return 1;
	// ori instruction
	} else if (fieldsIn->opcode == 13) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 2;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 1;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 0;
		new_idex->ALU.op 		= 1;
		new_idex->ALU.bNegate 	= 0;
		return 1;
	// lui instruction
	} else if (fieldsIn->opcode == 15) {
		new_idex->regDst 		= 0;
		new_idex->ALUsrc 		= 1;
		new_idex->memToReg 		= 0;
		new_idex->regWrite 		= 1;
		new_idex->memRead 		= 0;
		new_idex->memWrite 		= 0;
		// TODO Double check alu op to see what to set this to.
		// Should resemble a shift operation
		new_idex->ALU.op 		= 6;
		new_idex->ALU.bNegate 	= 0;
		return 1;
	} else {
		return 0;
	}


}

WORD EX_getALUinput1(ID_EX *in, EX_MEM *old_exMem, MEM_WB *old_memWb) {

}

WORD EX_getALUinput2(ID_EX *in, EX_MEM *old_exMem, MEM_WB *old_memWb) {

}

void execute_EX(ID_EX *in, WORD input1, WORD input2,
                EX_MEM *new_exMem) {

}

void execute_MEM(EX_MEM *in, MEM_WB *old_memWb,
                 WORD *mem, MEM_WB *new_memwb) {

}

void execute_WB (MEM_WB *in, WORD *regs) {

}