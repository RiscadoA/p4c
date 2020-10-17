#pragma once

#include <p4c/utils.h>

#define P4C_TOKEN_VOID					0x00
#define P4C_TOKEN_I16					0x01
#define P4C_TOKEN_U16					0x02

#define P4C_TOKEN_INT_LITERAL			0x10
#define P4C_TOKEN_IDENTIFIER			0x11

#define P4C_TOKEN_FUNCTION				0x20
#define P4C_TOKEN_IF					0x21
#define P4C_TOKEN_ELSE					0x22
#define P4C_TOKEN_WHILE					0x23
#define P4C_TOKEN_FOR					0x24
#define P4C_TOKEN_RETURN				0x25
#define P4C_TOKEN_LET					0x26

#define P4C_TOKEN_ADD					0x30
#define P4C_TOKEN_SUBTRACT				0x31
#define P4C_TOKEN_EQUAL					0x32
#define P4C_TOKEN_DIFFERENT				0x33
#define P4C_TOKEN_GREATER				0x34
#define P4C_TOKEN_LESS					0x35
#define P4C_TOKEN_GEQUAL				0x36
#define P4C_TOKEN_LEQUAL				0x37
#define P4C_TOKEN_LOGICAL_AND			0x38
#define P4C_TOKEN_LOGICAL_OR			0x39
#define P4C_TOKEN_LOGICAL_NOT			0x3A
#define P4C_TOKEN_BINARY_AND			0x3B
#define P4C_TOKEN_BINARY_OR				0x3C
#define P4C_TOKEN_BINARY_NOT			0x3D
#define P4C_TOKEN_ASSIGN				0x3E

#define P4C_TOKEN_OPEN_BRACES			0x50
#define P4C_TOKEN_CLOSE_BRACES			0x51
#define P4C_TOKEN_OPEN_BRACKETS			0x52
#define P4C_TOKEN_CLOSE_BRACKETS		0x53
#define P4C_TOKEN_OPEN_PARENTHESIS		0x54
#define P4C_TOKEN_CLOSE_PARENTHESIS		0x55
#define P4C_TOKEN_SEMICOLON				0x56
#define P4C_TOKEN_COLON					0x57
#define P4C_TOKEN_COMMA					0x58
#define P4C_TOKEN_ARROW					0x59

typedef struct {
	p4c_enum_t type;
	p4c_bool_t is_type;
	p4c_bool_t is_punctuation;
	p4c_bool_t is_operator;
	const char* name;
} p4c_token_info_t;

typedef struct {
	const p4c_token_info_t* info;
	const char* attribute;		// Points to the beginning of the attribute string
	int	attribute_sz;			// Number of bytes in the attribute string
} p4c_token_t;

void p4c_print_token(const p4c_token_t* token);

int p4c_run_lexer(const char* source_code, p4c_token_t* tokens, int tokens_sz);
