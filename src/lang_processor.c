#include "lang_processor.h"

code_editor_t editor;

#define GET_GTK_OBJ(obj) \
        gtk_builder_get_object(editor.builder, obj)

#define GTK_BUTTON_CB_CONN(obj, cb) \
        g_signal_connect(GET_GTK_OBJ(obj), "clicked", G_CALLBACK(cb), NULL)

static void on_close_info_bar(GtkWidget *widget, GdkEventKey *event)
{
	g_print("Invoked %s\n", __func__);
	gtk_widget_hide(GTK_WIDGET(editor.info_bar.bar));
}

static void on_jump_to_error(GtkWidget *widget, GdkEventKey *event)
{
	g_print("Invoked %s\n", __func__);
}

static void on_quick_fix(GtkWidget *widget, GdkEventKey *event)
{
	g_print("Invoked %s\n", __func__);
	parser_t parser;
	parser.state = STATE_INIT;
	editor.tokens = editor.tokens_bak;
	int error = 0;
	char *text = gtk_text_buffer_get_text( editor.editor_view.buffer, &editor.editor_view.start, &editor.editor_view.end, TRUE );
	char *new_text = malloc(strlen(text) + 1);
	char *new_text_bak = new_text;
states_t last_successfull_state;
	while (editor.tokens != NULL)
	{
		parse(&parser, *((token_t *)editor.tokens->data), &error);
		if (!error)
		{
			last_successfull_state = parser.state;
			//g_print("%s [%u - %u]\n", ((token_t *)editor.tokens->data)->value, ((token_t *)editor.tokens->data)->start, ((token_t *)editor.tokens->data)->end);
			strcpy(new_text, ((token_t *)editor.tokens->data)->value);
			new_text += strlen(((token_t *)editor.tokens->data)->value);
		}
		else
		{
			parser.state = last_successfull_state;
			error = 0;
		}
		editor.tokens = editor.tokens->next;
	}
	new_text = new_text_bak;
	if (new_text != "")
		gtk_text_buffer_set_text(editor.editor_view.buffer, new_text, strlen(new_text));
}

int autocomplete();

static gboolean code_key_pressed(GtkWidget *widget, GdkEventKey *event)
{
	switch (event->keyval)
	{
		case GDK_KEY_Control_L:
			MODIFIER_ACTION(LCTRL, 1);
			break;
		case GDK_KEY_Control_R:
			MODIFIER_ACTION(RCTRL, 1);
			break;
		case GDK_KEY_Shift_L:
			MODIFIER_ACTION(LSHIFT, 1);
			break;
		case GDK_KEY_Shift_R:
			MODIFIER_ACTION(RSHIFT, 1);
			break;
		case GDK_KEY_Alt_L:
			MODIFIER_ACTION(LALT, 1);
			break;
		case GDK_KEY_Alt_R:
			MODIFIER_ACTION(RALT, 1);
		case GDK_KEY_Tab:
			delete(editor.tokens_bak, dispose_token);
			gtk_text_buffer_get_bounds(editor.editor_view.buffer, &editor.editor_view.start, &editor.editor_view.end);
			char *text = gtk_text_buffer_get_text(editor.editor_view.buffer, &editor.editor_view.start, &editor.editor_view.end, TRUE);
			int ret = tokenize(text, &editor.tokens);
			if (ret != 0)
			{
				editor.tokens_bak = editor.tokens;

				int min = autocomplete();
				token_t *tok;
				editor.tokens = editor.tokens_bak;
				while (editor.tokens != NULL)
				{
					tok = (token_t *)editor.tokens->data;
					editor.tokens = editor.tokens->next;
				}
				strcpy(text + tok->start, keywords_list[min]);
				gtk_text_buffer_set_text(editor.editor_view.buffer, text, strlen(text));

			}
	return TRUE;
	}
	return FALSE;
}

int abs(int val)
{
	return val < 0 ? -val : val;
}

char *to_lower(char *word)
{
  char *lower = malloc(strlen(word) + 1);
  strcpy(lower, word);

  for (int i = 0; i < strlen(lower); i++)
    lower[i] = lower[i] >=  'A' && lower[i] <= 'Z' ? lower[i] + 32 : lower[i];

  return lower;
}
double smart_compare(char *ch1, char *ch2)
{
  double diff = 0;
  g_print("%s-%s, ", ch1, ch2);
  ch1 = to_lower(ch1);
  ch2 = to_lower(ch2);
  if(strlen(ch1) > strlen(ch2))
    return -1;

  double first_round_inc = strlen(ch1) + strlen(ch2) * 2;
  for (; *ch1 != '\0', *ch2 != '\0'; ch1++, ch2++, first_round_inc /= 5.0)
    if (*ch1 != *ch2)
      diff += first_round_inc;
  return diff;
}

int autocomplete()
{
	double comparisons[KEYWORDS_COUNT];
	while (editor.tokens != NULL)
	{
		for (int i = 0; i < KEYWORDS_COUNT; i++)
			comparisons[i] = smart_compare(((token_t *)editor.tokens->data)->value, keywords_list[i]);
		editor.tokens = editor.tokens->next;
	}
	int min = 0;
	double best_comparison = comparisons[KEYWORDS_COUNT - 1];
	for (int i = 0; i < KEYWORDS_COUNT; i++)
	{
		if(comparisons[i] < comparisons[min] && comparisons[i] != -1)
			min = i;
	}
	return min;

}

typedef struct error
{
	int start, end;
	token_type_e awaited, actual;
	char *data;
} error_t;

static void on_editor_view_text_changed(GtkWidget *widget, GdkEventKey *event)
{
	//g_print("Invoked %s\n", __func__);

	gtk_text_buffer_set_text(editor.debug_view.buffer, "", 0);
	gtk_text_buffer_get_bounds(editor.editor_view.buffer,
	                           &editor.editor_view.start,
	                           &editor.editor_view.end);
	char *text = gtk_text_buffer_get_text( editor.editor_view.buffer, &editor.editor_view.start, &editor.editor_view.end, TRUE );
	if (text[0] == '\0')
	{
		gtk_widget_hide(GTK_WIDGET(editor.info_bar.bar));
		return;
	}

	int ret = tokenize(text, &editor.tokens);
	editor.tokens_bak = editor.tokens;
	if (ret)
	{
		char *tokens_str = token_collection_to_str(&editor.tokens);
		free(tokens_str);
	}

	parser_t parser;
	parser.state = STATE_INIT;
	editor.tokens = editor.tokens_bak;
	int error = 0;
	int errors_count = 0;

	token_type_e awaited;
	editor.tokens = editor.tokens_bak;

	error_t errors[100];
	int err_count = 0;
	states_t last_successfull_state = STATE_INIT;
	while (editor.tokens != NULL)
	{
		token_type_e aw = parse(&parser, *((token_t *)editor.tokens->data), &error);
		if (!error || ((token_t *)editor.tokens->data)->type == WHITESPACE)
			last_successfull_state = parser.state;
		else
		{
			errors[err_count] =
			(error_t){
				.start = ((token_t *)editor.tokens->data)->start,
				.end = ((token_t *)editor.tokens->data)->end,
				.awaited = aw,
				.actual = ((token_t *)editor.tokens->data)->type,
				.data = ((token_t *)editor.tokens->data)->value
			};
			err_count++;
			parser.state = last_successfull_state;
		}
		editor.tokens = editor.tokens->next;
	}
	char *err_desc = malloc(100);
	int err_len = 0;
	int total_len = 0;
	g_print("\033c");
	char *reserve = editor.error_description;
	for (int i = 0; i < 10000; i++)
		editor.error_description[i] = ' ';
	for (int i = 0; i < err_count; i++)
	{

		err_len = snprintf(NULL, 0, "awa=%s, act=%s, dat=%s, sta=%d, end=%d\n", token_type_to_str(errors[i].awaited), token_type_to_str(errors[i].actual), errors[i].data, errors[i].start, errors[i].end) + 1;
		snprintf(err_desc, err_len, "awa=%s, act=%s, dat=%s, sta=%d, end=%d\n", token_type_to_str(errors[i].awaited), token_type_to_str(errors[i].actual), errors[i].data, errors[i].start, errors[i].end);
		memcpy(editor.error_description, err_desc, err_len);
		editor.error_description += err_len - 1;
	}
	editor.error_description = reserve;
	gtk_text_buffer_set_text(editor.debug_view.buffer, editor.error_description, strlen(editor.error_description));
		free(err_desc);
}


static void stub_cb(GtkWidget *widget, GdkEventKey *event)
{
	g_print("Invoked %s, THIS IS STUB, TBA\n", __func__);
}

void init_info_bar()
{
	editor.info_bar.bar = GET_GTK_OBJ("info_bar");
	editor.info_bar.info_bar_close_bar_button = GET_GTK_OBJ("info_bar_close_bar_button");
	editor.info_bar.info_bar_jump_to_error_bar_button = GET_GTK_OBJ("info_bar_jump_to_error_bar_button");
	editor.info_bar.info_bar_label = GET_GTK_OBJ("info_bar_label");

	GTK_BUTTON_CB_CONN("info_bar_close_bar_button", on_close_info_bar);
	GTK_BUTTON_CB_CONN("info_bar_jump_to_error_bar_button", on_jump_to_error);
	GTK_BUTTON_CB_CONN("info_bar_quick_fix_button", on_quick_fix);
	gtk_widget_hide(GTK_WIDGET(editor.info_bar.bar));
}

void init_editor_view()
{
	editor.editor_view.textview = GET_GTK_OBJ("editor_view");
	editor.editor_view.buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor.editor_view.textview));
	g_signal_connect(editor.editor_view.buffer, "changed", G_CALLBACK(on_editor_view_text_changed), NULL);
	g_signal_connect(GTK_TEXT_VIEW(editor.editor_view.textview), "key-press-event", G_CALLBACK(code_key_pressed), NULL);

}

void init_debug_view()
{
	editor.debug_view.textview = GET_GTK_OBJ("debug_view");
	editor.debug_view.buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor.debug_view.textview));
}

void init_toolbox()
{
	GTK_BUTTON_CB_CONN("add_document_button", stub_cb);
	GTK_BUTTON_CB_CONN("open_document_button", stub_cb);
	GTK_BUTTON_CB_CONN("save_document_button", stub_cb);
	GTK_BUTTON_CB_CONN("undo_button", stub_cb);
	GTK_BUTTON_CB_CONN("redo_button", stub_cb);
	GTK_BUTTON_CB_CONN("copy_button", stub_cb);
	GTK_BUTTON_CB_CONN("cut_button", stub_cb);
	GTK_BUTTON_CB_CONN("paste_button", stub_cb);
	GTK_BUTTON_CB_CONN("run_button", stub_cb);
	GTK_BUTTON_CB_CONN("interrogation_button", stub_cb);
	GTK_BUTTON_CB_CONN("info_button", stub_cb);
}

void init_editor()
{
	editor.tokens = editor.tokens_bak = NULL;
}

static void activate(GtkApplication *app, gpointer user_data)
{
	editor.builder = gtk_builder_new();
	gtk_builder_add_from_file(editor.builder, "resources/xml/main_window.ui", NULL);
	editor.window = GET_GTK_OBJ("window");
	editor.error_description = malloc(10000);
	init_info_bar();
	init_editor_view();
	init_debug_view();
	init_toolbox();
	init_editor();
	gtk_window_set_application(GTK_WINDOW(editor.window), app);
	gtk_widget_set_visible(GTK_WIDGET(editor.window), TRUE);
}

int main(int argc, char **argv)
{
	GtkApplication *app = gtk_application_new("mstr_i.lang.processor", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), app);

	int status = g_application_run(G_APPLICATION(app), argc, argv);

	g_object_unref(app);
	return status;
}
