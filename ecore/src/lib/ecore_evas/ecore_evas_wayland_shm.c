#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define LOGFNS 1

#ifdef LOGFNS
# include <stdio.h>
# define LOGFN(fl, ln, fn) \
   printf("-ECORE_EVAS-WL: %25s: %5i - %s\n", fl, ln, fn);
#else
# define LOGFN(fl, ln, fn)
#endif

# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/mman.h>

# include <Eina.h>
# include <Evas.h>
# include <Ecore.h>

# include "ecore_evas_private.h"
# include "Ecore_Evas.h"

#ifdef BUILD_ECORE_EVAS_WAYLAND_SHM
# include <Evas_Engine_Wayland_Shm.h>
# include <Ecore_Wayland.h>
#endif

#ifdef BUILD_ECORE_EVAS_WAYLAND_SHM
/* local function prototypes */
static int _ecore_evas_wl_init(void);
static int _ecore_evas_wl_shutdown(void);
static void _ecore_evas_wl_pre_free(Ecore_Evas *ee);
static void _ecore_evas_wl_free(Ecore_Evas *ee);
static void _ecore_evas_wl_callback_resize_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee));
static void _ecore_evas_wl_callback_move_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee));
static void _ecore_evas_wl_callback_delete_request_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee));
static void _ecore_evas_wl_callback_focus_in_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee));
static void _ecore_evas_wl_callback_focus_out_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee));
static void _ecore_evas_wl_resize(Ecore_Evas *ee, int w, int h);
static void _ecore_evas_wl_show(Ecore_Evas *ee);
static int _ecore_evas_wl_render(Ecore_Evas *ee);
static void _ecore_evas_wl_screen_geometry_get(const Ecore_Evas *ee __UNUSED__, int *x, int *y, int *w, int *h);
static void _ecore_evas_wl_buffer_new(Ecore_Evas *ee, void **dest);

static Eina_Bool _ecore_evas_wl_event_mouse_down(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _ecore_evas_wl_event_mouse_up(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _ecore_evas_wl_event_mouse_move(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _ecore_evas_wl_event_mouse_wheel(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _ecore_evas_wl_event_mouse_in(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _ecore_evas_wl_event_mouse_out(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _ecore_evas_wl_event_focus_in(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _ecore_evas_wl_event_focus_out(void *data __UNUSED__, int type __UNUSED__, void *event);

/* local variables */
static int _ecore_evas_wl_init_count = 0;
static Ecore_Event_Handler *_ecore_evas_wl_event_handlers[8];

static Ecore_Evas_Engine_Func _ecore_wl_engine_func = 
{
   _ecore_evas_wl_free, 
   _ecore_evas_wl_callback_resize_set, 
   _ecore_evas_wl_callback_move_set, 
   NULL, // callback show set
   NULL, // callback hide set
   _ecore_evas_wl_callback_delete_request_set, 
   NULL, // callback destroy set
   _ecore_evas_wl_callback_focus_in_set, 
   _ecore_evas_wl_callback_focus_out_set, 
   NULL, // callback mouse in set
   NULL, // callback mouse out set
   NULL, // callback sticky set
   NULL, // callback unsticky set
   NULL, // callback pre render set
   NULL, // callback post render set
   NULL, // func move
   NULL, // func managed move
   _ecore_evas_wl_resize, 
   NULL, // func move_resize
   NULL, // func rotation set
   NULL, // func shaped set
   _ecore_evas_wl_show, 
   NULL, // func hide
   NULL, // func raise
   NULL, // func lower
   NULL, // func activate
   NULL, // func title set
   NULL, // func name_class set
   NULL, // func size min set
   NULL, // func size max set
   NULL, // func size base set
   NULL, // func size step set
   NULL, // func object cursor set
   NULL, // func layer set
   NULL, // func focus set
   NULL, // func iconified set
   NULL, // func borderless set
   NULL, // func override set
   NULL, // func maximized set
   NULL, // func fullscreen set
   NULL, // func avoid_damage set
   NULL, // func withdrawn set
   NULL, // func sticky set
   NULL, // func ignore_events set
   NULL, // func alpha set
   NULL, // func transparent set
   _ecore_evas_wl_render, 
   _ecore_evas_wl_screen_geometry_get
};

/* external variables */
#endif

#ifdef BUILD_ECORE_EVAS_WAYLAND_SHM
EAPI Ecore_Evas *
ecore_evas_wayland_shm_new(const char *disp_name, int x, int y, int w, int h, int frame)
{
   Evas_Engine_Info_Wayland_Shm *einfo;
   Ecore_Evas *ee;
   int method = 0;
   static int _win_id = 1;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (!(method = evas_render_method_lookup("wayland_shm"))) 
     {
        ERR("Render method lookup failed.");
        return NULL;
     }

   if (!(ecore_wl_init(disp_name))) 
     {
        ERR("Failed to initialize Ecore Wayland.");
        return NULL;
     }

   if (!(ee = calloc(1, sizeof(Ecore_Evas)))) 
     {
        ERR("Failed to allocate Ecore_Evas.");
        ecore_wl_shutdown();
        return NULL;
     }

   ECORE_MAGIC_SET(ee, ECORE_MAGIC_EVAS);

   _ecore_evas_wl_init();

   ee->engine.func = (Ecore_Evas_Engine_Func *)&_ecore_wl_engine_func;

   ee->driver = "wayland_shm";
   if (disp_name) ee->name = strdup(disp_name);

   if (w < 1) w = 1;
   if (h < 1) h = 1;

   ee->req.x = ee->x = x;
   ee->req.y = ee->y = y;
   ee->req.w = ee->w = w;
   ee->req.h = ee->h = h;
   ee->rotation = 0;
   ee->prop.max.w = ee->prop.max.h = 32767;
   ee->prop.layer = 4;
   ee->prop.request_pos = 0;
   ee->prop.sticky = 0;
   ee->prop.draw_frame = frame;
   ee->prop.window = _win_id++;

   ee->evas = evas_new();
   evas_data_attach_set(ee->evas, ee);
   evas_output_method_set(ee->evas, method);
   evas_output_size_set(ee->evas, ee->w, ee->h);
   evas_output_viewport_set(ee->evas, 0, 0, ee->w, ee->h);

   if (ee->prop.draw_frame) 
     evas_output_framespace_set(ee->evas, 4, 18, 8, 22);

   if ((einfo = (Evas_Engine_Info_Wayland_Shm *)evas_engine_info_get(ee->evas))) 
     {
        einfo->info.rotation = ee->rotation;
        einfo->info.debug = EINA_FALSE;
        if (!evas_engine_info_set(ee->evas, (Evas_Engine_Info *)einfo)) 
          {
             ERR("Failed to set Evas Engine Info for '%s'.", ee->driver);
             ecore_evas_free(ee);
             return NULL;
          }
     }
   else 
     {
        ERR("Failed to get Evas Engine Info for '%s'.", ee->driver);
        ecore_evas_free(ee);
        return NULL;
     }

   /* NB: we need to be notified before 'free' so we can munmap the evas 
    * engine destination */
   ecore_evas_callback_pre_free_set(ee, _ecore_evas_wl_pre_free);

   ecore_evas_input_event_register(ee);
   _ecore_evas_register(ee);

   ecore_event_window_register(ee->prop.window, ee, ee->evas, 
                               (Ecore_Event_Mouse_Move_Cb)_ecore_evas_mouse_move_process, 
                               (Ecore_Event_Multi_Move_Cb)_ecore_evas_mouse_multi_move_process, 
                               (Ecore_Event_Multi_Down_Cb)_ecore_evas_mouse_multi_down_process, 
                               (Ecore_Event_Multi_Up_Cb)_ecore_evas_mouse_multi_up_process);

   evas_event_feed_mouse_in(ee->evas, (unsigned int)((unsigned long long)(ecore_time_get() * 1000.0) & 0xffffffff), NULL);

   return ee;
}

/* local functions */
static int 
_ecore_evas_wl_init(void)
{
   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (++_ecore_evas_wl_init_count != 1)
     return _ecore_evas_wl_init_count;

   _ecore_evas_wl_event_handlers[0] = 
     ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, 
                             _ecore_evas_wl_event_mouse_down, NULL);
   _ecore_evas_wl_event_handlers[1] = 
     ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, 
                             _ecore_evas_wl_event_mouse_up, NULL);
   _ecore_evas_wl_event_handlers[2] = 
     ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, 
                             _ecore_evas_wl_event_mouse_move, NULL);
   _ecore_evas_wl_event_handlers[3] = 
     ecore_event_handler_add(ECORE_EVENT_MOUSE_WHEEL, 
                             _ecore_evas_wl_event_mouse_wheel, NULL);
   _ecore_evas_wl_event_handlers[4] = 
     ecore_event_handler_add(ECORE_WL_EVENT_MOUSE_IN, 
                             _ecore_evas_wl_event_mouse_in, NULL);
   _ecore_evas_wl_event_handlers[5] = 
     ecore_event_handler_add(ECORE_WL_EVENT_MOUSE_OUT, 
                             _ecore_evas_wl_event_mouse_out, NULL);
   _ecore_evas_wl_event_handlers[6] = 
     ecore_event_handler_add(ECORE_WL_EVENT_FOCUS_IN, 
                             _ecore_evas_wl_event_focus_in, NULL);
   _ecore_evas_wl_event_handlers[7] = 
     ecore_event_handler_add(ECORE_WL_EVENT_FOCUS_OUT, 
                             _ecore_evas_wl_event_focus_out, NULL);

   ecore_event_evas_init();

   return _ecore_evas_wl_init_count;
}

static int 
_ecore_evas_wl_shutdown(void)
{
   unsigned int i = 0;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (--_ecore_evas_wl_init_count != 0)
     return _ecore_evas_wl_init_count;

   for (i = 0; i < sizeof(_ecore_evas_wl_event_handlers) / sizeof(Ecore_Event_Handler *); i++) 
     {
        if (_ecore_evas_wl_event_handlers[i])
          ecore_event_handler_del(_ecore_evas_wl_event_handlers[i]);
     }

   ecore_event_evas_shutdown();

   return _ecore_evas_wl_init_count;
}

static void 
_ecore_evas_wl_pre_free(Ecore_Evas *ee)
{
   Evas_Engine_Info_Wayland_Shm *einfo;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   /* get engine info */
   einfo = (Evas_Engine_Info_Wayland_Shm *)evas_engine_info_get(ee->evas);
   if ((einfo) && (einfo->info.dest))
     {
        int ret = 0;

        /* munmap previous engine destination */
        ret = munmap(einfo->info.dest, ((ee->w * sizeof(int)) * ee->h));
     }
}

static void 
_ecore_evas_wl_free(Ecore_Evas *ee)
{
   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   /* destroy buffer */
   if (ee->engine.wl.buffer) wl_buffer_destroy(ee->engine.wl.buffer);
   ee->engine.wl.buffer = NULL;

   /* destroy shell surface */
   if (ee->engine.wl.shell_surface)
     wl_shell_surface_destroy(ee->engine.wl.shell_surface);
   ee->engine.wl.shell_surface = NULL;

   /* destroy surface */
   if (ee->engine.wl.surface) wl_surface_destroy(ee->engine.wl.surface);
   ee->engine.wl.surface = NULL;

   ecore_event_window_unregister(ee->prop.window);

   _ecore_evas_wl_shutdown();
   ecore_wl_shutdown();
}

static void 
_ecore_evas_wl_callback_resize_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee))
{
   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (!ee) return;
   ee->func.fn_resize = func;
}

static void 
_ecore_evas_wl_callback_move_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee))
{
   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (!ee) return;
   ee->func.fn_move = func;
}

static void 
_ecore_evas_wl_callback_delete_request_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee))
{
   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (!ee) return;
   ee->func.fn_delete_request = func;
}

static void 
_ecore_evas_wl_callback_focus_in_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee))
{
   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (!ee) return;
   ee->func.fn_focus_in = func;
}

static void 
_ecore_evas_wl_callback_focus_out_set(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee))
{
   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (!ee) return;
   ee->func.fn_focus_out = func;
}

static void 
_ecore_evas_wl_resize(Ecore_Evas *ee, int w, int h)
{
   Evas_Engine_Info_Wayland_Shm *einfo;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (!ee) return;
   if (w < 1) w = 1;
   if (h < 1) h = 1;
   ee->req.w = w;
   ee->req.h = h;
   if ((ee->w == w) && (ee->h == h)) return;

   /* get engine info */
   einfo = (Evas_Engine_Info_Wayland_Shm *)evas_engine_info_get(ee->evas);
   if (einfo->info.dest)
     {
        int ret = 0;

        /* munmap previous engine destination */
        ret = munmap(einfo->info.dest, ((ee->w * sizeof(int)) * ee->h));
     }

   /* free old buffer */
   if (ee->engine.wl.buffer) wl_buffer_destroy(ee->engine.wl.buffer);
   ee->engine.wl.buffer = NULL;

   ee->w = w;
   ee->h = h;

   /* create buffer @ new size (also mmaps the new destination) */
   _ecore_evas_wl_buffer_new(ee, &einfo->info.dest);

   /* change evas output & viewport sizes */
   evas_output_size_set(ee->evas, ee->w, ee->h);
   evas_output_viewport_set(ee->evas, 0, 0, ee->w, ee->h);

   /* set new engine destination */
   evas_engine_info_set(ee->evas, (Evas_Engine_Info *)einfo);

   /* flush new buffer fd */
   ecore_wl_flush();

   /* damage buffer */
   wl_buffer_damage(ee->engine.wl.buffer, 0, 0, ee->w, ee->h);

   if (ee->visible) 
     {
        /* if visible, attach to surface */
        wl_surface_attach(ee->engine.wl.surface, ee->engine.wl.buffer, 0, 0);

        /* damage surface */
        wl_surface_damage(ee->engine.wl.surface, 0, 0, ee->w, ee->h);
     }

   if (ee->func.fn_resize) ee->func.fn_resize(ee);
}

static void 
_ecore_evas_wl_show(Ecore_Evas *ee)
{
   Evas_Engine_Info_Wayland_Shm *einfo;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (!ee) return;
   if (ee->visible) return;

   /* get engine info */
   einfo = (Evas_Engine_Info_Wayland_Shm *)evas_engine_info_get(ee->evas);

   /* create new surface */
   ee->engine.wl.surface = 
     wl_compositor_create_surface(ecore_wl_compositor_get());
   wl_surface_set_user_data(ee->engine.wl.surface, (void *)ee->prop.window);

   /* get new shell surface */
   ee->engine.wl.shell_surface = 
     wl_shell_get_shell_surface(ecore_wl_shell_get(), ee->engine.wl.surface);

   /* set toplevel */
   wl_shell_surface_set_toplevel(ee->engine.wl.shell_surface);

   /* create buffer @ new size (also mmaps the new destination) */
   _ecore_evas_wl_buffer_new(ee, &einfo->info.dest);

   /* set new engine destination */
   evas_engine_info_set(ee->evas, (Evas_Engine_Info *)einfo);

   /* flush new buffer fd */
   ecore_wl_flush();

   /* attach buffer to surface */
   wl_surface_attach(ee->engine.wl.surface, ee->engine.wl.buffer, 0, 0);

   /* NB: No need to do a damage here. If we do, we end up w/ screen 
    * artifacts in the compositor */
   /* wl_surface_damage(ee->engine.wl.surface, 0, 0, ee->w, ee->h); */

   ee->visible = 1;
   if (ee->func.fn_show) ee->func.fn_show(ee);
}

static int 
_ecore_evas_wl_render(Ecore_Evas *ee)
{
   int rend = 0;

   if (!ee) return 0;
   if (!ee->visible) 
     evas_norender(ee->evas);
   else 
     {
        Eina_List *ll = NULL, *updates = NULL;
        Ecore_Evas *ee2 = NULL;

        if (ee->func.fn_pre_render) ee->func.fn_pre_render(ee);

        EINA_LIST_FOREACH(ee->sub_ecore_evas, ll, ee2) 
          {
             if (ee2->func.fn_pre_render) ee2->func.fn_pre_render(ee2);
             if (ee2->engine.func->fn_render)
               rend |= ee2->engine.func->fn_render(ee2);
             if (ee2->func.fn_post_render) ee2->func.fn_post_render(ee2);
          }

        if ((updates = evas_render_updates(ee->evas))) 
          {
             Eina_List *l = NULL;
             Eina_Rectangle *r;

             EINA_LIST_FOREACH(updates, l, r) 
               {
                  if (ee->engine.wl.buffer)
                    wl_buffer_damage(ee->engine.wl.buffer, 
                                     r->x, r->y, r->w, r->h);

                  if (ee->engine.wl.surface)
                    wl_surface_damage(ee->engine.wl.surface, 
                                      r->x, r->y, r->w, r->h);
               }

             evas_render_updates_free(updates);
             _ecore_evas_idle_timeout_update(ee);
             rend = 1;
          }

        if (ee->func.fn_post_render) ee->func.fn_post_render(ee);
     }

   return rend;
}

static void 
_ecore_evas_wl_screen_geometry_get(const Ecore_Evas *ee __UNUSED__, int *x, int *y, int *w, int *h)
{
   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (x) *x = 0;
   if (y) *y = 0;
   ecore_wl_screen_size_get(w, h);
}

static Eina_Bool 
_ecore_evas_wl_event_mouse_down(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Evas *ee;
   Ecore_Event_Mouse_Button *ev;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   ev = event;
   ee = ecore_event_window_match(ev->window);
   if ((!ee) || (ee->ignore_events)) return ECORE_CALLBACK_PASS_ON;
   if (ev->window != ee->prop.window) return ECORE_CALLBACK_PASS_ON;
   /* printf("Mouse Down: %d %d\t%d %d\n",  */
   /*        ev->x, ev->y, ev->root.x, ev->root.y); */
   evas_event_feed_mouse_down(ee->evas, ev->buttons, ev->modifiers, 
                              ev->timestamp, NULL);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool 
_ecore_evas_wl_event_mouse_up(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Evas *ee;
   Ecore_Event_Mouse_Button *ev;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   ev = event;
   ee = ecore_event_window_match(ev->window);
   if ((!ee) || (ee->ignore_events)) return ECORE_CALLBACK_PASS_ON;
   if (ev->window != ee->prop.window) return ECORE_CALLBACK_PASS_ON;
   evas_event_feed_mouse_up(ee->evas, ev->buttons, ev->modifiers, 
                            ev->timestamp, NULL);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool 
_ecore_evas_wl_event_mouse_move(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Evas *ee;
   Ecore_Event_Mouse_Move *ev;

   ev = event;
   ee = ecore_event_window_match(ev->window);
   if ((!ee) || (ee->ignore_events)) return ECORE_CALLBACK_PASS_ON;
   if (ev->window != ee->prop.window) return ECORE_CALLBACK_PASS_ON;
   ee->mouse.x = ev->x;
   ee->mouse.y = ev->y;
   evas_event_feed_mouse_move(ee->evas, ev->x, ev->y, ev->timestamp, NULL);
   _ecore_evas_mouse_move_process(ee, ev->x, ev->y, ev->timestamp);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool 
_ecore_evas_wl_event_mouse_wheel(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Evas *ee;
   Ecore_Event_Mouse_Wheel *ev;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   ev = event;
   ee = ecore_event_window_match(ev->window);
   if ((!ee) || (ee->ignore_events)) return ECORE_CALLBACK_PASS_ON;
   if (ev->window != ee->prop.window) return ECORE_CALLBACK_PASS_ON;
   evas_event_feed_mouse_wheel(ee->evas, ev->direction, ev->z, 
                               ev->timestamp, NULL);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool 
_ecore_evas_wl_event_mouse_in(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Evas *ee;
   Ecore_Wl_Event_Mouse_In *ev;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   ev = event;
   ee = ecore_event_window_match(ev->window);
   if ((!ee) || (ee->ignore_events)) return ECORE_CALLBACK_PASS_ON;
   if (ev->window != ee->prop.window) return ECORE_CALLBACK_PASS_ON;
   if (ee->func.fn_mouse_in) ee->func.fn_mouse_in(ee);
   ecore_event_evas_modifier_lock_update(ee->evas, ev->modifiers);
   evas_event_feed_mouse_in(ee->evas, ev->time, NULL);
   _ecore_evas_mouse_move_process(ee, ev->x, ev->y, ev->time);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool 
_ecore_evas_wl_event_mouse_out(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Evas *ee;
   Ecore_Wl_Event_Mouse_Out *ev;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   ev = event;
   ee = ecore_event_window_match(ev->window);
   if ((!ee) || (ee->ignore_events)) return ECORE_CALLBACK_PASS_ON;
   if (ev->window != ee->prop.window) return ECORE_CALLBACK_PASS_ON;
   ecore_event_evas_modifier_lock_update(ee->evas, ev->modifiers);
   _ecore_evas_mouse_move_process(ee, ev->x, ev->y, ev->time);
   evas_event_feed_mouse_out(ee->evas, ev->time, NULL);
   if (ee->func.fn_mouse_out) ee->func.fn_mouse_out(ee);
   if (ee->prop.cursor.object) evas_object_hide(ee->prop.cursor.object);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool 
_ecore_evas_wl_event_focus_in(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Evas *ee;
   Ecore_Wl_Event_Focus_In *ev;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   ev = event;
   ee = ecore_event_window_match(ev->window);
   if ((!ee) || (ee->ignore_events)) return ECORE_CALLBACK_PASS_ON;
   if (ev->window != ee->prop.window) return ECORE_CALLBACK_PASS_ON;
   ee->prop.focused = 1;
   evas_focus_in(ee->evas);
   if (ee->func.fn_focus_in) ee->func.fn_focus_in(ee);
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool 
_ecore_evas_wl_event_focus_out(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Evas *ee;
   Ecore_Wl_Event_Focus_Out *ev;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   ev = event;
   ee = ecore_event_window_match(ev->window);
   if ((!ee) || (ee->ignore_events)) return ECORE_CALLBACK_PASS_ON;
   if (ev->window != ee->prop.window) return ECORE_CALLBACK_PASS_ON;
   evas_focus_out(ee->evas);
   ee->prop.focused = 0;
   if (ee->func.fn_focus_out) ee->func.fn_focus_out(ee);
   return ECORE_CALLBACK_PASS_ON;
}

static void 
_ecore_evas_wl_buffer_new(Ecore_Evas *ee, void **dest)
{
   static unsigned int format;
   char tmp[PATH_MAX];
   int fd = -1, stride = 0, size = 0;
   void *ret;

   LOGFN(__FILE__, __LINE__, __FUNCTION__);

   if (dest) *dest = NULL;

   if (!format) format = ecore_wl_format_get();

   strcpy(tmp, "/tmp/ecore-wayland_shm-XXXXXX");
   if ((fd = mkstemp(tmp)) < 0) 
     {
        ERR("Could not create temporary file.");
        return;
     }

   stride = (ee->w * sizeof(int));
   size = (stride * ee->h);
   if (ftruncate(fd, size) < 0) 
     {
        ERR("Could not truncate temporary file.");
        close(fd);
        return;
     }

   ret = mmap(NULL, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0);
   unlink(tmp);

   if (ret == MAP_FAILED) 
     {
        ERR("mmap of temporary file failed.");
        close(fd);
        return;
     }

   if (dest) *dest = ret;

   ee->engine.wl.buffer = 
     wl_shm_create_buffer(ecore_wl_shm_get(), fd, ee->w, ee->h, stride, format);

   close(fd);
}

#else
EAPI Ecore_Evas *
ecore_evas_wayland_shm_new(const char *disp_name __UNUSED__, int x __UNUSED__, int y __UNUSED__, int w __UNUSED__, int h __UNUSED__, int frame __UNUSED__)
{
   return NULL;
}
#endif