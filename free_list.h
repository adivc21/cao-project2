#include "apex_cpu.h"
// #define PHY_REG_FILE_SIZE 20

// int free_list[PHY_REG_FILE_SIZE];     /* Free list for Physical register file */


// Initialize the Free List
void initializeFreeList(APEX_CPU *cpu);

// Check if the Physical Register Free List is full
int isFreeListFull(APEX_CPU *cpu);

// Check if Physical Register Free List is empty
int isFreeListEmpty(APEX_CPU *cpu);

// Adding freed Physical Register back to the Free List
void addPhysicalRegister(APEX_CPU *cpu, int phyRegIndex);

// Retrieve Free Physical Register from Free List
int getPhysicalRegister(APEX_CPU *cpu);

// Print the Physical Registers available in the Free List
void printFreeList(APEX_CPU *cpu);