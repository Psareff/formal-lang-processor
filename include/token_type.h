#ifndef TOKEN_TYPE_H_
#define TOKEN_TYPE_H_

#define KEYWORDS_COUNT 9

static const char* keywords_list[KEYWORDS_COUNT] = {
	"Operation", "short", "int", "long", "float", "double", "boolean", "char", "string"
};

static enum token_type_e
{ 
	KEYWORD, 
	IDENT, 
	EQUALS, 
	NUMBER,
	WHITESPACE, 
	OPERATION, 
	ASSIGNMENT, 
	OPEN_PAREN, 
	CLOSE_PAREN,
	OPEN_BRACE, 
	CLOSE_BRACE, 
	COMMA, 
	SEMICOLON,
	NEWLINE,
	INVALID
};

#endif //TOKEN_TYPE_H_
