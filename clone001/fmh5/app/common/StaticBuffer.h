/*
 * StaticBuffer.h
 *
 *  Created on: 2012-2-15
 *      Author: dada
 */

#ifndef STATICBUFFER_H_
#define STATICBUFFER_H_

template<size_t Capacity>
class CStaticBuffer : public IBuffer
{
public:

	CStaticBuffer() : m_uSize(0) {}
	CStaticBuffer(const CStaticBuffer& other){
		this->m_uSize = other.m_uSize;
		memcpy(m_pBuffer,other.m_pBuffer,Capacity);
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
		if(size > Capacity)
		{
			return false;
		}
		m_uSize = size;
		return true;
	}

	virtual uint32_t GetCapacity() const
	{
		return Capacity;
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
		if(pcBuffer == NULL || uSize > Capacity)
		{
			return false;
		}
		memcpy(m_pBuffer, pcBuffer, uSize);
		m_uSize = uSize;
		return true;
	}

	virtual bool Append(const byte *pcBuffer, uint32_t uSize)
	{
		if(pcBuffer == NULL || m_uSize + uSize > Capacity)
		{
			return false;
		}
		memcpy(m_pBuffer + m_uSize, pcBuffer, uSize);
		m_uSize += uSize;
		return true;
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

	CStaticBuffer& operator = (const CStaticBuffer& other){
		this->m_uSize = other.m_uSize;
		memcpy(m_pBuffer,other.m_pBuffer,Capacity);
		return *this;
	}

private:
	byte m_pBuffer[Capacity];
	uint32_t m_uSize;
};

#endif /* STATICBUFFER_H_ */
