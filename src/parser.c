#include <stdio.h>
#include "parser.h"

#define INIT_JUNCTION(_prev_state, _token_type, _next_state) \
{ \
	if (token.type == _token_type && parser->state == _prev_state) \
	{ \
		parser->state = _next_state; \
		return; \
	} \
}



// Operation op = (int x) -> x + 2;
// op = () -> 2;
void parse(parser_t *parser, token_t token, int *is_error)
{

	INIT_JUNCTION(STATE_INVALID, KEYWORD, STATE_CLASS_ID);
	INIT_JUNCTION(STATE_CLASS_ID, IDENT, STATE_OPER_ID);
	INIT_JUNCTION(STATE_OPER_ID, EQUALS, STATE_EQUALS);
	INIT_JUNCTION(STATE_EQUALS, OPEN_PAREN, STATE_OPEN_PAREN);
	INIT_JUNCTION(STATE_OPEN_PAREN, KEYWORD, STATE_KEYWORD);
	INIT_JUNCTION(STATE_OPEN_PAREN, CLOSE_PAREN, STATE_CLOSE_PAREN);
	INIT_JUNCTION(STATE_KEYWORD, IDENT, STATE_PARAMS_ID);
	INIT_JUNCTION(STATE_PARAMS_ID, COMMA, STATE_COMMA);
	INIT_JUNCTION(STATE_COMMA, KEYWORD, STATE_KEYWORD);
	INIT_JUNCTION(STATE_PARAMS_ID, CLOSE_PAREN, STATE_CLOSE_PAREN);


	if (token.type == WHITESPACE)
		return;
	*is_error = 1;
	parser->state = STATE_INVALID;

}
