#include <stdlib.h>
#include <stdio.h>

#include "reorder_buffer.h"

// Print ROB contents
void printROB(APEX_CPU *cpu)
{
    printf("----------\n%s\n----------\n", "Reorder Buffer entries: ");
    if (isROBEmpty(cpu))
    {
        printf("No entries in ROB.\n");
        return;
    }

    int i;
    // int entryNo = 0;

    // printf("\nROB entries:");
    for(i=cpu->rob_front; i != cpu->rob_rear; i = (i+1)%REORDER_BUFFER_SIZE)
    {
        printf("Index: %d | Arch dest: R%d | Result: %d | ",
                i, cpu->reorder_buffer[i].arch_dest_reg, cpu->reorder_buffer[i].result);
        printf("Result status: %d | Store value: %d | Sval status: %d | ",
                cpu->reorder_buffer[i].res_mem_add_status, 
                cpu->reorder_buffer[i].store_value, 
                cpu->reorder_buffer[i].store_value_valid);
        printf("Excodes: %d | Instr type: %d\n", 
                cpu->reorder_buffer[i].exception_codes, 
                cpu->reorder_buffer[i].instr_type);
    //         int pc_value;
    // int arch_dest_reg;
    // int result;
    // int res_mem_add_status;
    // int store_value;
    // int store_value_valid;
    // int exception_codes;
    // int instr_type;

    //     printf("\n\nROB Entry No.: %d", entryNo);
    //     printf("\nROB index: %d", i);
    //     printf("\n PC value: %d", cpu->reorder_buffer[i].pc_value);
    //     printf("\n Architectural Destination Register: R%d", cpu->reorder_buffer[i].arch_dest_reg);
    //     printf("\n Result: %d", cpu->reorder_buffer[i].result);
    //     printf("\n Value for Store instruction: %d", cpu->reorder_buffer[i].store_value);
    //     printf("\n Is Store instruction value valid: %d", cpu->reorder_buffer[i].store_value_valid);
    //     printf("\n Exception codes: %d", cpu->reorder_buffer[i].exception_codes);
    //     printf("\n Result/Memory address valid: %d", cpu->reorder_buffer[i].res_mem_add_status);
    //     printf("\n Instruction type: %d", cpu->reorder_buffer[i].instr_type);
    //     entryNo++;
    }

    // printf("\n\nROB Entry No.: %d", entryNo);
    // printf("\nROB index: %d", i);
    // printf("\n PC value: %d", cpu->reorder_buffer[i].pc_value);
    // printf("\n Architectural Destination Register: R%d", cpu->reorder_buffer[i].arch_dest_reg);
    // printf("\n Result: %d", cpu->reorder_buffer[i].result);
    // printf("\n Value for Store instruction: %d", cpu->reorder_buffer[i].store_value);
    // printf("\n Is Store instruction value valid: %d", cpu->reorder_buffer[i].store_value_valid);
    // printf("\n Exception codes: %d", cpu->reorder_buffer[i].exception_codes);
    // printf("\n Result/Memory address valid: %d", cpu->reorder_buffer[i].res_mem_add_status);
    // printf("\n Instruction type: %d", cpu->reorder_buffer[i].instr_type);
    printf("Index: %d | Arch dest: R%d | Result: %d | ",
            i, cpu->reorder_buffer[i].arch_dest_reg, cpu->reorder_buffer[i].result);
    printf("Result status: %d | Store value: %d | Sval status: %d | ",
            cpu->reorder_buffer[i].res_mem_add_status, 
            cpu->reorder_buffer[i].store_value, 
            cpu->reorder_buffer[i].store_value_valid);
    printf("Excodes: %d | Instr type: %d\n", 
            cpu->reorder_buffer[i].exception_codes, 
            cpu->reorder_buffer[i].instr_type);

    return;
}

// Check if ROB is full
int isROBFull(APEX_CPU *cpu)
{
    if ((cpu->rob_front == cpu->rob_rear + 1)
    || (cpu->rob_front == 0 && cpu->rob_rear == REORDER_BUFFER_SIZE -1))
    {
        return 1;
    }
    return 0;
}

// Check if ROB is empty
int isROBEmpty(APEX_CPU *cpu)
{
    if (cpu->rob_front == -1)
    {
        return 1;
    }
    return 0;
}

// Adding a new entry to the ROB
void addROBEntry(APEX_CPU *cpu, ROB_Entry rob_entry)
{
    if (isROBFull(cpu))
    {
        printf("\nReorder Buffer is at maximum capacity.");
    }
    else
    {
        if (cpu->rob_front == -1)
        {
            cpu->rob_front = 0;
        }
        cpu->rob_rear = (cpu->rob_rear+1) % REORDER_BUFFER_SIZE;
        cpu->reorder_buffer[cpu->rob_rear] = rob_entry;
    }
    return;
}

// Update ROB entries with latest values
void updateROBEntries(APEX_CPU *cpu, int FU_type)
{
    if (isROBEmpty(cpu))
    {
        // printf("\nNo entries in ROB.");
        return;
    }

    int i;

    if (FU_type == INT_FU)
    {
        for(i=cpu->rob_front; i != cpu->rob_rear; i = (i+1)%REORDER_BUFFER_SIZE)
        {
            if (cpu->reorder_buffer[i].pc_value == cpu->intFU_fwd_bus.pc)
            {
                switch (cpu->reorder_buffer[i].instr_type)
                {
                case R2R:
                    cpu->reorder_buffer[i].result = cpu->intFU_fwd_bus.result_buffer;
                    cpu->reorder_buffer[i].res_mem_add_status = VALID;
                    break;

                case LOAD:
                    cpu->reorder_buffer[i].result = cpu->intFU_fwd_bus.result_buffer;
                    cpu->reorder_buffer[i].res_mem_add_status = VALID;
                    break;    

                case STORE:
                    cpu->reorder_buffer[i].res_mem_add_status = VALID;
                
                default:
                    break;
                }    

            }
        }

        if (cpu->reorder_buffer[i].pc_value == cpu->intFU_fwd_bus.pc)
        {
            switch (cpu->reorder_buffer[i].instr_type)
            {
            case R2R:
                cpu->reorder_buffer[i].result = cpu->intFU_fwd_bus.result_buffer;
                cpu->reorder_buffer[i].res_mem_add_status = VALID;
                break;

            case LOAD:
                cpu->reorder_buffer[i].result = cpu->intFU_fwd_bus.result_buffer;
                cpu->reorder_buffer[i].res_mem_add_status = VALID;
                break; 

            case STORE:
                cpu->reorder_buffer[i].result = cpu->intFU_fwd_bus.memory_address;
                cpu->reorder_buffer[i].res_mem_add_status = VALID;
                
            default:
                break;
            }    

        }
    }

    if (FU_type == MUL_FU)
    {
        for(i=cpu->rob_front; i != cpu->rob_rear; i = (i+1)%REORDER_BUFFER_SIZE)
        {
            if (cpu->reorder_buffer[i].pc_value == cpu->mulFU_fwd_bus.pc)
            {    
                cpu->reorder_buffer[i].result = cpu->mulFU_fwd_bus.result_buffer;
                cpu->reorder_buffer[i].res_mem_add_status = VALID;
            }
        }

        if (cpu->reorder_buffer[i].pc_value == cpu->mulFU_fwd_bus.pc)
        {
            cpu->reorder_buffer[i].result = cpu->mulFU_fwd_bus.result_buffer;
            cpu->reorder_buffer[i].res_mem_add_status = VALID;
        }
    }
    return;    
}

// Commit the instruction at the head of ROB
int commitROBHead(APEX_CPU *cpu)
{

    cpu->phy_reg_to_be_freed = -1;
    if (isROBEmpty(cpu))
    {
        return 0;
    }

    switch (cpu->reorder_buffer[cpu->rob_front].instr_type)
    {
        case R2R:
            if (cpu->reorder_buffer[cpu->rob_front].res_mem_add_status)
            {
                // printf("ROB instruction committed: %s %d,#%d", cpu->reorder_buffer[cpu->rob_front].cpu_stage.opcode_str,
                //     cpu->reorder_buffer[cpu->rob_front].cpu_stage.rd,
                //     cpu->reorder_buffer[cpu->rob_front].cpu_stage.imm);

                cpu->regs[cpu->reorder_buffer[cpu->rob_front].arch_dest_reg]
                    = cpu->reorder_buffer[cpu->rob_front].result;
                cpu->phy_reg_to_be_freed = cpu->reorder_buffer[cpu->rob_front].cpu_stage.pd;                    
                cpu->rob_front = (cpu->rob_front + 1) % REORDER_BUFFER_SIZE;

                
                // printf("Physical reg to be freed: P%d\n", cpu->reorder_buffer[cpu->rob_front].cpu_stage.pd);
                // printf("Arch dest reg: R%d\n", cpu->reorder_buffer[cpu->rob_front].cpu_stage.rd);
                // printf("ROB Arch dest reg: R%d\n", cpu->reorder_buffer[cpu->rob_front].arch_dest_reg);
                // printf("PC value: R%d\n", cpu->reorder_buffer[cpu->rob_front].cpu_stage.pc);



                cpu->insn_completed++;
                return 0;
            }
            break;
        
        case LOAD:
            if (cpu->reorder_buffer[cpu->rob_front].res_mem_add_status)
            {
                cpu->regs[cpu->reorder_buffer[cpu->rob_front].arch_dest_reg]
                    = cpu->reorder_buffer[cpu->rob_front].result;

                cpu->phy_reg_to_be_freed = cpu->reorder_buffer[cpu->rob_front].cpu_stage.pd;    

                cpu->rob_front = (cpu->rob_front + 1) % REORDER_BUFFER_SIZE;


                cpu->insn_completed++;
                return 0;
            }
            break;

        case STORE:
            if (cpu->reorder_buffer[cpu->rob_front].res_mem_add_status)
            {
                // cpu->regs[cpu->reorder_buffer[cpu->rob_front].arch_dest_reg]
                //     = cpu->reorder_buffer[cpu->rob_front].result;
                cpu->phy_reg_to_be_freed = -1;
                cpu->rob_front = (cpu->rob_front + 1) % REORDER_BUFFER_SIZE;

                

                cpu->insn_completed++;
                return 0;
            }
            break;

        case HALT:
            cpu->phy_reg_to_be_freed = -1;
            cpu->rob_front = (cpu->rob_front + 1) % REORDER_BUFFER_SIZE;
            cpu->insn_completed++;
            return 1;

        default:
            printf("PC value: R%d\n", cpu->reorder_buffer[cpu->rob_front].cpu_stage.pc);
            cpu->phy_reg_to_be_freed = -1;
            break;
    }

    return 0;
}