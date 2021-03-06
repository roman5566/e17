#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>

#include "eo_suite.h"
#include "Eo.h"

#include "class_simple.h"

START_TEST(eo_simple)
{
   eo_init();
   Eo *obj = eo_add(EO_BASE_CLASS, NULL);
   fail_if(obj);

   eo_shutdown();
}
END_TEST

START_TEST(eo_data_fetch)
{
   eo_init();

   /* Usually should be const, not const only for the test... */
   static Eo_Class_Description class_desc = {
        "Simple2",
        EO_CLASS_TYPE_REGULAR,
        EO_CLASS_DESCRIPTION_OPS(NULL, NULL, 0),
        NULL,
        10,
        NULL,
        NULL,
        NULL,
        NULL
   };

   const Eo_Class *klass = eo_class_new(&class_desc, EO_BASE_CLASS, NULL);
   fail_if(!klass);

   Eo *obj = eo_add(klass, NULL);
   fail_if(!obj);
#ifndef NDEBUG
   fail_if(eo_data_get(obj, SIMPLE_CLASS));
#endif
   eo_unref(obj);

   class_desc.data_size = 0;
   klass = eo_class_new(&class_desc, EO_BASE_CLASS, NULL);
   fail_if(!klass);

   obj = eo_add(klass, NULL);
   fail_if(!obj);
   fail_if(eo_data_get(obj, klass));
   eo_unref(obj);

   eo_shutdown();
}
END_TEST

START_TEST(eo_refs)
{
   eo_init();
   Eo *obj = eo_add(SIMPLE_CLASS, NULL);
   Eo *obj2 = eo_add(SIMPLE_CLASS, NULL);
   Eo *obj3 = eo_add(SIMPLE_CLASS, NULL);

   eo_xref(obj, obj2);
   fail_if(eo_ref_get(obj) != 2);
   eo_xref(obj, obj3);
   fail_if(eo_ref_get(obj) != 3);

   eo_xunref(obj, obj2);
   fail_if(eo_ref_get(obj) != 2);
   eo_xunref(obj, obj3);
   fail_if(eo_ref_get(obj) != 1);

#ifndef NDEBUG
   eo_xunref(obj, obj3);
   fail_if(eo_ref_get(obj) != 1);

   eo_xref(obj, obj2);
   fail_if(eo_ref_get(obj) != 2);

   eo_xunref(obj, obj3);
   fail_if(eo_ref_get(obj) != 2);

   eo_xunref(obj, obj2);
   fail_if(eo_ref_get(obj) != 1);
#endif

   eo_unref(obj);
   eo_unref(obj2);
   eo_unref(obj3);

   /* Just check it doesn't seg atm. */
   obj = eo_add(SIMPLE_CLASS, NULL);
   eo_ref(obj);
   eo_del(obj);
   eo_del(obj);

   eo_shutdown();
}
END_TEST

START_TEST(eo_weak_reference)
{
   eo_init();

   Eo *obj = eo_add(SIMPLE_CLASS, NULL);
   Eo *obj2 = eo_add(SIMPLE_CLASS, NULL);
   Eo *wref, *wref2, *wref3;
   eo_do(obj, eo_wref_add(&wref));
   fail_if(!wref);

   eo_unref(obj);
   fail_if(wref);

   obj = eo_add(SIMPLE_CLASS, NULL);
   eo_do(obj, eo_wref_add(&wref));

   eo_ref(obj);
   fail_if(!wref);

   eo_del(obj);
   fail_if(wref);

   eo_unref(obj);
   fail_if(wref);

   obj = eo_add(SIMPLE_CLASS, NULL);

   eo_do(obj, eo_wref_add(&wref));
   eo_do(obj, eo_wref_del(&wref));
   fail_if(wref);

   eo_do(obj, eo_wref_add(&wref));
   eo_do(obj2, eo_wref_del(&wref));
   fail_if(!wref);
   eo_wref_del_safe(&wref);
   fail_if(wref);

   wref = obj;
   eo_do(obj, eo_wref_del(&wref));
   fail_if(wref);

   wref = wref2 = wref3 = NULL;
   eo_do(obj, eo_wref_add(&wref), eo_wref_add(&wref2), eo_wref_add(&wref3));
   fail_if(!wref);
   fail_if(!wref2);
   fail_if(!wref3);
   eo_do(obj, eo_wref_del(&wref), eo_wref_del(&wref2), eo_wref_del(&wref3));
   fail_if(wref);
   fail_if(wref2);
   fail_if(wref3);

   eo_do(obj, eo_wref_add(&wref2), eo_wref_add(&wref3));
   wref = obj;
   eo_do(obj, eo_wref_del(&wref));
   fail_if(wref);
   eo_do(obj, eo_wref_del(&wref2), eo_wref_del(&wref3));

   eo_unref(obj);
   eo_unref(obj2);


   eo_shutdown();
}
END_TEST

static void
_a_set(Eo *obj EINA_UNUSED, void *class_data EINA_UNUSED, va_list *list EINA_UNUSED)
{
   fail_if(EINA_TRUE);
}

static void
_op_errors_class_constructor(Eo_Class *klass)
{
   const Eo_Op_Func_Description func_desc[] = {
        EO_OP_FUNC(SIMPLE_ID(SIMPLE_SUB_ID_LAST), _a_set),
        EO_OP_FUNC(SIMPLE_ID(SIMPLE_SUB_ID_LAST + 1), _a_set),
        EO_OP_FUNC(0x0F010111, _a_set),
        EO_OP_FUNC_SENTINEL
   };

   eo_class_funcs_set(klass, func_desc);
}

START_TEST(eo_op_errors)
{
   eo_init();

   static const Eo_Class_Description class_desc = {
        "Simple",
        EO_CLASS_TYPE_REGULAR,
        EO_CLASS_DESCRIPTION_OPS(NULL, NULL, 0),
        NULL,
        0,
        NULL,
        NULL,
        _op_errors_class_constructor,
        NULL
   };

   const Eo_Class *klass = eo_class_new(&class_desc, SIMPLE_CLASS, NULL);
   fail_if(!klass);

   Eo *obj = eo_add(klass, NULL);

   /* Out of bounds op for a legal class. */
   fail_if(eo_do(obj, 0x00010111));

   /* Ilegal class. */
   fail_if(eo_do(obj, 0x0F010111));

   fail_if(eo_ref_get(obj) != 1);

   eo_ref(obj);
   fail_if(eo_ref_get(obj) != 2);

   eo_ref(obj);
   fail_if(eo_ref_get(obj) != 3);

   eo_unref(obj);
   fail_if(eo_ref_get(obj) != 2);

   eo_unref(obj);
   fail_if(eo_ref_get(obj) != 1);

   eo_unref(obj);

   obj = eo_add(SIMPLE_CLASS, NULL);
   fail_if(!eo_do(obj, simple_a_print()));
   fail_if(!eo_query(obj, simple_a_print()));
   fail_if(eo_query(obj, simple_a_set(1)));

   eo_shutdown();
}
END_TEST

static void
_fake_free_func(void *data)
{
   if (!data)
      return;

   int *a = data;
   ++*a;
}

START_TEST(eo_generic_data)
{
   eo_init();
   Eo *obj = eo_add(SIMPLE_CLASS, NULL);
   void *data;

   eo_do(obj, eo_base_data_set("test1", (void *) 1, NULL));
   eo_do(obj, eo_base_data_get("test1", &data));
   fail_if(1 != (int) data);
   eo_do(obj, eo_base_data_del("test1"));
   eo_do(obj, eo_base_data_get("test1", &data));
   fail_if(data);

   eo_do(obj, eo_base_data_set("test1", (void *) 1, NULL));
   eo_do(obj, eo_base_data_set("test2", (void *) 2, NULL));
   eo_do(obj, eo_base_data_get("test1", &data));
   fail_if(1 != (int) data);
   eo_do(obj, eo_base_data_get("test2", &data));
   fail_if(2 != (int) data);

   eo_do(obj, eo_base_data_get("test2", &data));
   fail_if(2 != (int) data);
   eo_do(obj, eo_base_data_del("test2"));
   eo_do(obj, eo_base_data_get("test2", &data));
   fail_if(data);

   eo_do(obj, eo_base_data_get("test1", &data));
   fail_if(1 != (int) data);
   eo_do(obj, eo_base_data_del("test1"));
   eo_do(obj, eo_base_data_get("test1", &data));
   fail_if(data);

   int a = 0;
   eo_do(obj, eo_base_data_set("test3", &a, _fake_free_func));
   eo_do(obj, eo_base_data_get("test3", &data));
   fail_if(&a != data);
   eo_do(obj, eo_base_data_get("test3", NULL));
   eo_do(obj, eo_base_data_del("test3"));
   fail_if(a != 1);

   a = 0;
   eo_do(obj, eo_base_data_set("test3", &a, _fake_free_func));
   eo_do(obj, eo_base_data_set("test3", NULL, _fake_free_func));
   fail_if(a != 1);
   a = 0;
   data = (void *) 123;
   eo_do(obj, eo_base_data_set(NULL, &a, _fake_free_func));
   eo_do(obj, eo_base_data_get(NULL, &data));
   fail_if(data);
   eo_do(obj, eo_base_data_del(NULL));

   a = 0;
   eo_do(obj, eo_base_data_set("test3", &a, _fake_free_func));
   eo_do(obj, eo_base_data_set("test3", NULL, NULL));
   fail_if(a != 1);
   eo_do(obj, eo_base_data_set("test3", &a, _fake_free_func));

   eo_unref(obj);
   fail_if(a != 2);

   eo_shutdown();
}
END_TEST

START_TEST(eo_magic_checks)
{
   char buf[sizeof(long)]; /* Just enough to hold eina magic + a bit more. */
   eo_init();

   memset(buf, 1, sizeof(buf));
   Eo *obj = eo_add((Eo_Class *) buf, NULL);
   fail_if(obj);

   obj = eo_add(SIMPLE_CLASS, (Eo *) buf);
   fail_if(obj);

   obj = eo_add(SIMPLE_CLASS, NULL);
   fail_if(!obj);

   fail_if(eo_do((Eo *) buf, EO_NOOP));
   fail_if(eo_do_super((Eo *) buf, EO_NOOP));
   fail_if(eo_class_get((Eo *) buf));
   fail_if(eo_class_name_get((Eo_Class*) buf));
   eo_class_funcs_set((Eo_Class *) buf, NULL);

   fail_if(eo_class_new(NULL, (Eo_Class *) buf), NULL);

   eo_xref(obj, (Eo *) buf);
   eo_xunref(obj, (Eo *) buf);
   eo_xref((Eo *) buf, obj);
   eo_xunref((Eo *) buf, obj);

   eo_ref((Eo *) buf);
   eo_unref((Eo *) buf);

   fail_if(0 != eo_ref_get((Eo *) buf));

   Eo *wref = NULL;
   eo_do((Eo *) buf, eo_wref_add(&wref));
   fail_if(wref);

   eo_del((Eo *) buf);

   fail_if(eo_parent_get((Eo *) buf));

   eo_error_set((Eo *) buf);

   eo_constructor_super((Eo *) buf);
   eo_destructor_super((Eo *) buf);

   fail_if(eo_data_get((Eo *) buf, SIMPLE_CLASS));

   eo_composite_object_attach((Eo *) buf, obj);
   eo_composite_object_attach(obj, (Eo *) buf);
   eo_composite_object_detach((Eo *) buf, obj);
   eo_composite_object_detach(obj, (Eo *) buf);
   eo_composite_is((Eo *) buf);

   eo_do(obj, eo_event_callback_forwarder_add(NULL, (Eo *) buf));
   eo_do(obj, eo_event_callback_forwarder_del(NULL, (Eo *) buf));

   eo_unref(obj);

   eo_shutdown();
}
END_TEST

void eo_test_general(TCase *tc)
{
   tcase_add_test(tc, eo_generic_data);
   tcase_add_test(tc, eo_op_errors);
   tcase_add_test(tc, eo_simple);
   tcase_add_test(tc, eo_weak_reference);
   tcase_add_test(tc, eo_refs);
   tcase_add_test(tc, eo_magic_checks);
   tcase_add_test(tc, eo_data_fetch);
}
