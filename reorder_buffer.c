#include <stdlib.h>
#include <stdio.h>

#include "reorder_buffer.h"

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