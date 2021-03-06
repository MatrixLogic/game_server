// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: UserConfig.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "UserConfig.pb.h"

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

namespace UserCfg {

namespace {

const ::google::protobuf::Descriptor* UserBase_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  UserBase_reflection_ = NULL;
const ::google::protobuf::Descriptor* Build_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Build_reflection_ = NULL;
const ::google::protobuf::Descriptor* User_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  User_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_UserConfig_2eproto() {
  protobuf_AddDesc_UserConfig_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "UserConfig.proto");
  GOOGLE_CHECK(file != NULL);
  UserBase_descriptor_ = file->message_type(0);
  static const int UserBase_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserBase, coin_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserBase, cash_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserBase, level_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserBase, exp_),
  };
  UserBase_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      UserBase_descriptor_,
      UserBase::default_instance_,
      UserBase_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserBase, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(UserBase, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(UserBase));
  Build_descriptor_ = file->message_type(1);
  static const int Build_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Build, buildid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Build, pos_),
  };
  Build_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Build_descriptor_,
      Build::default_instance_,
      Build_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Build, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Build, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Build));
  User_descriptor_ = file->message_type(2);
  static const int User_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(User, user_init_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(User, user_exp_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(User, builds_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(User, initprops_),
  };
  User_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      User_descriptor_,
      User::default_instance_,
      User_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(User, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(User, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(User));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_UserConfig_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    UserBase_descriptor_, &UserBase::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Build_descriptor_, &Build::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    User_descriptor_, &User::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_UserConfig_2eproto() {
  delete UserBase::default_instance_;
  delete UserBase_reflection_;
  delete Build::default_instance_;
  delete Build_reflection_;
  delete User::default_instance_;
  delete User_reflection_;
}

void protobuf_AddDesc_UserConfig_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::CommonGiftConfig::protobuf_AddDesc_CommonGiftConfig_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\020UserConfig.proto\022\007UserCfg\032\026CommonGiftC"
    "onfig.proto\"B\n\010UserBase\022\014\n\004coin\030\001 \002(\r\022\014\n"
    "\004cash\030\002 \002(\r\022\r\n\005level\030\003 \002(\r\022\013\n\003exp\030\004 \002(\r\""
    "%\n\005Build\022\017\n\007buildid\030\001 \002(\r\022\013\n\003pos\030\002 \003(\r\"\225"
    "\001\n\004User\022$\n\tuser_init\030\001 \002(\0132\021.UserCfg.Use"
    "rBase\022\020\n\010user_exp\030\002 \003(\004\022\036\n\006builds\030\003 \003(\0132"
    "\016.UserCfg.Build\0225\n\tinitprops\030\004 \002(\0132\".Com"
    "monGiftConfig.CommonModifyItem", 310);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "UserConfig.proto", &protobuf_RegisterTypes);
  UserBase::default_instance_ = new UserBase();
  Build::default_instance_ = new Build();
  User::default_instance_ = new User();
  UserBase::default_instance_->InitAsDefaultInstance();
  Build::default_instance_->InitAsDefaultInstance();
  User::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_UserConfig_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_UserConfig_2eproto {
  StaticDescriptorInitializer_UserConfig_2eproto() {
    protobuf_AddDesc_UserConfig_2eproto();
  }
} static_descriptor_initializer_UserConfig_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int UserBase::kCoinFieldNumber;
const int UserBase::kCashFieldNumber;
const int UserBase::kLevelFieldNumber;
const int UserBase::kExpFieldNumber;
#endif  // !_MSC_VER

UserBase::UserBase()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:UserCfg.UserBase)
}

void UserBase::InitAsDefaultInstance() {
}

UserBase::UserBase(const UserBase& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:UserCfg.UserBase)
}

void UserBase::SharedCtor() {
  _cached_size_ = 0;
  coin_ = 0u;
  cash_ = 0u;
  level_ = 0u;
  exp_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

UserBase::~UserBase() {
  // @@protoc_insertion_point(destructor:UserCfg.UserBase)
  SharedDtor();
}

void UserBase::SharedDtor() {
  if (this != default_instance_) {
  }
}

void UserBase::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* UserBase::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return UserBase_descriptor_;
}

const UserBase& UserBase::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_UserConfig_2eproto();
  return *default_instance_;
}

UserBase* UserBase::default_instance_ = NULL;

UserBase* UserBase::New() const {
  return new UserBase;
}

void UserBase::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<UserBase*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  ZR_(coin_, exp_);

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool UserBase::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:UserCfg.UserBase)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required uint32 coin = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &coin_)));
          set_has_coin();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_cash;
        break;
      }

      // required uint32 cash = 2;
      case 2: {
        if (tag == 16) {
         parse_cash:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &cash_)));
          set_has_cash();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_level;
        break;
      }

      // required uint32 level = 3;
      case 3: {
        if (tag == 24) {
         parse_level:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &level_)));
          set_has_level();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(32)) goto parse_exp;
        break;
      }

      // required uint32 exp = 4;
      case 4: {
        if (tag == 32) {
         parse_exp:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &exp_)));
          set_has_exp();
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
  // @@protoc_insertion_point(parse_success:UserCfg.UserBase)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:UserCfg.UserBase)
  return false;
#undef DO_
}

void UserBase::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:UserCfg.UserBase)
  // required uint32 coin = 1;
  if (has_coin()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->coin(), output);
  }

  // required uint32 cash = 2;
  if (has_cash()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(2, this->cash(), output);
  }

  // required uint32 level = 3;
  if (has_level()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(3, this->level(), output);
  }

  // required uint32 exp = 4;
  if (has_exp()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(4, this->exp(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:UserCfg.UserBase)
}

::google::protobuf::uint8* UserBase::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:UserCfg.UserBase)
  // required uint32 coin = 1;
  if (has_coin()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->coin(), target);
  }

  // required uint32 cash = 2;
  if (has_cash()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(2, this->cash(), target);
  }

  // required uint32 level = 3;
  if (has_level()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(3, this->level(), target);
  }

  // required uint32 exp = 4;
  if (has_exp()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(4, this->exp(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:UserCfg.UserBase)
  return target;
}

int UserBase::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required uint32 coin = 1;
    if (has_coin()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->coin());
    }

    // required uint32 cash = 2;
    if (has_cash()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->cash());
    }

    // required uint32 level = 3;
    if (has_level()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->level());
    }

    // required uint32 exp = 4;
    if (has_exp()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->exp());
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

void UserBase::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const UserBase* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const UserBase*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void UserBase::MergeFrom(const UserBase& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_coin()) {
      set_coin(from.coin());
    }
    if (from.has_cash()) {
      set_cash(from.cash());
    }
    if (from.has_level()) {
      set_level(from.level());
    }
    if (from.has_exp()) {
      set_exp(from.exp());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void UserBase::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void UserBase::CopyFrom(const UserBase& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool UserBase::IsInitialized() const {
  if ((_has_bits_[0] & 0x0000000f) != 0x0000000f) return false;

  return true;
}

void UserBase::Swap(UserBase* other) {
  if (other != this) {
    std::swap(coin_, other->coin_);
    std::swap(cash_, other->cash_);
    std::swap(level_, other->level_);
    std::swap(exp_, other->exp_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata UserBase::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = UserBase_descriptor_;
  metadata.reflection = UserBase_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int Build::kBuildidFieldNumber;
const int Build::kPosFieldNumber;
#endif  // !_MSC_VER

Build::Build()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:UserCfg.Build)
}

void Build::InitAsDefaultInstance() {
}

Build::Build(const Build& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:UserCfg.Build)
}

void Build::SharedCtor() {
  _cached_size_ = 0;
  buildid_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Build::~Build() {
  // @@protoc_insertion_point(destructor:UserCfg.Build)
  SharedDtor();
}

void Build::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Build::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Build::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Build_descriptor_;
}

const Build& Build::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_UserConfig_2eproto();
  return *default_instance_;
}

Build* Build::default_instance_ = NULL;

Build* Build::New() const {
  return new Build;
}

void Build::Clear() {
  buildid_ = 0u;
  pos_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Build::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:UserCfg.Build)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required uint32 buildid = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &buildid_)));
          set_has_buildid();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_pos;
        break;
      }

      // repeated uint32 pos = 2;
      case 2: {
        if (tag == 16) {
         parse_pos:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 1, 16, input, this->mutable_pos())));
        } else if (tag == 18) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, this->mutable_pos())));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_pos;
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
  // @@protoc_insertion_point(parse_success:UserCfg.Build)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:UserCfg.Build)
  return false;
#undef DO_
}

void Build::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:UserCfg.Build)
  // required uint32 buildid = 1;
  if (has_buildid()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->buildid(), output);
  }

  // repeated uint32 pos = 2;
  for (int i = 0; i < this->pos_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(
      2, this->pos(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:UserCfg.Build)
}

::google::protobuf::uint8* Build::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:UserCfg.Build)
  // required uint32 buildid = 1;
  if (has_buildid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->buildid(), target);
  }

  // repeated uint32 pos = 2;
  for (int i = 0; i < this->pos_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteUInt32ToArray(2, this->pos(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:UserCfg.Build)
  return target;
}

int Build::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required uint32 buildid = 1;
    if (has_buildid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->buildid());
    }

  }
  // repeated uint32 pos = 2;
  {
    int data_size = 0;
    for (int i = 0; i < this->pos_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        UInt32Size(this->pos(i));
    }
    total_size += 1 * this->pos_size() + data_size;
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

void Build::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Build* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Build*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Build::MergeFrom(const Build& from) {
  GOOGLE_CHECK_NE(&from, this);
  pos_.MergeFrom(from.pos_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_buildid()) {
      set_buildid(from.buildid());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Build::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Build::CopyFrom(const Build& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Build::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void Build::Swap(Build* other) {
  if (other != this) {
    std::swap(buildid_, other->buildid_);
    pos_.Swap(&other->pos_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Build::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Build_descriptor_;
  metadata.reflection = Build_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int User::kUserInitFieldNumber;
const int User::kUserExpFieldNumber;
const int User::kBuildsFieldNumber;
const int User::kInitpropsFieldNumber;
#endif  // !_MSC_VER

User::User()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:UserCfg.User)
}

void User::InitAsDefaultInstance() {
  user_init_ = const_cast< ::UserCfg::UserBase*>(&::UserCfg::UserBase::default_instance());
  initprops_ = const_cast< ::CommonGiftConfig::CommonModifyItem*>(&::CommonGiftConfig::CommonModifyItem::default_instance());
}

User::User(const User& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:UserCfg.User)
}

void User::SharedCtor() {
  _cached_size_ = 0;
  user_init_ = NULL;
  initprops_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

User::~User() {
  // @@protoc_insertion_point(destructor:UserCfg.User)
  SharedDtor();
}

void User::SharedDtor() {
  if (this != default_instance_) {
    delete user_init_;
    delete initprops_;
  }
}

void User::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* User::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return User_descriptor_;
}

const User& User::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_UserConfig_2eproto();
  return *default_instance_;
}

User* User::default_instance_ = NULL;

User* User::New() const {
  return new User;
}

void User::Clear() {
  if (_has_bits_[0 / 32] & 9) {
    if (has_user_init()) {
      if (user_init_ != NULL) user_init_->::UserCfg::UserBase::Clear();
    }
    if (has_initprops()) {
      if (initprops_ != NULL) initprops_->::CommonGiftConfig::CommonModifyItem::Clear();
    }
  }
  user_exp_.Clear();
  builds_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool User::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:UserCfg.User)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .UserCfg.UserBase user_init = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_user_init()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_user_exp;
        break;
      }

      // repeated uint64 user_exp = 2;
      case 2: {
        if (tag == 16) {
         parse_user_exp:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 1, 16, input, this->mutable_user_exp())));
        } else if (tag == 18) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, this->mutable_user_exp())));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_user_exp;
        if (input->ExpectTag(26)) goto parse_builds;
        break;
      }

      // repeated .UserCfg.Build builds = 3;
      case 3: {
        if (tag == 26) {
         parse_builds:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_builds()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_builds;
        if (input->ExpectTag(34)) goto parse_initprops;
        break;
      }

      // required .CommonGiftConfig.CommonModifyItem initprops = 4;
      case 4: {
        if (tag == 34) {
         parse_initprops:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_initprops()));
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
  // @@protoc_insertion_point(parse_success:UserCfg.User)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:UserCfg.User)
  return false;
#undef DO_
}

void User::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:UserCfg.User)
  // required .UserCfg.UserBase user_init = 1;
  if (has_user_init()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->user_init(), output);
  }

  // repeated uint64 user_exp = 2;
  for (int i = 0; i < this->user_exp_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(
      2, this->user_exp(i), output);
  }

  // repeated .UserCfg.Build builds = 3;
  for (int i = 0; i < this->builds_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      3, this->builds(i), output);
  }

  // required .CommonGiftConfig.CommonModifyItem initprops = 4;
  if (has_initprops()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      4, this->initprops(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:UserCfg.User)
}

::google::protobuf::uint8* User::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:UserCfg.User)
  // required .UserCfg.UserBase user_init = 1;
  if (has_user_init()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->user_init(), target);
  }

  // repeated uint64 user_exp = 2;
  for (int i = 0; i < this->user_exp_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteUInt64ToArray(2, this->user_exp(i), target);
  }

  // repeated .UserCfg.Build builds = 3;
  for (int i = 0; i < this->builds_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        3, this->builds(i), target);
  }

  // required .CommonGiftConfig.CommonModifyItem initprops = 4;
  if (has_initprops()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        4, this->initprops(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:UserCfg.User)
  return target;
}

int User::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .UserCfg.UserBase user_init = 1;
    if (has_user_init()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->user_init());
    }

    // required .CommonGiftConfig.CommonModifyItem initprops = 4;
    if (has_initprops()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->initprops());
    }

  }
  // repeated uint64 user_exp = 2;
  {
    int data_size = 0;
    for (int i = 0; i < this->user_exp_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        UInt64Size(this->user_exp(i));
    }
    total_size += 1 * this->user_exp_size() + data_size;
  }

  // repeated .UserCfg.Build builds = 3;
  total_size += 1 * this->builds_size();
  for (int i = 0; i < this->builds_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->builds(i));
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

void User::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const User* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const User*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void User::MergeFrom(const User& from) {
  GOOGLE_CHECK_NE(&from, this);
  user_exp_.MergeFrom(from.user_exp_);
  builds_.MergeFrom(from.builds_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_user_init()) {
      mutable_user_init()->::UserCfg::UserBase::MergeFrom(from.user_init());
    }
    if (from.has_initprops()) {
      mutable_initprops()->::CommonGiftConfig::CommonModifyItem::MergeFrom(from.initprops());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void User::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void User::CopyFrom(const User& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool User::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000009) != 0x00000009) return false;

  if (has_user_init()) {
    if (!this->user_init().IsInitialized()) return false;
  }
  if (!::google::protobuf::internal::AllAreInitialized(this->builds())) return false;
  if (has_initprops()) {
    if (!this->initprops().IsInitialized()) return false;
  }
  return true;
}

void User::Swap(User* other) {
  if (other != this) {
    std::swap(user_init_, other->user_init_);
    user_exp_.Swap(&other->user_exp_);
    builds_.Swap(&other->builds_);
    std::swap(initprops_, other->initprops_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata User::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = User_descriptor_;
  metadata.reflection = User_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace UserCfg

// @@protoc_insertion_point(global_scope)
