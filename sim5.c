/* Implementation of a pipelined cpu simulation.
 *
 * Author: Jacob Scott
 */

#include "sim5.h"

void extract_instructionFields(WORD instruction, InstructionFields *fieldsOut) {

}

int IDtoIF_get_stall(InstructionFields *fields,
                     ID_EX  *old_idex, EX_MEM *old_exmem) {

}

int IDtoIF_get_branchControl(InstructionFields *fields, WORD rsVal, WORD rtVal) {

}

WORD calc_branchAddr(WORD pcPlus4, InstructionFields *fields) {

}

WORD calc_jumpAddr  (WORD pcPlus4, InstructionFields *fields) {

}

int execute_ID(int IDstall,
               InstructionFields *fieldsIn,
               WORD pcPlus4,
               WORD rsVal, WORD rtVal,
               ID_EX *new_idex) {

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