#include <stdio.h>
#include "parser.h"

#define INIT_JUNCTION(_prev_state, _token_type, _next_state) \
{ \
	if (parser->state == _prev_state) \
	{ \
		awaited = _token_type; \
		if (token.type == _token_type) \
		{ \
			parser->state = _next_state; \
			goto successful_exit; \
		} \
	} \
}



// Operation op = (int x) -> x + 2;
// op = () -> 2;
token_type_e parse(parser_t *parser, token_t token, int *is_error)
{
	token_type_e awaited;
	*is_error = 0;
	INIT_JUNCTION(STATE_INIT, KEYWORD, STATE_CLASS_ID);
	INIT_JUNCTION(STATE_CLASS_ID, IDENT, STATE_OPER_ID);
	INIT_JUNCTION(STATE_OPER_ID, EQUALS, STATE_EQUALS);
	INIT_JUNCTION(STATE_EQUALS, DECIMAL_NUMBER, STATE_NUMBER);
	INIT_JUNCTION(STATE_NUMBER, SEMICOLON, STATE_SEMICOLON);

	if (token.type == WHITESPACE)
		goto successful_exit;

	*is_error = 1;
	parser->state = STATE_INVALID;

successful_exit:

	return awaited;
}
