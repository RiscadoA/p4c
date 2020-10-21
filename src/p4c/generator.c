#include <p4c/generator.h>
#include <p4c/utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	// The base type of the variable, eg i16, u16
	const p4c_token_info_t* base;

	// Number of indirections this type has.
	// i16 -> 0
	// *i16 -> 1
	// **i16 -> 2
	// (...)
	int indirection;
} p4c_type_t;

typedef struct {
	p4c_type_t value_type;
	p4c_bool_t is_reference;
} p4c_expression_type_t;

typedef struct p4c_function_param_t p4c_function_param_t;

struct p4c_function_param_t {
	const char* name;
	int name_sz;
	p4c_type_t type;
	p4c_function_param_t* next;
};

typedef struct p4c_function_t p4c_function_t;

struct p4c_function_t {
	int label;
	const char* name;
	int name_sz;
	p4c_type_t return_type;
	p4c_function_param_t* first_param;
	const p4c_node_t* body;
	p4c_function_t* next;
	int param_count;
};

typedef struct p4c_stack_frame_t p4c_stack_frame_t;
typedef struct p4c_stack_variable_t p4c_stack_variable_t;

struct p4c_stack_variable_t {
	p4c_type_t type;
	const char* name;
	int name_sz;

	p4c_stack_frame_t* frame;

	// Address relative to the beginning of the current stack frame
	int address;

	// Pointer to the next variable in the stack frame
	p4c_stack_variable_t* next;
};

struct p4c_stack_frame_t {
	int size;
	p4c_stack_variable_t* first;
	p4c_stack_frame_t* prev;
};

typedef struct {
	p4c_instruction_t* instructions;
	int instruction_count, instructions_sz;

	p4c_function_t* main_func;
	p4c_function_t* first_func;
	p4c_function_t* current_func;
	p4c_stack_frame_t* stack_frame;

	int put_label;
	int next_label;
	int stack_frame_depth;
	int temp_stack_sz;
} p4c_generator_state_t;

static int p4c_get_literal_val(const char* str, int str_sz) {
	int base = 10;
	if (str_sz >= 3 && str[0] == '0') {
		if (str[1] == 'x') {
			base = 16;
			str += 2;
			str_sz -= 2;
		}
		else if (str[1] == 'd') {
			base = 10;
			str += 2;
			str_sz -= 2;
		}
		else if (str[1] == 'b') {
			base = 2;
			str += 2;
			str_sz -= 2;
		}
	}

	return strtol(str, NULL, base);
}

static void p4c_put_label(p4c_generator_state_t* state, int label) {
	state->put_label = label;
}

static void p4c_put_instruction(p4c_generator_state_t* state, const p4c_instruction_t* ins) {
	if (state->instruction_count >= state->instructions_sz) {
		fprintf(stderr, "Generator stage failed:\nInstruction buffer overflow\n");
		exit(5);
	}

	state->instructions[state->instruction_count] = *ins;
	state->instructions[state->instruction_count++].label = state->put_label;
	state->put_label = 0;
}

static void p4c_put_u16(p4c_generator_state_t* state, p4c_enum_t reg, unsigned int u16) {
	p4c_instruction_t ins;
	ins.op = P4C_OP_MVI;
	ins.arg1 = reg;
	ins.arg2 = (u16 & 0xFF00) >> 8;
	ins.arg3 = (u16 & 0x00FF) >> 0;
	p4c_put_instruction(state, &ins);
}

static void p4c_jmp_reg(p4c_generator_state_t* state, unsigned char op, p4c_enum_t reg) {
	p4c_instruction_t ins;
	ins.op = op;
	ins.arg3 = reg;
	p4c_put_instruction(state, &ins);
}

static void p4c_jmp_label(p4c_generator_state_t* state, unsigned char op, unsigned int label) {
	p4c_instruction_t ins;
	ins.op = op;
	ins.arg1 = (label & 0xFF00) >> 8;
	ins.arg2 = (label & 0x00FF) >> 0;
	ins.arg3 = 0xFF;
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

static p4c_type_t p4c_type_from_node(const p4c_node_t* node) {
	p4c_type_t type;
	type.indirection = -1;
	for (p4c_node_t* c = node; c != NULL; c = c->first) {
		type.base = c->info;
		type.indirection += 1;
	}
	return type;
}

static void p4c_push_stack_frame(p4c_generator_state_t* state) {
	p4c_stack_frame_t* frame = (p4c_stack_frame_t*)malloc(sizeof(p4c_stack_frame_t));
	frame->prev = state->stack_frame;
	frame->first = NULL;
	frame->size = 0;
	state->stack_frame = frame;
	state->stack_frame_depth += 1;
}

static void p4c_pop_stack_frame(p4c_generator_state_t* state) {
	if (state->stack_frame->prev == NULL) {
		fprintf(stderr, "Generator stage failed:\np4c_pop_stack_frame() called but there are no more stack frames\n");
		exit(5);
	}

	p4c_stack_frame_t* frame = state->stack_frame;
	state->stack_frame = state->stack_frame->prev;
	
	for (p4c_stack_variable_t* var = frame->first; var != NULL;) {
		p4c_stack_variable_t* next = var->next;
		free(var);
		var = next;
	}

	free(frame);

	state->stack_frame_depth -= 1;
}

void p4c_declare_parameter(p4c_generator_state_t* state, const char* name, int name_sz, p4c_type_t type) {
	for (p4c_stack_variable_t* var = state->stack_frame->first; var != NULL; var = var->next) {
		if (var->name_sz == name_sz && strncmp(var->name, name, name_sz) == 0) {
			fprintf(stderr, "Generator stage failed:\nParameter declared two times\n");
			exit(5);
		}
	}

	p4c_stack_variable_t* var = (p4c_stack_variable_t*)malloc(sizeof(p4c_stack_variable_t));
	var->frame = state->stack_frame;
	var->address = 0;
	var->name = name;
	var->name_sz = name_sz;
	var->type = type;
	for (p4c_stack_variable_t* var = state->stack_frame->first; var != NULL; var = var->next) {
		var->address += 1; // TODO, add other possibles sizes other than 1 word
	}
	var->next = state->stack_frame->first;
	state->stack_frame->first = var;
	state->stack_frame->size += 1;
}

p4c_stack_variable_t* p4c_declare_variable(p4c_generator_state_t* state, const char* name, int name_sz, p4c_type_t type) {
	for (p4c_stack_variable_t* var = state->stack_frame->first; var != NULL; var = var->next) {
		if (var->name_sz == name_sz && strncmp(var->name, name, name_sz) == 0) {
			fprintf(stderr, "Generator stage failed:\nVariable declared two times\n");
			exit(5);
		}
	}

	p4c_stack_variable_t* var = (p4c_stack_variable_t*)malloc(sizeof(p4c_stack_variable_t));
	var->frame = state->stack_frame;
	var->address = 0;
	var->name = name;
	var->name_sz = name_sz;
	var->type = type;
	for (p4c_stack_variable_t* var = state->stack_frame->first; var != NULL; var = var->next) {
		var->address += 1; // TODO, add other possibles sizes other than 1 word
	}
	var->next = state->stack_frame->first;
	state->stack_frame->first = var;
	state->stack_frame->size += 1;

	return var;
}

p4c_stack_variable_t* p4c_find_variable(p4c_generator_state_t* state, const char* name, int name_sz) {
	for (p4c_stack_frame_t* frame = state->stack_frame; frame != NULL; frame = frame->prev) {
		for (p4c_stack_variable_t* var = frame->first; var != NULL; var = var->next) {
			if (var->name_sz == name_sz && strncmp(var->name, name, name_sz) == 0) {
				return var;
			}
		}
	}

	fprintf(stderr, "Generator stage failed:\nCouldn't find variable '%.*s'\n", name_sz, name);
	exit(5);
}

static p4c_bool_t p4c_analyze_compound_statement(p4c_generator_state_t* state, const p4c_node_t* node, p4c_bool_t push_stack);

static p4c_expression_type_t p4c_analyze_expression(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_expression_type_t type, type2;

	switch (node->info->type) {
	case P4C_TOKEN_LOGICAL_AND:
	case P4C_TOKEN_LOGICAL_OR:
	case P4C_TOKEN_LOGICAL_NOT:
		fprintf(stderr, "Generator stage failed:\nLogical operators are only allowed inside conditions\n");
		exit(5);
	case P4C_TOKEN_EQUAL:
	case P4C_TOKEN_DIFFERENT:
	case P4C_TOKEN_GREATER:
	case P4C_TOKEN_LESS:
	case P4C_TOKEN_GEQUAL:
	case P4C_TOKEN_LEQUAL:
		fprintf(stderr, "Generator stage failed:\nComparison operators are only allowed inside conditions\n");
		exit(5);

	case P4C_TOKEN_ADD:
	case P4C_TOKEN_SUBTRACT:
		if (node->first->next == NULL) {
			type = p4c_analyze_expression(state, node->first);
			type.is_reference = P4C_FALSE;
			if (type.value_type.base->type != P4C_TOKEN_I16 && type.value_type.indirection == 0) {
				fprintf(stderr, "Generator stage failed:\nUnsupported expression type '%s' on unary operator '%s'\n", type.value_type.base->name, node->info->name);
				exit(5);
			}

			if (type.value_type.base == P4C_TOKEN_U16 &&
				type.value_type.indirection == 0 &&
				!type.is_reference) {
				type.value_type.base = P4C_TOKEN_I16;
			}

			break;
		}

		// The break is missing on purpose ;)

	case P4C_TOKEN_BINARY_AND:
	case P4C_TOKEN_BINARY_OR:
	case P4C_TOKEN_BINARY_XOR:
		type = p4c_analyze_expression(state, node->first);
		type.is_reference = P4C_FALSE;
		if (type.value_type.base->type != P4C_TOKEN_I16 && type.value_type.base->type != P4C_TOKEN_U16 && type.value_type.indirection == 0) {
			fprintf(stderr, "Generator stage failed:\nUnsupported expression type '%s' on binary operator '%s'\n", type.value_type.base->name, node->info->name);
			exit(5);
		}
		type2 = p4c_analyze_expression(state, node->first);
		if (type.value_type.base->type != type2.value_type.base->type) {
			fprintf(stderr, "Generator stage failed:\nOperands with differents types ('%s' vs '%s') on binary operator '%s'\n", type.value_type.base->name, type2.value_type.base->name, node->info->name);
			exit(5);
		}
		break;

	case P4C_TOKEN_ASSIGN:
		type = p4c_analyze_expression(state, node->first);
		if (type.value_type.base->type != P4C_TOKEN_I16 && type.value_type.base->type != P4C_TOKEN_U16 && type.value_type.indirection == 0) {
			fprintf(stderr, "Generator stage failed:\nUnsupported expression type '%s' on binary operator '%s'\n", type.value_type.base->name, node->info->name);
			exit(5);
		}
		if (!type.is_reference) {
			fprintf(stderr, "Generator stage failed:\nThe left hand side operand of an assignment must be a reference\n");
			exit(5);
		}
		type2 = p4c_analyze_expression(state, node->first);
		if (type.value_type.base->type != type2.value_type.base->type) {
			fprintf(stderr, "Generator stage failed:\nOperands with differents types ('%s' vs '%s') on binary operator '%s'\n", type.value_type.base->name, type2.value_type.base->name, node->info->name);
			exit(5);
		}
		break;

	case P4C_TOKEN_BINARY_NOT:
	case P4C_TOKEN_INC:
	case P4C_TOKEN_DEC:
		type = p4c_analyze_expression(state, node->first);
		type.is_reference = P4C_FALSE;
		if (type.value_type.base->type != P4C_TOKEN_I16 && type.value_type.base->type != P4C_TOKEN_U16 && type.value_type.indirection == 0) {
			fprintf(stderr, "Generator stage failed:\nUnsupported expression type '%s' on unary operator '%s'\n", type.value_type.base->name, node->info->name);
			exit(5);
		}
		break;

	case P4C_TOKEN_AS:
		p4c_analyze_expression(state, node->first);
		type.value_type = p4c_type_from_node(node->first->next);
		type.is_reference = P4C_FALSE;
		break;

	case P4C_TOKEN_IDENTIFIER:
	{
		p4c_stack_variable_t* var = p4c_find_variable(state, node->attribute, node->attribute_sz);
		type.value_type = var->type;
		type.is_reference = P4C_TRUE;
		break;
	}
		

	case P4C_TOKEN_CALL:
	{
		// Find function
		if (node->attribute_sz == 3 && strncmp("shl", node->attribute, node->attribute_sz) == 0) {
			if (node->first == NULL || node->first->next != NULL) {
				fprintf(stderr, "Generator stage failed:\nFunction 'shl' called with wrong number of arguments (should be 1)\n");
				exit(5);
			}

			type.value_type = p4c_analyze_expression(state, node->first).value_type;
		}
		else if (node->attribute_sz == 3 && strncmp("shr", node->attribute, node->attribute_sz) == 0) {
			if (node->first == NULL || node->first->next != NULL) {
				fprintf(stderr, "Generator stage failed:\nFunction 'shr' called with wrong number of arguments (should be 1)\n");
				exit(5);
			}

			type.value_type = p4c_analyze_expression(state, node->first).value_type;
		}
		else {
			p4c_function_t* func = state->first_func;
			for (; func != NULL; func = func->next) {
				if (strncmp(func->name, node->attribute, min(node->attribute_sz, func->name_sz)) == 0) {
					break;
				}
			}

			if (func == NULL) {
				fprintf(stderr, "Generator stage failed:\nFunction '%.*s' called but not defined\n", node->attribute_sz, node->attribute);
				exit(5);
			}

			type.value_type = func->return_type;

			// Check number of arguments
			const p4c_node_t* n_param = node->first;
			p4c_function_param_t* f_param = func->first_param;
			
			for (;;) {
				if ((n_param == NULL && f_param != NULL) || (n_param != NULL && f_param == NULL)) {
					fprintf(stderr, "Generator stage failed:\nFunction '%.*s' called with wrong number of arguments\n", node->attribute_sz, node->attribute);
					exit(5);
				}
				else if (n_param == NULL) {
					break;
				}

				p4c_expression_type_t exp_type = p4c_analyze_expression(state, n_param);
				if (exp_type.value_type.base->type != f_param->type.base->type || exp_type.value_type.indirection != f_param->type.indirection) {
					fprintf(stderr, "Generator stage failed:\nFunction '%.*s' called with wrong type of argument\n", node->attribute_sz, node->attribute);
					exit(5);
				}

				n_param = n_param->next;
				f_param = f_param->next;
			}
		}

		type.is_reference = P4C_FALSE;
		break;
	}
		

	case P4C_TOKEN_INT_LITERAL:
		type.value_type.base = &P4C_TINFO_U16;
		type.value_type.indirection = 0;
		type.is_reference = P4C_FALSE;
		break;

	case P4C_TOKEN_REFERENCE:
		type = p4c_analyze_expression(state, node->first);
		if (!type.is_reference) {
			fprintf(stderr, "Generator stage failed:\nTemporary values must not be referenced\n");
			exit(5);
		}
		type.value_type.indirection += 1;
		type.is_reference = P4C_FALSE;
		break;

	case P4C_TOKEN_DEREFERENCE:
		type = p4c_analyze_expression(state, node->first);
		if (type.value_type.indirection <= 0) {
			fprintf(stderr, "Generator stage failed:\nOnly pointers may be dereferenced\n");
			exit(5);
		}
		type.value_type.indirection -= 1;
		type.is_reference = P4C_TRUE;
		break;

	default:
		fprintf(stderr, "Generator stage failed:\nUnexpected token '%s' on expression\n", node->info->name);
		exit(5);
	}

	return type;
}

static void p4c_analyze_return_statement(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_expression_type_t exp_type;
	if (node->first == NULL) {
		exp_type.value_type.base = &P4C_TINFO_VOID;
		exp_type.value_type.indirection = 0;
		exp_type.is_reference = P4C_FALSE;
	}
	else {
		exp_type = p4c_analyze_expression(state, node->first);
	}

	if (exp_type.value_type.base != state->current_func->return_type.base ||
		exp_type.value_type.indirection != state->current_func->return_type.indirection) {
		fprintf(stderr, "Generator stage failed:\nReturn expression type doesn't match the function's return type\n");
		exit(5);
	}
}

static void p4c_analyze_let_statement(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_expression_type_t exp_type = p4c_analyze_expression(state, node->first);
	p4c_declare_variable(state, node->attribute, node->attribute_sz, exp_type.value_type);
}

static void p4c_analyze_condition(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_expression_type_t type1, type2;
	switch (node->info->type) {
	case P4C_TOKEN_EQUAL:
	case P4C_TOKEN_DIFFERENT:
	case P4C_TOKEN_GREATER:
	case P4C_TOKEN_LESS:
	case P4C_TOKEN_GEQUAL:
	case P4C_TOKEN_LEQUAL:
		type1 = p4c_analyze_expression(state, node->first);
		type2 = p4c_analyze_expression(state, node->first->next);
		if (type1.value_type.base->type != type2.value_type.base->type || type1.value_type.indirection != type2.value_type.indirection) {
			fprintf(stderr, "Generator stage failed:\nComparison operator '%s' argument types don't match\n", node->info->name);
			exit(5);
		}
		break;
	case P4C_TOKEN_LOGICAL_AND:
	case P4C_TOKEN_LOGICAL_OR:
		p4c_analyze_condition(state, node->first);
		p4c_analyze_condition(state, node->first->next);
		break;
	case P4C_TOKEN_LOGICAL_NOT:
		p4c_analyze_condition(state, node->first);
		break;
	default:
		fprintf(stderr, "Generator stage failed:\nA condition must be formed using logical and comparison operators (unsupported operator '%s')\n", node->info->name);
		exit(5);
	}
}

static p4c_bool_t p4c_analyze_if_statement(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_bool_t finished = P4C_FALSE;

	p4c_analyze_condition(state, node->first);
	finished = p4c_analyze_compound_statement(state, node->first->next, P4C_TRUE);
	if (node->first->next->next != NULL) {
		finished = finished && p4c_analyze_compound_statement(state, node->first->next->next, P4C_TRUE);
	}
	else {
		finished = P4C_FALSE;
	}

	return finished;
}

static void p4c_analyze_while_statement(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_analyze_condition(state, node->first);
	p4c_analyze_compound_statement(state, node->first->next, P4C_TRUE);
}

static p4c_bool_t p4c_analyze_compound_statement(p4c_generator_state_t* state, const p4c_node_t* node, p4c_bool_t push_stack) {
	if (push_stack) {
		p4c_push_stack_frame(state);
	}

	p4c_bool_t finished = P4C_FALSE;

	for (const p4c_node_t* c = node->first; c != NULL; c = c->next) {
		if (finished) {
			fprintf(stderr, "Generator stage failed:\nFound a statement after a return statement (code never executed).\n");
			exit(5);
		}

		switch (c->info->type) {
		case P4C_TOKEN_COMPOUND_STATEMENT: finished = p4c_analyze_compound_statement(state, c, P4C_TRUE); break;
		case P4C_TOKEN_RETURN: p4c_analyze_return_statement(state, c); finished = P4C_TRUE; break;
		case P4C_TOKEN_LET: p4c_analyze_let_statement(state, c); break;
		case P4C_TOKEN_IF: finished = p4c_analyze_if_statement(state, c); break;
		case P4C_TOKEN_WHILE: p4c_analyze_while_statement(state, c); break;
		default: p4c_analyze_expression(state, c); break;
		}
	}

	if (push_stack) {
		p4c_pop_stack_frame(state);
	}

	return finished;
}

static void p4c_analyze_function(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_function_t* func = (p4c_function_t*)malloc(sizeof(p4c_function_t));
	state->current_func = func;
	func->next = state->first_func;
	state->first_func = func;
	func->label = state->next_label++;

	// Get identifier
	func->name = node->attribute;
	func->name_sz = node->attribute_sz;

	// Get param types
	func->param_count = 0;
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
		f_param->type = p4c_type_from_node(param->first);
		f_param->name = param->attribute;
		f_param->name_sz = param->attribute_sz;

		last_f_param = f_param;

		func->param_count += 1;
	}

	// Get return type
	func->return_type = p4c_type_from_node(node->first->next);

	// Get body
	func->body = node->first->next->next;
	p4c_push_stack_frame(state);
	for (p4c_function_param_t* param = state->current_func->first_param; param != NULL; param = param->next) {
		p4c_declare_variable(state, param->name, param->name_sz, param->type);
	}
	if (!p4c_analyze_compound_statement(state, func->body, P4C_FALSE)) {
		// Missing return statement
		fprintf(stderr, "Generator stage failed:\nNot all code paths have a return statement on function '%.*s'.\n", func->name_sz, func->name);
		exit(5);
	}
	p4c_pop_stack_frame(state);

	state->current_func = NULL;
}

static void p4c_push_temp_var(p4c_generator_state_t* state, unsigned char reg);

static p4c_stack_variable_t* p4c_gen_var_address(p4c_generator_state_t* state, const char* name, int name_sz, unsigned char target) {
	int var_offset = 0;
	p4c_stack_variable_t* var = p4c_find_variable(state, name, name_sz);
	p4c_instruction_t ins;

	// If global variable
	if (var->frame->prev == NULL) {
		abort(); // TODO
	}
	// If local variable
	else {
		var_offset = var->address + state->temp_stack_sz + 1;
		for (p4c_stack_frame_t* frame = state->stack_frame; frame != var->frame; frame = frame->prev) {
			var_offset += frame->size;
		}

		// Check if variable is a parameter
		if (var->frame->prev->prev == NULL &&
			var->address >= state->stack_frame->size - state->current_func->param_count) {
			var_offset += 1;
		}

		if (target == 0xFF) {
			p4c_put_u16(state, P4C_R4, var_offset);
			ins.op = P4C_OP_ADD;
			ins.arg1 = P4C_R4;
			ins.arg2 = P4C_R4;
			ins.arg3 = P4C_R6;
			p4c_put_instruction(state, &ins);
			p4c_push_temp_var(state, P4C_R4);
		}
		else {
			p4c_put_u16(state, target, var_offset);
			ins.op = P4C_OP_ADD;
			ins.arg1 = target;
			ins.arg2 = target;
			ins.arg3 = P4C_R6;
			p4c_put_instruction(state, &ins);
		}
	}

	return var;
}

static p4c_expression_type_t p4c_gen_expression(p4c_generator_state_t* state, const p4c_node_t* node, unsigned char target);

static void p4c_push_temp_var(p4c_generator_state_t* state, unsigned char reg) {
	p4c_instruction_t ins;
	ins.op = P4C_OP_STOR;
	ins.arg1 = P4C_R6;
	ins.arg2 = reg;
	p4c_put_instruction(state, &ins);
	ins.op = P4C_OP_DEC;
	ins.arg1 = P4C_R6;
	p4c_put_instruction(state, &ins);
	state->temp_stack_sz += 1;
}

static void p4c_pop_temp_var(p4c_generator_state_t* state, p4c_enum_t reg) {
	p4c_instruction_t ins;
	ins.op = P4C_OP_INC;
	ins.arg1 = P4C_R6;
	p4c_put_instruction(state, &ins);
	ins.op = P4C_OP_LOAD;
	ins.arg1 = reg;
	ins.arg2 = P4C_R6;
	p4c_put_instruction(state, &ins);
	state->temp_stack_sz -= 1;
}

static void p4c_gen_compound_statement(p4c_generator_state_t* state, const p4c_node_t* node, p4c_bool_t push_stack);

static p4c_expression_type_t p4c_gen_reference(p4c_generator_state_t* state, const p4c_node_t* node, unsigned char target) {
	p4c_instruction_t ins;
	p4c_expression_type_t type;

	switch (node->info->type) {
	case P4C_TOKEN_IDENTIFIER:
		type.value_type = p4c_find_variable(state, node->attribute, node->attribute_sz)->type;
		type.is_reference = P4C_TRUE;
		p4c_gen_var_address(state, node->attribute, node->attribute_sz, target);
		break;
	case P4C_TOKEN_DEREFERENCE:
		type = p4c_gen_expression(state, node->first, target);
		type.value_type.indirection -= 1;
		type.is_reference = P4C_TRUE;
		break;
	case P4C_TOKEN_MEMBER:
		abort(); // TODO: implement member access
		break;
	default:
		abort();
	}
	return type;
}

// Expression results are always stored in R4
static p4c_expression_type_t p4c_gen_expression(p4c_generator_state_t* state, const p4c_node_t* node, unsigned char target) {
	p4c_instruction_t ins;
	p4c_expression_type_t type;
	type.is_reference = P4C_FALSE;
	type.value_type.base = P4C_TOKEN_VOID;
	type.value_type.indirection = 0;

	// Function call
	if (node->info->type == P4C_TOKEN_CALL) {
		unsigned char current_reg;

		// Find function
		if (node->attribute_sz == 3 && strncmp("shl", node->attribute, node->attribute_sz) == 0) {
			type.value_type = p4c_gen_expression(state, node->first, (target == 0xFF ? P4C_R4 : target)).value_type;
			ins.op = P4C_OP_SHL;
			ins.arg1 = (target == 0xFF ? P4C_R4 : target);
			p4c_put_instruction(state, &ins);
			current_reg = (target == 0xFF ? P4C_R4 : target);
		}
		else if (node->attribute_sz == 3 && strncmp("shr", node->attribute, node->attribute_sz) == 0) {
			type.value_type = p4c_gen_expression(state, node->first, (target == 0xFF ? P4C_R4 : target)).value_type;
			ins.op = P4C_OP_SHR;
			ins.arg1 = (target == 0xFF ? P4C_R4 : target);
			p4c_put_instruction(state, &ins);
			current_reg = (target == 0xFF ? P4C_R4 : target);
		}
		else {
			p4c_function_t* func = state->first_func;
			for (; func != NULL; func = func->next) {
				if (strncmp(func->name, node->attribute, min(node->attribute_sz, func->name_sz)) == 0) {
					break;
				}
			}

			if (func == NULL) {
				abort();
			}

			type.value_type = func->return_type;

			// Push each argument into the stack
			for (const p4c_node_t* param = node->first; param != NULL; param = param->next) {
				p4c_gen_expression(state, param, 0xFF);
				state->temp_stack_sz -= 1;
			}

			p4c_jmp_label(state, P4C_OP_JAL, func->label);
			current_reg = P4C_R3;
		}

		if (target == 0xFF) {
			p4c_push_temp_var(state, current_reg);
		}
		else if (target != current_reg) {
			ins.op = P4C_OP_MOV;
			ins.arg1 = target;
			ins.arg2 = current_reg;
			p4c_put_instruction(state, &ins);
		}

		type.is_reference = P4C_FALSE;
	}
	// Type conversion
	else if (node->info->type == P4C_TOKEN_AS) {
		// Since the only types supported for now are i16 and u16, there are no conversions to be made
		type = p4c_gen_expression(state, node->first, target);
	}
	// Leaf node
	else if (node->first == NULL) {
		switch (node->info->type) {
		case P4C_TOKEN_IDENTIFIER:
		{
			type.value_type = p4c_gen_var_address(state, node->attribute, node->attribute_sz, P4C_R4)->type;
			type.is_reference = P4C_TRUE;

			if (target == 0xFF) {
				ins.op = P4C_OP_LOAD;
				ins.arg1 = P4C_R4;
				ins.arg2 = P4C_R4;
				p4c_put_instruction(state, &ins);
				p4c_push_temp_var(state, P4C_R4);
			}
			else {
				ins.op = P4C_OP_LOAD;
				ins.arg1 = target;
				ins.arg2 = P4C_R4;
				p4c_put_instruction(state, &ins);
			}
			
			break;
		}
		case P4C_TOKEN_INT_LITERAL:
		{
			if (target == 0xFF) {
				p4c_put_u16(state, P4C_R4, p4c_get_literal_val(node->attribute, node->attribute_sz));
				p4c_push_temp_var(state, P4C_R4);
			}
			else {
				p4c_put_u16(state, target, p4c_get_literal_val(node->attribute, node->attribute_sz));
			}
		}
		}
	}
	// 1 operand
	else if (node->first->next == NULL) {
		switch (node->info->type) {
		case P4C_TOKEN_ADD:
			// Do nothing
			type = p4c_gen_expression(state, node->first, target);
			break;
		case P4C_TOKEN_SUBTRACT:
		case P4C_TOKEN_BINARY_NOT:
		{
			unsigned char op = P4C_OP_SUB;
			switch (node->info->type) {
			case P4C_TOKEN_SUBTRACT: op = P4C_OP_SUB; break;
			case P4C_TOKEN_BINARY_NOT: op = P4C_OP_NEG; break;
			}

			ins.op = op;
			ins.arg2 = P4C_R0;

			if (target == 0xFF) {
				type = p4c_gen_expression(state, node->first, P4C_R4);
				ins.arg1 = P4C_R4;
				ins.arg3 = P4C_R4;
				p4c_put_instruction(state, &ins);
				p4c_push_temp_var(state, P4C_R4);
			}
			else {
				type = p4c_gen_expression(state, node->first, target);
				ins.arg1 = target;
				ins.arg3 = target;
				p4c_put_instruction(state, &ins);
			}

			break;
		}
		case P4C_TOKEN_INC:
		case P4C_TOKEN_DEC:
		{
			unsigned char op = P4C_OP_INC;
			switch (node->info->type) {
			case P4C_TOKEN_INC: op = P4C_OP_INC; break;
			case P4C_TOKEN_DEC: op = P4C_OP_DEC; break;
			}

			if (p4c_analyze_expression(state, node->first).is_reference) {
				if (target == 0xFF) {
					type = p4c_gen_reference(state, node->first, P4C_R5);
					ins.op = P4C_OP_LOAD;
					ins.arg1 = P4C_R4;
					ins.arg2 = P4C_R5;
					p4c_put_instruction(state, &ins);
					ins.op = op;
					ins.arg1 = P4C_R4;
					p4c_put_instruction(state, &ins);
					ins.op = P4C_OP_STOR;
					ins.arg1 = P4C_R5;
					ins.arg2 = P4C_R4;
					p4c_put_instruction(state, &ins);
					p4c_push_temp_var(state, P4C_R4);
				}
				else {
					type = p4c_gen_reference(state, node->first, P4C_R5);
					ins.op = P4C_OP_LOAD;
					ins.arg1 = P4C_R4;
					ins.arg2 = P4C_R5;
					p4c_put_instruction(state, &ins);
					ins.op = op;
					ins.arg1 = P4C_R4;
					p4c_put_instruction(state, &ins);
					ins.op = P4C_OP_STOR;
					ins.arg1 = P4C_R5;
					ins.arg2 = P4C_R4;
					p4c_put_instruction(state, &ins);
					if (target != P4C_R4 && target != P4C_R0) {
						ins.op = P4C_OP_MOV;
						ins.arg1 = target;
						ins.arg2 = P4C_R4;
						p4c_put_instruction(state, &ins);
					}
				}
			}
			else {
				ins.op = op;
				
				if (target == 0xFF) {
					type = p4c_gen_expression(state, node->first, P4C_R4);
					ins.arg1 = P4C_R4;
					p4c_put_instruction(state, &ins);
					p4c_push_temp_var(state, P4C_R4);
				}
				else {
					type = p4c_gen_expression(state, node->first, target);
					ins.arg1 = target;
					p4c_put_instruction(state, &ins);
				}
			}

			break;
		}
		case P4C_TOKEN_REFERENCE:
			p4c_gen_reference(state, node->first, target);
			break;
		case P4C_TOKEN_DEREFERENCE:
			ins.op = P4C_OP_LOAD;

			if (target == 0xFF) {
				type = p4c_gen_expression(state, node->first, P4C_R4);
				ins.arg1 = P4C_R4;
				ins.arg2 = P4C_R4;
				p4c_put_instruction(state, &ins);
				p4c_push_temp_var(state, P4C_R4);
			}
			else {
				type = p4c_gen_expression(state, node->first, target);
				ins.arg1 = target;
				ins.arg2 = target;
				p4c_put_instruction(state, &ins);
			}

			type.value_type.indirection -= 1;
			type.is_reference = P4C_TRUE;
			break;
		}
	}
	// 2 operands
	else if (node->first->next->next == NULL) {
		switch (node->info->type) {
		case P4C_TOKEN_ADD:
		case P4C_TOKEN_SUBTRACT:
		case P4C_TOKEN_BINARY_AND:
		case P4C_TOKEN_BINARY_OR:
		case P4C_TOKEN_BINARY_XOR:
		{
			unsigned char op = P4C_OP_ADD;
			switch (node->info->type) {
			case P4C_TOKEN_ADD: op = P4C_OP_ADD; break;
			case P4C_TOKEN_SUBTRACT: op = P4C_OP_SUB; break;
			case P4C_TOKEN_BINARY_AND: op = P4C_OP_AND; break;
			case P4C_TOKEN_BINARY_OR: op = P4C_OP_OR; break;
			case P4C_TOKEN_BINARY_XOR: op = P4C_OP_XOR; break;
			}

			p4c_gen_expression(state, node->first, 0xFF);
			p4c_gen_expression(state, node->first->next, P4C_R5);
			p4c_pop_temp_var(state, P4C_R4);

			if (target == 0xFF) {
				ins.op = op;
				ins.arg1 = P4C_R4;
				ins.arg2 = P4C_R4;
				ins.arg3 = P4C_R5;
				p4c_put_instruction(state, &ins);
				p4c_push_temp_var(state, P4C_R4);
			}
			else {
				ins.op = op;
				ins.arg1 = target;
				ins.arg2 = P4C_R4;
				ins.arg3 = P4C_R5;
				p4c_put_instruction(state, &ins);
			}
			
			break;
		}
		case P4C_TOKEN_ASSIGN:
		{
			p4c_gen_expression(state, node->first->next, 0xFF);
			p4c_gen_reference(state, node->first, P4C_R4);
			p4c_pop_temp_var(state, P4C_R5);

			// Assign value
			ins.op = P4C_OP_STOR;
			ins.arg1 = P4C_R4;
			ins.arg2 = P4C_R5;
			p4c_put_instruction(state, &ins);

			if (target != P4C_R0) {
				if (target == 0xFF) {
					// Reuse the previous temporary value
					ins.op = P4C_OP_DEC;
					ins.arg1 = P4C_R6;
					p4c_put_instruction(state, &ins);
					state->temp_stack_sz += 1;
				}
				else if (target != P4C_R5) {
					ins.op = P4C_OP_MOV;
					ins.arg1 = target;
					ins.arg2 = P4C_R5;
					p4c_put_instruction(state, &ins);
				}
			}
		}

		}
	}
	else {
		fprintf(stderr, "Generator stage failed:\nUnsupported operator '%s'\n", node->info->name);
		exit(5);
	}

	return type;
}

static void p4c_gen_return_statement(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_instruction_t ins;
	
	if (node->first != NULL) {
		p4c_gen_expression(state, node->first, P4C_R3);
	}

	if (state->current_func == state->main_func) {
		// Halt program
		p4c_put_label(state, state->next_label);
		p4c_jmp_label(state, P4C_OP_BR, state->next_label++);
	}
	else {
		// Unwind stack and get return address
		int offset = 1;
		p4c_stack_frame_t* frame = state->stack_frame;
		for (int i = 0; i < state->stack_frame_depth; ++i) {
			offset += frame->size;
			frame = frame->prev;
		}
		offset -= state->current_func->param_count;

		if (offset > 1) {
			p4c_put_u16(state, P4C_R4, offset);
			ins.op = P4C_OP_ADD;
			ins.arg1 = P4C_R6;
			ins.arg2 = P4C_R6;
			ins.arg3 = P4C_R4;
			p4c_put_instruction(state, &ins); // ADD R6, R6, R4
		}
		else if (offset == 1) {
			ins.op = P4C_OP_INC;
			ins.arg1 = P4C_R6;
			p4c_put_instruction(state, &ins); // INC R6
		}
		ins.op = P4C_OP_LOAD;
		ins.arg1 = P4C_R4;
		ins.arg2 = P4C_R6;
		p4c_put_instruction(state, &ins); // LOAD R4, M[R6]
		if (state->current_func->param_count > 1) {
			p4c_put_u16(state, P4C_R5, state->current_func->param_count);
			ins.op = P4C_OP_ADD;
			ins.arg1 = P4C_R6;
			ins.arg2 = P4C_R6;
			ins.arg3 = P4C_R5;
			p4c_put_instruction(state, &ins); // ADD R6, R6, R5
		}
		else if (state->current_func->param_count == 1) {
			ins.op = P4C_OP_INC;
			ins.arg1 = P4C_R6;
			p4c_put_instruction(state, &ins); // INC R6
		}
		p4c_jmp_reg(state, P4C_OP_JMP, P4C_R4); // JMP R4
	}
}

static void p4c_gen_let_statement(p4c_generator_state_t* state, const p4c_node_t* node) {
	p4c_expression_type_t exp_type = p4c_gen_expression(state, node->first, P4C_R4);
	p4c_stack_variable_t* var = p4c_declare_variable(state, node->attribute, node->attribute_sz, exp_type.value_type);
	p4c_instruction_t ins;
	ins.op = P4C_OP_STOR;
	ins.arg1 = P4C_R6;
	ins.arg2 = P4C_R4;
	p4c_put_instruction(state, &ins); // STOR M[R6], R4
	ins.op = P4C_OP_DEC;
	ins.arg1 = P4C_R6;
	p4c_put_instruction(state, &ins); // DEC R6
}

static void p4c_gen_if_statement(p4c_generator_state_t* state, const p4c_node_t* node) {
	// TODO
}

static void p4c_gen_while_statement(p4c_generator_state_t* state, const p4c_node_t* node) {
	// TODO
}

static void p4c_gen_compound_statement(p4c_generator_state_t* state, const p4c_node_t* node, p4c_bool_t push_stack) {
	if (push_stack) {
		p4c_push_stack_frame(state);
	}

	for (const p4c_node_t* c = node->first; c != NULL; c = c->next) {
		switch (c->info->type) {
		case P4C_TOKEN_COMPOUND_STATEMENT: p4c_gen_compound_statement(state, c, P4C_TRUE); break;
		case P4C_TOKEN_RETURN: p4c_gen_return_statement(state, c); break;
		case P4C_TOKEN_LET: p4c_gen_let_statement(state, c); break;
		case P4C_TOKEN_IF: p4c_gen_if_statement(state, c); break;
		case P4C_TOKEN_WHILE: p4c_gen_while_statement(state, c); break;
		default: p4c_gen_expression(state, c, P4C_R0); break;
		}
	}

	if (push_stack) {
		p4c_pop_stack_frame(state);
	}
}

static void p4c_gen_function(p4c_generator_state_t* state, const p4c_function_t* func) {
	state->current_func = func;
	p4c_push_stack_frame(state);
	p4c_put_label(state, func->label);

	if (func != state->main_func) {
		// Declare parameters
		for (p4c_function_param_t* param = func->first_param; param != NULL; param = param->next) {
			p4c_declare_variable(state, param->name, param->name_sz, param->type);
		}

		// Push R7 to stack
		p4c_instruction_t ins;
		ins.op = P4C_OP_STOR;
		ins.arg1 = P4C_R6;
		ins.arg2 = P4C_R7;
		p4c_put_instruction(state, &ins);
		ins.op = P4C_OP_DEC;
		ins.arg1 = P4C_R6;
		p4c_put_instruction(state, &ins);
	}

	p4c_gen_compound_statement(state, func->body, P4C_FALSE);
	p4c_pop_stack_frame(state);
	state->current_func = NULL;
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

	// Get main function
	state->main_func = state->first_func;
	while (state->main_func != NULL) {
		if (strncmp(state->main_func->name, "main", state->main_func->name_sz) == 0) {
			break;
		}
		state->main_func = state->main_func->next;
	}

	if (state->main_func == NULL) {
		fprintf(stderr, "Generator stage failed:\nfn main() -> void is not defined\n");
		exit(5);
	}
	else if (state->main_func->return_type.base->type != P4C_TOKEN_VOID || state->main_func->return_type.indirection != 0) {
		fprintf(stderr, "Generator stage failed:\nfn main() has an invalid return type (must return void)\n");
		exit(5);
	}
	else if (state->main_func->first_param != NULL) {
		fprintf(stderr, "Generator stage failed:\nfn main() must not have parameters\n");
		exit(5);
	}

	// Initialize stack
	p4c_put_u16(state, P4C_R6, 0x7FFF);

	// Generate main function
	p4c_gen_function(state, state->main_func);

	// Generate other functions
	for (p4c_function_t* func = state->first_func; func != NULL; func = func->next) {
		if (func == state->main_func) {
			continue;
		}

		p4c_gen_function(state, func);
	}
}

int p4c_run_generator(const p4c_node_t* ast, p4c_instruction_t* instructions, int instructions_sz) {
	p4c_generator_state_t state;
	state.instructions = instructions;
	state.instruction_count = 0;
	state.instructions_sz = instructions_sz;
	state.next_label = 1;
	state.first_func = NULL;
	state.stack_frame = NULL;
	state.stack_frame_depth = -1;
	state.temp_stack_sz = 0;
	state.put_label = 0;

	p4c_push_stack_frame(&state);
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
