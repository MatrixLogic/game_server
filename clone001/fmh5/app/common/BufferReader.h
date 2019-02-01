/*
 * BufferReader.h
 *
 *  Created on: 2012-2-8
 *      Author: dada
 */

#ifndef BUFFERREADER_H_
#define BUFFERREADER_H_

#include "Common.h"

class CBufferReader
{
public:
	CBufferReader();
	CBufferReader(IBuffer *pBuffer);
	bool Attach(IBuffer *pBuffer);
	bool Detach();
	bool SetSwapByte(bool bSwapByte);	//设置是否自动转换字节序
	uint32_t GetReadSize();
	bool Skip(int offset);

	bool GetByte(byte &value);
	bool GetInt16(int16_t &value);
	bool GetUInt16(uint16_t &value);
	bool GetInt32(int32_t &value);
	bool GetUInt32(uint32_t &value);
	bool GetInt64(int64_t &value);
	bool GetUInt64(uint64_t &value);
	bool GetFloat(float &value);
	bool GetDouble(double &value);
	bool GetShortString(string &value);
	bool GetString(string &value);
	bool GetPBMsg(Message* &msg);
	bool GetBuffer(IBuffer *value);
	bool GetBytes(byte *value, uint32_t size);

private:
	IBuffer *m_pBuffer;
	uint32_t m_index;
	bool m_bSwapByte;
};


#endif /* BUFFERREADER_H_ */
