// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Demo.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Demo.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace Demo {

namespace {

const ::google::protobuf::Descriptor* Demo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Demo_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_Demo_2eproto() {
  protobuf_AddDesc_Demo_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "Demo.proto");
  GOOGLE_CHECK(file != NULL);
  Demo_descriptor_ = file->message_type(0);
  static const int Demo_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Demo, test_),
  };
  Demo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Demo_descriptor_,
      Demo::default_instance_,
      Demo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Demo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Demo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Demo));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_Demo_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Demo_descriptor_, &Demo::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_Demo_2eproto() {
  delete Demo::default_instance_;
  delete Demo_reflection_;
}

void protobuf_AddDesc_Demo_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\nDemo.proto\022\004Demo\"\024\n\004Demo\022\014\n\004test\030\001 \002(\r", 40);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "Demo.proto", &protobuf_RegisterTypes);
  Demo::default_instance_ = new Demo();
  Demo::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_Demo_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_Demo_2eproto {
  StaticDescriptorInitializer_Demo_2eproto() {
    protobuf_AddDesc_Demo_2eproto();
  }
} static_descriptor_initializer_Demo_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int Demo::kTestFieldNumber;
#endif  // !_MSC_VER

Demo::Demo()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Demo.Demo)
}

void Demo::InitAsDefaultInstance() {
}

Demo::Demo(const Demo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Demo.Demo)
}

void Demo::SharedCtor() {
  _cached_size_ = 0;
  test_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Demo::~Demo() {
  // @@protoc_insertion_point(destructor:Demo.Demo)
  SharedDtor();
}

void Demo::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Demo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Demo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Demo_descriptor_;
}

const Demo& Demo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_Demo_2eproto();
  return *default_instance_;
}

Demo* Demo::default_instance_ = NULL;

Demo* Demo::New() const {
  return new Demo;
}

void Demo::Clear() {
  test_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Demo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Demo.Demo)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required uint32 test = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &test_)));
          set_has_test();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Demo.Demo)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Demo.Demo)
  return false;
#undef DO_
}

void Demo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Demo.Demo)
  // required uint32 test = 1;
  if (has_test()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->test(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Demo.Demo)
}

::google::protobuf::uint8* Demo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:Demo.Demo)
  // required uint32 test = 1;
  if (has_test()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->test(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Demo.Demo)
  return target;
}

int Demo::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required uint32 test = 1;
    if (has_test()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->test());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Demo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Demo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Demo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Demo::MergeFrom(const Demo& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_test()) {
      set_test(from.test());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Demo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Demo::CopyFrom(const Demo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Demo::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void Demo::Swap(Demo* other) {
  if (other != this) {
    std::swap(test_, other->test_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Demo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Demo_descriptor_;
  metadata.reflection = Demo_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Demo

// @@protoc_insertion_point(global_scope)
