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
// #include "free_list.h"
// #include "issue_queue.h"
// #include "load_store_queue.h"
// #include "reorder_buffer.h"

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
    // int rs1_value;
    // int rs2_value;
    int result_buffer;
    int memory_address;
    int has_insn;

    int regs_renamed;
    int pd;
    int ps1;
    int ps2;
    int ps1_value;
    int ps2_value;
} CPU_Stage;

/* Format of an IQ Entry  */
typedef struct IQ_Entry
{
    int status;
    int FU_type;
    int imm;
    int src1_ready_bit;
    int src1_tag;
    int src1_value;
    int src2_ready_bit;
    int src2_tag;
    int src2_value;
    int dest_reg_or_lsq_index;
    struct IQ_Entry *next;
} IQ_Entry;

/* Format of an LSQ Entry  */
typedef struct LSQ_Entry
{
    int status;
    int load_or_store;
    int mem_add_is_valid;
    int dest_reg_for_load;
    int src1_ready_bit;
    int src1_tag;
    int src1_value;
    // struct LSQ_Entry *next;
} LSQ_Entry;

/* Format of an ROB Entry  */
typedef struct ROB_Entry
{
    int pc_value;
    int arch_dest_reg;
    int result;
    int store_value;
    int store_value_valid;
    int exception_codes;
    int res_mem_add_status;
    int instr_type;
} ROB_Entry;

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
    int reg_is_renamed[REG_FILE_SIZE];
    int rename_table_ccr;                           /* Rename Table Condition Code Register */

    int phy_regs[PHY_REG_FILE_SIZE];                /* Physical register file */
    int phy_regs_zero_flags[PHY_REG_FILE_SIZE];         /* Zero flags for Physical register file */
    int phy_regs_positive_flags[PHY_REG_FILE_SIZE];     /* Positive flags for Physical register file */

    int free_list[PHY_REG_FILE_SIZE];     /* Free list for Physical register file */
    int free_list_front;
    int free_list_rear;

    // IQ_Entry *issue_queue;
    int current_iq_size;
    IQ_Entry *iq_head;
    IQ_Entry *iq_current;

    LSQ_Entry load_store_queue[LOAD_STORE_QUEUE_SIZE];
    int lsq_front;
    int lsq_rear;

    ROB_Entry reorder_buffer[REORDER_BUFFER_SIZE];
    int rob_front;
    int rob_rear;
    
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
    CPU_Stage iq_stage;
    // CPU_Stage execute;
    CPU_Stage intFU;
    CPU_Stage mulFU;
    CPU_Stage branchFU;
    CPU_Stage memory;
    CPU_Stage writeback;
} APEX_CPU;

APEX_Instruction *create_code_memory(const char *filename, int *size);
APEX_CPU *APEX_cpu_init(const char *filename);
void APEX_cpu_run(APEX_CPU *cpu);
void APEX_cpu_stop(APEX_CPU *cpu);
#endif
