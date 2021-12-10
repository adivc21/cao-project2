#include <stdio.h>

#include "load_store_queue.h"

// // Initialize the Free List
// void initializeLSQ(APEX_CPU *cpu)
// {
//     for(int i=0; i<LOAD_STORE_QUEUE_SIZE; i++)
//     {
//         addPhysicalRegister(cpu, i);
//     }
// }

// Check if LSQ is full
int isLSQFull(APEX_CPU *cpu)
{
    if ((cpu->lsq_front == cpu->lsq_rear + 1)
    || (cpu->lsq_front == 0 && cpu->lsq_rear == LOAD_STORE_QUEUE_SIZE -1))
    {
        return 1;
    }
    return 0;
}

// Check if Physical Register Free List is empty
int isLSQEmpty(APEX_CPU *cpu)
{
    if (cpu->lsq_front == -1)
    {
        return 1;
    }
    return 0;
}

// Adding freed Physical Register back to the Free List
void addLSQEntry(APEX_CPU *cpu, LSQ_Entry lsq_entry)
{
    if (isLSQFull(cpu))
    {
        printf("\nLoad Store Queue is at maximum capacity.");
    }
    else
    {
        if (cpu->lsq_front == -1)
        {
            cpu->lsq_front = 0;
        }
        cpu->lsq_rear = (cpu->lsq_rear+1) % LOAD_STORE_QUEUE_SIZE;
        cpu->load_store_queue[cpu->lsq_rear] = lsq_entry;
    }
    return;
}

// // Retrieve Free Physical Register from Free List
// int getPhysicalRegister(APEX_CPU *cpu)
// {
//     if (isLSQEmpty(cpu))
//     {
//         printf("No Physical Register is available");
//         return -1;
//     }
//     else
//     {
//         int free_reg = cpu->load_store_queue[cpu->lsq_front];
        
//         if (cpu->lsq_front == cpu->lsq_rear)
//         {
//             cpu->lsq_front = cpu->lsq_rear = -1;
//         }
//         else
//         {
//             cpu->lsq_front = (cpu->lsq_front+1) % LOAD_STORE_QUEUE_SIZE;
//         }

//         return free_reg;
//     }
// }

// Print the Physical Registers available in the Free List
void printLSQ(APEX_CPU *cpu)
{
    if (isLSQEmpty(cpu))
    {
        printf("\nNo Physical Register is available.");
        return;
    }

    // int i;

    // printf("\nRegisters in the Free List: [");
    // for(i=cpu->lsq_front; i != cpu->lsq_rear; i = (i+1)%LOAD_STORE_QUEUE_SIZE)
    // {
    //     printf("P%d, ", cpu->load_store_queue[i]);
    // }
    // printf("P%d]\n", cpu->load_store_queue[i]);

    return;
}

// Update LSQ entries with latest values
void updateLSQEntries(APEX_CPU *cpu, int FU_type)
{
    return;
}