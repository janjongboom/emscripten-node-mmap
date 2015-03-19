#include <node.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <node_buffer.h>
#include <v8.h>
#include <cstdlib>
#include <nan.h>

#define define_getter_setter(T, name, parseValue) \
NAN_METHOD(get_##name) { \
  NanScope(); \
  if (args.Length() != 2) { NanThrowError("method requires 2 argument (address, index)"); } \
  T* data = (T*)std::strtoull(*v8::String::Utf8Value(args[0]), NULL, 16); \
  NanReturnValue(NanNew<Number>(static_cast<T>(data[args[1]->ToInteger()->Value()]))); \
} \
\
NAN_METHOD(set_##name) { \
  NanScope(); \
  if (args.Length() != 3) { NanThrowError("method requires 3 argument (address, index, value)"); } \
  T* data = (T*)std::strtoull(*v8::String::Utf8Value(args[0]), NULL, 16); \
  data[args[1]->ToInteger()->Value()] = static_cast<T>(args[2]->To##parseValue()->Value()); \
}

using namespace v8;

/**
 * Allocation (mmap, msync, munmap) methods
 */
NAN_METHOD(Mmap) {
  NanScope();

  if (args.Length() != 5)
  {
    NanThrowError("mmap requires 5 arguments (length, protection, flags, fd, offset)");
  }

  const size_t length  = args[0]->ToInteger()->Value();
  const int protection = args[1]->ToInteger()->Value();
  const int flags      = args[2]->ToInteger()->Value();
  const int fd         = args[3]->ToInteger()->Value();
  const int offset     = args[4]->ToInteger()->Value();

  char* map = (char*)mmap(NULL, length, protection, flags, fd, offset);
  if (map == MAP_FAILED)
  {
    NanThrowError("Could not initialize map");
  }
  
  char ptrStringBuffer[20];
  sprintf(ptrStringBuffer, "%p", map);

  NanReturnValue(NanNew<String>(ptrStringBuffer));
}

NAN_METHOD(Msync) {
  NanScope();

  if (args.Length() != 3)
  {
    NanThrowError("msync requires 3 arguments (addr, length, flags)");
  }

  unsigned long long mapPtr  = std::strtoull(*v8::String::Utf8Value(args[0]), NULL, 16);
  void* map                  = (void*)mapPtr;
  size_t        len          = args[1]->ToInteger()->Value();
  const int flags            = args[2]->ToInteger()->Value();

  NanReturnValue(NanNew<Number>(msync(map, len, flags)));
}

NAN_METHOD(Munmap) {
  NanScope();

  if (args.Length() != 2)
  {
    NanThrowError("munmap requires 2 arguments (addr, length)");
  }

  unsigned long long mapPtr = std::strtoull(*v8::String::Utf8Value(args[0]), NULL, 16);
  void* map                 = (void*)mapPtr;
  const size_t length       = args[1]->ToInteger()->Value();

  NanReturnValue(NanNew<Number>(munmap(map, length)));
}

/**
 * Reading values from the map
 */
define_getter_setter(int8_t, int8_t, Integer);
define_getter_setter(int16_t, int16_t, Integer);
define_getter_setter(int32_t, int32_t, Integer);
define_getter_setter(uint8_t, uint8_t, Integer);
define_getter_setter(uint16_t, uint16_t, Integer);
define_getter_setter(uint32_t, uint32_t, Integer);
define_getter_setter(float, float, Number);
define_getter_setter(double, double, Number);

/**
 * Module initialization
 */
void init(Handle<Object> exports) {
  exports->Set(NanNew<String>("mmap"), NanNew<FunctionTemplate>(Mmap)->GetFunction());
  exports->Set(NanNew<String>("msync"), NanNew<FunctionTemplate>(Msync)->GetFunction());
  exports->Set(NanNew<String>("munmap"), NanNew<FunctionTemplate>(Munmap)->GetFunction());
  
  exports->Set(NanNew<String>("get_int8_t"), NanNew<FunctionTemplate>(get_int8_t)->GetFunction());
  exports->Set(NanNew<String>("get_int16_t"), NanNew<FunctionTemplate>(get_int16_t)->GetFunction());
  exports->Set(NanNew<String>("get_int32_t"), NanNew<FunctionTemplate>(get_int32_t)->GetFunction());
  exports->Set(NanNew<String>("get_uint8_t"), NanNew<FunctionTemplate>(get_uint8_t)->GetFunction());
  exports->Set(NanNew<String>("get_uint16_t"), NanNew<FunctionTemplate>(get_uint16_t)->GetFunction());
  exports->Set(NanNew<String>("get_uint32_t"), NanNew<FunctionTemplate>(get_uint32_t)->GetFunction());
  exports->Set(NanNew<String>("get_float"), NanNew<FunctionTemplate>(get_float)->GetFunction());
  exports->Set(NanNew<String>("get_double"), NanNew<FunctionTemplate>(get_double)->GetFunction());
  
  exports->Set(NanNew<String>("set_int8_t"), NanNew<FunctionTemplate>(set_int8_t)->GetFunction());
  exports->Set(NanNew<String>("set_int16_t"), NanNew<FunctionTemplate>(set_int16_t)->GetFunction());
  exports->Set(NanNew<String>("set_int32_t"), NanNew<FunctionTemplate>(set_int32_t)->GetFunction());
  exports->Set(NanNew<String>("set_uint8_t"), NanNew<FunctionTemplate>(set_uint8_t)->GetFunction());
  exports->Set(NanNew<String>("set_uint16_t"), NanNew<FunctionTemplate>(set_uint16_t)->GetFunction());
  exports->Set(NanNew<String>("set_uint32_t"), NanNew<FunctionTemplate>(set_uint32_t)->GetFunction());
  exports->Set(NanNew<String>("set_float"), NanNew<FunctionTemplate>(set_float)->GetFunction());
  exports->Set(NanNew<String>("set_double"), NanNew<FunctionTemplate>(set_double)->GetFunction());
  
  // const v8::PropertyAttribute attribs = (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete);
  // I don't know how to get this f&* attribs on properties now in new native modules
  exports->Set(NanNew<v8::String>("PROT_READ"), NanNew<Number>(PROT_READ));
  exports->Set(NanNew<v8::String>("PROT_WRITE"), NanNew<Number>(PROT_WRITE));
  exports->Set(NanNew<v8::String>("PROT_EXEC"), NanNew<Number>(PROT_EXEC));
  exports->Set(NanNew<v8::String>("PROT_NONE"), NanNew<Number>(PROT_NONE));
  exports->Set(NanNew<v8::String>("MAP_SHARED"), NanNew<Number>(MAP_SHARED));
  exports->Set(NanNew<v8::String>("MAP_PRIVATE"), NanNew<Number>(MAP_PRIVATE));
  exports->Set(NanNew<v8::String>("PAGESIZE"), NanNew<Number>(sysconf(_SC_PAGESIZE)));
  exports->Set(NanNew<v8::String>("MS_ASYNC"), NanNew<Number>(MS_ASYNC));
  exports->Set(NanNew<v8::String>("MS_SYNC"), NanNew<Number>(MS_SYNC));
  exports->Set(NanNew<v8::String>("MS_INVALIDATE"), NanNew<Number>(MS_INVALIDATE));
}

NODE_MODULE(mmap, init)
