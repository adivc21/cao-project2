#include <stdlib.h>
#include <stdio.h>

#include "issue_queue.h"

// Print the Issue Queue details
void printIssueQueue(APEX_CPU *cpu) 
{
    IQ_Entry *ptr = cpu->iq_head;
    // printf("\n\nIssue Queue entries: ");
    printf("----------\n%s\n----------\n", "Issue Queue entries: ");

    if (ptr == NULL)
    {
        printf("Issue Queue is empty.\n");
    }

    int entryNo = 0;
    while(ptr != NULL) 
    {
        printf("Index: %d | Opcode: %d | FU_type: %d | Literal: %d | ",
                entryNo, ptr->opcode, ptr->FU_type, ptr->imm);
        printf("Src1 Ready: %d | Src1 tag: P%d | Src1 value: %d | ",
                ptr->src1_ready_bit, ptr->src1_tag, ptr->src1_value);
        printf("Src2 Ready: %d | Src2 tag: P%d | Src2 value: %d | ",
                ptr->src2_ready_bit, ptr->src2_tag, ptr->src2_value);
        printf("Dest: %d\n", ptr->dest_reg_or_lsq_index);

    //         int status;
    // int opcode;
    // int FU_type;
    // int imm;
    // int src1_ready_bit;
    // int src1_tag;
    // int src1_value;
    // int src2_ready_bit;
    // int src2_tag;
    // int src2_value;
    // int dest_reg_or_lsq_index;

    //     printf("\nIssue Queue index: %d", entryNo);
    //     printf("\n Status: %d", ptr->status);
    //     printf("\n FU type: %d", ptr->FU_type);
    //     printf("\n Literal value: %d", ptr->imm);
    //     printf("\n Src1 ready bit: %d", ptr->src1_ready_bit);
    //     printf("\n Src1 tag: %d", ptr->src1_tag);
    //     printf("\n Src1 value: %d", ptr->src1_value);
    //     printf("\n Src2 ready bit: %d", ptr->src2_ready_bit);
    //     printf("\n Src2 tag: %d", ptr->src2_tag);
    //     printf("\n Src2 value: %d", ptr->src2_value);
    //     printf("\n Phy dest reg or LSQ index: %d", ptr->dest_reg_or_lsq_index);
        
        entryNo++;
        ptr = ptr->next;
    }

    // printf("\n");

}


// Check if IQ is full
int isIQFull(APEX_CPU *cpu) 
{
    if (cpu->current_iq_size == ISSUE_QUEUE_SIZE)
    {
        return 1;
    }
    return 0;
}

// Inserting an IQ Entry
int addIQEntry(APEX_CPU *cpu, IQ_Entry *iq_entry) 
{
    if (isIQFull(cpu))
    {
        printf("\nIssue Queue full");
        return -1;
    }

    if (cpu->iq_head == NULL)
    {
        cpu->iq_head = iq_entry;
        cpu->iq_head->next = NULL;
        cpu->current_iq_size++;

        return 0;
    }

    cpu->iq_current = cpu->iq_head;

    while (cpu->iq_current->next != NULL) 
    {
        cpu->iq_current = cpu->iq_current->next;
    }

    cpu->iq_current->next = iq_entry;
    cpu->iq_current->next->next = NULL;
    cpu->current_iq_size++;
    
    return 0;
}

// Get IntFU instruction ready for execution
IQ_Entry * getInstructionForIntFU(APEX_CPU *cpu)
{
    // printf("\nTest line 1\n");
    IQ_Entry *temp, *current, *ret_ptr;
    current = cpu->iq_head;
    // printf("\nTest line 1\n");
    if (current != NULL 
            && current->FU_type == INT_FU)
    {
        // if (cpu->is_phy_reg_valid[current->src1_tag])
        // {
        //     current->src1_value = cpu->phy_regs[current->src1_tag];
        //     current->src1_ready_bit = READY;
        // }
        // if (cpu->is_phy_reg_valid[current->src2_tag])
        // {
        //     current->src2_value = cpu->phy_regs[current->src2_tag];
        //     current->src2_ready_bit = READY;
        // }        
        if (current->src1_ready_bit && current->src2_ready_bit)
        {
            ret_ptr = current;
            // current = current->next;
            cpu->iq_head = current->next;
            ret_ptr->next = NULL;
            // printf("\n Retrieved IQ entry PC value: %d", ret_ptr->pc_value);
            // printf("\n Status: %d", ptr->status);
            // printf("\n FU type: %d", ptr->FU_type);
            // printf("\n Literal value: %d", ptr->imm);
            // printf("\n Src1 ready bit: %d", ptr->src1_ready_bit);
            // printf("\n Src1 tag: %d", ptr->src1_tag);
            // printf("\n Src1 value: %d", ptr->src1_value);
            // printf("\n Src2 ready bit: %d", ptr->src2_ready_bit);
            // printf("\n Src2 tag: %d", ptr->src2_tag);
            // printf("\n Src2 value: %d", ptr->src2_value);
            // printf("\n Phy dest reg or LSQ index: %d", ptr->dest_reg_or_lsq_index);
            return ret_ptr;
        }

    }
    else
    {
        // printf("Head ptr is not an INT instruction");
    }

    while(current != NULL) 
    {
        // printf("\nTest line 1\n");
        temp = current;
        current = current->next;

        // printf("\nTest line 1\n");
        // printf("\nCurrent FU type: %d\n", current->FU_type);
        if (current == NULL)
        {
            return NULL;
        }
        if (current->FU_type == INT_FU)
        {
            // printf("\nTest line 1\n");
            // if (cpu->is_phy_reg_valid[current->src1_tag])
            // {
            //     current->src1_value = cpu->phy_regs[current->src1_tag];
            //     current->src1_ready_bit = READY;
            // }
            // if (cpu->is_phy_reg_valid[current->src2_tag])
            // {
            //     current->src2_value = cpu->phy_regs[current->src2_tag];
            //     current->src2_ready_bit = READY;
            // }        
            // if (current->src1_ready_bit && current->src2_ready_bit)
            // {
            //     ret_ptr = current;
            //     // current = current->next;
            //     cpu->iq_head = current->next;
            //     ret_ptr->next = NULL;
            //     // printf("\n Retrieved IQ entry PC value: %d", ret_ptr->pc_value);
            //     // printf("\n Status: %d", ptr->status);
            //     // printf("\n FU type: %d", ptr->FU_type);
            //     // printf("\n Literal value: %d", ptr->imm);
            //     // printf("\n Src1 ready bit: %d", ptr->src1_ready_bit);
            //     // printf("\n Src1 tag: %d", ptr->src1_tag);
            //     // printf("\n Src1 value: %d", ptr->src1_value);
            //     // printf("\n Src2 ready bit: %d", ptr->src2_ready_bit);
            //     // printf("\n Src2 tag: %d", ptr->src2_tag);
            //     // printf("\n Src2 value: %d", ptr->src2_value);
            //     // printf("\n Phy dest reg or LSQ index: %d", ptr->dest_reg_or_lsq_index);
            //     return ret_ptr;
            // }
            if (current->src1_ready_bit && current->src2_ready_bit)
            {
                ret_ptr = current;
                temp->next = current->next;
                ret_ptr->next = NULL;
                
                return ret_ptr;
            }
        }
    }
    // printf("\nTest line 1\n");
    return NULL;
}

// Get MulFU instruction ready for execution
IQ_Entry * getInstructionForMulFU(APEX_CPU *cpu)
{
    IQ_Entry *temp, *current, *ret_ptr;
    current = cpu->iq_head;
    if (current != NULL 
            && current->FU_type == MUL_FU 
            && current->src1_ready_bit && current->src2_ready_bit)
    {
        
        ret_ptr = current;
        // current = current->next;
        cpu->iq_head = current->next;
        ret_ptr->next = NULL;
        // printf("\n Retrieved IQ entry PC value: %d", ret_ptr->pc_value);
        return ret_ptr;
    }
    else
    {
        // printf("Head ptr is not an INT instruction");
    }

    while(current != NULL) 
    {
        temp = current;
        current = current->next;
        if (current == NULL)
        {
            return NULL;
        }

        if (current->FU_type == MUL_FU)
        {
            if (current->src1_ready_bit && current->src2_ready_bit)
            {
                ret_ptr = current;
                temp->next = current->next;
                ret_ptr->next = NULL;
                return ret_ptr;
            }
        }
    }
    return NULL;
}


// Get BranchFU instruction ready for execution
IQ_Entry * getInstructionForBranchFU(APEX_CPU *cpu)
{
    IQ_Entry *temp, *current, *ret_ptr;
    current = cpu->iq_head;
    if (current != NULL 
            && current->FU_type == BRANCH_FU 
            && current->src1_ready_bit && current->src2_ready_bit)
    {
        
        ret_ptr = current;
        // current = current->next;
        cpu->iq_head = current->next;
        ret_ptr->next = NULL;
        // printf("\n Retrieved IQ entry PC value: %d", ret_ptr->pc_value);
        return ret_ptr;
    }
    else
    {
        // printf("Head ptr is not an INT instruction");
    }

    while(current != NULL) 
    {
        temp = current;
        current = current->next;
        if (current == NULL)
        {
            return NULL;
        }

        if (current->FU_type == BRANCH_FU)
        {
            if (current->src1_ready_bit && current->src2_ready_bit)
            {
                ret_ptr = current;
                temp->next = current->next;
                ret_ptr->next = NULL;
                return ret_ptr;
            }
        }
    }
    return NULL;
}


// Update IQ entries with latest values
void updateIQEntries(APEX_CPU *cpu, int FU_type)
{
    IQ_Entry *current = cpu->iq_head;

    if (FU_type == INT_FU)
    {
        while (current != NULL)
        {
            switch (cpu->intFU_fwd_bus.opcode)
            {
            // case STORE:
            case OPCODE_MOVC:
            // printf("-----------------------------Updating MOVC result in IQ---------------------------------\n");
                if (current->src1_tag == cpu->intFU_fwd_bus.pd)
                {
                    current->src1_value = cpu->intFU_fwd_bus.result_buffer;
                    current->src1_ready_bit = 1;
                }
                if (current->src2_tag == cpu->intFU_fwd_bus.pd)
                {
                    current->src2_value = cpu->intFU_fwd_bus.result_buffer;
                    current->src2_ready_bit = 1;
                }   
                break;
            
            default:
                break;
            }
            current = current->next;
        }
    }    


    if (FU_type == MUL_FU)
    {
        while (current != NULL)
        {
            switch (cpu->mulFU_fwd_bus.opcode)
            {
                case OPCODE_MUL:
                    if (current->src1_tag == cpu->mulFU_fwd_bus.pd)
                    {
                        current->src1_value = cpu->mulFU_fwd_bus.result_buffer;
                        current->src1_ready_bit = 1;
                    }
                    if (current->src2_tag == cpu->mulFU_fwd_bus.pd)
                    {
                        current->src2_value = cpu->mulFU_fwd_bus.result_buffer;
                        current->src2_ready_bit = 1;
                    }   
                    break;
                
                default:
                    break;
            }
            current = current->next;
        }
    }

    if (FU_type == MEMORY)
    {
        while (current != NULL)
        {
            switch (cpu->memory_fwd_bus.opcode)
            {
            case LOAD:
                if (current->src1_tag == cpu->intFU_fwd_bus.pd)
                {
                    current->src1_value = cpu->intFU_fwd_bus.result_buffer;
                    current->src1_ready_bit = 1;
                }
                if (current->src2_tag == cpu->intFU_fwd_bus.pd)
                {
                    current->src2_value = cpu->intFU_fwd_bus.result_buffer;
                    current->src2_ready_bit = 1;
                }   
                break;
            
            default:
                break;
            }
            current = current->next;
        }
    }    

    return;
}