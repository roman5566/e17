#ifndef MIXIN2_H
#define MIXIN2_H

#include "Eo.h"

typedef struct
{
   int count;
} Mixin2_Public_Data;

#define MIXIN2_CLASS mixin2_class_get()
const Eo_Class *mixin2_class_get(void) EINA_CONST;

#endif
