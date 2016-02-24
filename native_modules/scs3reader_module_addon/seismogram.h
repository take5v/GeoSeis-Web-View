#ifndef SEISMOGRAM_H
#define SEISMOGRAM_H

#include <node.h>
#include <node_object_wrap.h>

#include "seismogramimpl.h"

namespace demo {

class Seismogram : public node::ObjectWrap
{
public:
	static void Init(v8::Local<v8::Object> exports);

private:
	explicit Seismogram();
	~Seismogram();
	
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Parse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static v8::Persistent<v8::Function> constructor;

	void Parse(std::string path_file);

	SeismogramImpl seismogramImpl;
};

}

#endif