#include "Eo.h"
#include "mixin.h"
#include "simple.h"

#include "config.h"

EAPI Eo_Op SIMPLE_BASE_ID = 0;

typedef struct
{
   int a;
   int b;
} Private_Data;

#define MY_CLASS SIMPLE_CLASS

static char *class_var = NULL;

#define _GET_SET_FUNC(name) \
static void \
_##name##_get(const Eo *obj EINA_UNUSED, const void *class_data, va_list *list) \
{ \
   const Private_Data *pd = class_data; \
   int *name; \
   name = va_arg(*list, int *); \
   *name = pd->name; \
   printf("%s %d\n", __func__, pd->name); \
} \
static void \
_##name##_set(Eo *obj EINA_UNUSED, void *class_data, va_list *list) \
{ \
   Private_Data *pd = class_data; \
   int name; \
   name = va_arg(*list, int); \
   pd->name = name; \
   printf("%s %d\n", __func__, pd->name); \
}

_GET_SET_FUNC(a)
_GET_SET_FUNC(b)

extern int my_init_count;

static void
_constructor(Eo *obj, void *class_data EINA_UNUSED)
{
   eo_constructor_super(obj);

   my_init_count++;
}

static void
_destructor(Eo *obj, void *class_data EINA_UNUSED)
{
   eo_destructor_super(obj);

   my_init_count--;
}

static void
_class_constructor(Eo_Class *klass)
{
   const Eo_Op_Func_Description func_desc[] = {
        EO_OP_FUNC(SIMPLE_ID(SIMPLE_SUB_ID_A_SET), _a_set),
        EO_OP_FUNC_CONST(SIMPLE_ID(SIMPLE_SUB_ID_A_GET), _a_get),
        EO_OP_FUNC(SIMPLE_ID(SIMPLE_SUB_ID_B_SET), _b_set),
        EO_OP_FUNC_CONST(SIMPLE_ID(SIMPLE_SUB_ID_B_GET), _b_get),
        EO_OP_FUNC_SENTINEL
   };

   eo_class_funcs_set(klass, func_desc);

   class_var = malloc(10);
}

static void
_class_destructor(Eo_Class *klass EINA_UNUSED)
{
   free(class_var);
}

static const Eo_Op_Description op_desc[] = {
     EO_OP_DESCRIPTION(SIMPLE_SUB_ID_A_SET, "i", "Set property A"),
     EO_OP_DESCRIPTION_CONST(SIMPLE_SUB_ID_A_GET, "i", "Get property A"),
     EO_OP_DESCRIPTION(SIMPLE_SUB_ID_B_SET, "i", "Set property B"),
     EO_OP_DESCRIPTION_CONST(SIMPLE_SUB_ID_B_GET, "i", "Get property B"),
     EO_OP_DESCRIPTION_SENTINEL
};

static const Eo_Class_Description class_desc = {
     "Simple",
     EO_CLASS_TYPE_REGULAR,
     EO_CLASS_DESCRIPTION_OPS(&SIMPLE_BASE_ID, op_desc, SIMPLE_SUB_ID_LAST),
     NULL,
     sizeof(Private_Data),
     _constructor,
     _destructor,
     _class_constructor,
     _class_destructor
};

EO_DEFINE_CLASS(simple_class_get, &class_desc, EO_BASE_CLASS,
      MIXIN_CLASS, NULL);

