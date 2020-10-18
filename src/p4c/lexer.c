#include <p4c/lexer.h>

#include <stdio.h>
#include <stdlib.h>

const p4c_token_info_t P4C_TINFO_VOID = { P4C_TOKEN_VOID, P4C_TRUE, P4C_FALSE, P4C_FALSE, "void" };
const p4c_token_info_t P4C_TINFO_I16 = { P4C_TOKEN_I16, P4C_TRUE, P4C_FALSE, P4C_FALSE, "i16" };
const p4c_token_info_t P4C_TINFO_U16 = { P4C_TOKEN_U16, P4C_TRUE, P4C_FALSE, P4C_FALSE, "u16" };

const p4c_token_info_t P4C_TINFO_INT_LITERAL = { P4C_TOKEN_INT_LITERAL, P4C_FALSE, P4C_FALSE, P4C_FALSE, "int_literal" };
const p4c_token_info_t P4C_TINFO_IDENTIFIER = { P4C_TOKEN_IDENTIFIER, P4C_FALSE, P4C_FALSE, P4C_FALSE, "identifier" };

const p4c_token_info_t P4C_TINFO_FUNCTION = { P4C_TOKEN_FUNCTION, P4C_FALSE, P4C_FALSE, P4C_FALSE, "function" };
const p4c_token_info_t P4C_TINFO_IF = { P4C_TOKEN_IF, P4C_FALSE, P4C_FALSE, P4C_FALSE, "if" };
const p4c_token_info_t P4C_TINFO_ELSE = { P4C_TOKEN_ELSE, P4C_FALSE, P4C_FALSE, P4C_FALSE, "else" };
const p4c_token_info_t P4C_TINFO_WHILE = { P4C_TOKEN_WHILE, P4C_FALSE, P4C_FALSE, P4C_FALSE, "while" };
const p4c_token_info_t P4C_TINFO_FOR = { P4C_TOKEN_FOR, P4C_FALSE, P4C_FALSE, P4C_FALSE, "for" };
const p4c_token_info_t P4C_TINFO_RETURN = { P4C_TOKEN_RETURN, P4C_FALSE, P4C_FALSE, P4C_FALSE, "return" };
const p4c_token_info_t P4C_TINFO_LET = { P4C_TOKEN_LET, P4C_FALSE, P4C_FALSE, P4C_FALSE, "let" };

const p4c_token_info_t P4C_TINFO_ADD = { P4C_TOKEN_ADD, P4C_FALSE, P4C_FALSE, P4C_TRUE, "+" };
const p4c_token_info_t P4C_TINFO_SUBTRACT = { P4C_TOKEN_SUBTRACT, P4C_FALSE, P4C_FALSE, P4C_TRUE, "-" };
const p4c_token_info_t P4C_TINFO_EQUAL = { P4C_TOKEN_EQUAL, P4C_FALSE, P4C_FALSE, P4C_TRUE, "==" };
const p4c_token_info_t P4C_TINFO_DIFFERENT = { P4C_TOKEN_DIFFERENT, P4C_FALSE, P4C_FALSE, P4C_TRUE, "!=" };
const p4c_token_info_t P4C_TINFO_GREATER = { P4C_TOKEN_GREATER, P4C_FALSE, P4C_FALSE, P4C_TRUE, ">" };
const p4c_token_info_t P4C_TINFO_LESS = { P4C_TOKEN_LESS, P4C_FALSE, P4C_FALSE, P4C_TRUE, "<" };
const p4c_token_info_t P4C_TINFO_GEQUAL = { P4C_TOKEN_GEQUAL, P4C_FALSE, P4C_FALSE, P4C_TRUE, ">=" };
const p4c_token_info_t P4C_TINFO_LEQUAL = { P4C_TOKEN_LEQUAL, P4C_FALSE, P4C_FALSE, P4C_TRUE, "<=" };
const p4c_token_info_t P4C_TINFO_LOGICAL_AND = { P4C_TOKEN_LOGICAL_AND, P4C_FALSE, P4C_FALSE, P4C_TRUE, "&&" };
const p4c_token_info_t P4C_TINFO_LOGICAL_OR = { P4C_TOKEN_LOGICAL_OR, P4C_FALSE, P4C_FALSE, P4C_TRUE, "||" };
const p4c_token_info_t P4C_TINFO_LOGICAL_NOT = { P4C_TOKEN_LOGICAL_NOT, P4C_FALSE, P4C_FALSE, P4C_TRUE, "!" };
const p4c_token_info_t P4C_TINFO_BINARY_AND = { P4C_TOKEN_BINARY_AND, P4C_FALSE, P4C_FALSE, P4C_TRUE, "&" };
const p4c_token_info_t P4C_TINFO_BINARY_OR = { P4C_TOKEN_BINARY_OR, P4C_FALSE, P4C_FALSE, P4C_TRUE, "|" };
const p4c_token_info_t P4C_TINFO_BINARY_XOR = { P4C_TOKEN_BINARY_XOR, P4C_FALSE, P4C_FALSE, P4C_TRUE, "^" };
const p4c_token_info_t P4C_TINFO_BINARY_NOT = { P4C_TOKEN_BINARY_NOT, P4C_FALSE, P4C_FALSE, P4C_TRUE, "~" };
const p4c_token_info_t P4C_TINFO_ASSIGN = { P4C_TOKEN_ASSIGN, P4C_FALSE, P4C_FALSE, P4C_TRUE, "=" };
const p4c_token_info_t P4C_TINFO_SHL = { P4C_TOKEN_SHL, P4C_FALSE, P4C_FALSE, P4C_TRUE, "<<" };
const p4c_token_info_t P4C_TINFO_SHR = { P4C_TOKEN_SHR, P4C_FALSE, P4C_FALSE, P4C_TRUE, ">>" };
const p4c_token_info_t P4C_TINFO_INC = { P4C_TOKEN_SHL, P4C_FALSE, P4C_FALSE, P4C_TRUE, "++" };
const p4c_token_info_t P4C_TINFO_DEC = { P4C_TOKEN_SHR, P4C_FALSE, P4C_FALSE, P4C_TRUE, "--" };
const p4c_token_info_t P4C_TINFO_MEMBER = { P4C_TOKEN_MEMBER, P4C_FALSE, P4C_FALSE, P4C_TRUE, "member-access" };

const p4c_token_info_t P4C_TINFO_OPEN_BRACES = { P4C_TOKEN_OPEN_BRACES, P4C_FALSE, P4C_TRUE, P4C_FALSE, "{" };
const p4c_token_info_t P4C_TINFO_CLOSE_BRACES = { P4C_TOKEN_CLOSE_BRACES, P4C_FALSE, P4C_TRUE, P4C_FALSE, "}" };
const p4c_token_info_t P4C_TINFO_OPEN_BRACKETS = { P4C_TOKEN_OPEN_BRACKETS, P4C_FALSE, P4C_TRUE, P4C_FALSE, "[" };
const p4c_token_info_t P4C_TINFO_CLOSE_BRACKETS = { P4C_TOKEN_CLOSE_BRACKETS, P4C_FALSE, P4C_TRUE, P4C_FALSE, "]" };
const p4c_token_info_t P4C_TINFO_OPEN_PARENTHESIS = { P4C_TOKEN_OPEN_PARENTHESIS, P4C_FALSE, P4C_TRUE, P4C_FALSE, "(" };
const p4c_token_info_t P4C_TINFO_CLOSE_PARENTHESIS = { P4C_TOKEN_CLOSE_PARENTHESIS, P4C_FALSE, P4C_TRUE, P4C_FALSE, ")" };
const p4c_token_info_t P4C_TINFO_SEMICOLON = { P4C_TOKEN_SEMICOLON, P4C_FALSE, P4C_TRUE, P4C_FALSE, ";" };
const p4c_token_info_t P4C_TINFO_COLON = { P4C_TOKEN_COLON, P4C_FALSE, P4C_TRUE, P4C_FALSE, ":" };
const p4c_token_info_t P4C_TINFO_COMMA = { P4C_TOKEN_COMMA, P4C_FALSE, P4C_TRUE, P4C_FALSE, "," };
const p4c_token_info_t P4C_TINFO_ARROW = { P4C_TOKEN_ARROW, P4C_FALSE, P4C_TRUE, P4C_FALSE, "->" };

typedef struct {
	const char* it;
	p4c_token_t* tokens;
	int tokens_count, tokens_sz;
} p4c_lexer_state_t;

static p4c_bool_t p4c_is_whitespace(char c) {
	return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\0';
}

static p4c_bool_t p4c_is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static p4c_bool_t p4c_is_numeric(char c) {
	return c >= '0' && c <= '9';
}

static void p4c_put_token(p4c_lexer_state_t* state, const p4c_token_t* token) {
	if (state->tokens_count + 1 >= state->tokens_sz) {
		fprintf(stderr, "Lexer stage failed:\nCouldn't put token, token buffer overflow detected\n");
		exit(3);
	}

	state->tokens[state->tokens_count++] = *token;
}

static p4c_bool_t p4c_read_token(p4c_lexer_state_t* state) {
	// Skip comments
	if (state->it[0] == '/' && state->it[1] == '/') {
		while (*state->it != '\0' && *state->it != '\n') {
			++state->it;
		}
	}
	else if (state->it[0] == '/' && state->it[1] == '*') {
		while (1) {
			if (state->it[0] == '*' && state->it[1] == '/') {
				state->it += 2;
				break;
			}

			if (*state->it == '\0') {
				fprintf(stderr, "Lexer stage failed:\nUnexpected EOF\nComment was not closed (matching \"*/\" was not found)");
				exit(3);
			}

			++state->it;
		}
	}

	p4c_token_t tok;
	tok.info = NULL;
	tok.attribute_sz = 0;
	tok.attribute = NULL;

	size_t attr_it = 0;

#define SINGLE_CHAR_TOK(chr, tok_info) else if (*state->it == chr) { tok.info = &tok_info; p4c_put_token(state, &tok); ++state->it; return P4C_TRUE; }

#define TWO_CHAR_TOK(chr, tok_info) else if (state->it[0] == chr[0] && state->it[1] == chr[1]) { tok.info = &tok_info; p4c_put_token(state, &tok); state->it += 2; return P4C_TRUE; }

#define KEYWORD_TOK(str, tok_info) if (state->it[0] == str[0]) {\
		attr_it = 0;\
		while (1) {\
			if (str[attr_it] == '\0') {\
				if (p4c_is_alpha(state->it[attr_it]) || p4c_is_numeric(state->it[attr_it]))\
					break;\
				tok.info = &tok_info;\
				p4c_put_token(state, &tok);\
				state->it += attr_it;\
				return P4C_TRUE;\
			}\
			else if (state->it[attr_it] != str[attr_it])\
				break;\
			++attr_it;\
		}\
	}

	if (p4c_is_whitespace(*state->it)) {
		if (*state->it == '\0')
			return P4C_FALSE;
		++state->it;
		return P4C_TRUE;
	}

	// Keywords
	KEYWORD_TOK("void", P4C_TINFO_VOID)
	KEYWORD_TOK("i16", P4C_TINFO_I16)
	KEYWORD_TOK("u16", P4C_TINFO_U16)
	TWO_CHAR_TOK("fn", P4C_TINFO_FUNCTION)
	TWO_CHAR_TOK("if", P4C_TINFO_IF)
	KEYWORD_TOK("else", P4C_TINFO_ELSE)
	KEYWORD_TOK("while", P4C_TINFO_WHILE)
	KEYWORD_TOK("for", P4C_TINFO_FOR)
	KEYWORD_TOK("return", P4C_TINFO_RETURN)
	KEYWORD_TOK("let", P4C_TINFO_LET)
	TWO_CHAR_TOK("->", P4C_TINFO_ARROW)
	SINGLE_CHAR_TOK('+', P4C_TINFO_ADD)
	SINGLE_CHAR_TOK('-', P4C_TINFO_SUBTRACT)
	TWO_CHAR_TOK("<<", P4C_TINFO_SHL)
	TWO_CHAR_TOK(">>", P4C_TINFO_SHR)
	TWO_CHAR_TOK("==", P4C_TINFO_EQUAL)
	TWO_CHAR_TOK("!=", P4C_TINFO_DIFFERENT)
	TWO_CHAR_TOK(">=", P4C_TINFO_GEQUAL)
	TWO_CHAR_TOK("<=", P4C_TINFO_LEQUAL)
	TWO_CHAR_TOK("&&", P4C_TINFO_LOGICAL_AND)
	TWO_CHAR_TOK("||", P4C_TINFO_LOGICAL_OR)
	SINGLE_CHAR_TOK('>', P4C_TINFO_GREATER)
	SINGLE_CHAR_TOK('<', P4C_TINFO_LESS)
	SINGLE_CHAR_TOK('!', P4C_TINFO_LOGICAL_NOT)
	SINGLE_CHAR_TOK('&', P4C_TINFO_BINARY_AND)
	SINGLE_CHAR_TOK('|', P4C_TINFO_BINARY_OR)
	SINGLE_CHAR_TOK('^', P4C_TINFO_BINARY_XOR)
	SINGLE_CHAR_TOK('~', P4C_TINFO_BINARY_NOT)
	SINGLE_CHAR_TOK('=', P4C_TINFO_ASSIGN)
	SINGLE_CHAR_TOK('{', P4C_TINFO_OPEN_BRACES)
	SINGLE_CHAR_TOK('}', P4C_TINFO_CLOSE_BRACES)
	SINGLE_CHAR_TOK('[', P4C_TINFO_OPEN_BRACKETS)
	SINGLE_CHAR_TOK(']', P4C_TINFO_CLOSE_BRACKETS)
	SINGLE_CHAR_TOK('(', P4C_TINFO_OPEN_PARENTHESIS)
	SINGLE_CHAR_TOK(')', P4C_TINFO_CLOSE_PARENTHESIS)
	SINGLE_CHAR_TOK(';', P4C_TINFO_SEMICOLON)
	SINGLE_CHAR_TOK(':', P4C_TINFO_COLON)
	SINGLE_CHAR_TOK(',', P4C_TINFO_COMMA)
	SINGLE_CHAR_TOK('.', P4C_TINFO_MEMBER)

	// Integer literal
	if (p4c_is_numeric(*state->it)) {
		attr_it = 0;

		while (1) {
			++attr_it;
			if (p4c_is_whitespace(state->it[attr_it]) || (!p4c_is_numeric(state->it[attr_it]) && !p4c_is_alpha(state->it[attr_it]))) {
				tok.attribute = state->it;
				tok.attribute_sz = attr_it;
				tok.info = &P4C_TINFO_INT_LITERAL;
				p4c_put_token(state, &tok);
				state->it += attr_it;
				return P4C_TRUE;
			}
			else if (!p4c_is_numeric(state->it[attr_it]))
				break;
		}
	}

	// Identifier
	if (p4c_is_alpha(*state->it) || *state->it == '_') {
		attr_it = 0;

		while (1) {
			++attr_it;
			if (p4c_is_whitespace(state->it[attr_it]) ||
				(!p4c_is_numeric(state->it[attr_it]) && !p4c_is_alpha(state->it[attr_it]) && state->it[attr_it] != '_')) {
				tok.attribute = state->it;
				tok.attribute_sz = attr_it;
				tok.info = &P4C_TINFO_IDENTIFIER;
				p4c_put_token(state, &tok);
				state->it += attr_it;
				return P4C_TRUE;
			}
			else if (!p4c_is_alpha(state->it[attr_it]) && !p4c_is_numeric(state->it[attr_it]) && state->it[attr_it] != '_')
				break;
		}
	}

	// Unknown token
	fprintf(stderr, "Lexer stage failed:\nUnknown token found on:\n<<%s>>\n", state->it);
	exit(3);

#undef SINGLE_CHAR_TOK
#undef MULTI_CHAR_2_TOK
#undef KEYWORD_TOK
}

void p4c_print_token(const p4c_token_t* token) {
	if (token->attribute_sz > 0) {
		fprintf(stdout, "'%s'(%.*s)", token->info->name, token->attribute_sz, token->attribute);
	}
	else {
		fprintf(stdout, "'%s'", token->info->name);
	}
}

int p4c_run_lexer(const char* source_code, p4c_token_t* tokens, int tokens_sz) {
	p4c_lexer_state_t state;
	state.it = source_code;
	state.tokens = tokens;
	state.tokens_count = 0;
	state.tokens_sz = tokens_sz;
	while (p4c_read_token(&state));
	return state.tokens_count;
}
