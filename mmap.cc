#include <node.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <node_buffer.h>
#include <v8.h>
#include <cstdlib>
#include <nan.h>

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
NAN_METHOD(geti8) {
  NanScope();
  printf("yo geti8 %s\n", *v8::String::Utf8Value(args[0]));
  char* data = (char*)std::strtoull(*v8::String::Utf8Value(args[0]), NULL, 16);
  printf("geti8 %p %d\n", data, data[0]);
  NanReturnValue(NanNew<Number>(data[0]));
}

NAN_METHOD(geti16) {
  NanScope();
  uint16_t* data = (uint16_t*)std::strtoull(*v8::String::Utf8Value(args[0]), NULL, 16);
  NanReturnValue(NanNew<Number>(data[0]));
}

NAN_METHOD(geti32) {
  NanScope();
  uint32_t* data = (uint32_t*)std::strtoull(*v8::String::Utf8Value(args[0]), NULL, 16);
  NanReturnValue(NanNew<Number>(data[0]));
}

  // Module['HEAP8']   = HEAP8   = createNewProxy(RAWHEAP8, 1, 'geti8');
  // Module['HEAP16']  = HEAP16  = createNewProxy(RAWHEAP16, 2, 'geti16');
  // Module['HEAP32']  = HEAP32  = createNewProxy(RAWHEAP32, 4, 'geti32');
  // Module['HEAPU8']  = HEAPU8  = createNewProxy(RAWHEAPU8, 1, 'getu8');
  // Module['HEAPU16'] = HEAPU16 = createNewProxy(RAWHEAPU16, 2, 'getu16');
  // Module['HEAPU32'] = HEAPU32 = createNewProxy(RAWHEAPU32, 4, 'getu32');
  // Module['HEAPF32'] = HEAPF32 = createNewProxy(RAWHEAPF32, 4, 'getf32');
  // Module['HEAPF64'] = HEAPF64 = createNewProxy(RAWHEAPF64, 8, 'getf64'); 

/**
 * Module initialization
 */
void init(Handle<Object> exports) {
  exports->Set(NanNew<String>("mmap"), NanNew<FunctionTemplate>(Mmap)->GetFunction());
  exports->Set(NanNew<String>("msync"), NanNew<FunctionTemplate>(Msync)->GetFunction());
  exports->Set(NanNew<String>("munmap"), NanNew<FunctionTemplate>(Munmap)->GetFunction());
  
  exports->Set(NanNew<String>("geti8"), NanNew<FunctionTemplate>(geti8)->GetFunction());
  exports->Set(NanNew<String>("geti16"), NanNew<FunctionTemplate>(geti16)->GetFunction());
  exports->Set(NanNew<String>("geti32"), NanNew<FunctionTemplate>(geti32)->GetFunction());
  
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
