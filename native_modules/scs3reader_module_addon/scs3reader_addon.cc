#include <node.h>

#include "seismogram.h"

namespace demo {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

using v8::Array;
using v8::ArrayBuffer;
using v8::Float32Array;
using v8::Number;

void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
}

void Parse(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  Local<Object> obj = Object::New(isolate);
  obj->Set(String::NewFromUtf8(isolate, "url"), args[0]->ToString());

  float arrayNative[3] = {9999990, 0.1, 0.5};
  int length = sizeof(arrayNative) / sizeof(arrayNative[0]);
  Local<Array> array;
  array = Array::New(isolate, length);
  for (int i = 0; i < length; i++)
  {
    array->Set(i, Number::New(isolate, arrayNative[i]));
  }

  obj->Set(String::NewFromUtf8(isolate, "array"), array);

  args.GetReturnValue().Set(obj);
}

void init(Local<Object> exports, Local<Object> module) {
  // NODE_SET_METHOD(exports, "hello", Method);
  // NODE_SET_METHOD(exports, "parse", Parse);
  Seismogram::Init(exports);
}

NODE_MODULE(addon, init)

}  // namespace demo
