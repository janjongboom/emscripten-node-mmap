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

static void Map_finalise(char *data, void*hint)
{
	munmap(data, (size_t)hint);
}

NAN_METHOD(Mmap) {
  NanScope();

  if (args.Length() != 5)
  {
    NanThrowError("mmap requires 5 arguments (length, protection, flags, fd, offset)");
  }

  const size_t length    = args[0]->ToInteger()->Value();
  const int protection = args[1]->ToInteger()->Value();
  const int flags      = args[2]->ToInteger()->Value();
  const int fd         = args[3]->ToInteger()->Value();
  const int offset   = args[4]->ToInteger()->Value();

  char* map = (char*)mmap(NULL, length, protection, flags, fd, offset);
  if (map == MAP_FAILED)
  {
    NanThrowError("Could not initialize map");
  }
  
  Local<Object> slowBuffer = node::Buffer::New(map, length, Map_finalise, (void*)length);
  
  char ptrStringBuffer[20];
  sprintf(ptrStringBuffer, "%p", map);

  slowBuffer->ForceSet(NanNew("ptr"), NanNew<v8::String>(ptrStringBuffer));

  NanReturnValue(slowBuffer);
}

NAN_METHOD(Msync) {
  NanScope();

  if (args.Length() != 3)
  {
    NanThrowError("msync requires 3 arguments (addr, buffer, flags)");
  }

  unsigned long long mapPtr  = std::strtoull(*v8::String::Utf8Value(args[0]), NULL, 16);
  char* map                  = (char*)mapPtr;
  printf("msync %d %d %d!\n", map[12], map[13], map[14]);
  Local<Object> bufferObj    = args[1]->ToObject();
  char*         data         = node::Buffer::Data(bufferObj);
  size_t        len          = node::Buffer::Length(bufferObj);
  const int flags            = args[2]->ToInteger()->Value();

  for (size_t i = 0; i < len; i++) {
    map[i] = data[i];
  }

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

void init(Handle<Object> exports) {
  exports->Set(NanNew<String>("mmap"), NanNew<FunctionTemplate>(Mmap)->GetFunction());
  exports->Set(NanNew<String>("msync"), NanNew<FunctionTemplate>(Msync)->GetFunction());
  exports->Set(NanNew<String>("munmap"), NanNew<FunctionTemplate>(Munmap)->GetFunction());
  
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
