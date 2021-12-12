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
            && current->FU_type == INT_FU 
            && current->src1_ready_bit && current->src2_ready_bit)
    {
        // printf("\nTest line 1\n");
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
            switch (current->opcode)
            {
            case OPCODE_MOVC:
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
            switch (current->opcode)
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
            switch (current->opcode)
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


// //insert link at the first location
// void insertFirst(int status, int FU_type) 
// {
//    //create a link
//    struct IQ_Entry *link = (struct IQ_Entry*) malloc(sizeof(struct IQ_Entry));
	
//    link->status = status;
//    link->FU_type = FU_type;
	
//    //point it to old first IQ_Entry
//    link->next = head;
	
//    //point first to new first IQ_Entry
//    head = link;
// }

// //delete first item
// struct IQ_Entry* deleteFirst() 
// {
//    //save reference to first link
//    struct IQ_Entry *tempLink = head;
	
//    //mark next to first link as first 
//    head = head->next;
	
//    //return the deleted link
//    return tempLink;
// }

// //is list empty
// int isEmpty() 
// {
//    return head == NULL;
// }

// int length() 
// {
//    int length = 0;
//    struct IQ_Entry *current;
	
//    for(current = head; current != NULL; current = current->next) {
//       length++;
//    }
	
//    return length;
// }

// //find a link with given status
// struct IQ_Entry* find(int status) 
// {
//    //start from the first link
//    struct IQ_Entry* current = head;

//    //if list is empty
//    if(head == NULL) 
//    {
//       return NULL;
//    }

//    //navigate through list
//    while(current->status != status) 
//    {
//       //if it is last IQ_Entry
//       if(current->next == NULL) 
//       {
//          return NULL;
//       } 
//       else 
//       {
//          //go to next link
//          current = current->next;
//       }
//    }      
	
//    //if FU_type found, return the current Link
//    return current;
// }

// //delete a link with given status
// struct IQ_Entry* delete(int status) 
// {
//    //start from the first link
//    struct IQ_Entry* current = head;
//    struct IQ_Entry* previous = NULL;
	
//    //if list is empty
//    if(head == NULL) 
//    {
//       return NULL;
//    }

//    //navigate through list
//    while(current->status != status) 
//    {
//       //if it is last IQ_Entry
//       if(current->next == NULL) 
//       {
//          return NULL;
//       } 
//       else 
//       {
//          //store reference to current link
//          previous = current;
//          //move to next link
//          current = current->next;
//       }
//    }

//    //found a match, update the link
//    if(current == head) 
//    {
//       //change first to point to next link
//       head = head->next;
//    } 
//    else 
//    {
//       //bypass the current link
//       previous->next = current->next;
//    }    
	
//    return current;
// }


// void main() {
//    insertFirst(1,10);
//    insertFirst(2,20);
//    insertFirst(3,30);
//    insertFirst(4,1);
//    insertFirst(5,40);
//    insertFirst(6,56); 

//    printf("Original List: "); 
	
//    //print list
//    printList();

//    while(!isEmpty()) {            
//       struct IQ_Entry *temp = deleteFirst();
//       printf("\nDeleted value:");
//       printf("(%d,%d) ",temp->status,temp->FU_type);
//    }  
	
//    printf("\nList after deleting all items: ");
//    printList();
//    insertFirst(1,10);
//    insertFirst(2,20);
//    insertFirst(3,30);
//    insertFirst(4,1);
//    insertFirst(5,40);
//    insertFirst(6,56);
   
//    printf("\nRestored List: ");
//    printList();
//    printf("\n");  

//    struct IQ_Entry *foundLink = find(4);
	
//    if(foundLink != NULL) {
//       printf("Element found: ");
//       printf("(%d,%d) ",foundLink->status,foundLink->FU_type);
//       printf("\n");  
//    } else {
//       printf("Element not found.");
//    }

//    delete(4);
//    printf("List after deleting an item: ");
//    printList();
//    printf("\n");
//    foundLink = find(4);
	
//    if(foundLink != NULL) {
//       printf("Element found: ");
//       printf("(%d,%d) ",foundLink->status,foundLink->FU_type);
//       printf("\n");
//    } else {
//       printf("Element not found.");
//    }
	
//    printf("\n");
// }