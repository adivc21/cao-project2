/*
 * apex_cpu.h
 * Contains APEX cpu pipeline declarations
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_

#include "apex_macros.h"

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
    char opcode_str[128];
    int opcode;
    int rd;
    int rs1;
    int rs2;
    int imm;
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
    int pc;
    char opcode_str[128];
    int opcode;
    int rs1;
    int rs2;
    int rd;
    int imm;
    int rs1_value;
    int rs2_value;
    int result_buffer;
    int memory_address;
    int has_insn;
    
    int renamed;
    int pd;
    int ps1;
    int ps2;
} CPU_Stage;


/* Model of APEX CPU */
typedef struct APEX_CPU
{
    int pc;                        /* Current program counter */
    int clock;                     /* Clock cycles elapsed */
    int insn_completed;            /* Instructions retired */
    
    int regs[REG_FILE_SIZE];                        /* Integer register file */
    int regs_zero_flags[REG_FILE_SIZE];             /* Zero flags for Integer register file */
    int regs_positive_flags[REG_FILE_SIZE];         /* Positive flags for Integer register file */

    int rename_table[REG_FILE_SIZE];                /* Rename Table for Integer register file */
    int rename_table_ccr;                           /* Rename Table Condition Code Register */

    int phy_regs[PHY_REG_FILE_SIZE];                /* Physical register file */
    int phy_regs_zero_flags[PHY_REG_FILE_SIZE];         /* Zero flags for Physical register file */
    int phy_regs_positive_flags[PHY_REG_FILE_SIZE];     /* Positive flags for Physical register file */

    int free_list[PHY_REG_FILE_SIZE];     /* Free list for Physical register file */
    int free_list_front;
    int free_list_rear;

    int code_memory_size;          /* Number of instruction in the input file */
    APEX_Instruction *code_memory; /* Code Memory */
    int data_memory[DATA_MEMORY_SIZE]; /* Data Memory */
    int single_step;               /* Wait for user input after every cycle */
    int zero_flag;                 /* {TRUE, FALSE} Used by BZ and BNZ to branch */
    int fetch_from_next_cycle;

    int is_int_unit_free;
    int is_mul_unit_free;
    int is_branch_unit_free;

    /* Pipeline stages */
    CPU_Stage fetch;
    /* Split the CPU stage 'decode' into two CPU stages, 'dr1' and 'r2d',
	for the stages 'Decode/Rename 1' and 'Rename 2/Dispatch'
	respectively. */
    CPU_Stage dr1;
    CPU_Stage r2d;
    CPU_Stage execute;
    CPU_Stage memory;
    CPU_Stage writeback;
} APEX_CPU;

APEX_Instruction *create_code_memory(const char *filename, int *size);
APEX_CPU *APEX_cpu_init(const char *filename);
void APEX_cpu_run(APEX_CPU *cpu);
void APEX_cpu_stop(APEX_CPU *cpu);
#endif
