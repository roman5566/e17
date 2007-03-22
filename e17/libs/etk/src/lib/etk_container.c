/** @file etk_container.c */
#include "etk_container.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Container
 * @{
 */

enum Etk_Container_Signal_Id
{
   ETK_CONTAINER_CHILD_ADDED_SIGNAL,
   ETK_CONTAINER_CHILD_REMOVED_SIGNAL,
   ETK_CONTAINER_NUM_SIGNALS
};

enum Etk_Container_Property_Id
{
   ETK_CONTAINER_BORDER_WIDTH_PROPERTY
};

static void _etk_container_constructor(Etk_Container *container);
static void _etk_container_destructor(Etk_Container *container);
static void _etk_container_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_container_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static Etk_Signal *_etk_container_signals[ETK_CONTAINER_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Container
 * @return Returns the type of an Etk_Container
 */
Etk_Type *etk_container_type_get(void)
{
   static Etk_Type *container_type = NULL;

   if (!container_type)
   {
      container_type = etk_type_new("Etk_Container", ETK_WIDGET_TYPE, sizeof(Etk_Container),
            ETK_CONSTRUCTOR(_etk_container_constructor), ETK_DESTRUCTOR(_etk_container_destructor));
   
      _etk_container_signals[ETK_CONTAINER_CHILD_ADDED_SIGNAL] = etk_signal_new("child-added",
            container_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_container_signals[ETK_CONTAINER_CHILD_REMOVED_SIGNAL] = etk_signal_new("child-removed",
            container_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);

      etk_type_property_add(container_type, "border-width", ETK_CONTAINER_BORDER_WIDTH_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
   
      container_type->property_set = _etk_container_property_set;
      container_type->property_get = _etk_container_property_get;
   }

   return container_type;
}

/**
 * @brief Adds a child to the container. It simply calls the "child_add()" method of the container
 * @param container a container
 * @param widget the widget to add
 */
void etk_container_add(Etk_Container *container, Etk_Widget *widget)
{
   if (!container || !widget || !container->child_add)
      return;
   container->child_add(container, widget);
}

/**
 * @brief Removes a child from the container It simply calls the "child_remove()" method of the container.
 * The child is not destroyed, it is just unpacked
 * @param container a container
 * @param widget the widget to remove
 */
void etk_container_remove(Etk_Container *container, Etk_Widget *widget)
{
   if (!container || !widget || !container->child_remove)
      return;
   container->child_remove(container, widget);
}

/**
 * @brief Unpacks all the children of the container
 * @param container a container
 */
void etk_container_remove_all(Etk_Container *container)
{
   Evas_List *children, *l;
   
   if (!container)
      return;
   
   children = etk_container_children_get(container);
   for (l = children; l; l = l->next)
      etk_container_remove(container, ETK_WIDGET(l->data));
   evas_list_free(children);
}

/**
 * @brief Sets the border width of a container. The border width is the amount of space left around the *inside* of
 * the container. To add free space around the outside of a container, you can use etk_widget_padding_set()
 * @param container a container
 * @param border_width the border width to set
 * @see etk_widget_padding_set()
 */
void etk_container_border_width_set(Etk_Container *container, int border_width)
{
   if (!container)
      return;

   container->border_width = border_width;
   etk_widget_size_recalc_queue(ETK_WIDGET(container));
   etk_object_notify(ETK_OBJECT(container), "border-width");
}

/**
 * @brief Gets the border width of the container
 * @param container a container
 * @return Returns the border width of the container
 */
int etk_container_border_width_get(Etk_Container *container)
{
   if (!container)
      return 0;
   return container->border_width;
}

/**
 * @brief Gets the list of the children of the container.
 * It simply calls the "childrend_get()" method of the container. @n
 * The list will have to be freed with evas_list_free() when you no longer need it
 * @param container a container
 * @return Returns the list of the container's children
 * @warning The returned list has to be freed with evas_list_free()
 */
Evas_List *etk_container_children_get(Etk_Container *container)
{
   if (!container || !container->children_get)
      return NULL;
   return container->children_get(container);
}

/**
 * @brief Gets whether the widget is a child of the container
 * @param container a container
 * @param widget the widget you want to check if it is a child of the container
 * @return Returns ETK_TRUE if the widget is a child of the container, ETK_FALSE otherwise
 */
Etk_Bool etk_container_is_child(Etk_Container *container, Etk_Widget *widget)
{
   Evas_List *children;
   Etk_Bool is_child;
   
   if (!container || !widget)
      return ETK_FALSE;
   
   children = etk_container_children_get(container);
   is_child = (evas_list_find(children, widget) != NULL);
   evas_list_free(children);
   
   return is_child;
}

/**
 * @brief Calls @a for_each_cb(child) for each child of the container
 * @param container the container
 * @param for_each_cb the function to call
 */
void etk_container_for_each(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child))
{
   Evas_List *children, *l;

   if (!container || !for_each_cb)
      return;

   children = etk_container_children_get(container);
   for (l = children; l; l = l->next)
      for_each_cb(ETK_WIDGET(l->data));
   evas_list_free(children);
}

/**
 * @brief Calls @a for_each_cb(child, data) for each child of the container
 * @param container the container
 * @param for_each_cb the function to call
 * @param data the data to pass as the second argument of @a for_each_cb()
 */
void etk_container_for_each_data(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child, void *data), void *data)
{
   Evas_List *children, *l;

   if (!container || !for_each_cb)
      return;

   children = etk_container_children_get(container);
   for (l = children; l; l = l->next)
      for_each_cb(ETK_WIDGET(l->data), data);
   evas_list_free(children);
}

/**
 * @brief A utility function that resizes the given space according to the specified fill-policy.
 * It is mainly used by container implementations
 * @param child a child
 * @param child_space the allocated space for the child. It will be modified according to the fill options
 * @param hfill if @a hfill == ETK_TRUE, the child will fill the space horizontally
 * @param vfill if @a vfill == ETK_TRUE, the child will fill the space vertically
 * @param xalign the horizontal alignment of the child widget in the child space (has no effect if @a hfill is ETK_TRUE)
 * @param yalign the vertical alignment of the child widget in the child space (has no effect if @a vfill is ETK_TRUE)
 */
void etk_container_child_space_fill(Etk_Widget *child, Etk_Geometry *child_space, Etk_Bool hfill, Etk_Bool vfill, float xalign, float yalign)
{
   Etk_Size min_size;

   if (!child || !child_space)
      return;
   
   xalign = ETK_CLAMP(xalign, 0.0, 1.0);
   yalign = ETK_CLAMP(yalign, 0.0, 1.0);

   etk_widget_size_request(child, &min_size);
   if (!hfill && child_space->w > min_size.w)
   {
      child_space->x += (child_space->w - min_size.w) * xalign;
      child_space->w = min_size.w;
   }
   if (!vfill && child_space->h > min_size.h)
   {
      child_space->y += (child_space->h - min_size.h) * yalign;
      child_space->h = min_size.h;
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the container */
static void _etk_container_constructor(Etk_Container *container)
{
   if (!container)
      return;

   container->child_add = NULL;
   container->child_remove = NULL;
   container->children_get = NULL;
   container->border_width = 0;
}

/* Destroys the container */
static void _etk_container_destructor(Etk_Container *container)
{
   if (!container)
      return;
   
   /* We need to do that because Etk_Widget's destructor may
    * still want to access those methods (TODO: not when parent_set will be fixed...) */
   container->child_add = NULL;
   container->child_remove = NULL;
   container->children_get = NULL;
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_container_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Container *container;

   if (!(container = ETK_CONTAINER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CONTAINER_BORDER_WIDTH_PROPERTY:
         etk_container_border_width_set(container, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_container_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Container *container;

   if (!(container = ETK_CONTAINER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CONTAINER_BORDER_WIDTH_PROPERTY:
         etk_property_value_int_set(value, etk_container_border_width_get(container));
         break;
      default:
         break;
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Container
 *
 * Etk_Container is an abstract class which allows the user to add or remove children to an inheriting widget. @n @n
 * etk_container_add() calls the @a child_add() method of the inheriting container, such as etk_bin_child_set()
 * for Etk_Bin, or etk_box_append() for Etk_Box. But, you will often have to call directly a function of the API
 * of the inheriting widget, in order to add the child at a specific place. For example, you'll have to call directly
 * etk_box_append() with the ETK_BOX_END parameter to pack a child at the end of a box (since etk_container_add() packs
 * the child at the start of the box by default). @n
 * etk_container_remove() calls the @a child_remove() method of the inheriting container, which will remove the child
 * from the container. @n @n
 * You can also get the list of the children of the container with etk_container_children_get(). @n
 *
 * Note that when a container is destroyed, all its children are automatically destroyed too. If you want to avoid that,
 * before destroying the container, you can call etk_container_remove_all().
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *
 * \par Signals:
 * @signal_name "child-added": Emitted when a child has been added to the container
 * @signal_cb void callback(Etk_Container *container, Etk_Widget *child, void *data)
 * @signal_arg container: the container connected to the callback
 * @signal_arg child: the child which has been added
 * @signal_data
 * \par
 * @signal_name "child-removed": Emitted when a child has been removed from the container
 * @signal_cb void callback(Etk_Container *container, Etk_Widget *child, void *data)
 * @signal_arg container: the container connected to the callback
 * @signal_arg child: the child which has been removed
 * @signal_data
 *
 * \par Properties:
 * @prop_name "border-width": The amount of space left around the inside of the container
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 */
