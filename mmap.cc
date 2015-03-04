#include <node.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

using namespace v8;

v8::Handle<v8::Value> Mmap(const v8::Arguments& args) {
  v8::HandleScope handle_scope;

  if (args.Length() != 5)
  {
    return v8::ThrowException(
      v8::Exception::Error(
        v8::String::New("mmap requires 5 arguments (length, protection, flags, fd, offset)")));
  }

  const size_t length    = args[0]->ToInteger()->Value();
  const int protection = args[1]->ToInteger()->Value();
  const int flags      = args[2]->ToInteger()->Value();
  const int fd         = args[3]->ToInteger()->Value();
  const int offset   = args[4]->ToInteger()->Value();
  
  printf("mmap.cc %lu %d %d %d %d\n", length, protection, flags, fd, offset);

  void* map = mmap(NULL, length, protection, flags, fd, offset);
  if (map == NULL)
  {
    return v8::ThrowException(node::ErrnoException(errno, "mmap", ""));
  }
  return Number::New((long)map);
}

v8::Handle<v8::Value> Msync(const v8::Arguments& args) {
  v8::HandleScope handle_scope;
  
  if (args.Length() != 3)
  {
    return v8::ThrowException(
      v8::Exception::Error(
        v8::String::New("msync requires 3 arguments (addr, length, flags)")));
  }

  void* map              = (void*)args[0]->ToInteger()->Value();  
  const size_t length    = args[1]->ToInteger()->Value();
  const int flags        = args[2]->ToInteger()->Value();
  
  return Number::New(msync(map, length, flags));
}

v8::Handle<v8::Value> Munmap(const v8::Arguments& args) {
  v8::HandleScope handle_scope;
  
  if (args.Length() != 2)
  {
    return v8::ThrowException(
      v8::Exception::Error(
        v8::String::New("munmap requires 2 arguments (addr, length)")));
  }
  
  void* map              = (void*)args[0]->ToInteger()->Value();  
  const size_t length    = args[1]->ToInteger()->Value();
  
  return Number::New(munmap(map, length));
}

void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "mmap", Mmap);
  NODE_SET_METHOD(exports, "msync", Msync);
  NODE_SET_METHOD(exports, "munmap", Munmap);
  
  const v8::PropertyAttribute attribs = (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete);
  exports->Set(v8::String::New("PROT_READ"), v8::Integer::New(PROT_READ), attribs);
  exports->Set(v8::String::New("PROT_WRITE"), v8::Integer::New(PROT_WRITE), attribs);
  exports->Set(v8::String::New("PROT_EXEC"), v8::Integer::New(PROT_EXEC), attribs);
  exports->Set(v8::String::New("PROT_NONE"), v8::Integer::New(PROT_NONE), attribs);
  exports->Set(v8::String::New("MAP_SHARED"), v8::Integer::New(MAP_SHARED), attribs);
  exports->Set(v8::String::New("MAP_PRIVATE"), v8::Integer::New(MAP_PRIVATE), attribs);
  exports->Set(v8::String::New("PAGESIZE"), v8::Integer::New(sysconf(_SC_PAGESIZE)), attribs);
  exports->Set(v8::String::New("MS_ASYNC"), v8::Integer::New(MS_ASYNC), attribs);
  exports->Set(v8::String::New("MS_SYNC"), v8::Integer::New(MS_SYNC), attribs);
  exports->Set(v8::String::New("MS_INVALIDATE"), v8::Integer::New(MS_INVALIDATE), attribs);
}

NODE_MODULE(mmap, init)
