#ifndef EKBD_H_
#define EKBD_H_

#include <Evas.h>


typedef enum _Ekbd_Layout_Type
{
   EKBD_TYPE_UNKNOWN = 0,
   EKBD_TYPE_ALPHA = (1 << 0),
   EKBD_TYPE_NUMERIC = (1 << 1),
   EKBD_TYPE_PIN = (1 << 2),
   EKBD_TYPE_PHONE_NUMBER = (1 << 3),
   EKBD_TYPE_HEX = (1 << 4),
   EKBD_TYPE_TERMINAL = (1 << 5),
   EKBD_TYPE_PASSWORD = (1 << 6),
   EKBD_TYPE_IP = (1 << 7),
   EKBD_TYPE_HOST = (1 << 8),
   EKBD_TYPE_FILE = (1 << 9),
   EKBD_TYPE_URL = (1 << 10),
   EKBD_TYPE_KEYPAD = (1 << 11),
   EKBD_TYPE_J2ME = (1 << 12)
} Ekbd_Layout_Type;


 /* The natural text direction of the keyboard */
typedef enum _Ekbd_Direction
{
   EKBD_DIRECTION_LTR = (1 << 0),
   EKBD_DIRECTION_RTL = (1 << 1)
} Ekbd_Direction;

typedef struct _Ekbd_Layout Ekbd_Layout;

EAPI int              ekbd_init(void);
EAPI int              ekbd_shutdown(void);
EAPI Evas_Object *    ekbd_object_add(Evas *evas);
EAPI void             ekbd_object_layout_add(Evas_Object *obj, const char *path);
EAPI void             ekbd_object_layout_clear(Evas_Object *obj);
EAPI void             ekbd_object_layout_select(Evas_Object *obj, Ekbd_Layout *layout);
EAPI const Eina_List *ekbd_object_layout_get(const Evas_Object *obj);

EAPI const char *     ekbd_layout_file_get(const Ekbd_Layout *layout);
EAPI const char *     ekbd_layout_path_get(const Ekbd_Layout *layout);
EAPI const char *     ekbd_layout_icon_get(const Ekbd_Layout *layout);
EAPI Ekbd_Layout_Type        ekbd_layout_type_get(const Ekbd_Layout *layout);

#endif /* EKBD_H_ */