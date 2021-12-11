#include "apex_cpu.h"

// Check if LSQ is full
int isLSQFull(APEX_CPU *cpu);

// Check if Physical Register Free List is empty
int isLSQEmpty(APEX_CPU *cpu);

// Adding freed Physical Register back to the Free List
void addLSQEntry(APEX_CPU *cpu, LSQ_Entry lsq_entry);

// Update LSQ entries with latest values
void updateLSQEntries(APEX_CPU *cpu, int FU_type);

// Fetch instruction for memory operation
LSQ_Entry getInstructionForMemory(APEX_CPU *cpu);

// Print the LSQ entries
void printLSQ(APEX_CPU *cpu);