#include "apex_cpu.h"

// Print the Issue Queue details
void printIssueQueue();

// Check if IQ is full
int isIQFull(APEX_CPU *cpu);

// Inserting an IQ Entry
int addIQEntry(APEX_CPU *cpu, IQ_Entry *iq_entry);