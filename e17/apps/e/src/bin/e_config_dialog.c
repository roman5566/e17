/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"

/* local subsystem functions */
static void _e_config_dialog_free(E_Config_Dialog *cfd);
static void _e_config_dialog_go(E_Config_Dialog *cfd, E_Config_Dialog_CFData_Type type);
static void _e_config_dialog_cb_dialog_del(void *obj);
static void _e_config_dialog_cb_ok(void *data, E_Dialog *dia);
static void _e_config_dialog_cb_apply(void *data, E_Dialog *dia);
static void _e_config_dialog_cb_advanced(void *data, void *data2);
static void _e_config_dialog_cb_basic(void *data, void *data2);
static void _e_config_dialog_cb_changed(void *data, Evas_Object *obj);
static void _e_config_dialog_cb_close(void *data, E_Dialog *dia);

/* local subsystem globals */

/* externally accessible functions */

EAPI E_Config_Dialog *
e_config_dialog_new(E_Container *con, char *title, char *icon, int icon_size, E_Config_Dialog_View *view, void *data)
{
   E_Config_Dialog *cfd;
   

   cfd = E_OBJECT_ALLOC(E_Config_Dialog, E_CONFIG_DIALOG_TYPE, _e_config_dialog_free);
   cfd->view = *view;
   /* Seems that every user of this allocates view it on the stack and doesn't clear it, 
    * so I can't rely on this being NULL.  I currently set it to NULL in e_config_dialog_new()
    * and if you want to use it, set it in create_widgets();
    * I suspect that allocating a structure on the stack that lives beyond the function 
    * call is just asking for trouble.
    */
   cfd->view.close_cfdata = NULL;
   cfd->con = con;
   cfd->title = evas_stringshare_add(title);
   if (icon)
     {
	cfd->icon = evas_stringshare_add(icon);
	cfd->icon_size = icon_size;
     }
   cfd->data = data;
   cfd->hide_buttons = 1;

   if ((cfd->view.basic.apply_cfdata) || (cfd->view.advanced.apply_cfdata))
      cfd->hide_buttons = 0;

   _e_config_dialog_go(cfd, E_CONFIG_DIALOG_CFDATA_TYPE_BASIC);

   return cfd;
}

/* local subsystem functions */

static void
_e_config_dialog_free(E_Config_Dialog *cfd)
{
   if (cfd->title) evas_stringshare_del(cfd->title);
   if (cfd->icon) evas_stringshare_del(cfd->icon);
   if (cfd->cfdata)
     {
	cfd->view.free_cfdata(cfd, cfd->cfdata);
	cfd->cfdata = NULL;
     }
   if (cfd->dia)
     {
	e_object_del_attach_func_set(E_OBJECT(cfd->dia), NULL);
	e_object_del(E_OBJECT(cfd->dia));
	cfd->dia = NULL;
     }
   free(cfd);
}

static void
_e_config_dialog_go(E_Config_Dialog *cfd, E_Config_Dialog_CFData_Type type)
{
   E_Dialog *pdia;
   Evas_Object *o, *ob;
   Evas_Coord mw = 0, mh = 0;
   
   pdia = cfd->dia;
   cfd->dia = e_dialog_new(cfd->con);
   cfd->dia->data = cfd;
   cfd->view_dirty=0;
   e_object_del_attach_func_set(E_OBJECT(cfd->dia), _e_config_dialog_cb_dialog_del);
   e_dialog_title_set(cfd->dia, cfd->title);
   if (cfd->icon) e_dialog_icon_set(cfd->dia, cfd->icon, cfd->icon_size);
   
   if (!cfd->cfdata) cfd->cfdata = cfd->view.create_cfdata(cfd);
   
   if (type == E_CONFIG_DIALOG_CFDATA_TYPE_BASIC)
     {
	if (cfd->view.advanced.create_widgets)
	  {
	     o = e_widget_list_add(e_win_evas_get(cfd->dia->win), 0, 0);
	     ob = cfd->view.basic.create_widgets(cfd, e_win_evas_get(cfd->dia->win), cfd->cfdata);
	     e_widget_list_object_append(o, ob, 1, 1, 0.0);
	     ob = e_widget_button_add(e_win_evas_get(cfd->dia->win),
				      _("Advanced"), "widget/new_dialog",
				      _e_config_dialog_cb_advanced, cfd, NULL);
	     e_widget_list_object_append(o, ob, 0, 0, 1.0);
	  }
	else
	  o = cfd->view.basic.create_widgets(cfd, e_win_evas_get(cfd->dia->win), cfd->cfdata);
     }
   else
     {
	if (cfd->view.basic.create_widgets)
	  {
	     o = e_widget_list_add(e_win_evas_get(cfd->dia->win), 0, 0);
	     ob = cfd->view.advanced.create_widgets(cfd, e_win_evas_get(cfd->dia->win), cfd->cfdata);
	     e_widget_list_object_append(o, ob, 1, 1, 0.0);
	     ob = e_widget_button_add(e_win_evas_get(cfd->dia->win), 
				      _("Basic"), "widget/new_dialog",
				      _e_config_dialog_cb_basic, cfd, NULL);
	     e_widget_list_object_append(o, ob, 0, 0, 1.0);
	  }
	else
	  o = cfd->view.advanced.create_widgets(cfd, e_win_evas_get(cfd->dia->win), cfd->cfdata);
     }
   
   e_widget_min_size_get(o, &mw, &mh);
   e_widget_on_change_hook_set(o, _e_config_dialog_cb_changed, cfd);
   e_dialog_content_set(cfd->dia, o, mw, mh);
   
   if (!cfd->hide_buttons)
     {
	e_dialog_button_add(cfd->dia, _("OK"), NULL, _e_config_dialog_cb_ok, cfd);
	e_dialog_button_add(cfd->dia, _("Apply"), NULL, _e_config_dialog_cb_apply, cfd);
	e_dialog_button_disable_num_set(cfd->dia, 0, 1);
	e_dialog_button_disable_num_set(cfd->dia, 1, 1);
     }
   e_dialog_button_add(cfd->dia, _("Close"), NULL, _e_config_dialog_cb_close, cfd);
   if (!pdia)
     {
	e_win_centered_set(cfd->dia->win, 1);
	e_dialog_show(cfd->dia);
     }
   else
     {
	int x, y;

	e_dialog_show(cfd->dia);
	x = pdia->win->border->x + ((pdia->win->w - cfd->dia->win->w) / 2);
	y = pdia->win->border->y + ((pdia->win->h - cfd->dia->win->h) / 2);
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if ((x + cfd->dia->win->w) > (pdia->win->container->w))
	  x = pdia->win->container->w - cfd->dia->win->w;
	if ((y + cfd->dia->win->h) > (pdia->win->container->h))
	  x = pdia->win->container->h - cfd->dia->win->h;
	e_win_move(cfd->dia->win, x, y);
	e_win_placed_set(cfd->dia->win, 1);
     }
   cfd->view_type = type;
   
   if (pdia)
     {
	e_object_del_attach_func_set(E_OBJECT(pdia), NULL);
	e_object_del(E_OBJECT(pdia));
     }
}

static void
_e_config_dialog_cb_dialog_del(void *obj)
{
   E_Dialog *dia;
   E_Config_Dialog *cfd;
   
   dia = obj;
   cfd = dia->data;
   cfd->dia = NULL;
   e_object_del(E_OBJECT(cfd));
}

static void
_e_config_dialog_cb_ok(void *data, E_Dialog *dia)
{
   E_Config_Dialog *cfd;
   int ok = 0;
   
   cfd = dia->data;
   if (cfd->view_type == E_CONFIG_DIALOG_CFDATA_TYPE_BASIC)
      {
         if (cfd->view.basic.apply_cfdata)
            ok = cfd->view.basic.apply_cfdata(cfd, cfd->cfdata);
      }
   else
      {
         if (cfd->view.advanced.apply_cfdata)
            ok = cfd->view.advanced.apply_cfdata(cfd, cfd->cfdata);
      }
   if (ok) e_object_del(E_OBJECT(cfd));
}

static void
_e_config_dialog_cb_apply(void *data, E_Dialog *dia)
{
   E_Config_Dialog *cfd;
   int ok = 0;

   cfd = dia->data;
   if (cfd->view_type == E_CONFIG_DIALOG_CFDATA_TYPE_BASIC)
      {
         if (cfd->view.basic.apply_cfdata)
            ok = cfd->view.basic.apply_cfdata(cfd, cfd->cfdata);
      }
   else
      {
         if (cfd->view.advanced.apply_cfdata)
            ok = cfd->view.advanced.apply_cfdata(cfd, cfd->cfdata);
      }
   if ((ok) && (!cfd->hide_buttons))
     {
	e_dialog_button_disable_num_set(cfd->dia, 0, 1);
	e_dialog_button_disable_num_set(cfd->dia, 1, 1);
     }
}

static void
_e_config_dialog_cb_advanced(void *data, void *data2)
{
   E_Config_Dialog *cfd;
   
   cfd = data;
   _e_config_dialog_go(cfd, E_CONFIG_DIALOG_CFDATA_TYPE_ADVANCED);
}

static void
_e_config_dialog_cb_basic(void *data, void *data2)
{
   E_Config_Dialog *cfd;
   
   cfd = data;
   _e_config_dialog_go(cfd, E_CONFIG_DIALOG_CFDATA_TYPE_BASIC);
}

static void
_e_config_dialog_cb_changed(void *data, Evas_Object *obj)
{
   E_Config_Dialog *cfd;
   
   cfd = data;
   if (!cfd->hide_buttons)
      {
         e_dialog_button_disable_num_set(cfd->dia, 0, 0);
         e_dialog_button_disable_num_set(cfd->dia, 1, 0);
      }
}

static void
_e_config_dialog_cb_close(void *data, E_Dialog *dia)
{
   E_Config_Dialog *cfd;
   int ok = 1;

   cfd = dia->data;
   if (cfd->view.close_cfdata)
      ok = cfd->view.close_cfdata(cfd, cfd->cfdata);

   if (ok)
      e_object_del(E_OBJECT(dia));
}
