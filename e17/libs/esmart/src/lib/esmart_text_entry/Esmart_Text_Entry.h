#ifndef _ESMART_TEXT_ENTRY_H
#define _ESMART_TEXT_ENTRY_H

#include <Evas.h>
#include <Edje.h>
#include <Ecore.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/* create a new text entry */
Evas_Object *esmart_text_entry_new (Evas * e);

void esmart_text_entry_text_set (Evas_Object * o, const char *str);
void esmart_text_entry_is_password_set (Evas_Object * o, int val);
void esmart_text_entry_max_chars_set (Evas_Object * o, int max);
void esmart_text_entry_edje_part_set (Evas_Object * o, Evas_Object * edje,
				      const char *part);


/* you've gotta free this resut */
//char * esmart_text_entry_string_get(Evas_Object *o)

void esmart_text_entry_return_key_callback_set (Evas_Object * o,
						void (*func) (void *data,
							      const char
							      *str),
						void *data);

#ifdef __cplusplus
}
#endif

#endif
