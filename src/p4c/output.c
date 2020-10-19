#include <p4c/output.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char* it;
	char* it_end;
} p4c_output_state_t;

static void p4c_write_str(p4c_output_state_t* state, const char* str) {
	while (*str != '\0') {
		if (state->it > state->it_end) {
			fprintf(stderr, "Output stage failed:\nOutput string buffer overflow detected\n");
			exit(6);
		}

		*state->it = *str;
		++state->it;
		++str;
	}
}

static void p4c_write_label(p4c_output_state_t* state, unsigned int label, p4c_bool_t force_16_chr) {
	char str[17];
	str[0] = 'l';
	int i = 1;
	int d = label;
	for (; d > 0; ++i) {
		str[i] = '0' + d % 10;
		if (i >= 14) {
			fprintf(stderr, "Output stage failed:\nLabel %d too big\n", label);
			exit(6);
		}
		d /= 10;
	}

	str[i++] = ':';

	if (force_16_chr) {
		for (; i < 16; ++i) {
			str[i] = ' ';
		}
	}
	
	str[i] = '\0';
	p4c_write_str(state, str);
}

static void p4c_write_instruction(p4c_output_state_t* state, const p4c_instruction_t* ins) {
	if (ins->label != 0) {
		p4c_write_label(state, ins->label, P4C_TRUE);
	}
	else {
		p4c_write_str(state, "                "); // 16-character spacing
	}

	const char* op_str = "NOP     ";
	char arg_str[32];
	int arg_str_i = 0;

#define PRINT_REG(reg) do { arg_str[arg_str_i++] = 'R'; arg_str[arg_str_i++] = '0' + reg; } while(0)
#define PRINT_M_REG(reg) do {\
	arg_str[arg_str_i++] = '[';\
	arg_str[arg_str_i++] = 'R';\
	arg_str[arg_str_i++] = '0' + reg;\
    arg_str[arg_str_i++] = ']'; } while(0)
#define PRINT_SEP() do { arg_str[arg_str_i++] = ','; arg_str[arg_str_i++] = ' '; } while(0)
#define PRINT_INT(lhs, rhs) do {\
	unsigned int x = ((unsigned int)(lhs) << 8) | (unsigned int)(rhs);\
	int i = arg_str_i;\
	if (x == 0) { arg_str[arg_str_i++] = '0'; }\
	while (x > 0) {\
		arg_str[arg_str_i++] = '0' + x % 10;\
		x /= 10;\
	}\
	for (int j = 0; j < (arg_str_i - i) / 2; ++j) {\
		char swp = arg_str[i + j];\
		arg_str[i + j] = arg_str[arg_str_i - j - 1];\
		arg_str[arg_str_i - j - 1] = swp;\
	}\
	} while(0)
#define PRINT_LABEL(lhs, rhs) do {\
	arg_str[arg_str_i++] = 'l';\
	PRINT_INT(lhs, rhs);\
	} while(0)
#define PRINT_2_REG() do { PRINT_REG(ins->arg1); PRINT_SEP(); PRINT_REG(ins->arg2); } while(0)
#define PRINT_3_REG() do { PRINT_REG(ins->arg1); PRINT_SEP(); PRINT_REG(ins->arg2); PRINT_SEP(); PRINT_REG(ins->arg3); } while(0)
#define PRINT_PADDRESS() do {\
	if (ins->arg3 == 0xFF) { PRINT_LABEL(ins->arg1, ins->arg2); }\
	else { PRINT_REG(ins->arg3); }\
	} while(0)

	switch (ins->op) {
	case P4C_OP_NEG: op_str = "NEG     "; PRINT_REG(ins->arg1); break;
	case P4C_OP_INC: op_str = "INC     "; PRINT_REG(ins->arg1); break;
	case P4C_OP_DEC: op_str = "DEC     "; PRINT_REG(ins->arg1); break;
	case P4C_OP_ADD: op_str = "ADD     "; PRINT_3_REG(); break;
	case P4C_OP_ADDC: op_str = "ADDC    "; PRINT_3_REG(); break;
	case P4C_OP_SUB: op_str = "SUB     "; PRINT_3_REG(); break;
	case P4C_OP_SUBB: op_str = "SUBB    "; PRINT_3_REG(); break;
	case P4C_OP_CMP: op_str = "CMP     "; PRINT_2_REG(); break;

	case P4C_OP_COM: op_str = "COM     "; PRINT_REG(ins->arg1); break;
	case P4C_OP_AND: op_str = "AND     "; PRINT_3_REG(); break;
	case P4C_OP_OR: op_str = "OR      "; PRINT_3_REG(); break;
	case P4C_OP_XOR: op_str = "XOR     "; PRINT_3_REG(); break;
	case P4C_OP_TEST: op_str = "TEST    "; PRINT_2_REG(); break;

	case P4C_OP_SHR: op_str = "SHR     "; PRINT_REG(ins->arg1); break;
	case P4C_OP_SHL: op_str = "SHL     "; PRINT_REG(ins->arg1); break;
	case P4C_OP_SHRA: op_str = "SHRA    "; PRINT_REG(ins->arg1); break;
	case P4C_OP_SHLA: op_str = "SHLA    "; PRINT_REG(ins->arg1); break;
	case P4C_OP_ROR: op_str = "ROR     "; PRINT_REG(ins->arg1); break;
	case P4C_OP_ROL: op_str = "ROL     "; PRINT_REG(ins->arg1); break;
	case P4C_OP_RORC: op_str = "RORC    "; PRINT_REG(ins->arg1); break;
	case P4C_OP_ROLC: op_str = "ROLC    "; PRINT_REG(ins->arg1); break;

	case P4C_OP_BR: op_str = "BR      "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_Z: op_str = "BR.Z    "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_NZ: op_str = "BR.NZ   "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_N: op_str = "BR.N    "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_NN: op_str = "BR.NN   "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_P: op_str = "BR.P    "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_NP: op_str = "BR.NP   "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_C: op_str = "BR.C    "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_NC: op_str = "BR.NC   "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_O: op_str = "BR.O    "; PRINT_PADDRESS(); break;
	case P4C_OP_BR_NO: op_str = "BR.NO   "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP: op_str = "JMP     "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_Z: op_str = "JMP.Z   "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_NZ: op_str = "JMP.NZ  "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_N: op_str = "JMP.N   "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_NN: op_str = "JMP.NN  "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_P: op_str = "JMP.P   "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_NP: op_str = "JMP.NP  "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_C: op_str = "JMP.C   "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_NC: op_str = "JMP.NC  "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_O: op_str = "JMP.O   "; PRINT_PADDRESS(); break;
	case P4C_OP_JMP_NO: op_str = "JMP.NO  "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL: op_str = "JAL     "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_Z: op_str = "JAL.Z   "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_NZ: op_str = "JAL.NZ  "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_N: op_str = "JAL.N   "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_NN: op_str = "JAL.NN  "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_P: op_str = "JAL.P   "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_NP: op_str = "JAL.NP  "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_C: op_str = "JAL.C   "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_NC: op_str = "JAL.NC  "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_O: op_str = "JAL.O   "; PRINT_PADDRESS(); break;
	case P4C_OP_JAL_NO: op_str = "JAL.NO  "; PRINT_PADDRESS(); break;
	case P4C_OP_RTI: op_str = "RTI"; break;
	case P4C_OP_INT: op_str = "INT     "; PRINT_INT(ins->arg1, ins->arg2); break;
	
	case P4C_OP_MOV: op_str = "MOV     "; PRINT_2_REG(); break;
	case P4C_OP_LOAD: op_str = "LOAD    "; PRINT_REG(ins->arg1); PRINT_SEP(); PRINT_M_REG(ins->arg2, ins->arg3); break;
	case P4C_OP_STOR: op_str = "STOR    "; PRINT_M_REG(ins->arg1, ins->arg2); PRINT_SEP(); PRINT_REG(ins->arg3); break;
	case P4C_OP_MVI: op_str = "MVI     "; PRINT_REG(ins->arg1); PRINT_SEP(); PRINT_INT(ins->arg2, ins->arg3); break;
	case P4C_OP_MVIH: op_str = "MVIH    "; PRINT_REG(ins->arg1); PRINT_SEP(); PRINT_INT(ins->arg2, ins->arg3); break;
	case P4C_OP_MVIL: op_str = "MVIL    "; PRINT_REG(ins->arg1); PRINT_SEP(); PRINT_INT(ins->arg2, ins->arg3); break;

	case P4C_OP_NOP: op_str = "NOP"; break;
	case P4C_OP_ENI: op_str = "ENI"; break;
	case P4C_OP_DSI: op_str = "DSI"; break;
	case P4C_OP_STC: op_str = "STC"; break;
	case P4C_OP_CLC: op_str = "CLC"; break;
	case P4C_OP_CMC: op_str = "CMC"; break;
	}

	arg_str[arg_str_i] = '\0';
	p4c_write_str(state, op_str);
	p4c_write_str(state, arg_str);
	p4c_write_str(state, "\n");

#undef PRINT_REG
#undef PRINT_M_REG
#undef PRINT_SEP
#undef PRINT_INT
#undef PRINT_LABEL
#undef PRINT_2_REG
#undef PRINT_3_REG
#undef PRINT_PADDRESS
}

void p4c_build_output(const p4c_instruction_t* instructions, int instruction_count, char* str, int str_sz) {
	p4c_output_state_t state;
	state.it = str;
	state.it_end = str + str_sz - 1;

	for (int i = 0; i < instruction_count; ++i) {
		p4c_write_instruction(&state, &instructions[i]);
	}

	*state.it = '\0';
}
