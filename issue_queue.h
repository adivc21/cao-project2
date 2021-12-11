#include "apex_cpu.h"

// Print the Issue Queue details
void printIssueQueue();

// Check if IQ is full
int isIQFull(APEX_CPU *cpu);

// Inserting an IQ Entry
int addIQEntry(APEX_CPU *cpu, IQ_Entry *iq_entry);

// Get IntFU instruction ready for execution
IQ_Entry * getInstructionForIntFU(APEX_CPU *cpu);

// Get MulFU instruction ready for execution
IQ_Entry * getInstructionForMulFU(APEX_CPU *cpu);

// Get BranchFU instruction ready for execution
IQ_Entry * getInstructionForBranchFU(APEX_CPU *cpu);

// Update IQ entries with latest values
void updateIQEntries(APEX_CPU *cpu, int FU_type);