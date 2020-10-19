#include <p4c/generator.h>
#include <p4c/utils.h>

#include <stdlib.h>

typedef struct {
	// The base type of the variable, eg i16, u16
	p4c_enum_t base;

	// Number of indirections this type has.
	// i16 -> 0
	// *i16 -> 1
	// **i16 -> 2
	// (...)
	int indirection;
} p4c_type_t;

typedef struct p4c_stack_variable_t p4c_stack_variable_t;

struct p4c_stack_variable_t {
	p4c_type_t type;
	const char* name;
	int name_sz;

	// Address relative to the beginning of the current stack frame
	int address;
	// Index of the current stack frame (0 = global variable)
	int stack_frame;

	// Pointer to the previous and next variables (theses variables may be in different stack frames)
	p4c_stack_variable_t* prev;
	p4c_stack_variable_t* next;
};

typedef struct p4c_function_param_t p4c_function_param_t;

struct p4c_function_param_t {
	const char* name;
	int name_sz;
	p4c_type_t type;
	p4c_function_param_t* next;
};

typedef struct p4c_function_t p4c_function_t;

struct p4c_function_t {
	const char* name;
	int name_sz;
	p4c_type_t return_type;
	p4c_function_param_t* first_param;
	const p4c_node_t* body;
	p4c_function_t* next;
};

typedef struct {
	p4c_instruction_t* instructions;
	int instruction_count, instructions_sz;

	p4c_function_t* first_func;

	int next_label;
} p4c_generator_state_t;

static int p4c_put_label(p4c_generator_state_t* state, int label) {
	state->next_label = label;
}

static void p4c_put_instruction(p4c_generator_state_t* state, const p4c_instruction_t* ins) {
	if (state->instruction_count >= state->instructions_sz) {
		fprintf(stderr, "Generator stage failed:\nInstruction buffer overflow\n");
		exit(5);
	}

	state->instructions[state->instruction_count] = *ins;
	state->instructions[state->instruction_count++].label = state->next_label;
	state->next_label = 0;
}

static void p4c_put_u16(p4c_generator_state_t* state, p4c_enum_t reg, unsigned int u16) {
	p4c_instruction_t ins;
	ins.op = P4C_OP_MVI;
	ins.arg1 = reg;
	ins.arg2 = (u16 & 0xFF00) >> 8;
	ins.arg3 = (u16 & 0x00FF) >> 0;
	p4c_put_instruction(state, &ins);
}

static void p4c_read_variable(p4c_generator_state_t* state, const p4c_node_t* identifier, p4c_enum_t reg) {
	// Get variable offset 
	
	p4c_put_u16(state, P4C_R5, 0);

	p4c_instruction_t ins;
	ins.op = P4C_OP_LOAD;
	ins.arg1 = reg;
	// TO DO
}

static void p4c_gen_function(p4c_generator_state_t* state, const p4c_node_t* func) {

}

static void p4c_analyze_function(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_function_t* func = (p4c_function_t*)malloc(sizeof(p4c_function_t));
	func->next = state->first_func;
	state->first_func = func;

	// Get identifier
	func->name = node->attribute;
	func->name_sz = node->attribute_sz;

	// Get param types
	func->first_param = NULL;
	p4c_function_param_t* last_f_param = NULL;
	for (p4c_node_t* param = node->first->first; param != NULL; param = param->next) {
		p4c_function_param_t* f_param = (p4c_function_param_t*)malloc(sizeof(p4c_function_param_t));
		if (last_f_param == NULL) {
			func->first_param = f_param;
		}
		else {
			last_f_param->next = f_param;
		}
		f_param->next = NULL;

		f_param->type.indirection = -1;
		for (p4c_node_t* c = param->first; c != NULL; c = c->first) {
			f_param->type.base = c->info->type;
			f_param->type.indirection += 1;
		}

		f_param->name = param->attribute;
		f_param->name_sz = param->attribute_sz;

		last_f_param = f_param;
	}

	// Get return type
	func->return_type.indirection = -1;
	for (p4c_node_t* c = node->first->next; c != NULL; c = c->first) {
		func->return_type.base = c->info->type;
		func->return_type.indirection += 1;
	}

	// Get body
	func->body = node->first->next->next;
}

static void p4c_gen_program(p4c_generator_state_t* state, const p4c_node_t* root) {
	const p4c_node_t* c = root->first;
	while (c != NULL) {
		if (c->info->type == P4C_TOKEN_FUNCTION) {
			p4c_analyze_function(state, c);
		}
		else if (c->info->type == P4C_TOKEN_LET) {
			// TODO
		}
		c = c->next;
	}
}

int p4c_run_generator(const p4c_node_t* ast, p4c_instruction_t* instructions, int instructions_sz) {
	p4c_generator_state_t state;
	state.instructions = instructions;
	state.instruction_count = 0;
	state.instructions_sz = instructions_sz;
	state.next_label = 0;
	state.first_func = NULL;

	p4c_gen_program(&state, ast);

	// Clean-up state
	for (p4c_function_t* func = state.first_func; func != NULL;) {
		p4c_function_t* f = func;
		for (p4c_function_param_t* param = f->first_param; param != NULL;) {
			p4c_function_param_t* p = param;
			param = param->next;
			free(p);
		}
		func = func->next;
		free(f);
	}

	return state.instruction_count;
}
