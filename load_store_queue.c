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

// Print the LSQ entries
void printLSQ(APEX_CPU *cpu)
{
    printf("----------\n%s\n----------\n", "Load Store Queue entries: ");

    if (isLSQEmpty(cpu))
    {
        printf("LSQ is empty.\n");
        return;
    }

    int i;
    // int entryNo = 0;

    for(i=cpu->lsq_front; i != cpu->lsq_rear; i = (i+1)%REORDER_BUFFER_SIZE)
    {
        printf("Index: %d | Status: %d | Load or Store: %d | ",
                i, cpu->load_store_queue[i].status, cpu->load_store_queue[i].load_or_store);
        printf("Mem addr valid: %d | Mem addr: %d | Dest for Load: P%d | ",
                cpu->load_store_queue[i].mem_add_is_valid, 
                cpu->load_store_queue[i].mem_add, 
                cpu->load_store_queue[i].dest_reg_for_load);
        printf("Src1 ready: %d | Src1 tag: P%d | Src1 value: %d \n",
                cpu->load_store_queue[i].src1_ready_bit, 
                cpu->load_store_queue[i].src1_tag, 
                cpu->load_store_queue[i].src1_value);

        // int status;
        // int load_or_store;
        // int mem_add_is_valid;
        // int mem_add;
        // int dest_reg_for_load;
        // int src1_ready_bit;
        // int src1_tag;
        // int src1_value;
    }
    printf("Index: %d | Status: %d | Load or Store: %d | ",
            i, cpu->load_store_queue[i].status, cpu->load_store_queue[i].load_or_store);
    printf("Mem addr valid: %d | Mem addr: %d | Dest for Load: P%d | ",
            cpu->load_store_queue[i].mem_add_is_valid, 
            cpu->load_store_queue[i].mem_add, 
            cpu->load_store_queue[i].dest_reg_for_load);
    printf("Src1 ready: %d | Src1 tag: P%d | Src1 value: %d | \n",
            cpu->load_store_queue[i].src1_ready_bit, 
            cpu->load_store_queue[i].src1_tag, 
            cpu->load_store_queue[i].src1_value);

    return;
}

// Update LSQ entries with latest values
void updateLSQEntries(APEX_CPU *cpu, int FU_type)
{
    if (isLSQEmpty(cpu))
    {
        // printf("\nNo entries in ROB.");
        return;
    }

    int i;

    if (FU_type == INT_FU)
    {
        for(i=cpu->lsq_front; i != cpu->lsq_rear; i = (i+1)%REORDER_BUFFER_SIZE)
        {
            if (cpu->load_store_queue[i].pc_value == cpu->intFU_fwd_bus.pc)
            {
                switch (cpu->load_store_queue[i].load_or_store)
                {
                case LOAD:
                    cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                    cpu->load_store_queue[i].mem_add_is_valid = VALID;
                    break;

                case STORE:
                    if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                    {
                        cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                        cpu->load_store_queue[i].src1_ready_bit = READY;
                    }
                    cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                    cpu->load_store_queue[i].mem_add_is_valid = VALID;
                break;
                
                default:
                    break;
                }    
            }

            // if (cpu->load_store_queue[i].pc_value == cpu->intFU_fwd_bus.pc)
            // {
            switch (cpu->intFU_fwd_bus.opcode)
            {
            case OPCODE_MOVC:
                if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                {
                    cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                    cpu->load_store_queue[i].src1_ready_bit = READY;
                }            
            default:
                break;
            }    
            // }
        }

        if (cpu->load_store_queue[i].pc_value == cpu->intFU_fwd_bus.pc)
        {
            switch (cpu->load_store_queue[i].load_or_store)
            {
            case LOAD:
                cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                cpu->load_store_queue[i].mem_add_is_valid = VALID;
                break;

            case STORE:
                if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                {
                    cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                    cpu->load_store_queue[i].src1_ready_bit = READY;
                }
                cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                cpu->load_store_queue[i].mem_add_is_valid = VALID;
                break;
            
            default:
                break;
            }    
        }

        switch (cpu->intFU_fwd_bus.opcode)
        {
        case OPCODE_MOVC:
            if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
            {
                cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                cpu->load_store_queue[i].src1_ready_bit = READY;
            }            
        default:
            break;
        }    
    }

    if (FU_type == MUL_FU)
    {
        for(i=cpu->lsq_front; i != cpu->lsq_rear; i = (i+1)%REORDER_BUFFER_SIZE)
        {
            if (cpu->load_store_queue[i].pc_value == cpu->mulFU_fwd_bus.pc)
            {
                switch (cpu->load_store_queue[i].load_or_store)
                {
                case LOAD:
                if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                {
                    cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                    cpu->load_store_queue[i].src1_ready_bit = READY;
                }
                // cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                // cpu->load_store_queue[i].mem_add_is_valid = VALID;
                    break;

                case STORE:
                    if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                    {
                        cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                        cpu->load_store_queue[i].src1_ready_bit = READY;
                    }
                    // cpu->load_store_queue[i].mem_add = cpu->mulFU_fwd_bus.memory_address;
                    // cpu->load_store_queue[i].mem_add_is_valid = VALID;
                
                default:
                    break;
                }    
            }
        }

        if (cpu->load_store_queue[i].pc_value == cpu->mulFU_fwd_bus.pc)
        {
            switch (cpu->load_store_queue[i].load_or_store)
            {
            case LOAD:
            if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
            {
                cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                cpu->load_store_queue[i].src1_ready_bit = READY;
            }
            // cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
            // cpu->load_store_queue[i].mem_add_is_valid = VALID;
                break;

            case STORE:
                if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                {
                    cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                    cpu->load_store_queue[i].src1_ready_bit = READY;
                }
                // cpu->load_store_queue[i].mem_add = cpu->mulFU_fwd_bus.memory_address;
                // cpu->load_store_queue[i].mem_add_is_valid = VALID;
            
            default:
                break;
            }    
        }
    }

    if (FU_type == MEMORY)
    {
        for(i=cpu->lsq_front; i != cpu->lsq_rear; i = (i+1)%REORDER_BUFFER_SIZE)
        {
            if (cpu->load_store_queue[i].src1_tag == cpu->memory_fwd_bus.pd)
            {
                switch (cpu->load_store_queue[i].load_or_store)
                {
                case LOAD:
                    if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                    {
                        cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                        cpu->load_store_queue[i].src1_ready_bit = READY;
                    }
                    // cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                    // cpu->reorder_buffer[i].res_mem_add_status = VALID;
                    break;

                case STORE:
                    // if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                    // {
                    //     cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                    //     cpu->load_store_queue[i].src1_ready_bit = READY;
                    // }
                    // cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                    // cpu->load_store_queue[i].mem_add_is_valid = VALID;
                
                default:
                    break;
                }    
            }
        }

        if (cpu->load_store_queue[i].src1_tag == cpu->memory_fwd_bus.pd)
        {
            switch (cpu->load_store_queue[i].load_or_store)
            {
            case LOAD:
                if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                {
                    cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                    cpu->load_store_queue[i].src1_ready_bit = READY;
                }
                // cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                // cpu->reorder_buffer[i].res_mem_add_status = VALID;
                break;

            case STORE:
                // if (cpu->is_phy_reg_valid[cpu->load_store_queue[i].src1_tag])
                // {
                //     cpu->load_store_queue[i].src1_value = cpu->phy_regs[cpu->load_store_queue[i].src1_tag];
                //     cpu->load_store_queue[i].src1_ready_bit = READY;
                // }
                // cpu->load_store_queue[i].mem_add = cpu->intFU_fwd_bus.memory_address;
                // cpu->load_store_queue[i].mem_add_is_valid = VALID;
            
            default:
                break;
            }    
        }
    }

    return;
}


// Fetch instruction for memory operation
LSQ_Entry getInstructionForMemory(APEX_CPU *cpu)
{
    LSQ_Entry load_or_store;
    load_or_store.status = 0;
    
    if (isLSQEmpty(cpu))
    {
        return load_or_store;
    }

    switch (cpu->load_store_queue[cpu->lsq_front].load_or_store)
    {
        
        case LOAD:
            if (cpu->load_store_queue[cpu->lsq_front].mem_add_is_valid)
            {
                load_or_store = cpu->load_store_queue[cpu->lsq_front];                   
                if (cpu->lsq_front == cpu->lsq_rear) 
                {
                    cpu->lsq_front = -1;
                    cpu->lsq_rear = -1;
                }
                else 
                {
                    cpu->lsq_front = (cpu->lsq_front + 1) % LOAD_STORE_QUEUE_SIZE;
                }                    
                return load_or_store;                    
            }
            break;

        case STORE:
            
            if (cpu->reorder_buffer[cpu->rob_front].instr_type == STORE)
            {
                // printf("\n------------------------------------------------STORE entry to be fetched from LSQ---------------------------------------------------------------------\n");
                // printf("STORE instr mem addr valid: %d", cpu->load_store_queue[cpu->lsq_front].mem_add_is_valid);
                // printf("STORE instr Src 1 ready: %d", cpu->load_store_queue[cpu->lsq_front].src1_ready_bit);
                if (cpu->load_store_queue[cpu->lsq_front].mem_add_is_valid
                    && cpu->load_store_queue[cpu->lsq_front].src1_ready_bit)
                {
                    
                    load_or_store = cpu->load_store_queue[cpu->lsq_front];
                    if (cpu->lsq_front == cpu->lsq_rear) 
                    {
                        cpu->lsq_front = -1;
                        cpu->lsq_rear = -1;
                    }
                    else 
                    {
                        cpu->lsq_front = (cpu->lsq_front + 1) % LOAD_STORE_QUEUE_SIZE;
                    }                    
                    return load_or_store;                    
                }
            }
            break;

        default:
            break;
    }

    return load_or_store;
}