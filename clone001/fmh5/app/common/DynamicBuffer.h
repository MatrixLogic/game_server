/*
 * DynamicBuffer.h
 *
 *  Created on: 2014-7-25
 *      Author: kuroky
 */

#ifndef DYNAMICBUFFER_H_
#define DYNAMICBUFFER_H_

#include "Common.h"
#define TCP_CHANNEL_BUFFER_SIZE      0x800000
#define INCREMENT_FACTOR			 0x80000
#define DEFAULT_SIZE	 			 0x20000   //128K
template<size_t Capacity>
class CDynamicBuffer : public IBuffer
{
public:

	CDynamicBuffer():m_uSize(0)
	{
		m_capacity = Capacity;
		m_pBuffer = new byte[m_capacity];
	}

	CDynamicBuffer(const CDynamicBuffer& other)
	{
		m_capacity = other.m_capacity;
		m_pBuffer = new byte[m_capacity];
		this->m_uSize = other.m_uSize;
		memcpy(m_pBuffer,other.m_pBuffer,m_capacity);
	}
	~CDynamicBuffer()
	{
		delete [] m_pBuffer;
	}
	virtual byte *GetNativeBuffer()
	{
		return m_pBuffer;
	}
	virtual const byte *GetConstBuffer() const
	{
		return m_pBuffer;
	}
	virtual uint32_t GetSize() const
	{
		return m_uSize;
	}
	virtual bool SetSize(uint32_t size)
	{
		if(size > m_capacity)
		{
			return false;
		}
		m_uSize = size;
		return true;
	}
	uint32_t Reset()
	{
		m_capacity = DEFAULT_SIZE;
		return m_capacity;
	}
	virtual uint32_t GetCapacity() const
	{
		return m_capacity;
	}

	virtual bool IsEmpty() const
	{
		return m_uSize == 0;
	}

	virtual bool Clear()
	{
		m_uSize = 0;
		return true;
	}

	virtual byte GetAt(uint32_t uIndex) const
	{
		if(uIndex >= m_uSize)
		{
			return 0;
		}
		return m_pBuffer[uIndex];
	}

	virtual bool SetAt(uint32_t uIndex, byte cValue)
	{
		if(uIndex >= m_uSize)
		{
			return false;
		}
		m_pBuffer[uIndex] = cValue;
		return true;
	}

	virtual bool CopyFrom(const byte *pcBuffer, uint32_t uSize)
	{
		if(pcBuffer == NULL || uSize > m_capacity)
		{
			return false;
		}
		memcpy(m_pBuffer, pcBuffer, uSize);
		m_uSize = uSize;
		return true;
	}

	virtual bool Append(const byte *pcBuffer, uint32_t uSize)
	{
		if(pcBuffer == NULL)
		{
			return false;
		}
		if(m_uSize + uSize > TCP_CHANNEL_BUFFER_SIZE)
		{
			return false;
		}
		if(m_uSize + uSize > m_capacity)
		{
			uint32_t uNeedSize = m_uSize + uSize;
			uint32_t ucapacity = m_capacity;
			while(1)
			{
				ucapacity += INCREMENT_FACTOR;
				if(ucapacity >= uNeedSize)
				{
					if(ucapacity <= TCP_CHANNEL_BUFFER_SIZE)
					{
						break;
					}
					else
					{
						error_log("!!!!----need more buffer Capacity:%d m_uSize:%d uSize:%d\n",m_capacity,m_uSize,uSize);
						return false;
					}
				}
			}
			error_log("XXXX----need more buffer Capacity:%d m_uSize:%d uSize:%d\n",m_capacity,m_uSize,uSize);
			byte * pTempBuffer =  m_pBuffer;
			m_pBuffer = new byte[ucapacity];
			if(m_pBuffer == NULL)
			{
				m_pBuffer = pTempBuffer;
				pTempBuffer = NULL;
				return false;
			}
			m_capacity = ucapacity;
			memcpy(m_pBuffer, pTempBuffer, m_uSize);
			memcpy(m_pBuffer + m_uSize, pcBuffer, uSize);
			m_uSize += uSize;
			delete [] pTempBuffer;
			return true;
		}
		if(m_uSize + uSize <= m_capacity)
		{
			memcpy(m_pBuffer + m_uSize, pcBuffer, uSize);
			m_uSize += uSize;
			return true;
		}
		return false;
	}

	virtual bool Remove(uint32_t uStart, uint32_t uSize)
	{
		if(uStart + uSize > m_uSize)
		{
			return false;
		}
		memmove(m_pBuffer + uStart, m_pBuffer + uStart + uSize, m_uSize - uSize - uStart);
		m_uSize -= uSize;
		return true;
	}

	virtual bool GetData(byte *pBuffer, uint32_t uSize, uint32_t uIndex) const
	{
		if(pBuffer == NULL || uIndex + uSize > m_uSize)
		{
			return false;
		}
		memcpy(pBuffer, m_pBuffer + uIndex, uSize);
		return true;
	}

	CDynamicBuffer& operator = (const CDynamicBuffer& other){
		this->m_uSize = other.m_uSize;
		memcpy(m_pBuffer,other.m_pBuffer,m_capacity);
		return *this;
	}

private:
	byte * m_pBuffer;
	size_t m_capacity;
	uint32_t m_uSize;
};

#endif /* DYNAMICBUFFER_H_ */
