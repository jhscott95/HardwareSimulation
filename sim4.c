/* Implementation of a single-cycle cpu simulation in C.
 *
 * Author: Jacob Scott
 */

#include "sim4.h"

WORD getInstruction(WORD curPC, WORD *instructionMemory) {
	return instructionMemory[curPC / 4];
}

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

int  fill_CPUControl(InstructionFields *fields, CPUControl *controlOut) {
	// If instruction is an R-Format instruction
	if (fields->opcode == 0) {
		controlOut->regDst 		= 1;
		controlOut->ALUsrc 		= 0;
		controlOut->memToReg 	= 0;
		controlOut->regWrite 	= 1;
		controlOut->memRead 	= 0;
		controlOut->memWrite 	= 0;
		controlOut->branch 		= 0;
		controlOut->jump 		= 0;

		// sll instruction from funct field
		if (fields->funct == 0) {
			controlOut->ALU.op 		= 5;
			controlOut->ALU.bNegate = 0;
			// Storing shift amount to use in execute_ALU()
			controlOut->extra1		= fields->shamt;
		// add(u) instruction
		} else if (fields->funct == 32 || fields->funct == 33) {
			controlOut->ALU.op 		= 2;
			controlOut->ALU.bNegate = 0;
		// sub(u) instruction
		} else if (fields->funct == 34 || fields->funct == 35) {
			controlOut->ALU.op 		= 2;
			controlOut->ALU.bNegate = 1;
		// and instruction
		} else if (fields->funct == 36) {
			controlOut->ALU.op 		= 0;
			controlOut->ALU.bNegate = 0;
		// or instruction
		} else if (fields->funct == 37) {
			controlOut->ALU.op 		= 1;
			controlOut->ALU.bNegate = 0;
		// xor instruction
		} else if (fields->funct == 38) {
			controlOut->ALU.op 		= 4;
			controlOut->ALU.bNegate = 0;
		// slt instruction
		} else if (fields->funct == 42) {
			controlOut->ALU.op 		= 3;
			controlOut->ALU.bNegate = 1;
		// If funct is invalid, return 0
		} else {
			controlOut->regDst 		= 0;
			controlOut->regWrite 	= 0;
			return 0;
		}
	return 1;
	// j instruction
	} else if (fields->opcode == 2) {
		controlOut->regDst 		= 0;
		controlOut->ALUsrc 		= 0;
		controlOut->memToReg 	= 0;
		controlOut->regWrite 	= 0;
		controlOut->memRead 	= 0;
		controlOut->memWrite 	= 0;
		controlOut->branch 		= 0;
		controlOut->jump 		= 1;
		controlOut->ALU.op 		= 0;
		controlOut->ALU.bNegate = 0;
		return 1;
	// beq and bne instructions
	} else if (fields->opcode == 4 || fields->opcode == 5) {
		controlOut->regDst 		= 0;
		controlOut->ALUsrc 		= 0;
		controlOut->memToReg 	= 0;
		controlOut->regWrite 	= 0;
		controlOut->memRead 	= 0;
		controlOut->memWrite 	= 0;
		controlOut->branch 		= 1;
		controlOut->jump 		= 0;
		controlOut->ALU.op 		= 2;
		controlOut->ALU.bNegate = 1;
		return 1;
	// lw instruction
	} else if (fields->opcode == 35) {
		controlOut->regDst 		= 0;
		controlOut->ALUsrc 		= 1;
		controlOut->memToReg 	= 1;
		controlOut->regWrite 	= 1;
		controlOut->memRead 	= 1;
		controlOut->memWrite 	= 0;
		controlOut->branch 		= 0;
		controlOut->jump 		= 0;
		controlOut->ALU.op 		= 2;
		controlOut->ALU.bNegate = 0;
		return 1;
	// sw instruction
	} else if (fields->opcode == 43) {
		controlOut->regDst 		= 0;
		controlOut->ALUsrc 		= 1;
		controlOut->memToReg 	= 0;
		controlOut->regWrite 	= 0;
		controlOut->memRead 	= 0;
		controlOut->memWrite 	= 1;
		controlOut->branch 		= 0;
		controlOut->jump 		= 0;
		controlOut->ALU.op 		= 2;
		controlOut->ALU.bNegate = 0;
		return 1;
	// addi(u) instruction
	} else if (fields->opcode == 8 || fields->opcode == 9) {
		controlOut->regDst 		= 0;
		controlOut->ALUsrc 		= 1;
		controlOut->memToReg 	= 0;
		controlOut->regWrite 	= 1;
		controlOut->memRead 	= 0;
		controlOut->memWrite 	= 0;
		controlOut->branch 		= 0;
		controlOut->jump 		= 0;
		controlOut->ALU.op 		= 2;
		controlOut->ALU.bNegate = 0;
		return 1;
	// slti(u) instruction
	} else if (fields->opcode == 10 || fields->opcode == 11) {
		controlOut->regDst 		= 0;
		controlOut->ALUsrc 		= 1;
		controlOut->memToReg 	= 0;
		controlOut->regWrite 	= 1;
		controlOut->memRead 	= 0;
		controlOut->memWrite 	= 0;
		controlOut->branch 		= 0;
		controlOut->jump 		= 0;
		controlOut->ALU.op 		= 3;
		controlOut->ALU.bNegate = 1;
		return 1;
	// andi instruction
	} else if (fields->opcode == 12) {
		controlOut->regDst 		= 0;
		controlOut->ALUsrc 		= 1;
		controlOut->memToReg 	= 0;
		controlOut->regWrite 	= 1;
		controlOut->memRead 	= 0;
		controlOut->memWrite 	= 0;
		controlOut->branch 		= 0;
		controlOut->jump 		= 0;
		controlOut->ALU.op 		= 0;
		controlOut->ALU.bNegate = 0;
		return 1;
	} else {
		return 0;
	}
}

WORD getALUinput1(CPUControl *controlIn,
                  InstructionFields *fieldsIn,
                  WORD rsVal, WORD rtVal, WORD reg32, WORD reg33,
                  WORD oldPC) {
	return rsVal;
}

WORD getALUinput2(CPUControl *controlIn,
                  InstructionFields *fieldsIn,
                  WORD rsVal, WORD rtVal, WORD reg32, WORD reg33,
                  WORD oldPC) {
	if (controlIn->ALUsrc == 0) {
		return rtVal;
	// zero extending for andi instruction
	} else if (controlIn->ALU.op == 0 || controlIn->ALU.op == 1) {
		return fieldsIn->imm16 | 0x00000000;
	} else {
		return fieldsIn->imm32;
	}
}

void execute_ALU(CPUControl *controlIn,
                 WORD input1, WORD input2,
                 ALUResult  *aluResultOut) {
	// and two inputs
	if (controlIn->ALU.op == 0) {
		aluResultOut->result = input1 & input2;
	// or two inputs
	} else if (controlIn->ALU.op == 1) {
		aluResultOut->result = input1 | input2;
	// add two inputs
	} else if (controlIn->ALU.op == 2 && controlIn->ALU.bNegate == 0) {
		aluResultOut->result = input1 + input2;
	// sub two inputs
	} else if (controlIn->ALU.op == 2 && controlIn->ALU.bNegate == 1) {
		aluResultOut->result = input1 - input2;
	// less two inputs
	} else if (controlIn->ALU.op == 3) {
		if (input1 < input2) {
            aluResultOut->result = 1;
        } else {
            aluResultOut->result = 0;
        }
    // xor two inputs
	} else if (controlIn->ALU.op == 4) {
		aluResultOut->result = input1 ^ input2;
	// sll rt register by shamt
	} else if (controlIn->ALU.op == 5) {
		aluResultOut->result = input2 << controlIn->extra1;
	}

	// set zero output
	if (aluResultOut->result == 0) {
		aluResultOut->zero = 1;
	} else {
		aluResultOut->zero = 0;
	}
}

void execute_MEM(CPUControl *controlIn,
                 ALUResult  *aluResultIn,
                 WORD        rsVal, WORD rtVal,
                 WORD       *memory,
                 MemResult  *resultOut) {
	if (controlIn->memRead == 0 && controlIn->memWrite == 0) {
		resultOut->readVal = 0;
	} 
	// lw instruction
	if (controlIn->memRead == 1) {
		resultOut->readVal = memory[aluResultIn->result / 4];
	}
	// sw instruction
	if (controlIn->memWrite == 1) {
		memory[aluResultIn->result / 4] = rtVal;
	}
}

WORD getNextPC(InstructionFields *fields, CPUControl *controlIn, int aluZero,
               WORD rsVal, WORD rtVal,
               WORD oldPC) {
	if (controlIn->branch == 0 && controlIn->jump == 0) {
		return oldPC + 4;
	}
	// jump instruction
	if (controlIn->jump == 1) {
		// Shift to get in words and mask the top 4 bits
		int bottomAddress = fields->address << 2 & 0x0FFFFFFF;
		// Masking to only get top 4 bits of old PC address
		int pcBits = oldPC & 0xF0000000;
		// Or-ing the two to get the final address
		return pcBits | bottomAddress;
	}
	// beq instruction checking zero output from subtraction
	if (controlIn->branch == 1 && aluZero == 1 && fields->opcode == 4) {
		return oldPC + 4 + (fields->imm32 << 2);
	// bne instruction checking zero output to see if vals are not equal
	} else if (controlIn->branch == 1 && aluZero == 0 && fields->opcode == 5) { 
		return oldPC + 4 + (fields->imm32 << 2);
	}else {
		return oldPC + 4;
	}
}

void execute_updateRegs(InstructionFields *fields, CPUControl *controlIn,
                        ALUResult  *aluResultIn, MemResult *memResultIn,
                        WORD       *regs) {
	if (controlIn->regWrite == 1) {
		// R-Format
		if (controlIn->regDst == 1) {
			regs[fields->rd] = aluResultIn->result;
		// lw instruction
		} else if (controlIn->memToReg == 1) {
			regs[fields->rt] = memResultIn->readVal;
		} else {
			regs[fields->rt] = aluResultIn->result;
		}
	}
}