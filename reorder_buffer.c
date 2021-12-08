#include <stdlib.h>
#include <stdio.h>

#include "reorder_buffer.h"

// Print ROB contents
void printROB(APEX_CPU *cpu)
{
    if (isROBEmpty(cpu))
    {
        printf("\nNo entries in ROB.");
        return;
    }

    int i;
    int entryNo = 0;

    printf("\nROB entries:");
    for(i=cpu->rob_front; i != cpu->rob_rear; i = (i+1)%REORDER_BUFFER_SIZE)
    {
        printf("\n\nROB Entry No.: %d", entryNo);
        printf("\nROB index: %d", i);
        printf("\n PC value: %d", cpu->reorder_buffer[i].pc_value);
        printf("\n Architectural Destination Register: R%d", cpu->reorder_buffer[i].arch_dest_reg);
        printf("\n Result: %d", cpu->reorder_buffer[i].result);
        printf("\n Value for Store instruction: %d", cpu->reorder_buffer[i].store_value);
        printf("\n Is Store instruction value valid: %d", cpu->reorder_buffer[i].store_value_valid);
        printf("\n Exception codes: %d", cpu->reorder_buffer[i].exception_codes);
        printf("\n Result/Memory address valid: %d", cpu->reorder_buffer[i].res_mem_add_status);
        printf("\n Instruction type: %d", cpu->reorder_buffer[i].instr_type);
        entryNo++;
    }

    printf("\n\nROB Entry No.: %d", entryNo);
    printf("\nROB index: %d", i);
    printf("\n PC value: %d", cpu->reorder_buffer[i].pc_value);
    printf("\n Architectural Destination Register: R%d", cpu->reorder_buffer[i].arch_dest_reg);
    printf("\n Result: %d", cpu->reorder_buffer[i].result);
    printf("\n Value for Store instruction: %d", cpu->reorder_buffer[i].store_value);
    printf("\n Is Store instruction value valid: %d", cpu->reorder_buffer[i].store_value_valid);
    printf("\n Exception codes: %d", cpu->reorder_buffer[i].exception_codes);
    printf("\n Result/Memory address valid: %d", cpu->reorder_buffer[i].res_mem_add_status);
    printf("\n Instruction type: %d", cpu->reorder_buffer[i].instr_type);

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