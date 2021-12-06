#include <stdio.h>

#include "free_list.h"

// Initialize the Free List
void initializeFreeList(APEX_CPU *cpu)
{
    for(int i=0; i<PHY_REG_FILE_SIZE; i++)
    {
        addPhysicalRegister(cpu, i);
    }
}

// Check if the Physical Register Free List is full
int isFreeListFull(APEX_CPU *cpu)
{
    if ((cpu->free_list_front == cpu->free_list_rear + 1)
    || (cpu->free_list_front == 0 && cpu->free_list_rear == PHY_REG_FILE_SIZE -1))
    {
        return 1;
    }
    return 0;
}

// Check if Physical Register Free List is empty
int isFreeListEmpty(APEX_CPU *cpu)
{
    if (cpu->free_list_front == -1)
    {
        return 1;
    }
    return 0;
}

// Adding freed Physical Register back to the Free List
void addPhysicalRegister(APEX_CPU *cpu, int phyRegIndex)
{
    if (isFreeListFull(cpu))
    {
        printf("Free List is at maximum capacity.");
        printf("Cannot add anymore registers.");
    }
    else
    {
        if (cpu->free_list_front == -1)
        {
            cpu->free_list_front = 0;
        }
        cpu->free_list_rear = (cpu->free_list_rear+1) % PHY_REG_FILE_SIZE;
        cpu->free_list[cpu->free_list_rear] = phyRegIndex;
    }
    return;
}

// Retrieve Free Physical Register from Free List
int getPhysicalRegister(APEX_CPU *cpu)
{
    if (isFreeListEmpty(cpu))
    {
        printf("No Physical Register is available");
        return -1;
    }
    else
    {
        int free_reg = cpu->free_list[cpu->free_list_front];
        
        if (cpu->free_list_front == cpu->free_list_rear)
        {
            cpu->free_list_front = cpu->free_list_rear = -1;
        }
        else
        {
            cpu->free_list_front = (cpu->free_list_front+1) % PHY_REG_FILE_SIZE;
        }

        return free_reg;
    }
}

// Print the Physical Registers available in the Free List
void printFreeList(APEX_CPU *cpu)
{
    if (isFreeListEmpty(cpu))
    {
        printf("\nNo Physical Register is available.");
        return;
    }

    int i;

    printf("\nRegisters in the Free List: [");
    for(i=cpu->free_list_front; i != cpu->free_list_rear; i = (i+1)%PHY_REG_FILE_SIZE)
    {
        printf("P%d, ", cpu->free_list[i]);
    }
    printf("P%d]\n", cpu->free_list[i]);

    return;
}