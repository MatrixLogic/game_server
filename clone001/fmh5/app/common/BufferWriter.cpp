/*
 * BufferWriter.cpp
 *
 *  Created on: 2012-2-8
 *      Author: dada
 */

#include "BufferWriter.h"

CBufferWriter::CBufferWriter() :
	m_pBuffer(NULL),
	m_bSwapByte(false)
{
}

CBufferWriter::CBufferWriter(IBuffer *pBuffer) :
	m_pBuffer(pBuffer),
	m_bSwapByte(false)
{
}

bool CBufferWriter::Attach(IBuffer *pBuffer)
{
	m_pBuffer = pBuffer;
	return true;
}

bool CBufferWriter::Detach()
{
	m_pBuffer = NULL;
	return true;
}

bool CBufferWriter::SetSwapByte(bool bSwapByte)
{
	m_bSwapByte = bSwapByte;
	return true;
}

#define NOTHING(value) value

#define IMPLEMENT_BUFFER_WRITE(TypeName, Type, SwapByte)	\
	bool CBufferWriter::Append##TypeName(Type value)	\
	{	\
		if(m_pBuffer == NULL)	\
		{	\
			return false;	\
		}	\
		if(m_bSwapByte)	\
		{	\
			value = SwapByte(value);	\
		}	\
		return m_pBuffer->Append((byte *)&value, sizeof(Type));	\
	}	\

IMPLEMENT_BUFFER_WRITE(Byte, byte, htonb)
IMPLEMENT_BUFFER_WRITE(Int16, int16_t, htons)
IMPLEMENT_BUFFER_WRITE(UInt16, uint16_t, htons)
IMPLEMENT_BUFFER_WRITE(Int32, int32_t, htonl)
IMPLEMENT_BUFFER_WRITE(UInt32, uint32_t, htonl)
IMPLEMENT_BUFFER_WRITE(Int64, int64_t, hton64)
IMPLEMENT_BUFFER_WRITE(UInt64, uint64_t, hton64)
//IMPLEMENT_BUFFER_WRITE(Double, double, hton64)

bool CBufferWriter::AppendFloat(float value){
	if(m_pBuffer == NULL)
	{
		return false;
	}

	return m_pBuffer->Append((byte *)&value, sizeof(float));
}

bool CBufferWriter::AppendDouble(double value){
	if(m_pBuffer == NULL)
	{
		return false;
	}

	return m_pBuffer->Append((byte *)&value, sizeof(double));
}

bool CBufferWriter::AppendShortString(const string &value)
{
	if(m_pBuffer == NULL)
	{
		return false;
	}
	byte size = (byte)value.size();
	if(!m_pBuffer->Append(&size, sizeof(size)))
	{
		return false;
	}
	if(!m_pBuffer->Append((byte *)value.c_str(), size))
	{
		m_pBuffer->SetSize(m_pBuffer->GetSize() - sizeof(size));
		return false;
	}
	return true;
}

bool CBufferWriter::AppendString(const string &value)
{
	if(m_pBuffer == NULL)
	{
		return false;
	}
	uint16_t size = (uint16_t)value.size();
	if(m_bSwapByte)
	{
		uint16_t tempSize = htons(size);
		if(!m_pBuffer->Append((byte *)&tempSize, sizeof(tempSize)))
		{
			return false;
		}
	}
	else
	{
		if(!m_pBuffer->Append((byte *)&size, sizeof(size)))
		{
			return false;
		}
	}
	if(!m_pBuffer->Append((byte *)value.c_str(), size))
	{
		m_pBuffer->SetSize(m_pBuffer->GetSize() - sizeof(size));
		return false;
	}
	return true;
}
bool CBufferWriter::AppendPBMsg(const Message* msg)
{
	if(msg == NULL)
		return false;

	uint32_t oldSize = m_pBuffer->GetSize();
	if(!AppendString(msg->GetTypeName()))
	{
		error_log("append name:%s", msg->GetTypeName().c_str());
		return false;
	}

	uint32_t size = msg->ByteSize();
	if(m_bSwapByte)
	{
		uint32_t tempSize = htonl(size);
		if(!m_pBuffer->Append((byte *)&tempSize, sizeof(tempSize)))
		{
			error_log("append length:%s", msg->GetTypeName().c_str());
			m_pBuffer->SetSize(oldSize);
			return false;
		}
	}
	else
	{
		if(!m_pBuffer->Append((byte *)&size, sizeof(size)))
		{
			error_log("append length:%s", msg->GetTypeName().c_str());
			m_pBuffer->SetSize(oldSize);
			return false;
		}
	}
	if(size > m_pBuffer->GetFreeCapacity())
	{
		error_log("size out:%u", size);
		return false;
	}
	if(!msg->SerializeToArray(m_pBuffer->GetNativeBuffer() + m_pBuffer->GetSize(), size))
	{
		error_log("append array:%u,%s", size, msg->GetTypeName().c_str());
		m_pBuffer->SetSize(oldSize);
		return false;
	}
	m_pBuffer->SetSize(m_pBuffer->GetSize() + size);
	return true;
}

bool CBufferWriter::AppendBuffer(const IBuffer *value)
{
	if(m_pBuffer == NULL || value == NULL)
	{
		return false;
	}
	uint16_t size = (uint16_t)value->GetSize();
	if(m_bSwapByte)
	{
		uint16_t tempSize = htons(size);
		if(!m_pBuffer->Append((byte *)&tempSize, sizeof(tempSize)))
		{
			return false;
		}
	}
	else
	{
		if(!m_pBuffer->Append((byte *)&size, sizeof(size)))
		{
			return false;
		}
	}
	if(!m_pBuffer->AppendBuffer(value))
	{
		m_pBuffer->SetSize(m_pBuffer->GetSize() - sizeof(size));
		return false;
	}
	return true;
}

bool CBufferWriter::AppendBytes(const byte *value, uint32_t size)
{
	if(m_pBuffer == NULL || value == NULL)
	{
		return false;
	}
	if(!m_pBuffer->Append(value, size))
	{
		return false;
	}
	return true;
}
