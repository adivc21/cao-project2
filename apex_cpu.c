/*
 * apex_cpu.c
 * Contains APEX cpu pipeline implementation
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"
#include "apex_macros.h"
#include "free_list.h"
#include "issue_queue.h"
#include "load_store_queue.h"
#include "reorder_buffer.h"

/* Converts the PC(4000 series) into array index for code memory
 *
 * Note: You are not supposed to edit this function
 */
static int
get_code_memory_index_from_pc(const int pc)
{
    return (pc - 4000) / 4;
}

static void
print_instruction(const CPU_Stage *stage)
{
    if (!stage->regs_renamed)
    {
        switch (stage->opcode)
        {
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_MUL:
            case OPCODE_DIV:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_XOR:
            {
                printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->rs2);
                break;
            }

            case OPCODE_ADDL:
            case OPCODE_SUBL:
            {
                printf("%s,R%d,R%d,%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->imm);
                break;
            }

            case OPCODE_MOVC:
            {
                printf("%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
                break;
            }

            case OPCODE_LOAD:
            {
                printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->imm);
                break;
            }

            case OPCODE_STORE:
            {
                printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
                    stage->imm);
                break;
            }

            case OPCODE_BZ:
            case OPCODE_BNZ:
            {
                printf("%s,#%d ", stage->opcode_str, stage->imm);
                break;
            }

            case OPCODE_HALT:
            {
                printf("%s", stage->opcode_str);
                break;
            }
        }
    }
    else 
    {
        switch (stage->opcode)
        {
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_MUL:
            case OPCODE_DIV:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_XOR:
            {
                printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->rs2);
                break;
            }

            case OPCODE_MOVC:
            {
                printf("%s,R%d,#%d -> %s,P%d,#%d", stage->opcode_str, stage->rd, stage->imm,
                    stage->opcode_str, stage->pd, stage->imm);
                break;
            }

            case OPCODE_LOAD:
            {
                printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                    stage->imm);
                break;
            }

            case OPCODE_STORE:
            {
                printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
                    stage->imm);
                break;
            }

            case OPCODE_BZ:
            case OPCODE_BNZ:
            {
                printf("%s,#%d ", stage->opcode_str, stage->imm);
                break;
            }

            case OPCODE_HALT:
            {
                printf("%s", stage->opcode_str);
                break;
            }
        }
    }
}

/* Debug function which prints the CPU stage content
 *
 * Note: You can edit this function to print in more detail
 */
static void
print_stage_content(const char *name, const CPU_Stage *stage)
{
    printf("%-15s: pc(%d) ", name, stage->pc);
    print_instruction(stage);
    printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_reg_file(const APEX_CPU *cpu)
{
    int i;

    printf("----------\n%s\n----------\n", "Registers:");

    for (int i = 0; i < REG_FILE_SIZE / 2; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");

    for (i = (REG_FILE_SIZE / 2); i < REG_FILE_SIZE; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_phy_reg_file(const APEX_CPU *cpu)
{
    int i;

    printf("----------\n%s\n----------\n", "Registers:");

    for (int i = 0; i < PHY_REG_FILE_SIZE / 2; ++i)
    {
        printf("P%-3d[%-3d] ", i, cpu->phy_regs[i]);
    }

    printf("\n");

    for (i = (PHY_REG_FILE_SIZE / 2); i < PHY_REG_FILE_SIZE; ++i)
    {
        printf("P%-3d[%-3d] ", i, cpu->phy_regs[i]);
    }

    printf("\n");
}

/* Debug function which prints the renamed registers
 */
static void
print_rename_table(const APEX_CPU *cpu)
{
    printf("----------\n%s\n----------\n", "Rename Table:");

    for (int i = 0; i < REG_FILE_SIZE; ++i)
    {
        if (cpu->reg_is_renamed[i])
        {
            printf("R%-3d -> P%-3d ", i, cpu->rename_table[i]);
        }
    }

    printf("\n");

}

/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_fetch(APEX_CPU *cpu)
{
    APEX_Instruction *current_ins;

    if (cpu->fetch.has_insn)
    {
        /* This fetches new branch target instruction from next cycle */
        if (cpu->fetch_from_next_cycle == TRUE)
        {
            cpu->fetch_from_next_cycle = FALSE;

            /* Skip this cycle*/
            return;
        }

        /* Store current PC in fetch latch */
        cpu->fetch.pc = cpu->pc;

        /* Index into code memory using this pc and copy all instruction fields
         * into fetch latch  */
        current_ins = &cpu->code_memory[get_code_memory_index_from_pc(cpu->pc)];
        strcpy(cpu->fetch.opcode_str, current_ins->opcode_str);
        cpu->fetch.opcode = current_ins->opcode;
        cpu->fetch.rd = current_ins->rd;
        cpu->fetch.rs1 = current_ins->rs1;
        cpu->fetch.rs2 = current_ins->rs2;
        cpu->fetch.imm = current_ins->imm;
        cpu->fetch.regs_renamed = 0;

        /* Update PC for next instruction */
        cpu->pc += 4;

        /* Copy data from fetch latch to dr1 latch*/
        cpu->dr1 = cpu->fetch;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Fetch", &cpu->fetch);
        }

        /* Stop fetching new instructions if HALT is fetched */
        if (cpu->fetch.opcode == OPCODE_HALT)
        {
            cpu->fetch.has_insn = FALSE;
        }
    }
}

/*
 * Decode/Rename 1 Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_dr1(APEX_CPU *cpu)
{
    if (cpu->dr1.has_insn)
    {
        /* Read operands from register file based on the instruction type */
        switch (cpu->dr1.opcode)
        {
            // case OPCODE_ADD:
            // {
            //     cpu->dr1.rs1_value = cpu->regs[cpu->dr1.rs1];
            //     cpu->dr1.rs2_value = cpu->regs[cpu->dr1.rs2];
            //     break;
            // }

            // case OPCODE_LOAD:
            // {
            //     cpu->dr1.rs1_value = cpu->regs[cpu->dr1.rs1];
            //     break;
            // }

            case OPCODE_ADDL:
            case OPCODE_SUBL:
            {
                if(isIQFull(cpu) || isROBFull(cpu))
                {
                    cpu->fetch_from_next_cycle = TRUE;
                    return;
                }
                
                cpu->dr1.ps1=cpu->rename_table[cpu->dr1.rs1];

                int freeReg = getPhysicalRegister(cpu);
                if (freeReg != -1)
                {
                    // cpu->dr1.renamed = 1;
                    cpu->dr1.pd = freeReg;
                    cpu->rename_table[cpu->dr1.rd] = cpu->dr1.pd;
                    // printf("\nFree Physical Register: P%d\n", cpu->dr1.pd);
                }
                
                break;
            }

            case OPCODE_MOVC:
            {
                if(isIQFull(cpu) || isROBFull(cpu))
                {
                    cpu->fetch_from_next_cycle = TRUE;
                    return;
                }
                /* MOVC doesn't have register operands */
                int freeReg = getPhysicalRegister(cpu);
                if (freeReg != -1)
                {
                    // cpu->dr1.renamed = 1;
                    cpu->dr1.pd = freeReg;
                    cpu->rename_table[cpu->dr1.rd] = cpu->dr1.pd;
                    // printf("\nFree Physical Register: P%d\n", cpu->dr1.pd);
                }
                
                break;
            }

            case OPCODE_MUL:
            {
                if(isIQFull(cpu) || isROBFull(cpu))
                {
                    cpu->fetch_from_next_cycle = TRUE;
                    return;
                }
                
                cpu->dr1.ps1=cpu->rename_table[cpu->dr1.rs1];
                cpu->dr1.ps2=cpu->rename_table[cpu->dr1.rs2];


                int freeReg = getPhysicalRegister(cpu);
                if (freeReg != -1)
                {
                    // cpu->dr1.renamed = 1;
                    cpu->dr1.pd = freeReg;
                    cpu->rename_table[cpu->dr1.rd] = cpu->dr1.pd;
                    // printf("\nFree Physical Register: P%d\n", cpu->dr1.pd);
                }
                
                break;
            }

            default:
                break;
        }

        // /* Copy data from dr1 latch to execute latch*/
        // cpu->execute = cpu->dr1;
        /* Copy data from dr1 latch to execute latch*/
        cpu->r2d = cpu->dr1;
        cpu->dr1.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("DR1/RF", &cpu->dr1);
        }
    }
}

/*
 * Rename 2/Dispatch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_r2d(APEX_CPU *cpu)
{
    if (cpu->r2d.has_insn)
    {
        /* Read operands from register file based on the instruction type */
        switch (cpu->r2d.opcode)
        {
            // case OPCODE_ADD:
            // {
            //     cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
            //     cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
            //     break;
            // }

            // case OPCODE_LOAD:
            // {
            //     cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
            //     break;
            // }


            case OPCODE_ADDL:
            case OPCODE_SUBL:
            {
                cpu->reg_is_renamed[cpu->r2d.rd] = 1;
                cpu->r2d.regs_renamed = 1;

                cpu->is_phy_reg_valid[cpu->r2d.pd] = INVALID;

                IQ_Entry *iq_entry = (IQ_Entry *)calloc(1, sizeof(IQ_Entry));
                // IQ_Entry *iq_entry = malloc(sizeof(IQ_Entry));
                iq_entry->pc_value = cpu->r2d.pc;
                iq_entry->status=1;
                iq_entry->opcode = cpu->r2d.opcode;
                iq_entry->FU_type = INT_FU;
                iq_entry->imm = cpu->r2d.imm;
                
                // printf("\nPhy reg P%d validity: %d\n", cpu->r2d.ps1, cpu->is_phy_reg_valid[cpu->r2d.ps1]);
                if (cpu->is_phy_reg_valid[cpu->r2d.ps1])
                {
                    cpu->r2d.ps1_value = cpu->phy_regs[cpu->r2d.ps1];
                    iq_entry->src1_ready_bit = READY;
                    iq_entry->src1_tag = cpu->r2d.ps1;
                    iq_entry->src1_value = cpu->r2d.ps1_value;
                }
                else
                {
                    iq_entry->src1_ready_bit = NOT_READY;
                    iq_entry->src1_tag = cpu->r2d.ps1;
                }

                iq_entry->src2_ready_bit = READY;
                
                iq_entry->dest_reg_or_lsq_index = cpu->r2d.pd;
                iq_entry->cpu_stage = cpu->r2d;
                addIQEntry(cpu, iq_entry);

                ROB_Entry rob_entry;
                rob_entry.pc_value = cpu->r2d.pc;
                rob_entry.arch_dest_reg = cpu->r2d.rd;
                rob_entry.res_mem_add_status = INVALID;
                rob_entry.instr_type = R2R;
                addROBEntry(cpu, rob_entry);
                // printIssueQueue(cpu);
                // printIssueQueue(cpu);
                // printROB(cpu);

                break;
            }

            case OPCODE_MOVC:
            {
                cpu->reg_is_renamed[cpu->r2d.rd] = 1;
                cpu->r2d.regs_renamed = 1;

                cpu->is_phy_reg_valid[cpu->r2d.pd] = INVALID;
                
                IQ_Entry *iq_entry = (IQ_Entry *)calloc(1, sizeof(IQ_Entry));
                // IQ_Entry *iq_entry = malloc(sizeof(IQ_Entry));
                iq_entry->pc_value = cpu->r2d.pc;
                iq_entry->status=1;
                iq_entry->opcode = cpu->r2d.opcode;
                iq_entry->FU_type = INT_FU;
                iq_entry->imm = cpu->r2d.imm;
                iq_entry->src1_ready_bit = 1;
                iq_entry->src2_ready_bit = 1;
                iq_entry->dest_reg_or_lsq_index = cpu->r2d.pd;
                iq_entry->cpu_stage = cpu->r2d;
                addIQEntry(cpu, iq_entry);

                ROB_Entry rob_entry;
                rob_entry.pc_value = cpu->r2d.pc;
                rob_entry.arch_dest_reg = cpu->r2d.rd;
                rob_entry.res_mem_add_status = INVALID;
                rob_entry.instr_type = R2R;
                addROBEntry(cpu, rob_entry);

                // printIssueQueue(cpu);
                // printROB(cpu);

                break;
            }

            case OPCODE_MUL:
            {
                cpu->reg_is_renamed[cpu->r2d.rd] = 1;
                cpu->r2d.regs_renamed = 1;

                cpu->is_phy_reg_valid[cpu->r2d.pd] = INVALID;

                IQ_Entry *iq_entry = (IQ_Entry *)calloc(1, sizeof(IQ_Entry));
                // IQ_Entry *iq_entry = malloc(sizeof(IQ_Entry));
                iq_entry->pc_value = cpu->r2d.pc;
                iq_entry->status=1;
                iq_entry->opcode = cpu->r2d.opcode;
                iq_entry->FU_type = MUL_FU;
                iq_entry->imm = cpu->r2d.imm;
                
                // printf("\nPhy reg P%d validity: %d\n", cpu->r2d.ps1, cpu->is_phy_reg_valid[cpu->r2d.ps1]);
                if (cpu->is_phy_reg_valid[cpu->r2d.ps1])
                {
                    cpu->r2d.ps1_value = cpu->phy_regs[cpu->r2d.ps1];
                    iq_entry->src1_ready_bit = READY;
                    iq_entry->src1_tag = cpu->r2d.ps1;
                    iq_entry->src1_value = cpu->r2d.ps1_value;
                }
                else
                {
                    iq_entry->src1_ready_bit = NOT_READY;
                    iq_entry->src1_tag = cpu->r2d.ps1;
                }

                if (cpu->is_phy_reg_valid[cpu->r2d.ps2])
                {
                    cpu->r2d.ps2_value = cpu->phy_regs[cpu->r2d.ps2];
                    iq_entry->src2_ready_bit = READY;
                    iq_entry->src2_tag = cpu->r2d.ps2;
                    iq_entry->src2_value = cpu->r2d.ps2_value;
                }
                else
                {
                    iq_entry->src2_ready_bit = NOT_READY;
                    iq_entry->src2_tag = cpu->r2d.ps2;
                }
                
                iq_entry->dest_reg_or_lsq_index = cpu->r2d.pd;
                iq_entry->cpu_stage = cpu->r2d;
                addIQEntry(cpu, iq_entry);

                ROB_Entry rob_entry;
                rob_entry.pc_value = cpu->r2d.pc;
                rob_entry.arch_dest_reg = cpu->r2d.rd;
                rob_entry.res_mem_add_status = INVALID;
                rob_entry.instr_type = R2R;
                addROBEntry(cpu, rob_entry);
                // printIssueQueue(cpu);
                // printIssueQueue(cpu);
                // printROB(cpu);

                break;
            }

            default:
                break;

        }

        /* Copy data from decode latch to execute latch*/
        // cpu->iq_stage = cpu->r2d;
        cpu->r2d.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Rename 2/Dispatch", &cpu->r2d);
        }
    }
}

/*
 * Issue Queue of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_iq_stage(APEX_CPU *cpu)
{
    
    if (!cpu->intFU.has_insn)
    {
        // printf("\nTest line 1\n");
        cpu->intFUInstruction = getInstructionForIntFU(cpu);
        // printf("\nTest line 1\n");
        // printf("\nIQ stage retrieved instruction PC value: %d", cpu->intFUInstruction->pc_value);
        if (cpu->intFUInstruction != NULL)
        {
            cpu->current_iq_size--;
            // printf("\nIQ stage retrieved instruction PC value: %d\n", cpu->intFUInstruction->pc_value);
            // printf("\n IQ entry retrieved");
            cpu->intFU.has_insn = TRUE;
        }
        else
        {
            printf("\nIQ Entry retrieved is NULL\n");
        }
    }

    if (!cpu->mulFU.has_insn)
    {
        // printIssueQueue(cpu);
        cpu->mulFUInstruction = getInstructionForMulFU(cpu);
        // printf("\nTest line 1\n");
        // printf("\nIQ stage retrieved instruction PC value: %d", cpu->intFUInstruction->pc_value);
        if (cpu->mulFUInstruction != NULL)
        {
            cpu->current_iq_size--;
            // printf("\nIQ stage retrieved instruction PC value: %d\n", cpu->intFUInstruction->pc_value);
            // printf("\n IQ entry retrieved");
            cpu->mulFU.has_insn = TRUE;
        }
        else
        {
            printf("\nMUL IQ Entry retrieved is NULL\n");
        }
    }
}




/*
 * IntFU of APEX Pipeline
 */
static void
APEX_intFU(APEX_CPU *cpu)
{
    // printf("Does INTFU have an instruction: %d\n", cpu->intFU.has_insn);
    if (cpu->intFU.has_insn)
    {
        switch (cpu->intFUInstruction->opcode)
        {
            // case OPCODE_ADD:
            // {
            //     cpu->execute.result_buffer
            //         = cpu->execute.rs1_value + cpu->execute.rs2_value;

            //     /* Set the zero flag based on the result buffer */
            //     if (cpu->execute.result_buffer == 0)
            //     {
            //         cpu->zero_flag = TRUE;
            //     } 
            //     else 
            //     {
            //         cpu->zero_flag = FALSE;
            //     }
            //     break;
            // }

            // case OPCODE_LOAD:
            // {
            //     cpu->execute.memory_address
            //         = cpu->execute.rs1_value + cpu->execute.imm;
            //     break;
            // }

            // case OPCODE_BZ:
            // {
            //     if (cpu->zero_flag == TRUE)
            //     {
            //         /* Calculate new PC, and send it to fetch unit */
            //         cpu->pc = cpu->execute.pc + cpu->execute.imm;
                    
            //         /* Since we are using reverse callbacks for pipeline stages, 
            //          * this will prevent the new instruction from being fetched in the current cycle*/
            //         cpu->fetch_from_next_cycle = TRUE;

            //         /* Flush previous stages */
            //         cpu->dr1.has_insn = FALSE;

            //         /* Make sure fetch stage is enabled to start fetching from new PC */
            //         cpu->fetch.has_insn = TRUE;
            //     }
            //     break;
            // }

            // case OPCODE_BNZ:
            // {
            //     if (cpu->zero_flag == FALSE)
            //     {
            //         /* Calculate new PC, and send it to fetch unit */
            //         cpu->pc = cpu->execute.pc + cpu->execute.imm;
                    
            //         /* Since we are using reverse callbacks for pipeline stages, 
            //          * this will prevent the new instruction from being fetched in the current cycle*/
            //         cpu->fetch_from_next_cycle = TRUE;

            //         /* Flush previous stages */
            //         cpu->dr1.has_insn = FALSE;

            //         /* Make sure fetch stage is enabled to start fetching from new PC */
            //         cpu->fetch.has_insn = TRUE;
            //     }
            //     break;
            // }

            case OPCODE_ADDL: 
            {
                cpu->intFU = cpu->intFUInstruction->cpu_stage;
                cpu->intFU.result_buffer 
                    = cpu->intFUInstruction->src1_value + cpu->intFUInstruction->imm;
                cpu->intFU.pd = cpu->intFUInstruction->dest_reg_or_lsq_index;
                // printf("Dest Phy Reg: P%d\n", cpu->intFU.pd);
                // printf("Result value: %d\n", cpu->intFU.result_buffer);
                break;
            }

            case OPCODE_MOVC: 
            {
                cpu->intFU = cpu->intFUInstruction->cpu_stage;
                cpu->intFU.result_buffer = cpu->intFUInstruction->imm + 0;
                cpu->intFU.pd = cpu->intFUInstruction->dest_reg_or_lsq_index;
                // printf("Dest Phy Reg: P%d\n", cpu->intFU.pd);
                // printf("Result value: %d\n", cpu->intFU.result_buffer);
                break;
            }

            default:
                break;
        }
        // printf("Does INTFU have an instruction: %d\n", cpu->intFU.has_insn);
        /* Copy data from execute latch to intFU latch*/
        cpu->intFU_fwd_bus = cpu->intFU;
        cpu->intFU.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Integer FU", &cpu->intFU);
        }
    }
}

/*
 * MulFU of APEX Pipeline
 */
static void
APEX_mulFU(APEX_CPU *cpu)
{
    // printf("Does INTFU have an instruction: %d\n", cpu->intFU.has_insn);
    if (cpu->mulFU.has_insn)
    {
        switch (cpu->mulFUInstruction->opcode)
        {
            case OPCODE_MUL: 
            {
                if (cpu->mulFUstage != 3)
                {
                    printf("cpu->mulFUstage != 3");
                    cpu->mulFUstage++;
                    return;
                }
                else
                {
                    printf("cpu->mulFUstage == 3");
                    cpu->mulFU = cpu->mulFUInstruction->cpu_stage;
                    cpu->mulFU.result_buffer 
                        = cpu->mulFUInstruction->src1_value * cpu->mulFUInstruction->src2_value;
                    cpu->mulFU.pd = cpu->mulFUInstruction->dest_reg_or_lsq_index;
                    cpu->mulFUstage = 0;
                    // printf("Dest Phy Reg: P%d\n", cpu->intFU.pd);
                    // printf("Result value: %d\n", cpu->intFU.result_buffer);
                    break;
                }

            }

            default:
                break;
        }
        // printf("Does INTFU have an instruction: %d\n", cpu->intFU.has_insn);
        /* Copy data from execute latch to intFU latch*/
        cpu->mulFU_fwd_bus = cpu->mulFU;
        cpu->mulFU.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Multiplication FU", &cpu->mulFU);
        }
    }
}

/*
 * IntFU FWD bus of APEX Pipeline
 */
static void
APEX_intFU_fwd_bus(APEX_CPU *cpu)
{
    // printf("Does INTFU FWD bus have an instruction: %d\n", cpu->intFU_fwd_bus.has_insn);
    if (cpu->intFU_fwd_bus.has_insn)
    {
        cpu->phy_regs[cpu->intFU_fwd_bus.pd] = cpu->intFU_fwd_bus.result_buffer;
        cpu->is_phy_reg_valid[cpu->intFU_fwd_bus.pd] = VALID;
        // printf("\nINT FWD bus pc value: %d\n", cpu->intFU_fwd_bus.pc);
        updateIQEntries(cpu, INT_FU);
        updateROBEntries(cpu, INT_FU);
        updateLSQEntries(cpu, INT_FU);
        // printf("Dest Phy Reg in forward: P%d = %d\n", cpu->intFU_fwd_bus.pd, cpu->phy_regs[cpu->intFU_fwd_bus.pd]);
        // printIssueQueue(cpu);
        // printROB(cpu);
        //printf("Result value: %d\n", cpu->intFU.result_buffer);
    }

    /* Copy data from execute latch to intFU latch*/
    // cpu->intFU = cpu->iq_stage;
    cpu->intFU_fwd_bus.has_insn = FALSE;

    if (ENABLE_DEBUG_MESSAGES)
    {
        // print_stage_content("Integer FU", &cpu->intFU);
        printf("\nIntFU FWD bus: P%d = %d\n", 
            cpu->intFU_fwd_bus.pd, cpu->intFU_fwd_bus.result_buffer);
    }
}


/*
 * MulFU FWD bus of APEX Pipeline
 */
static void
APEX_mulFU_fwd_bus(APEX_CPU *cpu)
{
    // printf("Does INTFU FWD bus have an instruction: %d\n", cpu->intFU_fwd_bus.has_insn);
    if (cpu->mulFU_fwd_bus.has_insn)
    {
        cpu->phy_regs[cpu->mulFU_fwd_bus.pd] = cpu->mulFU_fwd_bus.result_buffer;
        cpu->is_phy_reg_valid[cpu->mulFU_fwd_bus.pd] = VALID;
        // printf("\nINT FWD bus pc value: %d\n", cpu->intFU_fwd_bus.pc);
        updateIQEntries(cpu, MUL_FU);
        updateROBEntries(cpu, MUL_FU);
        
        // printROB(cpu);
        // updateLSQEntries(cpu, MUL_FU);

        // printf("Dest Phy Reg in forward: P%d = %d\n", cpu->intFU_fwd_bus.pd, cpu->phy_regs[cpu->intFU_fwd_bus.pd]);
        // printIssueQueue(cpu);
        // printROB(cpu);
        //printf("Result value: %d\n", cpu->intFU.result_buffer);

        /* Copy data from execute latch to intFU latch*/
        // cpu->intFU = cpu->iq_stage;
        cpu->mulFU_fwd_bus.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            // print_stage_content("Integer FU", &cpu->intFU);
            printf("\nMulFU FWD bus: P%d = %d\n", 
                cpu->mulFU_fwd_bus.pd, cpu->mulFU_fwd_bus.result_buffer);
        }
    }


}


/*
 * Memory Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_memory(APEX_CPU *cpu)
{
    if (cpu->memory.has_insn)
    {
        switch (cpu->memory.opcode)
        {
            case OPCODE_ADD:
            {
                /* No work for ADD */
                break;
            }

            case OPCODE_LOAD:
            {
                /* Read from data memory */
                cpu->memory.result_buffer
                    = cpu->data_memory[cpu->memory.memory_address];
                break;
            }
        }

        /* Copy data from memory latch to writeback latch*/
        cpu->writeback = cpu->memory;
        cpu->memory.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Memory", &cpu->memory);
        }
    }
}

/*
 * Writeback Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static int
APEX_ROB_retirement(APEX_CPU *cpu)
{
    commitROBHead(cpu);

    // if (cpu->writeback.has_insn)
    // {
    //     /* Write result to register file based on instruction type */
    //     switch (cpu->writeback.opcode)
    //     {
    //         case OPCODE_ADD:
    //         {
    //             cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
    //             break;
    //         }

    //         case OPCODE_LOAD:
    //         {
    //             cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
    //             break;
    //         }

    //         case OPCODE_MOVC: 
    //         {
    //             cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
    //             break;
    //         }
    //     }

    //     cpu->insn_completed++;
    //     cpu->writeback.has_insn = FALSE;

    //     if (ENABLE_DEBUG_MESSAGES)
    //     {
    //         print_stage_content("Writeback", &cpu->writeback);
    //     }

    //     if (cpu->writeback.opcode == OPCODE_HALT)
    //     {
    //         /* Stop the APEX simulator */
    //         return TRUE;
    //     }
    // }

    /* Default */
    return 0;
}

/*
 * This function creates and initializes APEX cpu.
 *
 * Note: You are free to edit this function according to your implementation
 */
APEX_CPU *
APEX_cpu_init(const char *filename)
{
    int i;
    APEX_CPU *cpu;

    if (!filename)
    {
        return NULL;
    }

    cpu = calloc(1, sizeof(APEX_CPU));

    if (!cpu)
    {
        return NULL;
    }

    /* Initialize PC, Registers and all pipeline stages */
    cpu->pc = 4000;
    memset(cpu->regs, 0, sizeof(int) * REG_FILE_SIZE);
    memset(cpu->data_memory, 0, sizeof(int) * DATA_MEMORY_SIZE);
    cpu->single_step = ENABLE_SINGLE_STEP;


    cpu->free_list_front = -1;
    cpu->free_list_rear = -1;
    initializeFreeList(cpu);
    
    cpu->current_iq_size = 0;
    // cpu->issue_queue = (IQ_Entry *) calloc(1, sizeof(IQ_Entry));
    cpu->iq_head = (IQ_Entry *) calloc(1, sizeof(IQ_Entry));
    cpu->iq_current = (IQ_Entry *) calloc(1, sizeof(IQ_Entry));
    cpu->iq_head = NULL;
    cpu->iq_current = NULL;

    // cpu->intFUInstruction = (IQ_Entry *) calloc(1, sizeof(IQ_Entry));
    // cpu->intFUInstruction = NULL;
    cpu->mulFUstage = 0;

    cpu->lsq_front = -1;
    cpu->lsq_rear = -1;

    cpu->rob_front = -1;
    cpu->rob_rear = -1;

    cpu->clock++;

    /* Parse input file and create code memory */
    cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);
    if (!cpu->code_memory)
    {
        free(cpu);
        return NULL;
    }

    if (ENABLE_DEBUG_MESSAGES)
    {
        fprintf(stderr,
                "APEX_CPU: Initialized APEX CPU, loaded %d instructions\n",
                cpu->code_memory_size);
        fprintf(stderr, "APEX_CPU: PC initialized to %d\n", cpu->pc);
        fprintf(stderr, "APEX_CPU: Printing Code Memory\n");
        printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode_str", "rd", "rs1", "rs2",
               "imm");

        for (i = 0; i < cpu->code_memory_size; ++i)
        {
            printf("%-9s %-9d %-9d %-9d %-9d\n", cpu->code_memory[i].opcode_str,
                   cpu->code_memory[i].rd, cpu->code_memory[i].rs1,
                   cpu->code_memory[i].rs2, cpu->code_memory[i].imm);
        }
    }

    /* To start fetch stage */
    cpu->fetch.has_insn = TRUE;
    return cpu;
}

/*
 * APEX CPU simulation loop
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_run(APEX_CPU *cpu)
{
    char user_prompt_val;

    while (TRUE)
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("--------------------------------------------\n");
            printf("Clock Cycle #: %d\n", cpu->clock);
            printf("--------------------------------------------\n");
        }

        // if (APEX_writeback(cpu))
        // {
        //     /* Halt in writeback stage */
        //     printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
        //     break;
        // }

        APEX_ROB_retirement(cpu);
        // // APEX_memory(cpu);

        APEX_mulFU_fwd_bus(cpu);
        APEX_intFU_fwd_bus(cpu);

        APEX_mulFU(cpu);
        APEX_intFU(cpu);
        
        APEX_iq_stage(cpu);
        APEX_r2d(cpu);
        // APEX_execute(cpu);
        APEX_dr1(cpu);
        APEX_fetch(cpu);
        print_phy_reg_file(cpu);
        print_rename_table(cpu);
        print_reg_file(cpu);

        if (cpu->single_step)
        {
            printf("Press any key to advance CPU Clock or <q> to quit:\n");
            scanf("%c", &user_prompt_val);

            if ((user_prompt_val == 'Q') || (user_prompt_val == 'q'))
            {
                printf("APEX_CPU: Simulation Stopped, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
                break;
            }
        }

        cpu->clock++;
    }
}

/*
 * This function deallocates APEX CPU.
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_stop(APEX_CPU *cpu)
{
    free(cpu->code_memory);
    free(cpu);
}