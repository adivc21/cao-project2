#include "apex_cpu.h"

// Print ROB contents
void printROB(APEX_CPU *cpu);

// Check if ROB is full
int isROBFull(APEX_CPU *cpu);

// Check if ROB is empty
int isROBEmpty(APEX_CPU *cpu);

// Adding a new entry to the ROB
void addROBEntry(APEX_CPU *cpu, ROB_Entry rob_entry);

// Update ROB entries with latest values
void updateROBEntries(APEX_CPU *cpu, int FU_type);

// Commit the instruction at the head of ROB
void commitROBHead(APEX_CPU *cpu);