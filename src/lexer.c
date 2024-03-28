#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <gtk/gtk.h>

#define IS_KEYWORD(expr, tok) \
	{ \
		for (int i = 0; i < KEYWORDS_COUNT; i++) \
			if (!strcmp(expr, keywords_list[i])) \
			{ \
				tok->type = KEYWORD; \
				break; \
			} \
	}
	
token_t *create_token(const char *expr, int start, int end, enum token_type_e type)
{
	token_t *tok = malloc(sizeof(token_t));
	tok->start = start;
	tok->end = end;
	tok->value = expr;
	tok->type = type;
	IS_KEYWORD(expr, tok);
	return tok;
}

char *token_type_to_str(enum token_type_e type)
{
	TOKEN_TYPE_TO_STR(type);
}

char *token_to_str(token_t *token)
{
#define TOKEN_PRINT_FORMAT "%20s | [%4d - %4d] | %20s" 
	int count = snprintf(NULL, 0, TOKEN_PRINT_FORMAT, token->value,
			token->start,
			token->end,
			token_type_to_str(token->type)) + 1;
	char *buff = malloc(count);
	snprintf(buff, count, TOKEN_PRINT_FORMAT, token->value,
			token->start,
			token->end,
			token_type_to_str(token->type));
	return buff;

}
void dispose_token(token_t *token)
{
	free(token->value);
	free(token);
}

int tokenize (const char *expr, list_t **tokens)
{

	if (expr == "")
		return 0;
	char *buffer = malloc(strlen(expr) + 1);
	strcpy(buffer, expr);

	char *beginning = buffer;

	int start, 
	    end = 0;
	int count = 0;

	enum token_type_e type = INVALID;

	for (; *buffer != '\0'; buffer++)
	{
		start = end;
		switch (*buffer)
		{
			case 'A' ... 'Z':
			case 'a' ... 'z':
				while (buffer++ && 
				      (*buffer >= 'A' && *buffer <= 'Z') ||
				      (*buffer >= 'a' && *buffer <= 'z') ||
					  (*buffer >= '0' && *buffer <= '9'))
				{
					end++;
				}
				buffer--;
				type = IDENT;
				break;
			case '0' ... '9':
number_recognition:
				while (buffer++ &&
				      (*buffer >= '0' && *buffer <= '9') || *buffer == '.')
				{
					end++;
				}
				buffer--;
				type = NUMBER;
				break;
			case ' ':
				type = WHITESPACE;
				break;
			case '(':
				type = OPEN_PAREN;
				break;
			case ')':
				type = CLOSE_PAREN;
				break;
			case '{':
				type = OPEN_BRACE;
				break;
			case '}':
				type = CLOSE_BRACE;
				break;
			case '=':
				type = EQUALS;
				break;
			case ',':
				type = COMMA;
				break;
			case '-':
				buffer++;
				switch (*buffer)
				{
					case '>':
						type = ASSIGNMENT;
						end++;
						break;
					case '0' ... '9':
						end++;
						goto number_recognition;
					default:
						type = OPERATION;
						buffer--;
				}
				break;
			case '+':
			case '*':
			case '/':
				type = OPERATION;
				break;
			case ';':
				type = SEMICOLON;
				break;
			case '\n':
			case '\r':
				type = NEWLINE;
				goto newline;
				break;
			default:
				goto emergency;
		}

		char *token_value = malloc(end - start + 2);
		memcpy(token_value, beginning + start, end - start + 1);
		token_value[end - start + 1] = '\0';

		token_t *tok = create_token(token_value, start, end, type);
		goto non_newline;
newline:
		token_value = malloc(strlen("\\n") + 1);
		memcpy(token_value, "\\n\0", 3);
		tok = create_token(token_value, start, end + 1, type);
non_newline:
		*tokens = push(*tokens, tok);
		count++;
		end++;

	}

emergency:
	free(beginning);
	return count;
}

char *token_collection_to_str(list_t **tokens)
{
	if (*tokens == NULL)
		return "";
	#define MAX_TOKEN_COLLECTION_LEN 10240
	char *token_collection_str = malloc(MAX_TOKEN_COLLECTION_LEN);
	for (int i = 0; i < MAX_TOKEN_COLLECTION_LEN; i++)
		token_collection_str[i] = 0;
	while (*tokens != NULL)

	{
		char *tok_str = token_to_str((*tokens)->data);

		strcpy(token_collection_str + strlen(token_collection_str), tok_str);
		token_collection_str[strlen(token_collection_str)] = '\n';

		free(tok_str);
		*tokens = (*tokens)->next;
	}
	return token_collection_str;

}

