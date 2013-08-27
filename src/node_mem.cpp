// v8
#include <v8.h>

// node
#include <node.h>
#include <node_version.h>
#include <node_object_wrap.h>
#include <node_buffer.h>

// stl
#include <iostream>
#include <exception>
#include <string>

#include "pbf.hpp"
#include "nan.h"
#include "index.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "capnproto_helper.hpp"

// https://github.com/jasondelponte/go-v8/blob/master/src/v8context.cc#L41
// http://v8.googlecode.com/svn/trunk/test/cctest/test-threads.cc

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

namespace node_mem {

using namespace v8;

class Cache: public node::ObjectWrap {
public:
    static Persistent<FunctionTemplate> constructor;
    static void Initialize(Handle<Object> target);
    static NAN_METHOD(New);
    static NAN_METHOD(parseProto);
    static NAN_METHOD(parseCapnProto);
    static void AsyncRun(uv_work_t* req);
    static void AfterRun(uv_work_t* req);
    Cache();
    void _ref() { Ref(); }
    void _unref() { Unref(); }
private:
    ~Cache();
};

Persistent<FunctionTemplate> Cache::constructor;

void Cache::Initialize(Handle<Object> target) {
    NanScope();
    Local<FunctionTemplate> t = FunctionTemplate::New(Cache::New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(String::NewSymbol("Cache"));
    NODE_SET_PROTOTYPE_METHOD(t, "parseProto", parseProto);
    NODE_SET_PROTOTYPE_METHOD(t, "parseCapnProto", parseCapnProto);
    target->Set(String::NewSymbol("Cache"),t->GetFunction());
    NanAssignPersistent(FunctionTemplate, constructor, t);
}

Cache::Cache()
  : ObjectWrap()
    { }

Cache::~Cache() { }

NAN_METHOD(Cache::New)
{
    NanScope();
    if (!args.IsConstructCall()) {
        return NanThrowTypeError("Cannot call constructor as function, you need to use 'new' keyword");
    }
    try {
        Cache* im = new Cache();
        im->Wrap(args.This());
        NanReturnValue(args.This());
    } catch (std::exception const& ex) {
        return NanThrowTypeError(ex.what());
    }
    NanReturnValue(Undefined());
}


NAN_METHOD(Cache::parseCapnProto)
{
    NanScope();
    if (args.Length() < 1) {
        return NanThrowTypeError("first argument must be a buffer");
    }

    if (!args[0]->IsObject()) {
        return NanThrowTypeError("first argument must be a buffer");
    }

    Local<Object> obj = args[0]->ToObject();
    if (obj->IsNull() || obj->IsUndefined()) {
        return NanThrowTypeError("a buffer expected for first argument");
    }
    if (!node::Buffer::HasInstance(obj)) {
        return NanThrowTypeError("first argument must be a buffer");
    }
    bool packed = false;
    if (args.Length() == 2) {
        if (!args[1]->IsObject()) {
            return NanThrowTypeError("optional second argument must be an object");
        }
        Local<Object> options = args[1]->ToObject();
        if (options->IsNull() || options->IsUndefined()) {
            return NanThrowTypeError("a valid object expected for second argument");
        }
        if (options->Has(String::New("packed"))) {
            Local<Value> packed_opt = options->Get(String::New("packed"));
            if (!packed_opt->IsBoolean())
                return NanThrowTypeError("optional arg 'packed' must be a boolean");
            packed = packed_opt->BooleanValue();
        }

    }
    Local<Object> json = Object::New();
    try {
        const char * cdata = node::Buffer::Data(obj);
        size_t size = node::Buffer::Length(obj);

        ::capnp::ReaderOptions options;
        ScratchSpace scratch;
        //options.traversalLimitInWords = 8 * 1024 * 1024

        /*
        // slow since this allocates more memory
        ::kj::ArrayPtr<const ::kj::byte> arr_ptr(&cdata[0],size);
        ::kj::ArrayInputStream in(arr_ptr);
        ::capnp::PackedMessageReader reader(in,options,nullptr);
        */

        /*
        //todo - read from raw array unpacked
        ::kj::ArrayPtr<const ::capnp::word> arr_ptr(&cdata[0],size);
        FlatArrayMessageReader reader(arr_ptr);
        */

        // TODO - try readMessageUnchecked on single segment message
        //carmen::Message::Reader msg = ::capnp::readMessageUnchecked<carmen::Message>((const capnp::word *)&cdata[0]);
        
        // packed
        carmen::Message::Reader msg;
        if (packed) {
                BufferStream pipe(cdata,size);
                ::capnp::PackedMessageReader reader(pipe);
                // no faster...
                //::capnp::PackedMessageReader reader(pipe,options,kj::arrayPtr(scratchSpace, SCRATCH_SIZE));
                msg = reader.getRoot<carmen::Message>();
        } else {
                UnBufferedStream pipe(cdata,size,true);
                ::capnp::InputStreamMessageReader reader(pipe,options,kj::arrayPtr(scratchSpace, SCRATCH_SIZE));
                msg = reader.getRoot<carmen::Message>();
        }


        auto items = msg.getItems();
        unsigned items_size = items.size();
        //std::clog << "items: " << items_size << "\n";
        for (unsigned i=0;i<items_size;++i) {
            auto item = items[i];
            auto array = item.getArrays();
            unsigned array_size = array.size();
            //std::clog << "array_size " << array_size << "\n";
            #ifdef CREATE_JS_OBJ
            // Int32Array
            Local<Array> arr_obj = Array::New(array_size);
            #endif
            for (unsigned j=0;j<array_size;++j) {
                auto arr = array[0];
                auto vals = arr.getVal();
                unsigned vals_size = vals.size();
                #ifdef CREATE_JS_OBJ
                Local<Array> vals_obj = Array::New(vals_size);
                #endif
                for (unsigned k=0;k<vals_size;++k) {
                    #ifdef CREATE_JS_OBJ
                    vals_obj->Set(k,Number::New(vals[k]));
                    #endif
                }
                #ifdef CREATE_JS_OBJ
                arr_obj->Set(j,vals_obj);
                #endif
            }
            #ifdef CREATE_JS_OBJ
            uint64_t num = item.getKey();
            if (num < max_32_int) {
                json->Set(num,arr_obj);
            } else {
                json->Set(Number::New(item.getKey()),arr_obj);
            }
            #endif
        }
    } catch (std::exception const& ex) {
        return NanThrowTypeError(ex.what());
    }
    NanReturnValue(json);
}

NAN_METHOD(Cache::parseProto)
{
    NanScope();
    if (args.Length() < 1) {
        return NanThrowTypeError("first argument must be a buffer");
    }

    if (!args[0]->IsObject()) {
        return NanThrowTypeError("first argument must be a buffer");
    }

    Local<Object> obj = args[0]->ToObject();
    if (obj->IsNull() || obj->IsUndefined()) {
        return NanThrowTypeError("a buffer expected for first argument");
    }
    if (!node::Buffer::HasInstance(obj)) {
        return NanThrowTypeError("first argument must be a buffer");
    }
    Local<Object> json = Object::New();
    try {
        const char * cdata = node::Buffer::Data(obj);
        size_t size = node::Buffer::Length(obj);
        // @TODO - prevent crash on invalid data
        llmr::pbf message(cdata,size);
        while (message.next()) {
            if (message.tag == 1) {
                uint32_t bytes = message.varint();
                //std::clog << "bytes: " << bytes << "\n";
                llmr::pbf item(message.data, bytes);
                #ifdef CREATE_JS_OBJ
                Local<Array> val_array = Array::New();
                #endif
                while (item.next()) {
                    if (item.tag == 1) {
                        uint32_t arrays_length = item.varint();
                        llmr::pbf array(item.data,arrays_length);
                        unsigned idx = 0;
                        while (array.next()) {
                            if (array.tag == 1) {
                                uint32_t vals_length = array.varint();
                                llmr::pbf val(array.data,vals_length);
                                unsigned vidx = 0;
                                #ifdef CREATE_JS_OBJ
                                Local<Array> v2 = Array::New();
                                #endif
                                while (val.next()) {
                                    #ifdef CREATE_JS_OBJ
                                    v2->Set(vidx++,Number::New(val.value));
                                    #endif
                                }
                                #ifdef CREATE_JS_OBJ
                                val_array->Set(idx++,v2);
                                #endif
                                array.skipBytes(vals_length);
                            } else {
                                throw std::runtime_error("skipping when shouldnt");
                                array.skip();
                            }
                        }
                        item.skipBytes(arrays_length);
                    } else if (item.tag == 2) {
                        int64_t val = item.varint();
                        #ifdef CREATE_JS_OBJ
                        json->Set(Number::New(val),val_array);
                        #endif
                    } else {
                        throw std::runtime_error("hit unknown type");
                    }
                }
                message.skipBytes(bytes);
            } else {
                throw std::runtime_error("skipping when shouldnt");
                message.skip();
            }    
        }
    } catch (std::exception const& ex) {
        return NanThrowTypeError(ex.what());
    }
    NanReturnValue(json);
}

extern "C" {
    static void start(Handle<Object> target) {
        Cache::Initialize(target);
    }
}

} // namespace node_mem

NODE_MODULE(mem, node_mem::start)