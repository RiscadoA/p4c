#include <p4c/parser.h>


#include <stdlib.h>

const p4c_token_info_t P4C_TINFO_PARAMS = { P4C_TOKEN_PARAMS, P4C_FALSE, P4C_FALSE, P4C_FALSE, "params" };
const p4c_token_info_t P4C_TINFO_COMPOUND_STATEMENT = { P4C_TOKEN_COMPOUND_STATEMENT, P4C_FALSE, P4C_FALSE, P4C_FALSE, "compound-statement" };
const p4c_token_info_t P4C_TINFO_CALL = { P4C_TOKEN_CALL, P4C_FALSE, P4C_FALSE, P4C_FALSE, "call" };
const p4c_token_info_t P4C_TINFO_REFERENCE = { P4C_TOKEN_REFERENCE, P4C_FALSE, P4C_FALSE, P4C_TRUE, "reference" };
const p4c_token_info_t P4C_TINFO_DEREFERENCE = { P4C_TOKEN_DEREFERENCE, P4C_FALSE, P4C_FALSE, P4C_TRUE, "dereference" };

typedef struct {
	const p4c_token_t* it;
	const p4c_token_t* last_token;
	p4c_node_t* nodes;
	int node_count, nodes_sz;
} p4c_parser_state_t;

static p4c_node_t* p4c_get_node(p4c_parser_state_t* state) {
	for (int i = 0; i < state->nodes_sz; ++i) {
		if (!state->nodes[i].active) {
			state->nodes[i].active = P4C_TRUE;
			state->nodes[i].first = NULL;
			state->nodes[i].next = NULL;
			state->nodes[i].info = NULL;
			state->nodes[i].attribute = NULL;
			state->nodes[i].attribute_sz = 0;
			state->node_count += 1;
			return &state->nodes[i];
		}
	}

	fprintf(stderr, "Parser stage failed:\nCouldn't create node, node buffer overflow detected\n");
	exit(4);
}

static void p4c_release_node(p4c_parser_state_t* state, p4c_node_t* node) {
	node->active = P4C_FALSE;
	state->node_count -= 1;
}

static void p4c_add_to_node(p4c_node_t* parent, p4c_node_t* child) {
	p4c_node_t* c = parent->first;
	if (c == NULL) {
		parent->first = child;
	}
	else {
		while (c->next != NULL) {
			c = c->next;
		}
		c->next = child;
	}
	child->next = NULL;
}

static const p4c_token_t* p4c_expect_token(p4c_parser_state_t* state, const p4c_token_info_t* info) {
	if (state->it > state->last_token) {
		fprintf(stderr, "Parser stage failed:\nUnexpected end of file, expected token '%s'\n", info->name);
		exit(4);
	}

	if (state->it->info->type != info->type) {
		fprintf(stderr, "Parser stage failed:\nUnexpected token '%s', expected '%s'\n", state->it->info->name, info->name);
		exit(4);
	}

	return state->it++;
}

static const p4c_token_t* p4c_accept_token(p4c_parser_state_t* state, const p4c_token_info_t* info) {
	if (state->it > state->last_token) {
		return NULL;
	}

	if (state->it->info->type != info->type) {
		return NULL;
	}

	return state->it++;
}

static const p4c_token_t* p4c_accept_type(p4c_parser_state_t* state) {
	if (state->it > state->last_token) {
		return NULL;
	}

	if (!state->it->info->is_type) {
		return NULL;
	}

	return state->it++;
}

static const p4c_token_t* p4c_peek_token(p4c_parser_state_t* state) {
	if (state->it > state->last_token) {
		return NULL;
	}

	return state->it;
}

static void p4c_next_token(p4c_parser_state_t* state) {
	if (state->it > state->last_token) {
		fprintf(stderr, "Parser stage failed:\nUnexpected end of file\n");
		exit(4);
	}

	++state->it;
}

static p4c_node_t* p4c_parse_type(p4c_parser_state_t* state) {
	if (p4c_accept_token(state, &P4C_TINFO_POINTER) != NULL) {
		p4c_node_t* out_node = p4c_get_node(state);
		out_node->info = &P4C_TINFO_POINTER;
		p4c_node_t* sub_type = p4c_parse_type(state);
		if (sub_type == NULL) {
			fprintf(stderr, "Parser stage failed:\nExpected type token after *\n");
			exit(4);
		}
		p4c_add_to_node(out_node, sub_type);
		return out_node;
	}
	
	const p4c_token_t* tok = p4c_accept_type(state);
	if (tok == NULL) {
		return NULL;
	}

	p4c_node_t* out_node = p4c_get_node(state);
	out_node->info = tok->info;
	return out_node;
}

static p4c_node_t* p4c_parse_expression(p4c_parser_state_t* state);

static p4c_node_t* p4c_parse_operator_last(p4c_parser_state_t* state) {
	p4c_node_t* out_node = NULL;
	
	// "(" <expression> ")"
	if (p4c_accept_token(state, &P4C_TINFO_OPEN_PARENTHESIS) != NULL) {
		out_node = p4c_parse_expression(state);
		p4c_expect_token(state, &P4C_TINFO_CLOSE_PARENTHESIS);
		return out_node;
	}

	// <identifier> | <call>	
	const p4c_token_t* tok = p4c_accept_token(state, &P4C_TINFO_IDENTIFIER);
	if (tok != NULL) {
		// <call>
		if (p4c_accept_token(state, &P4C_TINFO_OPEN_PARENTHESIS) != NULL) {
			out_node = p4c_get_node(state);
			out_node->info = &P4C_TINFO_CALL;
			out_node->attribute = tok->attribute;
			out_node->attribute_sz = tok->attribute_sz;

			// Parse params
			if (p4c_accept_token(state, &P4C_TINFO_CLOSE_PARENTHESIS) == NULL) {
				for (int i = 0;; ++i) {
					p4c_node_t* param = p4c_parse_expression(state);
					if (param == NULL) {
						fprintf(stderr, "Parser stage failed:\nFailed to parse function call param %d\n", i);
						exit(4);
					}

					p4c_add_to_node(out_node, param);

					if (p4c_accept_token(state, &P4C_TINFO_COMMA) == NULL) {
						break;
					}
				}

				p4c_expect_token(state, &P4C_TINFO_CLOSE_PARENTHESIS);
			}

			return out_node;
		}
		// <identifier>
		else {
			out_node = p4c_get_node(state);
			out_node->info = tok->info;
			out_node->attribute = tok->attribute;
			out_node->attribute_sz = tok->attribute_sz;
			return out_node;
		}
	}

	// <int-literal>
	tok = p4c_accept_token(state, &P4C_TINFO_INT_LITERAL);
	if (tok != NULL) {
		out_node = p4c_get_node(state);
		out_node->info = tok->info;
		out_node->attribute = tok->attribute;
		out_node->attribute_sz = tok->attribute_sz;
		return out_node;
	}

	return NULL;
}

static p4c_node_t* p4c_parse_operator5(p4c_parser_state_t* state) {
	// Unary operator
	const p4c_token_t* tok;

	tok = p4c_accept_token(state, &P4C_TINFO_ADD);
	if (tok == NULL) {
		tok = p4c_accept_token(state, &P4C_TINFO_SUBTRACT);
	}
	if (tok == NULL) {
		tok = p4c_accept_token(state, &P4C_TINFO_BINARY_NOT);
	}
	if (tok == NULL) {
		tok = p4c_accept_token(state, &P4C_TINFO_LOGICAL_NOT);
	}
	if (tok == NULL) {
		tok = p4c_accept_token(state, &P4C_TINFO_INC);
	}
	if (tok == NULL) {
		tok = p4c_accept_token(state, &P4C_TINFO_DEC);
	}
	if (tok == NULL) {
		tok = p4c_accept_token(state, &P4C_TINFO_BINARY_AND);
	}
	if (tok == NULL) {
		// && = double &
		// This always produces invalid code, but allows us to give a better error message
		tok = p4c_accept_token(state, &P4C_TINFO_LOGICAL_AND);
	}
	if (tok == NULL) {
		tok = p4c_accept_token(state, &P4C_TINFO_POINTER);
	}
	if (tok != NULL) {
		p4c_node_t* op = p4c_get_node(state);
		p4c_node_t* term = p4c_parse_operator5(state);

		if (term == NULL) {
			fprintf(stderr, "Parser stage failed:\nFailed to parse unary operator '%s' term\n", tok->info->name);
			exit(4);
		}

		op->info = tok->info;
		if (op->info == &P4C_TINFO_BINARY_AND) {
			op->info = &P4C_TINFO_REFERENCE;
			p4c_add_to_node(op, term);
		}
		else if (op->info == &P4C_TINFO_POINTER) {
			op->info = &P4C_TINFO_DEREFERENCE;
			p4c_add_to_node(op, term);
		}
		else if (op->info == &P4C_TINFO_LOGICAL_AND) {
			p4c_node_t* op2 = p4c_get_node(state);
			op->info = &P4C_TINFO_REFERENCE;
			op2->info = &P4C_TINFO_REFERENCE;
			p4c_add_to_node(op, op2);
			p4c_add_to_node(op2, term);
		}
		else {
			p4c_add_to_node(op, term);
		}

		return op;
	}

	// Get first term
	p4c_node_t* term1 = p4c_parse_operator_last(state);
	if (term1 == NULL) {
		return NULL;
	}

	// Get next terms
	p4c_node_t* term2 = NULL;
	for (;;) {
		tok = p4c_peek_token(state);
		if (tok == NULL || tok->info->type != P4C_TOKEN_MEMBER) {
			break;
		}

		p4c_node_t* op = p4c_get_node(state);
		op->info = tok->info;
		p4c_next_token(state);

		// Parse second term
		term2 = p4c_parse_operator_last(state);
		if (term2 == NULL) {
			fprintf(stderr, "Parser stage failed:\nFailed to parse '%s' operator second term\n", op->info->name);
			exit(4);
		}

		p4c_add_to_node(op, term1);
		p4c_add_to_node(op, term2);
		term1 = op;
	}

	return term1;
}

static p4c_node_t* p4c_parse_operator4(p4c_parser_state_t* state) {
	// Get first term
	p4c_node_t* term1 = p4c_parse_operator5(state);
	if (term1 == NULL) {
		return NULL;
	}

	if (p4c_accept_token(state, &P4C_TINFO_AS) != NULL) {
		p4c_node_t* type = p4c_parse_type(state);
		if (type == NULL) {
			fprintf(stderr, "Parser stage failed:\nExpected type after 'as'\n");
			exit(4);
		}

		p4c_node_t* op = p4c_get_node(state);
		op->info = &P4C_TINFO_AS;
		p4c_add_to_node(op, term1);
		p4c_add_to_node(op, type);
		return op;
	}

	return term1;
}

static p4c_node_t* p4c_parse_operator3(p4c_parser_state_t* state) {
	// Get first term
	p4c_node_t* term1 = p4c_parse_operator4(state);
	if (term1 == NULL) {
		return NULL;
	}

	// Get next terms
	p4c_node_t* term2 = NULL;
	for (;;) {
		const p4c_token_t* tok = p4c_peek_token(state);
		if (tok == NULL || (
			tok->info->type != P4C_TOKEN_ADD &&
			tok->info->type != P4C_TOKEN_SUBTRACT)) {
			break;
		}

		p4c_node_t* op = p4c_get_node(state);
		op->info = tok->info;
		p4c_next_token(state);

		// Parse second term
		term2 = p4c_parse_operator4(state);
		if (term2 == NULL) {
			fprintf(stderr, "Parser stage failed:\nFailed to parse '%s' operator second term\n", op->info->name);
			exit(4);
		}

		p4c_add_to_node(op, term1);
		p4c_add_to_node(op, term2);
		term1 = op;
	}

	return term1;
}

static p4c_node_t* p4c_parse_operator2(p4c_parser_state_t* state) {
	// Get first term
	p4c_node_t* term1 = p4c_parse_operator3(state);
	if (term1 == NULL) {
		return NULL;
	}

	// Get next terms
	p4c_node_t* term2 = NULL;
	for (;;) {
		const p4c_token_t* tok = p4c_peek_token(state);
		if (tok == NULL || (
			tok->info->type != P4C_TOKEN_EQUAL &&
			tok->info->type != P4C_TOKEN_DIFFERENT &&
			tok->info->type != P4C_TOKEN_GREATER &&
			tok->info->type != P4C_TOKEN_LESS &&
			tok->info->type != P4C_TOKEN_GEQUAL &&
			tok->info->type != P4C_TOKEN_LEQUAL )) {
			break;
		}

		p4c_node_t* op = p4c_get_node(state);
		op->info = tok->info;
		p4c_next_token(state);

		// Parse second term
		term2 = p4c_parse_operator3(state);
		if (term2 == NULL) {
			fprintf(stderr, "Parser stage failed:\nFailed to parse '%s' operator second term\n", op->info->name);
			exit(4);
		}

		p4c_add_to_node(op, term1);
		p4c_add_to_node(op, term2);
		term1 = op;
	}

	return term1;
}

static p4c_node_t* p4c_parse_operator1(p4c_parser_state_t* state) {
	// Get first term
	p4c_node_t* term1 = p4c_parse_operator2(state);
	if (term1 == NULL) {
		return NULL;
	}

	// Get next terms
	p4c_node_t* term2 = NULL;
	for (;;) {
		const p4c_token_t* tok = p4c_peek_token(state);
		if (tok == NULL || (
			tok->info->type != P4C_TOKEN_BINARY_AND &&
			tok->info->type != P4C_TOKEN_BINARY_OR &&
			tok->info->type != P4C_TOKEN_LOGICAL_AND &&
			tok->info->type != P4C_TOKEN_LOGICAL_OR)) {
			break;
		}

		p4c_node_t* op = p4c_get_node(state);
		op->info = tok->info;
		p4c_next_token(state);

		// Parse second term
		term2 = p4c_parse_operator2(state);
		if (term2 == NULL) {
			fprintf(stderr, "Parser stage failed:\nFailed to parse '%s' operator second term\n", op->info->name);
			exit(4);
		}

		p4c_add_to_node(op, term1);
		p4c_add_to_node(op, term2);
		term1 = op;
	}

	return term1;
}

static p4c_node_t* p4c_parse_expression(p4c_parser_state_t* state) {
	// Get first term
	p4c_node_t* term1 = p4c_parse_operator1(state);
	if (term1 == NULL) {
		return NULL;
	}

	// Get next terms
	p4c_node_t* term2 = NULL;
	for (;;) {
		const p4c_token_t* tok = p4c_peek_token(state);
		if (tok == NULL || tok->info->type != P4C_TOKEN_ASSIGN) {
			break;
		}
	
		p4c_node_t* op = p4c_get_node(state);
		op->info = tok->info;
		p4c_next_token(state);

		// Parse second term
		term2 = p4c_parse_operator1(state);
		if (term2 == NULL) {
			fprintf(stderr, "Parser stage failed:\nFailed to parse '%s' operator second term\n", op->info->name);
			exit(4);
		}

		p4c_add_to_node(op, term1);
		p4c_add_to_node(op, term2);
		term1 = op;
	}

	return term1;
}

static p4c_node_t* p4c_parse_let(p4c_parser_state_t* state) {
	if (p4c_accept_token(state, &P4C_TINFO_LET) == NULL) {
		return NULL;
	}
	
	p4c_node_t* out_node = p4c_get_node(state);
	out_node->info = &P4C_TINFO_LET;

	const p4c_token_t* tok = p4c_expect_token(state, &P4C_TINFO_IDENTIFIER);
	out_node->attribute = tok->attribute;
	out_node->attribute_sz = tok->attribute_sz;

	if (p4c_accept_token(state, &P4C_TINFO_COLON) != NULL) {
		p4c_node_t* type = p4c_parse_type(state);
		if (type == NULL) {
			fprintf(stderr, "Parser stage failed:\nCouldn't parse type in let statement\n");
			exit(4);
		}

		p4c_node_t* op = p4c_get_node(state);
		op->info = &P4C_TINFO_AS;
		p4c_expect_token(state, &P4C_TINFO_ASSIGN);

		p4c_node_t* exp = p4c_parse_expression(state);
		if (exp == NULL) {
			fprintf(stderr, "Parser stage failed:\nCouldn't parse expression in let statement\n");
			exit(4);
		}

		p4c_add_to_node(op, exp);
		p4c_add_to_node(op, type);
		p4c_add_to_node(out_node, op);
	}
	else {
		p4c_expect_token(state, &P4C_TINFO_ASSIGN);

		p4c_node_t* exp = p4c_parse_expression(state);
		if (exp == NULL) {
			fprintf(stderr, "Parser stage failed:\nCouldn't parse expression in let statement\n");
			exit(4);
		}

		p4c_add_to_node(out_node, exp);
	}

	p4c_expect_token(state, &P4C_TINFO_SEMICOLON);

	return out_node;
}

static p4c_node_t* p4c_parse_statement(p4c_parser_state_t* state);

static p4c_node_t* p4c_parse_compound_statement(p4c_parser_state_t* state) {
	if (!p4c_accept_token(state, &P4C_TINFO_OPEN_BRACES)) {
		return NULL;
	}

	p4c_node_t* out_node = p4c_get_node(state);
	out_node->info = &P4C_TINFO_COMPOUND_STATEMENT;

	// Get statements
	for (;;) {
		p4c_node_t* statement = p4c_parse_statement(state);
		if (statement == NULL) {
			break;
		}
		p4c_add_to_node(out_node, statement);
	}

	p4c_expect_token(state, &P4C_TINFO_CLOSE_BRACES);

	return out_node;
}

static p4c_node_t* p4c_parse_return_statement(p4c_parser_state_t* state) {
	if (!p4c_accept_token(state, &P4C_TINFO_RETURN)) {
		return NULL;
	}

	p4c_node_t* out_node = p4c_get_node(state);
	out_node->info = &P4C_TINFO_RETURN;

	// Parse expression
	if (p4c_peek_token(state)->info->type != P4C_TOKEN_SEMICOLON) {
		p4c_add_to_node(out_node, p4c_parse_expression(state));
	}

	p4c_expect_token(state, &P4C_TINFO_SEMICOLON);

	return out_node;
}

static p4c_node_t* p4c_parse_expression_statement(p4c_parser_state_t* state) {
	p4c_node_t* out_node = p4c_parse_expression(state);
	if (out_node == NULL) {
		return NULL;
	}
	p4c_expect_token(state, &P4C_TINFO_SEMICOLON);
	return out_node;
}

static p4c_node_t* p4c_parse_if(p4c_parser_state_t* state) {
	if (!p4c_accept_token(state, &P4C_TINFO_IF)) {
		return NULL;
	}

	p4c_node_t* out_node = p4c_get_node(state);
	out_node->info = &P4C_TINFO_IF;

	// Parse condition
	p4c_node_t* condition = p4c_parse_expression(state);
	if (condition == NULL) {
		fprintf(stderr, "Parser stage failed:\nIf missing condition\n");
		exit(4);
	}
	p4c_add_to_node(out_node, condition);

	// Parse if / else body
	p4c_node_t* if_body = p4c_parse_compound_statement(state);
	if (if_body == NULL) {
		fprintf(stderr, "Parser stage failed:\nIf missing body\n");
		exit(4);
	}
	p4c_add_to_node(out_node, if_body);

	if (p4c_accept_token(state, &P4C_TINFO_ELSE) != NULL) {
		p4c_node_t* else_body = p4c_parse_if(state);
		if (else_body == NULL) {
			else_body = p4c_parse_compound_statement(state);
		}
		if (else_body == NULL) {
			fprintf(stderr, "Parser stage failed:\nElse missing body\n");
			exit(4);
		}

		p4c_add_to_node(out_node, else_body);
	}

	return out_node;
}

static p4c_node_t* p4c_parse_while(p4c_parser_state_t* state) {
	if (!p4c_accept_token(state, &P4C_TINFO_WHILE)) {
		return NULL;
	}

	p4c_node_t* out_node = p4c_get_node(state);
	out_node->info = &P4C_TINFO_WHILE;

	// Parse condition
	p4c_node_t* condition = p4c_parse_expression(state);
	if (condition == NULL) {
		fprintf(stderr, "Parser stage failed:\nIf missing condition\n");
		exit(4);
	}
	p4c_add_to_node(out_node, condition);

	// Parse while body
	p4c_node_t* while_body = p4c_parse_compound_statement(state);
	if (while_body == NULL) {
		fprintf(stderr, "Parser stage failed:\nWhile missing body\n");
		exit(4);
	}
	p4c_add_to_node(out_node, while_body);

	return out_node;
}

static p4c_node_t* p4c_parse_statement(p4c_parser_state_t* state) {
	p4c_node_t* out_node;

	out_node = p4c_parse_compound_statement(state);
	if (out_node != NULL) {
		return out_node;
	}

	out_node = p4c_parse_return_statement(state);
	if (out_node != NULL) {
		return out_node;
	}
	
	out_node = p4c_parse_let(state);
	if (out_node != NULL) {
		return out_node;
	}

	out_node = p4c_parse_if(state);
	if (out_node != NULL) {
		return out_node;
	}

	out_node = p4c_parse_while(state);
	if (out_node != NULL) {
		return out_node;
	}

	return p4c_parse_expression_statement(state);
}

static p4c_node_t* p4c_parse_function(p4c_parser_state_t* state) {
	p4c_node_t* out_node = p4c_get_node(state);
	out_node->info = &P4C_TINFO_FUNCTION;

	// Parse function identifier
	p4c_expect_token(state, &P4C_TINFO_FUNCTION);
	const p4c_token_t* identifier = p4c_expect_token(state, &P4C_TINFO_IDENTIFIER);
	out_node->attribute = identifier->attribute;
	out_node->attribute_sz = identifier->attribute_sz;

	// Parse function parameters
	p4c_expect_token(state, &P4C_TINFO_OPEN_PARENTHESIS);

	p4c_node_t* params = p4c_get_node(state);
	params->info = &P4C_TINFO_PARAMS;
	
	const p4c_token_t* tok = p4c_peek_token(state);
	while (tok->info->type != P4C_TOKEN_CLOSE_PARENTHESIS) {
		p4c_node_t* param = p4c_get_node(state);

		tok = p4c_expect_token(state, &P4C_TINFO_IDENTIFIER);
		param->info = tok->info;
		param->attribute = tok->attribute;
		param->attribute_sz = tok->attribute_sz;
		p4c_expect_token(state, &P4C_TINFO_COLON);
		
		p4c_node_t* type = p4c_parse_type(state);
		if (type == NULL) {
			fprintf(stderr, "Parser stage failed:\nCoudln't parse type in function param\n");
			exit(4);
		}
		p4c_add_to_node(param, type);
		p4c_add_to_node(params, param);

		if (p4c_accept_token(state, &P4C_TINFO_COMMA) == NULL) {
			break;
		}
	}

	p4c_expect_token(state, &P4C_TINFO_CLOSE_PARENTHESIS);
	p4c_add_to_node(out_node, params);

	// Parse function return type
	p4c_expect_token(state, &P4C_TINFO_ARROW);
	p4c_node_t* type = p4c_parse_type(state);
	if (type == NULL) {
		fprintf(stderr, "Parser stage failed:\nCoudln't parse type in function return type\n");
		exit(4);
	}
	p4c_add_to_node(out_node, type);

	// Parse function body
	p4c_node_t* body = p4c_parse_compound_statement(state);
	if (body == NULL) {
		fprintf(stderr, "Parser stage failed:\nFunction missing body\n");
		exit(4);
	}
	p4c_add_to_node(out_node, body);

	return out_node;
}

static p4c_node_t* p4c_parse_program(p4c_parser_state_t* state) {
	p4c_node_t* root = p4c_get_node(state);
	
	for (;;) {
		const p4c_token_t* tok = p4c_peek_token(state);
		if (tok == NULL) {
			break;
		}

		// Function declaration
		if (tok->info->type == P4C_TOKEN_FUNCTION) {
			p4c_add_to_node(root, p4c_parse_function(state));
		}
		// Variable declaration
		else if (tok->info->type == P4C_TOKEN_LET) {
			p4c_add_to_node(root, p4c_parse_let(state));
		}
	}

	return root;
}

void p4c_print_node(FILE* f, const p4c_node_t* node, int indentation) {
	for (int i = 0; i < indentation; ++i) {
		fprintf(f, "  ");
	}

	const char* name = "ROOT";
	if (node->info != NULL) {
		name = node->info->name;
	}

	if (node->attribute_sz > 0) {
		fprintf(f, "'%s'(%.*s)\n", name, node->attribute_sz, node->attribute);
	}
	else {
		fprintf(f, "'%s'\n", name);
	}

	const p4c_node_t* c = node->first;
	while (c != NULL) {
		p4c_print_node(f, c, indentation + 1);
		c = c->next;
	}
}

p4c_node_t* p4c_run_parser(const p4c_token_t* tokens, int token_count, p4c_node_t* nodes, int nodes_sz) {
	for (int i = 0; i < nodes_sz; ++i) {
		nodes[i].active = P4C_FALSE;
	}

	p4c_parser_state_t state;
	state.it = tokens;
	state.last_token = tokens + token_count - 1;
	state.nodes = nodes;
	state.nodes_sz = nodes_sz;
	state.node_count = 0;
	return p4c_parse_program(&state);
}
