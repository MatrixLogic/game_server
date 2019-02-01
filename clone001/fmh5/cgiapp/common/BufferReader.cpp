/*
 * BufferReader.cpp
 *
 *  Created on: 2012-2-8
 *      Author: dada
 */

#include "BufferReader.h"

CBufferReader::CBufferReader() :
	m_pBuffer(NULL),
	m_index(0),
	m_bSwapByte(false)
{
}

CBufferReader::CBufferReader(IBuffer *pBuffer) :
	m_pBuffer(pBuffer),
	m_index(0),
	m_bSwapByte(false)
{
}

bool CBufferReader::Attach(IBuffer *pBuffer)
{
	m_pBuffer = pBuffer;
	m_index = 0;
	return true;
}

bool CBufferReader::Detach()
{
	m_pBuffer = NULL;
	m_index = 0;
	return true;
}

bool CBufferReader::SetSwapByte(bool bSwapByte)
{
	m_bSwapByte = bSwapByte;
	return true;
}

uint32_t CBufferReader::GetReadSize()
{
	return m_index;
}

bool CBufferReader::Skip(int offset)
{
	m_index += offset;
	return true;
}

#define IMPLEMENT_BUFFER_READ(TypeName, Type, SwapByte)	\
	bool CBufferReader::Get##TypeName(Type &value)	\
	{	\
		if(m_pBuffer == NULL)	\
		{	\
			return false;	\
		}	\
		if(m_pBuffer->GetData((byte *)&value, sizeof(Type), m_index))	\
		{	\
			m_index += sizeof(Type);	\
			if(m_bSwapByte)	\
			{	\
				value = SwapByte(value);	\
			}	\
			return true;	\
		}	\
		else	\
		{	\
			return false;	\
		}	\
	}	\

IMPLEMENT_BUFFER_READ(Byte, byte, ntohb)
IMPLEMENT_BUFFER_READ(Int16, int16_t, ntohs)
IMPLEMENT_BUFFER_READ(UInt16, uint16_t, ntohs)
IMPLEMENT_BUFFER_READ(Int32, int32_t, ntohl)
IMPLEMENT_BUFFER_READ(UInt32, uint32_t, ntohl)
IMPLEMENT_BUFFER_READ(Int64, int64_t, ntoh64)
IMPLEMENT_BUFFER_READ(UInt64, uint64_t, ntoh64)

bool CBufferReader::GetShortString(string &value)
{
	if(m_pBuffer == NULL)
	{
		return false;
	}
	byte size;
	if(!m_pBuffer->GetData(&size, sizeof(size), m_index))
	{
		return false;
	}
	value.resize(size);
	if(!m_pBuffer->GetData((byte *)const_cast<char *>(value.c_str()), size, m_index + sizeof(size)))
	{
		return false;
	}
	m_index += sizeof(size) + size;
	return true;
}

bool CBufferReader::GetString(string &value)
{
	if(m_pBuffer == NULL)
	{
		return false;
	}
	uint16_t size;
	if(!m_pBuffer->GetData((byte *)&size, sizeof(size), m_index))
	{
		return false;
	}
	if(m_bSwapByte)
	{
		size = ntohs(size);
	}
	value.resize(size);
	if(!m_pBuffer->GetData((byte *)const_cast<char *>(value.c_str()), size, m_index + sizeof(size)))
	{
		return false;
	}
	m_index += sizeof(size) + size;
	return true;
}
bool CBufferReader::GetPBMsg(Message* &msg)
{
	string name;
	if(!GetString(name))
	{
		error_log("get name");
		return false;
	}
	msg = Convert::CreateMessage(name);
	if(msg == NULL)
	{
		error_log("create msg:%s", name.c_str());
		return false;
	}

	uint32_t size;
	if(!m_pBuffer->GetData((byte *)&size, sizeof(size), m_index))
	{
		error_log("get length:%s", name.c_str());
		delete msg;
		msg = NULL;
		return false;
	}
	if(m_bSwapByte)
		size = ntohl(size);
	if(!msg->ParseFromArray(m_pBuffer->GetConstBuffer() + m_index + sizeof(size), size))
	{
		error_log("get array:%u,%s",size, name.c_str());
		delete msg;
		msg = NULL;
		return false;
	}
	m_index += sizeof(size) + size;
	return true;
}

bool CBufferReader::GetBuffer(IBuffer *value)
{
	if(m_pBuffer == NULL || value == NULL)
	{
		return false;
	}
	uint16_t size;
	if(!m_pBuffer->GetData((byte *)&value, sizeof(size), m_index))
	{
		return false;
	}
	if(m_bSwapByte)
	{
		size = ntohs(size);
	}
	if(!m_pBuffer->SetSize(size))
	{
		return false;
	}
	if(!m_pBuffer->GetData(value->GetNativeBuffer(), size, m_index + sizeof(size)))
	{
		return false;
	}
	m_index += sizeof(size) + size;
	return true;
}

bool CBufferReader::GetBytes(byte *value, uint32_t size)
{
	if(m_pBuffer == NULL || value == NULL)
	{
		return false;
	}
	if(!m_pBuffer->GetData(value, size, m_index))
	{
		return false;
	}
	m_index += size;
	return true;
}
