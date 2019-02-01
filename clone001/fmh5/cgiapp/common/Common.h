/*
 * Common.h
 *
 *  Created on: 2011-5-20
 *      Author: dada
 */


#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <algorithm>
#include <iostream>
#include <fstream>

using std::string;
using std::pair;
using std::vector;
using std::list;
using std::map;
using std::set;
using std::queue;
using std::stack;


#include <google/protobuf/stubs/common.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include <google/protobuf/descriptor.h>

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::DescriptorPool;
using google::protobuf::MessageFactory;
using google::protobuf::Reflection;

#include "basic.h"
#include "trans.h"
#include "json.h"
#include "markupstl.h"
#include "commlog.h"
#include "ctime.h"
#include "tsc.h"
#include "crc.h"

#include "CommonUtil.h"
#include "StringUtil.h"
#include "Math.h"
#include "Convert.h"
#include "TimeUtil.h"
#include "FileUtil.h"
#include "JsonUtil.h"
#include "Crypt.h"
#include "HttpUtil.h"
#include "Compress.h"
#include "Config.h"
#include "IBase.h"

#define LogError(fmt, ...)	error_log("[" fmt "]",   __VA_ARGS__)

#include "SystemUtil.h"
#include "Thread.h"
#include "ILock.h"
#include "ISemaphore.h"
#include "IMemory.h"
#include "IShareMemory.h"
#include "HeapMemory.h"
#include "AutoLock.h"
#include "Mutex.h"
#include "SpinLock.h"
#include "Semaphore.h"
#include "SemaphoreLock.h"
#include "Vsem.h"
#include "ThreadSemaphore.h"
#include "Event.h"
#include "ShareMemory.h"
#include "ReadOnlyShareMemory.h"
#include "IBuffer.h"
#include "IMessageQueue.h"
#include "Buffer.h"
#include "StaticBuffer.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "MessageQueue.h"
#include "Socket.h"
#include "StringFilter.h"
#include "MainConfig.h"

#include "comm.h"
#include "SaveInc.h"

#endif //__COMMON_H__
