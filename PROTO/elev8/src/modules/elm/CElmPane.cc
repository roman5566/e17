#include "elm.h"
#include "CElmPane.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmPane, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmPane, left);
GENERATE_PROPERTY_CALLBACKS(CElmPane, right);

GENERATE_TEMPLATE(CElmPane,
                  PROPERTY(horizontal),
                  PROPERTY(left),
                  PROPERTY(right));

CElmPane::CElmPane(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_panes_add(elm_object_top_widget_get(parent->GetEvasObject())))
{
}

CElmPane::~CElmPane()
{
   cached.left.Dispose();
   cached.right.Dispose();
}

void CElmPane::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Pane"), GetTemplate()->GetFunction());
}

Handle<Value> CElmPane::horizontal_get() const
{
   return Number::New(elm_panes_horizontal_get(eo));
}

void CElmPane::horizontal_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_panes_horizontal_set(eo, val->BooleanValue());
}

Handle<Value> CElmPane::left_get() const
{
   return cached.left;
}

void CElmPane::left_set(Handle<Value> val)
{
   cached.left.Dispose();
   cached.left = Persistent<Value>::New(Realise(val, jsObject));
   elm_object_part_content_set(eo, "elm.swallow.left",
                               GetEvasObjectFromJavascript(cached.left));
}

Handle<Value> CElmPane::right_get() const
{
   return cached.right;
}

void CElmPane::right_set(Handle<Value> val)
{
   cached.right.Dispose();
   cached.right = Persistent<Value>::New(Realise(val, jsObject));
   elm_object_part_content_set(eo, "elm.swallow.right",
                               GetEvasObjectFromJavascript(cached.right));
}

}

