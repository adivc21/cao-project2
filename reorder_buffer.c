#include "reorder_buffer.h"

typedef struct ROB_Entry
{
    int pc_value;
    int arch_dest_reg;
    int result;
    int store_value;
    int store_value_valid;
    int exception_codes;
    int res_mem_add_status;
    int instr_type;
} ROB_Entry;