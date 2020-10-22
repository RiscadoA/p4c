#pragma once

#include <p4c/utils.h>

#include <stdio.h>

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

void p4c_print_token(FILE* f, const p4c_token_t* token);

int p4c_run_lexer(const char* source_code, p4c_token_t* tokens, int tokens_sz);

#define P4C_TOKEN_VOID					0x00
#define P4C_TOKEN_I16					0x01
#define P4C_TOKEN_U16					0x02
#define P4C_TOKEN_BOOL					0x03

#define P4C_TOKEN_INT_LITERAL			0x10
#define P4C_TOKEN_IDENTIFIER			0x11
#define P4C_TOKEN_CHAR_LITERAL			0x12
#define P4C_TOKEN_STRING_LITERAL		0x13

#define P4C_TOKEN_FUNCTION				0x20
#define P4C_TOKEN_IF					0x21
#define P4C_TOKEN_ELSE					0x22
#define P4C_TOKEN_WHILE					0x23
#define P4C_TOKEN_FOR					0x24
#define P4C_TOKEN_RETURN				0x25
#define P4C_TOKEN_LET					0x26
#define P4C_TOKEN_AS					0x27
#define P4C_TOKEN_TRUE					0x28
#define P4C_TOKEN_FALSE					0x29

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
#define P4C_TOKEN_BINARY_XOR			0x3D
#define P4C_TOKEN_BINARY_NOT			0x3E
#define P4C_TOKEN_ASSIGN				0x3F
#define P4C_TOKEN_INC					0x42
#define P4C_TOKEN_DEC					0x43
#define P4C_TOKEN_MEMBER				0x44
#define P4C_TOKEN_LOGICAL_XOR			0x45

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
#define P4C_TOKEN_POINTER				0x5A

const p4c_token_info_t P4C_TINFO_VOID;
const p4c_token_info_t P4C_TINFO_I16;
const p4c_token_info_t P4C_TINFO_U16;
const p4c_token_info_t P4C_TINFO_BOOL;

const p4c_token_info_t P4C_TINFO_INT_LITERAL;
const p4c_token_info_t P4C_TINFO_IDENTIFIER;
const p4c_token_info_t P4C_TINFO_CHAR_LITERAL;
const p4c_token_info_t P4C_TINFO_STRING_LITERAL;

const p4c_token_info_t P4C_TINFO_FUNCTION;
const p4c_token_info_t P4C_TINFO_IF;
const p4c_token_info_t P4C_TINFO_ELSE;
const p4c_token_info_t P4C_TINFO_WHILE;
const p4c_token_info_t P4C_TINFO_FOR;
const p4c_token_info_t P4C_TINFO_RETURN;
const p4c_token_info_t P4C_TINFO_LET;
const p4c_token_info_t P4C_TINFO_AS;
const p4c_token_info_t P4C_TINFO_TRUE;
const p4c_token_info_t P4C_TINFO_FALSE;

const p4c_token_info_t P4C_TINFO_ADD;
const p4c_token_info_t P4C_TINFO_SUBTRACT;
const p4c_token_info_t P4C_TINFO_EQUAL;
const p4c_token_info_t P4C_TINFO_DIFFERENT;
const p4c_token_info_t P4C_TINFO_GREATER;
const p4c_token_info_t P4C_TINFO_LESS;
const p4c_token_info_t P4C_TINFO_GEQUAL;
const p4c_token_info_t P4C_TINFO_LEQUAL;
const p4c_token_info_t P4C_TINFO_LOGICAL_AND;
const p4c_token_info_t P4C_TINFO_LOGICAL_OR;
const p4c_token_info_t P4C_TINFO_LOGICAL_NOT;
const p4c_token_info_t P4C_TINFO_BINARY_AND;
const p4c_token_info_t P4C_TINFO_BINARY_OR;
const p4c_token_info_t P4C_TINFO_BINARY_XOR;
const p4c_token_info_t P4C_TINFO_BINARY_NOT;
const p4c_token_info_t P4C_TINFO_ASSIGN;
const p4c_token_info_t P4C_TINFO_INC;
const p4c_token_info_t P4C_TINFO_DEC;
const p4c_token_info_t P4C_TINFO_MEMBER;
const p4c_token_info_t P4C_TINFO_LOGICAL_XOR;

const p4c_token_info_t P4C_TINFO_OPEN_BRACES;
const p4c_token_info_t P4C_TINFO_CLOSE_BRACES;
const p4c_token_info_t P4C_TINFO_OPEN_BRACKETS;
const p4c_token_info_t P4C_TINFO_CLOSE_BRACKETS;
const p4c_token_info_t P4C_TINFO_OPEN_PARENTHESIS;
const p4c_token_info_t P4C_TINFO_CLOSE_PARENTHESIS;
const p4c_token_info_t P4C_TINFO_SEMICOLON;
const p4c_token_info_t P4C_TINFO_COLON;
const p4c_token_info_t P4C_TINFO_COMMA;
const p4c_token_info_t P4C_TINFO_ARROW;
const p4c_token_info_t P4C_TINFO_POINTER;
