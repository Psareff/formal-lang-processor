#include "lexer.h"
#ifndef PARSER_H_
#define PARSER_H_

typedef enum states
{
	STATE_CLASS_ID,
	STATE_OPER_ID,
	STATE_EQUALS,
	STATE_OPEN_PAREN,
	STATE_CLOSE_PAREN,
	STATE_KEYWORD,
	STATE_PARAMS_ID,
	STATE_COMMA,
	STATE_ASSIGNMENT,
	STATE_IDENT,
	STATE_OPEN_BRACE,
	STATE_NUMBER,
	STATE_OPERATION,
	STATE_CLOSE_BRACE,
	STATE_SEMICOLON,
	STATE_INVALID,
	STATE_INIT
} states_t;

typedef struct parser
{
	states_t state;
} parser_t;

token_type_e parse(parser_t *parser, token_t token, int *is_error);

#endif // PARSER_H_
