/*
 * vim:ts=8:sw=3
 */

#include <stdio.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <stdlib.h>
#include "Esmart_Text_Entry.h"

struct _Esmart_Text_Entry
{
  Evas_Object *clip;
  Evas_Object *base;
  struct
  {
    int size;
    char *text;
    int index;
  } buf;
  struct
  {
    Evas_Object *o;
    char *part;
  } edje;
  struct
  {
    void (*func) (void *data, const char *str);
    void *arg;
  } return_key;
  int passwd;
};
typedef struct _Esmart_Text_Entry Esmart_Text_Entry;

#define DEBUG 0
static Evas_Smart *esmart_text_entry_smart_get (void);
static void esmart_text_entry_text_fix (Evas_Object * o);

Evas_Object *
esmart_text_entry_new (Evas * e)
{
  Evas_Object *o = NULL;

  o = evas_object_smart_add (e, esmart_text_entry_smart_get ());
  return (o);
}

#if 0 /* come on guys! use the basic focus stuff! :) */
int
esmart_text_entry_is_focused (Evas_Object * o)
{
   return (evas_object_focus_get (o));
}

void
esmart_text_entry_focus_set (Evas_Object * o, int val)
{
  evas_object_focus_set(o, val);
}
#endif

char *
esmart_text_entry_string_get (Evas_Object * o)
{
  char *result = NULL;
  Esmart_Text_Entry *e = NULL;

  if ((e = evas_object_smart_data_get (o)))
    {
      if (e->buf.text)
	result = strdup (e->buf.text);
    }
  return (result);
}

void
esmart_text_entry_is_password_set (Evas_Object * o, int val)
{
  Esmart_Text_Entry *e = NULL;

  if ((e = evas_object_smart_data_get (o)))
    {
      e->passwd = val;
    }

}

void
esmart_text_entry_edje_part_set (Evas_Object * o, Evas_Object * edje,
				 const char *part)
{
  Esmart_Text_Entry *e = NULL;

  if ((e = evas_object_smart_data_get (o)))
    {
      e->edje.o = edje;
      if (e->edje.part)
	free (e->edje.part);
      e->edje.part = strdup (part);
    }
}

void
esmart_text_entry_return_key_callback_set (Evas_Object * o,
					   void (*func) (void *data,
							 const char *str),
					   void *data)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  e->return_key.func = func;
  e->return_key.arg = data;
}

void
esmart_text_entry_max_chars_set (Evas_Object * o, int max)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  if (max < PATH_MAX)
    {
      char buf[PATH_MAX];

      snprintf (buf, PATH_MAX, "%s", e->buf.text);
      free (e->buf.text);
      e->buf.size = max;
      e->buf.text = (char *) malloc ((sizeof (char) * e->buf.size) + 1);
      memset (e->buf.text, 0, sizeof (e->buf.text));
      snprintf (e->buf.text, strlen (buf), "%s", buf);
    }
}
void
esmart_text_entry_text_set (Evas_Object * o, const char *str)
{
  Esmart_Text_Entry *e = NULL;

  if ((e = evas_object_smart_data_get (o)))
    {
      if (str)
	snprintf (e->buf.text, e->buf.size, "%s", str);
      else
	{
	  memset (e->buf.text, 0, e->buf.size);
	}
      e->buf.index = strlen (e->buf.text);
      esmart_text_entry_text_fix (o);
#if DEBUG
      fprintf (stderr, "Text Set (%d) %s\n", e->buf.index, e->buf.text);
#endif
    }
}

/*========================================================================*/
static void
esmart_text_entry_buffer_char_append (Evas_Object * o, char c)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);

  if (e->buf.index < e->buf.size)
    e->buf.text[e->buf.index++] = c;
  e->buf.text[e->buf.index] = 0;
  esmart_text_entry_text_fix (o);
}
static void
esmart_text_entry_buffer_backspace (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);

  if (e->buf.index > 0)
    e->buf.text[--e->buf.index] = 0;
  esmart_text_entry_text_fix (o);
}
static void
esmart_text_entry_text_fix (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  if (!o)
    return;
  if ((e = evas_object_smart_data_get (o)))
    {
      if (e->passwd)
	{
	  int i, len;
	  char buf[e->buf.size];

	  len = strlen (e->buf.text);
	  memset (buf, 0, e->buf.size);
	  for (i = 0; i < len; i++)
	    buf[i] = '*';
	  edje_object_part_text_set (e->edje.o, e->edje.part, buf);
	}
      else
	edje_object_part_text_set (e->edje.o, e->edje.part, e->buf.text);
    }
}
static void
_key_down_cb (void *data, Evas * e, Evas_Object * o, void *ev)
{
  Evas_Event_Key_Down *down = NULL;
  Ecore_X_Event_Key_Down *evx = NULL;
  Esmart_Text_Entry *entry = NULL;

  if (ecore_event_current_type_get () != ECORE_X_EVENT_KEY_DOWN)
    return;

  if (!(evx = (Ecore_X_Event_Key_Down *) ecore_event_current_event_get ()))
    return;

  down = ev;
  entry = evas_object_smart_data_get (data);

  /* handle modifiers */
  if ((!strcmp (evx->keyname, "Control_L"))
      || (!strcmp (evx->keyname, "Control_R"))
      || (!strcmp (evx->keyname, "Shift_R"))
      || (!strcmp (evx->keyname, "Shift_L"))
      || (!strcmp (evx->keyname, "Alt_R"))
      || (!strcmp (evx->keyname, "Alt_L")))
     {
	evas_key_modifier_on (e, evx->keyname);
     }
   
  if (evas_key_modifier_is_set_get (down->modifiers, evx->keyname))
     evas_key_modifier_off (e, evx->keyname);
   else if (evas_key_modifier_is_set_get (down->modifiers, "Control_L")
	    || evas_key_modifier_is_set_get (down->modifiers, "Control_R"))
     {
	switch ((int) evx->keyname[0])
	  {
	   case 117:
	     esmart_text_entry_text_set (data, "");
	     break;
	   default:
#if DEBUG
	     fprintf (stderr, "(%d) is the key value\n",
		      (int) evx->keyname[0]);
#endif
	     break;
	  }
	
     }
   else if ((strlen (evx->keyname) > 1)
	    && (!evx->key_compose || (strlen (evx->key_compose) > 1)))
     {
	if (!strcmp (evx->keyname, "BackSpace"))
	  {
	     esmart_text_entry_buffer_backspace (data);
	  }
	else
	  {
	     fprintf (stderr, "Unknown string %s\n", evx->keyname);
	  }
     }
   else
     {
	switch ((int) evx->key_compose[0])
	  {
	   case 127: /* Delete */
	   case 9:	  /* \t */
	     break;
	   case 8:	  /* Backspace */
	     esmart_text_entry_buffer_backspace (data);
	     break;
	   case 13:  /* \r */
	     if (entry->return_key.func)
	       {
		  entry->return_key.func (entry->return_key.arg,
					  entry->buf.text);
#if DEBUG
		  fprintf (stderr, "Buffer Length %d\n",
			   strlen (entry->buf.text));
#endif
	       }
	     break;
	   default:
	     esmart_text_entry_buffer_char_append (data,
						   evx->key_compose[0]);
#if DEBUG
	     fprintf (stderr, "(%d) is the key_compose value\n",
		      (int) evx->key_compose[0]);
#endif
	     break;
	  }
     }
   esmart_text_entry_text_fix (data);
}

static void
esmart_text_entry_add (Evas_Object * o)
{
  Esmart_Text_Entry *entry = NULL;

  entry = (Esmart_Text_Entry *) malloc (sizeof (Esmart_Text_Entry));
  memset (entry, 0, sizeof (Esmart_Text_Entry));
  entry->buf.size = 128;
  entry->buf.text = (char *) malloc ((sizeof (char) * 128) + 1);
  memset (entry->buf.text, 0, 128);

  evas_object_smart_data_set (o, entry);

  evas_key_modifier_add (evas_object_evas_get (o), "Control_L");
  evas_key_modifier_add (evas_object_evas_get (o), "Control_R");
  evas_key_modifier_add (evas_object_evas_get (o), "Shift_L");
  evas_key_modifier_add (evas_object_evas_get (o), "Shift_R");

  entry->clip = evas_object_rectangle_add (evas_object_evas_get (o));
  evas_object_layer_set (entry->clip, 0);

  entry->base = evas_object_rectangle_add (evas_object_evas_get (o));
  evas_object_layer_set (entry->base, 0);
  evas_object_color_set (entry->base, 255, 255, 255, 0);
  evas_object_clip_set (entry->base, entry->clip);
  evas_object_event_callback_add (o, EVAS_CALLBACK_KEY_DOWN,
				  _key_down_cb, o);

  evas_object_show (entry->base);
}
static void
esmart_text_entry_del (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_del (e->base);
  evas_object_del (e->clip);
  free (e->buf.text);
  free (e);
}
static void
esmart_text_entry_layer_set (Evas_Object * o, int l)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_layer_set (e->base, l);
}
static void
esmart_text_entry_raise (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_raise (e->base);
}
static void
esmart_text_entry_lower (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_lower (e->base);
}
static void
esmart_text_entry_stack_above (Evas_Object * o, Evas_Object * above)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_stack_above (e->base, above);

}
static void
esmart_text_entry_stack_below (Evas_Object * o, Evas_Object * below)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_stack_below (e->base, below);
}
static void
esmart_text_entry_move (Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_move (e->clip, x, y);
  evas_object_move (e->base, x, y);
}
static void
esmart_text_entry_resize (Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_resize (e->clip, w, h);
  evas_object_resize (e->base, w, h);
}
static void
esmart_text_entry_show (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_show (e->clip);
}
static void
esmart_text_entry_hide (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_hide (e->clip);
}
static void
esmart_text_entry_color_set (Evas_Object * o, int r, int g, int b, int a)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
}
static void
esmart_text_entry_clip_set (Evas_Object * o, Evas_Object * clip)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_clip_set (e->clip, clip);
}
static void
esmart_text_entry_clip_unset (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_clip_unset (e->clip);
}
static Evas_Smart *
esmart_text_entry_smart_get (void)
{
  static Evas_Smart *s = NULL;

  if (!s)
    s = evas_smart_new ("Text Entry", esmart_text_entry_add,
		    esmart_text_entry_del, esmart_text_entry_layer_set,
		    esmart_text_entry_raise, esmart_text_entry_lower,
		    esmart_text_entry_stack_above,
		    esmart_text_entry_stack_below, esmart_text_entry_move,
		    esmart_text_entry_resize, esmart_text_entry_show,
		    esmart_text_entry_hide, esmart_text_entry_color_set,
		    esmart_text_entry_clip_set, esmart_text_entry_clip_unset,
		    NULL);
  return (s);
}
