#include <node.h>

#include "seismogram.h"

namespace demo {

using v8::Array;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Exception;

Persistent<Function> Seismogram::constructor;

Seismogram::Seismogram()
{
}

Seismogram::~Seismogram() {
}


void Seismogram::Init(Local<Object> exports)
{
	Isolate* isolate = exports->GetIsolate();

	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
	tpl->SetClassName(String::NewFromUtf8(isolate, "Seismogram"));
	tpl->InstanceTemplate()->SetInternalFieldCount(4);

	// Prototype
	NODE_SET_PROTOTYPE_METHOD(tpl, "parse", Parse);

	constructor.Reset(isolate, tpl->GetFunction());
	exports->Set(String::NewFromUtf8(isolate, "Seismogram"),
				tpl->GetFunction());

}

void Seismogram::New(const FunctionCallbackInfo<Value>& args)
{
	Isolate* isolate = args.GetIsolate();

	if (args.IsConstructCall()) {
		// Invoked as constructor: `new Seismogram(...)`
		Seismogram* obj = new Seismogram();
		obj->Wrap(args.This());
		args.GetReturnValue().Set(args.This());
	} else {
    	isolate->ThrowException(Exception::TypeError(
        		String::NewFromUtf8(isolate, "Calling a builtin Seismogram constructor without new is forbidden")));
	}
}

void Seismogram::Parse(const FunctionCallbackInfo<Value>& args)
{
	Isolate* isolate = args.GetIsolate();

	Seismogram* obj = ObjectWrap::Unwrap<Seismogram>(args.Holder());

	v8::String::Utf8Value param1(args[0]->ToString());
	std::string path = std::string(*param1);
	obj->Parse(path);

	Local<Object> resObj = Object::New(isolate);
	resObj->Set(String::NewFromUtf8(isolate, "discreteCount"), Number::New(isolate, obj->seismogramImpl.m_iDiscreteCount));
	resObj->Set(String::NewFromUtf8(isolate, "traceCount"), Number::New(isolate, obj->seismogramImpl.m_iTraceCount));

	Local<Array> array;
	int length = obj->seismogramImpl.m_iTraceCount * obj->seismogramImpl.m_iDiscreteCount;
  	array = Array::New(isolate, length);
	for (int i = 0; i < length; i++)
	{
		array->Set(i, Number::New(isolate, obj->seismogramImpl.m_ptrData[i]));
	}
	resObj->Set(String::NewFromUtf8(isolate, "data"), array);
	args.GetReturnValue().Set(resObj);
}

void Seismogram::Parse(std::string path_file)
{
	seismogramImpl.read(path_file);
}

}