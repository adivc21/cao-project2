/*
 * apex_macros.h
 * Contains APEX cpu pipeline macros
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#ifndef _MACROS_H_
#define _MACROS_H_

#define FALSE 0x0
#define TRUE 0x1

/* Integers */
#define DATA_MEMORY_SIZE 4096

/* Size of integer register file */
#define REG_FILE_SIZE 16

/* Size of physical register file */
#define PHY_REG_FILE_SIZE 20

/* Size of Issue Queue */
#define ISSUE_QUEUE_SIZE 8

/* Size of Load Store Queue */
#define LOAD_STORE_QUEUE_SIZE 6

/* Size of Reorder Buffer */
#define REORDER_BUFFER_SIZE 16

/* Numeric OPCODE identifiers for instructions */
#define OPCODE_ADD 0x0
#define OPCODE_SUB 0x1
#define OPCODE_MUL 0x2
#define OPCODE_DIV 0x3
#define OPCODE_AND 0x4
#define OPCODE_OR 0x5
#define OPCODE_XOR 0x6
#define OPCODE_MOVC 0x7
#define OPCODE_LOAD 0x8
#define OPCODE_STORE 0x9
#define OPCODE_BZ 0xa
#define OPCODE_BNZ 0xb
#define OPCODE_HALT 0xc

/* Numeric FU identifiers for instructions */
#define INT_FU 0x0
#define MUL_FU 0x1
#define BRANCH_FU 0x2

/* Numeric FU identifiers for instructions */
#define INT_FU 0x0
#define MUL_FU 0x1
#define BRANCH_FU 0x2

/* Numeric Instruction type identifiers for ROB entries */
#define R2R 0x0
#define LOAD 0x1
#define STORE 0x2
#define BRANCH 0x3

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1

/* Set this flag to 1 to enable cycle single-step mode */
#define ENABLE_SINGLE_STEP 1

#define INVALID 0x0
#define VALID 0x1

#define READY 0x0
#define NOT_READY 0x1

#endif
