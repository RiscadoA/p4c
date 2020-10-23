#pragma once

#include <p4c/parser.h>

#define P4C_OP_NEG		0x3E
#define P4C_OP_INC		0x01
#define P4C_OP_DEC		0x02
#define P4C_OP_ADD		0x03
#define P4C_OP_ADDC		0x04
#define P4C_OP_SUB		0x05
#define P4C_OP_SUBB		0x06
#define P4C_OP_CMP		0x07

#define P4C_OP_COM		0x08
#define P4C_OP_AND		0x09
#define P4C_OP_OR		0x0A
#define P4C_OP_XOR		0x0B
#define P4C_OP_TEST		0x0C

#define P4C_OP_SHR		0x0D
#define P4C_OP_SHL		0x0E
#define P4C_OP_SHRA		0x0F
#define P4C_OP_SHLA		0x10
#define P4C_OP_ROR		0x11
#define P4C_OP_ROL		0x12
#define P4C_OP_RORC		0x13
#define P4C_OP_ROLC		0x14

#define P4C_OP_BR		0x15
#define P4C_OP_BR_Z		0x16
#define P4C_OP_BR_NZ	0x17
#define P4C_OP_BR_N		0x18
#define P4C_OP_BR_NN	0x19
#define P4C_OP_BR_P		0x1A
#define P4C_OP_BR_NP	0x1B
#define P4C_OP_BR_C		0x1C
#define P4C_OP_BR_NC	0x1D
#define P4C_OP_BR_O		0x1E
#define P4C_OP_BR_NO	0x1F
#define P4C_OP_JMP		0x20
#define P4C_OP_JMP_Z	0x21
#define P4C_OP_JMP_NZ	0x22
#define P4C_OP_JMP_N	0x23
#define P4C_OP_JMP_NN	0x24
#define P4C_OP_JMP_P	0x25
#define P4C_OP_JMP_NP	0x26
#define P4C_OP_JMP_C	0x27
#define P4C_OP_JMP_NC	0x28
#define P4C_OP_JMP_O	0x29
#define P4C_OP_JMP_NO	0x2A
#define P4C_OP_JAL		0x2B
#define P4C_OP_JAL_Z	0x2C
#define P4C_OP_JAL_NZ	0x2D
#define P4C_OP_JAL_N	0x2E
#define P4C_OP_JAL_NN	0x2F
#define P4C_OP_JAL_P	0x30
#define P4C_OP_JAL_NP	0x31
#define P4C_OP_JAL_C	0x32
#define P4C_OP_JAL_NC	0x33
#define P4C_OP_JAL_O	0x34
#define P4C_OP_JAL_NO	0x35
#define P4C_OP_RTI		0x36
#define P4C_OP_INT		0x37

#define P4C_OP_MOV  	0x38
#define P4C_OP_LOAD 	0x39
#define P4C_OP_STOR		0x3A
#define P4C_OP_MVI		0x3B
#define P4C_OP_MVIH		0x3C
#define P4C_OP_MVIL		0x3D

#define P4C_OP_NOP  	0x00
#define P4C_OP_ENI		0x3F
#define P4C_OP_DSI		0x40
#define P4C_OP_STC		0x41
#define P4C_OP_CLC  	0x42
#define P4C_OP_CMC		0x43

#define P4C_R0			0x00
#define P4C_R1			0x01
#define P4C_R2			0x02
#define P4C_R3			0x03
#define P4C_R4			0x04
#define P4C_R5			0x05
#define P4C_R6			0x06
#define P4C_R7			0x07

typedef struct {
	unsigned int label;
	unsigned char op, arg1, arg2, arg3;
} p4c_instruction_t;

typedef struct p4c_comment_t p4c_comment_t;

struct p4c_comment_t {
	int instruction;
	char txt[64];
	p4c_comment_t* next;
};

typedef struct {
	p4c_comment_t* comment;
} p4c_meta_data_t;

int p4c_run_generator(const p4c_node_t* ast, p4c_instruction_t* instructions, int instructions_sz, p4c_meta_data_t* meta_data);
