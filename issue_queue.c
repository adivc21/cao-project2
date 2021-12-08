#include <stdlib.h>
#include <stdio.h>

#include "issue_queue.h"

// Print the Issue Queue details
void printIssueQueue(APEX_CPU *cpu) 
{

    IQ_Entry *ptr = cpu->iq_head;
    printf("\n\nIssue Queue entries: ");

    int entryNo = 0;
    while(ptr != NULL) 
    {
        printf("\nIssue Queue index: %d", entryNo);
        printf("\nStatus: %d", ptr->status);
        printf("\nFU type: %d", ptr->FU_type);
        printf("\nLiteral value: %d", ptr->imm);
        printf("\nSrc1 ready bit: %d", ptr->src1_ready_bit);
        printf("\nSrc1 tag: %d", ptr->src1_tag);
        printf("\nSrc1 value: %d", ptr->src1_value);
        printf("\nSrc2 ready bit: %d", ptr->src2_ready_bit);
        printf("\nSrc2 tag: %d", ptr->src2_tag);
        printf("\nSrc2 value: %d", ptr->src2_value);
        printf("\nPhy dest reg or LSQ index: %d", ptr->dest_reg_or_lsq_index);
        
        entryNo++;
        ptr = ptr->next;
    }
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