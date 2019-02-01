// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Demo.proto

#ifndef PROTOBUF_Demo_2eproto__INCLUDED
#define PROTOBUF_Demo_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace Demo {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_Demo_2eproto();
void protobuf_AssignDesc_Demo_2eproto();
void protobuf_ShutdownFile_Demo_2eproto();

class Demo;

// ===================================================================

class Demo : public ::google::protobuf::Message {
 public:
  Demo();
  virtual ~Demo();

  Demo(const Demo& from);

  inline Demo& operator=(const Demo& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Demo& default_instance();

  void Swap(Demo* other);

  // implements Message ----------------------------------------------

  Demo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Demo& from);
  void MergeFrom(const Demo& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint32 test = 1;
  inline bool has_test() const;
  inline void clear_test();
  static const int kTestFieldNumber = 1;
  inline ::google::protobuf::uint32 test() const;
  inline void set_test(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:Demo.Demo)
 private:
  inline void set_has_test();
  inline void clear_has_test();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 test_;
  friend void  protobuf_AddDesc_Demo_2eproto();
  friend void protobuf_AssignDesc_Demo_2eproto();
  friend void protobuf_ShutdownFile_Demo_2eproto();

  void InitAsDefaultInstance();
  static Demo* default_instance_;
};
// ===================================================================


// ===================================================================

// Demo

// required uint32 test = 1;
inline bool Demo::has_test() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Demo::set_has_test() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Demo::clear_has_test() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Demo::clear_test() {
  test_ = 0u;
  clear_has_test();
}
inline ::google::protobuf::uint32 Demo::test() const {
  // @@protoc_insertion_point(field_get:Demo.Demo.test)
  return test_;
}
inline void Demo::set_test(::google::protobuf::uint32 value) {
  set_has_test();
  test_ = value;
  // @@protoc_insertion_point(field_set:Demo.Demo.test)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Demo

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Demo_2eproto__INCLUDED
