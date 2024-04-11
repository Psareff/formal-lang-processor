#ifndef TOKEN_TYPE_H_
#define TOKEN_TYPE_H_

#define KEYWORDS_COUNT 9

static const char* keywords_list[KEYWORDS_COUNT] = {
	"Operation", "short", "int", "long", "float", "double", "boolean", "char", "string"
};

typedef enum token_type
{
	KEYWORD,
	IDENT,
	EQUALS,
	DECIMAL_NUMBER,
	NEGATIVE_DECIMAL_NUMBER,
	FLOATING_NUMBER,
	NEGATIVE_FLOATING_NUMBER,
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
} token_type_e;

#endif //TOKEN_TYPE_H_
