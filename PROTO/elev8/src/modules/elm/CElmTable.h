#ifndef C_ELM_TABLE_H
#define C_ELM_TABLE_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmTable : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmTable(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   virtual void DidRealiseElement(Local<Value>);

   void homogeneous_set(Handle<Value> val);
   Handle<Value> homogeneous_get() const;

   void padding_set(Handle<Value> val);
   Handle<Value> padding_get() const;

   void pack(Handle<Object> obj);
   Handle<Value> pack(const Arguments&);

   Handle<Value> unpack(const Arguments&);

   Handle<Value> clear(const Arguments&);

   friend Handle<Value> CElmObject::New<CElmTable>(const Arguments& args);
};

}
#endif
