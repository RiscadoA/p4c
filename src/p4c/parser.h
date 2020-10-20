#pragma once

#include <p4c/lexer.h>

#include <stdio.h>

typedef struct p4c_node_t p4c_node_t;

struct p4c_node_t {
	const p4c_token_info_t* info;
	const char* attribute;
	int attribute_sz;
	p4c_node_t* first;
	p4c_node_t* next;
	p4c_bool_t active;
};

void p4c_print_node(FILE* f, const p4c_node_t* node, int indentation);

p4c_node_t* p4c_run_parser(const p4c_token_t* tokens, int token_count, p4c_node_t* nodes, int nodes_sz);

#define P4C_TOKEN_PARAMS				0x60
#define P4C_TOKEN_COMPOUND_STATEMENT	0x61
#define P4C_TOKEN_CALL					0x62
#define P4C_TOKEN_REFERENCE				0x63
#define P4C_TOKEN_DEREFERENCE			0x64

const p4c_token_info_t P4C_TINFO_PARAMS;
const p4c_token_info_t P4C_TINFO_COMPOUND_STATEMENT;
const p4c_token_info_t P4C_TINFO_CALL;
const p4c_token_info_t P4C_TINFO_REFERENCE;
const p4c_token_info_t P4C_TINFO_DEREFERENCE;
