#include "etk_test.h"

static Etk_Widget *label = NULL;
static Etk_Widget *entry = NULL;

static Etk_Bool _etk_test_entry_window_deleted_cb(void *data)
{
   Etk_Window *win = data;
   etk_widget_hide (ETK_WIDGET(win));
   return 1;
}

static void _etk_test_entry_print_clicked(Etk_Object *object, void *data)
{
   etk_label_set(ETK_LABEL(label), etk_entry_text_get(ETK_ENTRY(entry)));
}

void etk_test_entry_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *table;
   Etk_Widget *button;
   
	if (win)
	{
		etk_widget_show_all(ETK_WIDGET(win));
		return;
	}	
	
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Entry test");
   
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_entry_window_deleted_cb), win);	
	
   table = etk_table_new(2, 2, FALSE);
   etk_container_add(ETK_CONTAINER(win), table);

   entry = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), entry, 0, 0, 0, 0, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);

   button = etk_button_new_with_label("Print the text!!!");
   etk_table_attach(ETK_TABLE(table), button, 1, 1, 0, 0, 0, 0, ETK_FILL_POLICY_NONE);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_entry_print_clicked), NULL);

   label = etk_label_new(" ");
   etk_table_attach(ETK_TABLE(table), label, 0, 1, 1, 1, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);
   
   etk_widget_show_all(win);
}
