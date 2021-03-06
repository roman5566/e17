#include "elm.h"
#include "CElmBackground.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmBackground, image);
GENERATE_PROPERTY_CALLBACKS(CElmBackground, red);
GENERATE_PROPERTY_CALLBACKS(CElmBackground, green);
GENERATE_PROPERTY_CALLBACKS(CElmBackground, blue);

GENERATE_TEMPLATE(CElmBackground,
                  PROPERTY(image),
                  PROPERTY(red),
                  PROPERTY(green),
                  PROPERTY(blue));

CElmBackground::CElmBackground(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_bg_add(parent->GetEvasObject()))
{
}

void CElmBackground::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Background"),
               GetTemplate()->GetFunction());
}

void CElmBackground::image_set(Handle<Value> val)
{
   if (val->IsString())
      elm_bg_file_set(eo, *String::Utf8Value(val), NULL);
}

Handle<Value> CElmBackground::image_get(void) const
{
   const char *file = NULL;
   elm_bg_file_get(eo, &file, NULL);

   if (file)
     return String::New(file);

   return Null();
}

Handle<Value> CElmBackground::red_get() const
{
   int r;
   elm_bg_color_get(eo, &r, NULL, NULL);
   return Number::New(r);
}

void CElmBackground::red_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int g, b;
   elm_bg_color_get(eo, NULL, &g, &b);
   elm_bg_color_set(eo, val->ToNumber()->Value(), g, b);
}

Handle<Value> CElmBackground::green_get() const
{
   int g;
   elm_bg_color_get(eo, NULL, &g, NULL);
   return Number::New(g);
}

void CElmBackground::green_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int r, b;
   elm_bg_color_get(eo, &r, NULL, &b);
   elm_bg_color_set(eo, r, val->ToNumber()->Value(), b);
}

Handle<Value> CElmBackground::blue_get() const
{
   int b;
   elm_bg_color_get(eo, NULL, NULL, &b);
   return Number::New(b);
}

void CElmBackground::blue_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int r, g;
   elm_bg_color_get(eo, &r, &g, NULL);
   elm_bg_color_set(eo, r, g, val->ToNumber()->Value());
}

}
